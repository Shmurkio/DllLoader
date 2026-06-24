#pragma once

#include <UEFIpp/UEFIpp.hpp>
#include <cstdarg>

namespace Dll::Runtime
{
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
        auto RuntimeWcsncmp(const Foundation::WChar* Left, const Foundation::WChar* Right, Foundation::Size Count) -> Foundation::Int32;

        auto RuntimeStrcpy(Foundation::Char* Destination, const Foundation::Char* Source) -> Foundation::Char*;
        auto RuntimeStrncpy(Foundation::Char* Destination, const Foundation::Char* Source, Foundation::Size Count) -> Foundation::Char*;

        auto RuntimeWcscpy(Foundation::WChar* Destination, const Foundation::WChar* Source) -> Foundation::WChar*;
        auto RuntimeWcsncpy(Foundation::WChar* Destination, const Foundation::WChar* Source, Foundation::Size Count) -> Foundation::WChar*;

        auto RuntimeCeilf(Foundation::Float Value) -> Foundation::Float;
        extern Foundation::Int32 Fltused;

		auto RuntimeAcrtIobFunc(Foundation::Int32 Index) -> Foundation::Void*;
		auto RuntimeStdioCommonVfprintf(Foundation::Uint64 Options, Foundation::Void* Stream, const Foundation::Char* Format, Foundation::Void* Locale, va_list Args) -> Foundation::Int32;
        auto RuntimePutchar(Foundation::Int32 Character) -> Foundation::Int32;
		auto RuntimePuts(const Foundation::Char* String) -> Foundation::Int32;
		auto RuntimePrintf(const Foundation::Char* Format, ...) -> Foundation::Int32;
        auto RuntimeVprintf(const Foundation::Char* Format, va_list Args) -> Foundation::Int32;
		auto RuntimeVfprintf(Foundation::Void* File, const Foundation::Char* Format, va_list Args) -> Foundation::Int32;
    }
}