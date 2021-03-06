
#pragma once

#include <functional>

namespace utils
{
    void RunLoop(float deltaSec, std::function<bool(double, float)> loopFunc);
    void FixWorkingDirectory();
}
