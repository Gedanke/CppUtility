#ifndef BENCHMARK_TIMER_H
#define BENCHMARK_TIMER_H

#include <chrono>
#include <iostream>

class Timer
{
private:
    // 起始时间
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;

public:
    Timer()
    {
        // 从类创建开始
        this->startTimePoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        // 到类销毁结束
        this->stop();
    }

    void stop()
    {
        // 调用析构，结束
        auto endTimePoint = std::chrono::high_resolution_clock::now();
        // 开始时间
        auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(this->startTimePoint).time_since_epoch().count();
        // 结束时间
        auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimePoint).time_since_epoch().count();

        // 毫秒为单位
        auto duration = end - start;
        double result = duration * 0.001;
        printf("%ld us (%lf ms)\n", duration, result);

        fflush(stdout);
    }
};

#endif // BENCHMARK_TIMER_H