#pragma once

#include <chrono>
#include <cstdint>
#include <QString>

// Lightweight RAII timer that measures elapsed time with microsecond precision.
// Usage:
//   TimerUtils::ScopedTimer t;
//   // ... do work ...
//   qint64 us = t.elapsedMicroseconds();
//   qint64 ms = t.elapsedMilliseconds();

class TimerUtils
{
public:
    // ── ScopedTimer ──────────────────────────────────────────────────────────
    class ScopedTimer
    {
    public:
        ScopedTimer() : m_start(std::chrono::high_resolution_clock::now()) {}

        void reset() { m_start = std::chrono::high_resolution_clock::now(); }

        qint64 elapsedMicroseconds() const
        {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(now - m_start).count();
        }

        qint64 elapsedMilliseconds() const
        {
            return elapsedMicroseconds() / 1000LL;
        }

        double elapsedSeconds() const
        {
            return static_cast<double>(elapsedMicroseconds()) / 1'000'000.0;
        }

        // Returns a human-readable string: "1.234 ms" or "12.3 µs"
        QString elapsedString() const
        {
            qint64 us = elapsedMicroseconds();
            if (us >= 1000)
                return QString::number(us / 1000.0, 'f', 3) + " ms";
            return QString::number(us) + " µs";
        }

    private:
        std::chrono::high_resolution_clock::time_point m_start;
    };

    // ── Free-function helpers ─────────────────────────────────────────────────

    // Benchmark a callable and return elapsed microseconds
    template<typename Callable>
    static qint64 measureMicroseconds(Callable &&fn)
    {
        ScopedTimer t;
        fn();
        return t.elapsedMicroseconds();
    }

    template<typename Callable>
    static qint64 measureMilliseconds(Callable &&fn)
    {
        return measureMicroseconds(std::forward<Callable>(fn)) / 1000LL;
    }
};
