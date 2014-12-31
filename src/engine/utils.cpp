
#include "utils.h"

#include <functional>
#include <chrono>

namespace utils
{
    void RunLoop(float msTime, std::function<bool()> loopFunc)
    {
        using namespace std::chrono;
        auto period = duration_cast<nanoseconds>(duration<float, std::milli>(msTime));

        auto curTime = high_resolution_clock::now();
        auto accumulator = high_resolution_clock::duration();
        bool keepLooping = true;
        while (keepLooping)
        {
            auto now = high_resolution_clock::now();
            auto deltaTime = (now - curTime);
            curTime = now;
            accumulator += deltaTime;
            if (accumulator >= period)
            {
                keepLooping = loopFunc();
                accumulator %= duration_cast<decltype(accumulator)>(period);
            }
        }
    }
}
