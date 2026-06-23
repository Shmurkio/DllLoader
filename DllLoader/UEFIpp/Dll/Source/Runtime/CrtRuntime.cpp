#include <Dll/Runtime/CrtRuntime.hpp>

#define LogCall() \
    OnFuncCall(__func__)

namespace Dll::Runtime
{
    extern "C" auto RuntimeMalloc(Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return malloc(Size);
    }

    extern "C" auto RuntimeFree(Foundation::Void* Pointer) -> Foundation::Void
    {
        LogCall();
        free(Pointer);
    }

    extern "C" auto RuntimeCalloc(Foundation::Size Count, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return calloc(Count, Size);
    }

    extern "C" auto RuntimeRealloc(Foundation::Void* Pointer, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return realloc(Pointer, Size);
    }

    extern "C" auto RuntimeMemcpy(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return memcpy(Destination, Source, Size);
    }

    extern "C" auto RuntimeMemmove(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return memmove(Destination, Source, Size);
    }

    extern "C" auto RuntimeMemset(Foundation::Void* Destination, Foundation::Int32 Value, Foundation::Size Size) -> Foundation::Void*
    {
        LogCall();
        return memset(Destination, Value, Size);
    }

    extern "C" auto RuntimeMemcmp(const Foundation::Void* Left, const Foundation::Void* Right, Foundation::Size Size) -> Foundation::Int32
    {
        LogCall();
        return memcmp(Left, Right, Size);
    }

    extern "C" auto RuntimeStrlen(const Foundation::Char* String) -> Foundation::Size
    {
        LogCall();
        return strlen(String);
    }

    extern "C" auto RuntimeWcslen(const Foundation::WChar* String) -> Foundation::Size
    {
        LogCall();
        return wcslen(String);
    }

    extern "C" auto RuntimeStrcmp(const Foundation::Char* Left, const Foundation::Char* Right) -> Foundation::Int32
    {
        LogCall();
        return strcmp(Left, Right);
    }

    extern "C" auto RuntimeStrncmp(const Foundation::Char* Left, const Foundation::Char* Right, Foundation::Size Count) -> Foundation::Int32
    {
        LogCall();
        return strncmp(Left, Right, Count);
    }

    extern "C" auto RuntimeWcscmp(const Foundation::WChar* Left, const Foundation::WChar* Right) -> Foundation::Int32
    {
        LogCall();
        return wcscmp(Left, Right);
    }

    extern "C" auto RuntimeWcsncmp(const Foundation::WChar* Left, const Foundation::WChar* Right, Foundation::Size Count) -> Foundation::Int32
    {
        LogCall();
        return wcsncmp(Left, Right, Count);
    }

    extern "C" auto RuntimeStrcpy(Foundation::Char* Destination, const Foundation::Char* Source) -> Foundation::Char*
    {
        LogCall();
        return strcpy(Destination, Source);
    }

    extern "C" auto RuntimeStrncpy(Foundation::Char* Destination, const Foundation::Char* Source, Foundation::Size Count) -> Foundation::Char*
    {
        LogCall();
        return strncpy(Destination, Source, Count);
    }

    extern "C" auto RuntimeWcscpy(Foundation::WChar* Destination, const Foundation::WChar* Source) -> Foundation::WChar*
    {
        LogCall();
        return wcscpy(Destination, Source);
    }

    extern "C" auto RuntimeWcsncpy(Foundation::WChar* Destination, const Foundation::WChar* Source, Foundation::Size Count) -> Foundation::WChar*
    {
        LogCall();
        return wcsncpy(Destination, Source, Count);
    }
}