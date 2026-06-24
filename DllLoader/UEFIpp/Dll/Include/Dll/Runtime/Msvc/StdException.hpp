#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Runtime::Msvc
{
    struct StdException
    {
        UEFIpp::Foundation::Char* Message;
        UEFIpp::Foundation::Bool DoFree;
    };
}