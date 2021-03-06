
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
    void RunLoop(float deltaSec, std::function<bool(double, float)> loopFunc)
    {
        using namespace std::chrono;
        using DoubleDuration = duration<double>;

        const auto period = duration_cast<high_resolution_clock::duration>(duration<float>(deltaSec));

        auto currentTime = high_resolution_clock::now();
        auto accumulator = high_resolution_clock::duration();
        auto userTime = high_resolution_clock::duration();

        const auto MAX_FRAME_DURATION = duration_cast<high_resolution_clock::duration>(milliseconds(250));

        bool keepLooping = true;
        while (keepLooping)
        {
            auto now = high_resolution_clock::now();
            auto frameTime = (now - currentTime);
            if (frameTime > MAX_FRAME_DURATION)
            {
                frameTime = MAX_FRAME_DURATION;
            }
            currentTime = now;
            accumulator += frameTime;
            while (accumulator >= period)
            {
                DoubleDuration curTimeDouble = duration_cast<DoubleDuration>(userTime);
                keepLooping = loopFunc(curTimeDouble.count(), deltaSec) && keepLooping;
                accumulator -= period;
                userTime += period;
            }

        }
    }

    void FixWorkingDirectory()
    {
        char temp[512] = {};
        const char *dir = getcwd(temp, sizeof(temp));
        if (strstr(dir, "bin"))
        {
            chdir("..");
        }
    }
}
