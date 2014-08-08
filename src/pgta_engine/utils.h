
#pragma once

#include <functional>

namespace utils
{
    void RunLoop(float msTime, std::function<bool()> loopFunc);
}
