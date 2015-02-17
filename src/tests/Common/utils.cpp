
#include "utils.h"

#include <functional>
#include <chrono>
#include <cstring>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace utils
{
    void RunLoop(float deltaMs, std::function<bool(double, float)> loopFunc)
    {
        using namespace std::chrono;
        using DoubleDuration = duration<double>;
        using DoubleTimePoint = time_point<high_resolution_clock, DoubleDuration>;

        auto period = duration_cast<nanoseconds>(duration<float, std::milli>(deltaMs));

        auto curTime = high_resolution_clock::now();
        auto accumulator = high_resolution_clock::duration();
        bool keepLooping = true;
        while (keepLooping)
        {
            auto now = high_resolution_clock::now();
            auto deltaTime = (now - curTime);
            if (deltaTime > milliseconds(250))
            {
                deltaTime = milliseconds(250);
            }
            curTime += deltaTime;
            accumulator += deltaTime;

            while (accumulator >= period)
            {
                DoubleTimePoint curTimeDouble = time_point_cast<DoubleDuration>(curTime);
                keepLooping = loopFunc(curTimeDouble.time_since_epoch().count(), deltaMs) && keepLooping;
                accumulator -= duration_cast<decltype(accumulator)>(period);
                curTime += duration_cast<decltype(accumulator)>(period);
            }
        }
    }

    void FixWorkingDirectory()
    {
        char temp[128] = {};
        const char *dir = getcwd(temp, sizeof(temp));
        const char *bin_pos = strstr(dir, "bin");
        const char *build_pos = strstr(dir, "premake");
        if (bin_pos)
        {
            chdir("..");
        }
        else if (build_pos)
        {
            chdir("../../..");
        }
    }
}
