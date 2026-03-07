#pragma once
#include <chrono>
#include <QString>
#include <QVector>

struct BenchmarkResult {
    QString algorithmName;
    int dataSize = 0;
    double timeMs = 0.0;
    QString dataType;
};

class PerformanceMeter {
public:
    void start();
    double stop();     // returns milliseconds elapsed and stops timer
    double elapsed() const;  // current elapsed ms without stopping

    static QVector<BenchmarkResult> benchmarkSorts(int dataSize);
    static QPair<double, double> benchmarkSearchComparison(int dataSize);

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    bool running_ = false;
};
