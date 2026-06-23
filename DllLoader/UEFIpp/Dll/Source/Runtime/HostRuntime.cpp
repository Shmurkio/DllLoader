#include <Dll/Runtime/HostRuntime.hpp>
#include <Dll/Runtime/CrtRuntime.hpp>
#include <Dll/Runtime/CxxRuntime.hpp>
#include <Dll/Runtime/MsvcRuntime.hpp>

namespace Dll::Runtime
{
    auto HostRuntime::Register(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        RegisterMemoryFunctions(Resolver);
        RegisterStringFunctions(Resolver);
        RegisterCrt(Resolver);
        RegisterCppRuntime(Resolver);
		RegisterMsvcRuntime(Resolver);
    }

    auto HostRuntime::RegisterMemoryFunctions(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "memcpy", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeMemcpy));
        Resolver.AddFunction(HostModuleName, "memmove", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeMemmove));
        Resolver.AddFunction(HostModuleName, "memset", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeMemset));
        Resolver.AddFunction(HostModuleName, "memcmp", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeMemcmp));
    }

    auto HostRuntime::RegisterStringFunctions(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "strlen", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeStrlen));
        Resolver.AddFunction(HostModuleName, "wcslen", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeWcslen));

        Resolver.AddFunction(HostModuleName, "strcmp", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeStrcmp));
        Resolver.AddFunction(HostModuleName, "strncmp", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeStrncmp));

        Resolver.AddFunction(HostModuleName, "wcscmp", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeWcscmp));
        Resolver.AddFunction(HostModuleName, "wcsncmp", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeWcsncmp));

        Resolver.AddFunction(HostModuleName, "strcpy", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeStrcpy));
        Resolver.AddFunction(HostModuleName, "strncpy", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeStrncpy));

        Resolver.AddFunction(HostModuleName, "wcscpy", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeWcscpy));
        Resolver.AddFunction(HostModuleName, "wcsncpy", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeWcsncpy));
    }

    auto HostRuntime::RegisterCrt(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "malloc", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeMalloc));
        Resolver.AddFunction(HostModuleName, "free", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeFree));
        Resolver.AddFunction(HostModuleName, "calloc", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeCalloc));
        Resolver.AddFunction(HostModuleName, "realloc", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeRealloc));

        Resolver.AddFunction(HostModuleName, "terminate", Foundation::Cast::Auto<Foundation::Void*>(&Terminate));
    }

    auto HostRuntime::RegisterCppRuntime(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "??2@YAPEAX_K@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxNew));
        Resolver.AddFunction(HostModuleName, "??3@YAXPEAX@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxDelete));

        Resolver.AddFunction(HostModuleName, "??_U@YAPEAX_K@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxNewArray));
        Resolver.AddFunction(HostModuleName, "??_V@YAXPEAX@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxDeleteArray));

        Resolver.AddFunction(HostModuleName, "??2@YAPEAX_KAEBUnothrow_t@std@@@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxNewNoThrow));
        Resolver.AddFunction(HostModuleName, "??_U@YAPEAX_KAEBUnothrow_t@std@@@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxNewArrayNoThrow));

        Resolver.AddFunction(HostModuleName, "??3@YAXPEAX_K@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxDeleteSized));
        Resolver.AddFunction(HostModuleName, "??_V@YAXPEAX_K@Z", Foundation::Cast::Auto<Foundation::Void*>(&CxxDeleteArraySized));

        Resolver.AddData(HostModuleName, "?nothrow@std@@3Unothrow_t@1@B", StdNoThrowObject);
    }

    auto HostRuntime::RegisterMsvcRuntime(Dll::Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "__std_exception_destroy", Foundation::Cast::Auto<Foundation::Void*>(&__std_exception_destroy));
        Resolver.AddFunction(HostModuleName, "__std_exception_copy", Foundation::Cast::Auto<Foundation::Void*>(&__std_exception_copy));
        Resolver.AddFunction(HostModuleName, "_CxxThrowException", Foundation::Cast::Auto<Foundation::Void*>(&CxxThrowException));
        Resolver.AddFunction(HostModuleName, "?_Xlength_error@std@@YAXPEBD@Z", Foundation::Cast::Auto<Foundation::Void*>(&StdLengthError));
        Resolver.AddData(HostModuleName, "??_7type_info@@6B@", TypeInfoVftable);
        Resolver.AddFunction(HostModuleName, "?_Xbad_function_call@std@@YAXXZ", Foundation::Cast::Auto<Foundation::Void*>(&StdBadFunctionCall));
		Resolver.AddData(HostModuleName, "_fltused", Foundation::Cast::Auto<Foundation::Void*>(&Fltused));
		Resolver.AddFunction(HostModuleName, "ceilf", Foundation::Cast::Auto<Foundation::Void*>(&RuntimeCeilf));
		Resolver.AddFunction(HostModuleName, "__std_type_info_compare", Foundation::Cast::Auto<Foundation::Void*>(&StdTypeInfoCompare));
    }
}