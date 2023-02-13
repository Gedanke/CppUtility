#pragma once

#include <string>
#include <stdexcept>

namespace civitasv
{
    namespace json
    {
        inline void error(const std::string &msg)
        {
            throw std::logic_error(msg);
        }
    }
}