import subprocess
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError:
    sys.exit("Нужен matplotlib: pip install matplotlib")

ROOT = Path(__file__).resolve().parent
PLOTS = ROOT / "plots"
BUILD = ROOT / "build"
PLOTS.mkdir(exist_ok=True)
BUILD.mkdir(exist_ok=True)

CXX = "g++"
CXXFLAGS = ["-O2", "-std=c++20", "-pthread", "-Wall"]


def build(name: str, sources: list[Path]) -> Path:
    out = BUILD / name
    cmd = [CXX, *CXXFLAGS, *[str(s) for s in sources], "-o", str(out)]
    print(f"[build] {' '.join(cmd)}")
    subprocess.run(cmd, check=True)
    return out


def run(exe: Path, args: list[str] | None = None) -> str:
    cmd = [str(exe)] + (args or [])
    print(f"[run]   {' '.join(cmd)}")
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=True,
        timeout=600,
    )
    if result.stderr:
        for line in result.stderr.splitlines():
            print(f"        {line}")
    return result.stdout


def parse_output(stdout: str) -> tuple[list[int], list[int]]:
    threads, ops = [], []
    for line in stdout.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split()
        if len(parts) != 2:
            continue
        try:
            t, o = int(parts[0]), int(parts[1])
        except ValueError:
            continue
        threads.append(t)
        ops.append(o)
    return threads, ops


def plot(data: dict[str, tuple[list[int], list[int]]],
         title: str, out_path: Path) -> None:
    plt.figure(figsize=(10, 6))
    all_T = set()
    for name, (threads, ops) in data.items():
        plt.plot(threads, [o / 1e6 for o in ops],
                 marker="o", linewidth=2, label=name)
        all_T.update(threads)

    plt.xlabel("Число потоков T")
    plt.ylabel("Пропускная способность, млн оп/сек")
    plt.title(title)
    plt.xticks(sorted(all_T))
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()
    print(f"[plot]  saved {out_path}")


def run_stress(name: str, sources: list[Path]) -> str:
    """Собирает и запускает стресс-тест согласованности, возвращает stdout."""
    exe = build(f"{name}_stress", sources)
    out = run(exe)
    (PLOTS / f"{name}_stress.txt").write_text(out, encoding="utf-8")
    return out


def main() -> None:
    results: dict[str, tuple[list[int], list[int]]] = {}

    # ---------- Step 0 ----------
    step0 = ROOT / "Step0"
    exe0 = build("step0", [
        step0 / "main.cpp",
        step0 / "SimpleCollector.cpp",
        step0 / "Generator.cpp",
    ])
    out0 = run(exe0)
    (PLOTS / "step0_raw.txt").write_text(out0, encoding="utf-8")
    results["Step 0: без синхронизации"] = parse_output(out0)

    # ---------- Step 1 ----------
    step1 = ROOT / "Step1"
    exe1 = build("step1", [
        step1 / "main.cpp",
        step1 / "MutexCollector.cpp",
        step1 / "Generator.cpp",
    ])

    out1m = run(exe1, ["mutex"])
    (PLOTS / "step1_mutex_raw.txt").write_text(out1m, encoding="utf-8")
    results["Step 1: один общий лок"] = parse_output(out1m)

    out1e = run(exe1, ["empty"])
    (PLOTS / "step1_empty_raw.txt").write_text(out1e, encoding="utf-8")
    results["Step 1: пустой лок"] = parse_output(out1e)

    # ---------- Step 2 ----------
    step2 = ROOT / "Step2"
    exe2 = build("step2", [
        step2 / "main.cpp",
        step2 / "ShardedCollector.cpp",
        step2 / "PercentileCalc.cpp",
        step2 / "Generator.cpp",
    ])
    out2 = run(exe2)
    (PLOTS / "step2_raw.txt").write_text(out2, encoding="utf-8")
    results["Step 2: шардирование"] = parse_output(out2)

    # ---------- Step 3 ----------
    step3 = ROOT / "Step3"
    exe3 = build("step3", [
        step3 / "main.cpp",
        step3 / "ThreadLocalCollector.cpp",
        step3 / "PercentileCalc.cpp",
        step3 / "Generator.cpp",
    ])
    out3 = run(exe3)
    (PLOTS / "step3_raw.txt").write_text(out3, encoding="utf-8")
    results["Step 3: thread-local"] = parse_output(out3)

    # ---------- Step 4 ----------
    step4 = ROOT / "Step4"
    exe4 = build("step4", [
        step4 / "main.cpp",
        step4 / "DoubleBufferCollector.cpp",
        step4 / "PercentileCalc.cpp",
        step4 / "Generator.cpp",
    ])
    out4 = run(exe4)
    (PLOTS / "step4_raw.txt").write_text(out4, encoding="utf-8")
    results["Step 4: двойная буферизация"] = parse_output(out4)

    # ---------- Графики ----------
    plot(
        {
            "Общий лок (full)":   results["Step 1: один общий лок"],
            "Пустой лок (empty)": results["Step 1: пустой лок"],
        },
        "Этап 1: один общий лок",
        PLOTS / "step1.png",
    )

    plot(
        {
            "Step 2: шардирование":  results["Step 2: шардирование"],
            "Step 3: thread-local":  results["Step 3: thread-local"],
            "Step 4: double buffer": results["Step 4: двойная буферизация"],
        },
        "Этапы 2–4: масштабирование",
        PLOTS / "steps_2_4.png",
    )

    plot(results, "Сводный график: этапы 0–4", PLOTS / "all.png")

    # ---------- Стресс-тесты (опционально) ----------
    run_stress("step2", [
        step2 / "stress.cpp",
        step2 / "ShardedCollector.cpp",
        step2 / "PercentileCalc.cpp",
        step2 / "Generator.cpp",
    ])
    run_stress("step3", [
        step3 / "stress.cpp",
        step3 / "ThreadLocalCollector.cpp",
        step3 / "PercentileCalc.cpp",
        step3 / "Generator.cpp",
    ])
    run_stress("step4", [
        step4 / "stress.cpp",
        step4 / "DoubleBufferCollector.cpp",
        step4 / "PercentileCalc.cpp",
        step4 / "Generator.cpp",
    ])


if __name__ == "__main__":
    main()
