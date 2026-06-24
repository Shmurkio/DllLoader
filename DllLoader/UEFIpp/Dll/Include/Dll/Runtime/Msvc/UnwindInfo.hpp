#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Runtime::Msvc
{
    inline constexpr Foundation::Uint8 UnwindFlagExceptionHandler = 0x01;
    inline constexpr Foundation::Uint8 UnwindFlagUnwindHandler = 0x02;
    inline constexpr Foundation::Uint8 UnwindFlagChainInfo = 0x04;

    struct RuntimeFunction
    {
        Foundation::Uint32 BeginAddress;
        Foundation::Uint32 EndAddress;
        Foundation::Uint32 UnwindInfoAddress;
    };

    struct HandlerInfo
    {
        Foundation::Uint32 HandlerRva;
        Foundation::Uint32 HandlerDataRva;
    };

    struct UnwindInfoHeader
    {
        Foundation::Uint8 VersionAndFlags;
        Foundation::Uint8 PrologSize;
        Foundation::Uint8 CountOfCodes;
        Foundation::Uint8 FrameRegisterAndOffset;

        [[nodiscard]] auto Version() const -> Foundation::Uint8
        {
            return VersionAndFlags & 0x07;
        }

        [[nodiscard]] auto Flags() const -> Foundation::Uint8
        {
            return VersionAndFlags >> 3;
        }

        [[nodiscard]] auto FrameRegister() const -> Foundation::Uint8
        {
            return FrameRegisterAndOffset & 0x0F;
        }

        [[nodiscard]] auto FrameOffset() const -> Foundation::Uint8
        {
            return FrameRegisterAndOffset >> 4;
        }

        [[nodiscard]] auto HasExceptionHandler() const -> Foundation::Bool
        {
            return (Flags() & UnwindFlagExceptionHandler) != 0;
        }

        [[nodiscard]] auto HasUnwindHandler() const -> Foundation::Bool
        {
            return (Flags() & UnwindFlagUnwindHandler) != 0;
        }

        [[nodiscard]] auto HasChainedInfo() const -> Foundation::Bool
        {
            return (Flags() & UnwindFlagChainInfo) != 0;
        }
    };

    auto LookupRuntimeFunction(Foundation::Uint8* Base, Foundation::Uint64 Rip) -> RuntimeFunction*;
    auto HandlerInfoFromUnwindInfo(Foundation::Uint8* UnwindInfo) -> HandlerInfo*;
}