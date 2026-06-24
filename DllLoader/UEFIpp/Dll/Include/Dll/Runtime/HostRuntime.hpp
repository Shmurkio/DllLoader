#pragma once

#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Loader/ImportResolver.hpp>

namespace Dll::Runtime
{
    inline constexpr StringView HostModuleName{ "UEFIppHost.dll" };

    struct HostContext
    {
		UEFI::Table::System* SystemTable{};
		UEFI::Table::BootServices* BootServices{};
		UEFI::Table::RuntimeServices* RuntimeServices{};
    };

    extern "C" auto GetHostContext() -> HostContext*;
    auto InitializeHostContext(UEFI::Table::System* SystemTable,  UEFI::Table::BootServices* BootServices, UEFI::Table::RuntimeServices* RuntimeServices) -> Foundation::Void;

    class HostRuntime
    {
    public:
        static auto Register(Loader::ImportResolver& Resolver) -> Foundation::Void;

    private:
        static auto RegisterMemoryFunctions(Loader::ImportResolver& Resolver) -> Foundation::Void;
        static auto RegisterStringFunctions(Loader::ImportResolver& Resolver) -> Foundation::Void;
        static auto RegisterCrt(Loader::ImportResolver& Resolver) -> Foundation::Void;
        static auto RegisterCppRuntime(Loader::ImportResolver& Resolver) -> Foundation::Void;
        static auto RegisterMsvcRuntime(Loader::ImportResolver& Resolver) -> Foundation::Void;
        static auto RegisterHostFunctions(Loader::ImportResolver& Resolver) -> Foundation::Void;
    };
}