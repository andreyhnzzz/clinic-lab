#include "PerformanceMeter.h"

void PerformanceMeter::start() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

double PerformanceMeter::stop() {
    endTime_ = std::chrono::high_resolution_clock::now();
    running_ = false;
    return std::chrono::duration<double, std::milli>(endTime_ - startTime_).count();
}

double PerformanceMeter::elapsed() const {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(now - startTime_).count();
}
