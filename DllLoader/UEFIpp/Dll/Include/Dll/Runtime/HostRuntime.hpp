#pragma once

#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Loader/ImportResolver.hpp>

namespace Dll::Runtime
{
    inline constexpr StringView HostModuleName
    {
        "UEFIppHost.dll"
    };

    class HostRuntime
    {
    public:
        static auto Register(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void;

        static auto RegisterCrt(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void;

        static auto RegisterCppRuntime(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void;

        static auto RegisterStringFunctions(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void;

        static auto RegisterMemoryFunctions(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void;

		static auto RegisterMsvcRuntime(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void;
    };
}