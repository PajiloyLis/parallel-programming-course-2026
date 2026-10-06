cmake_minimum_required(VERSION 4.1)
project(ITMO_ParallelProg)

set(CMAKE_CXX_STANDARD 20)

add_executable(Step0 Step0/Snapshot.hpp
        Step0/MetricsCollector.hpp
        Step0/SimpleCollector.cpp
        Step0/Generator.cpp
        Step0/Generator.h
        Step0/main.cpp
        Step0/SimpleCollector.hpp)

add_executable(Step1 Step1/Snapshot.hpp
        Step1/MetricsCollector.hpp
        Step1/main.cpp
        Step1/Generator.h
        Step1/Generator.cpp
        Step1/MutexCollector.cpp
        Step1/MutexCollector.hpp
        Step1/EmptyMutexCollector.h)

add_executable(Step2 Step2/MetricsCollector.hpp
        Step2/ShardedCollector.cpp
        Step2/ShardedCollector.hpp
        Step2/main.cpp
        Step2/Snapshot.hpp
        Step2/Generator.cpp
        Step2/Generator.hpp
        Step2/PercentileCalc.cpp
        Step2/PercentileCalc.hpp)
add_executable(stress2 Step2/stress.cpp
        Step2/MetricsCollector.hpp
        Step2/ShardedCollector.cpp
        Step2/ShardedCollector.hpp
        Step2/Snapshot.hpp
        Step2/Generator.cpp
        Step2/Generator.hpp
        Step2/PercentileCalc.cpp
        Step2/PercentileCalc.hpp)
add_executable(Step3 Step3/ThreadState.h
        Step3/ThreadLocalCollector.cpp
        Step3/ThreadLocalCollector.h
        Step3/Generator.cpp
        Step3/Generator.hpp
        Step3/main.cpp
        Step3/Snapshot.hpp
        Step3/MetricsCollector.hpp
        Step3/PercentileCalc.cpp
        Step3/PercentileCalc.hpp)
add_executable(stress3 Step3/ThreadState.h
        Step3/ThreadLocalCollector.cpp
        Step3/ThreadLocalCollector.h
        Step3/Generator.cpp
        Step3/Generator.hpp
        Step3/stress.cpp
        Step3/Snapshot.hpp
        Step3/MetricsCollector.hpp
        Step3/PercentileCalc.cpp
        Step3/PercentileCalc.hpp)
add_executable(Step4 Step4/ThreadBuffers.h
        Step4/DoubleBufferCollector.cpp
        Step4/DoubleBufferCollector.h
        Step4/Generator.hpp
        Step4/Generator.cpp
        Step4/main.cpp
        Step4/MetricsCollector.hpp
        Step4/PercentileCalc.hpp
        Step4/PercentileCalc.cpp
        Step4/Snapshot.hpp)
add_executable(stress4 Step4/ThreadBuffers.h
        Step4/DoubleBufferCollector.cpp
        Step4/DoubleBufferCollector.h
        Step4/Generator.hpp
        Step4/Generator.cpp
        Step4/MetricsCollector.hpp
        Step4/PercentileCalc.hpp
        Step4/PercentileCalc.cpp
        Step4/Snapshot.hpp
        Step4/stress.cpp)
