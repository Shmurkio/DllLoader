#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Runtime::Msvc
{
    struct ThrowInfo
    {
        Foundation::Uint32 Attributes;
        Foundation::Int32 PmfnUnwind;
        Foundation::Int32 PForwardCompat;
        Foundation::Int32 PCatchableTypeArray;
    };

    struct CatchableTypeArray
    {
        Foundation::Int32 Count;
        Foundation::Int32 Types[1];
    };

    struct CatchableType
    {
        Foundation::Uint32 Properties;
        Foundation::Int32 TypeDescriptorRva;
        Foundation::Int32 ThisDisplacementMdisp;
        Foundation::Int32 ThisDisplacementPdisp;
        Foundation::Int32 ThisDisplacementVdisp;
        Foundation::Int32 SizeOrOffset;
        Foundation::Int32 CopyFunctionRva;
    };

    auto TypeNameFromDescriptor(Foundation::Uint8* Base, Foundation::Uint32 TypeDescriptorRva) -> const Foundation::Char*;
}