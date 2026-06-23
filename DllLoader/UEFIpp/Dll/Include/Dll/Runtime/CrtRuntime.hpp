#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace Dll::Runtime
{
    extern Event<StringView> OnFuncCall;

    extern "C"
    {
        auto RuntimeMalloc(Foundation::Size Size) -> Foundation::Void*;
        auto RuntimeFree(Foundation::Void* Pointer) -> Foundation::Void;
        auto RuntimeCalloc(Foundation::Size Count, Foundation::Size Size) -> Foundation::Void*;
        auto RuntimeRealloc(Foundation::Void* Pointer, Foundation::Size Size) -> Foundation::Void*;

        auto RuntimeMemcpy(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Void*;
        auto RuntimeMemmove(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Void*;

        auto RuntimeMemset(Foundation::Void* Destination, Foundation::Int32 Value, Foundation::Size Size) -> Foundation::Void*;

        auto RuntimeMemcmp(const Foundation::Void* Left, const Foundation::Void* Right, Foundation::Size Size) -> Foundation::Int32;

        auto RuntimeStrlen(const Foundation::Char* String) -> Foundation::Size;
        auto RuntimeWcslen(const Foundation::WChar* String) -> Foundation::Size;

        auto RuntimeStrcmp(const Foundation::Char* Left, const Foundation::Char* Right) -> Foundation::Int32;

        auto RuntimeStrncmp(const Foundation::Char* Left, const Foundation::Char* Right, Foundation::Size Count) -> Foundation::Int32;

        auto RuntimeWcscmp(const Foundation::WChar* Left, const Foundation::WChar* Right) -> Foundation::Int32;

        auto RuntimeWcsncmp( const Foundation::WChar* Left, const Foundation::WChar* Right, Foundation::Size Count) -> Foundation::Int32;

        auto RuntimeStrcpy(Foundation::Char* Destination, const Foundation::Char* Source) -> Foundation::Char*;

        auto RuntimeStrncpy(Foundation::Char* Destination, const Foundation::Char* Source, Foundation::Size Count) -> Foundation::Char*;

        auto RuntimeWcscpy(Foundation::WChar* Destination, const Foundation::WChar* Source) -> Foundation::WChar*;

        auto RuntimeWcsncpy(Foundation::WChar* Destination, const Foundation::WChar* Source, Foundation::Size Count) -> Foundation::WChar*;
    }
}