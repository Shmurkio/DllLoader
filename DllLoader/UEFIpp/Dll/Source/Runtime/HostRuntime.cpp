#include <Dll/Runtime/HostRuntime.hpp>
#include <Dll/Runtime/CrtRuntime.hpp>
#include <Dll/Runtime/CxxRuntime.hpp>
#include <Dll/Runtime/MsvcRuntime.hpp>

namespace Dll::Runtime
{
    static HostContext HostContext_{};

    auto InitializeHostContext(UEFI::Table::System* SystemTable, UEFI::Table::BootServices* BootServices, UEFI::Table::RuntimeServices* RuntimeServices) -> Foundation::Void
    {
        HostContext_.SystemTable = SystemTable;
        HostContext_.BootServices = BootServices;
        HostContext_.RuntimeServices = RuntimeServices;
    }

    extern "C" auto GetHostContext() -> HostContext*
    {
        return &HostContext_;
    }

    auto HostRuntime::Register(Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        RegisterMemoryFunctions(Resolver);
        RegisterStringFunctions(Resolver);
        RegisterCrt(Resolver);
        RegisterCppRuntime(Resolver);
        RegisterMsvcRuntime(Resolver);
		RegisterHostFunctions(Resolver);
    }

    auto HostRuntime::RegisterMemoryFunctions(Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "memcpy", &RuntimeMemcpy);
        Resolver.AddFunction(HostModuleName, "memmove", &RuntimeMemmove);
        Resolver.AddFunction(HostModuleName, "memset", &RuntimeMemset);
        Resolver.AddFunction(HostModuleName, "memcmp", &RuntimeMemcmp);
    }

    auto HostRuntime::RegisterStringFunctions(Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "strlen", &RuntimeStrlen);
        Resolver.AddFunction(HostModuleName, "wcslen", &RuntimeWcslen);
        Resolver.AddFunction(HostModuleName, "strcmp", &RuntimeStrcmp);
        Resolver.AddFunction(HostModuleName, "strncmp", &RuntimeStrncmp);
        Resolver.AddFunction(HostModuleName, "wcscmp", &RuntimeWcscmp);
        Resolver.AddFunction(HostModuleName, "wcsncmp", &RuntimeWcsncmp);
        Resolver.AddFunction(HostModuleName, "strcpy", &RuntimeStrcpy);
        Resolver.AddFunction(HostModuleName, "strncpy", &RuntimeStrncpy);
        Resolver.AddFunction(HostModuleName, "wcscpy", &RuntimeWcscpy);
        Resolver.AddFunction(HostModuleName, "wcsncpy", &RuntimeWcsncpy);
    }

    auto HostRuntime::RegisterCrt(Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "malloc", &RuntimeMalloc);
        Resolver.AddFunction(HostModuleName, "free", &RuntimeFree);
        Resolver.AddFunction(HostModuleName, "calloc", &RuntimeCalloc);
        Resolver.AddFunction(HostModuleName, "realloc", &RuntimeRealloc);
        Resolver.AddFunction(HostModuleName, "ceilf", &RuntimeCeilf);
        Resolver.AddData(HostModuleName, "_fltused", &Fltused);
        Resolver.AddFunction(HostModuleName, "__acrt_iob_func", &RuntimeAcrtIobFunc);
        Resolver.AddFunction(HostModuleName, "__stdio_common_vfprintf", &RuntimeStdioCommonVfprintf);
		Resolver.AddFunction(HostModuleName, "putchar", &RuntimePutchar);
		Resolver.AddFunction(HostModuleName, "puts", &RuntimePuts);
		Resolver.AddFunction(HostModuleName, "printf", &RuntimePrintf);
		Resolver.AddFunction(HostModuleName, "vprintf", &RuntimeVprintf);
		Resolver.AddFunction(HostModuleName, "vfprintf", &RuntimeVfprintf);
    }

    auto HostRuntime::RegisterCppRuntime(Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "terminate", &Terminate);

        Resolver.AddFunction(HostModuleName, "??2@YAPEAX_K@Z", &CxxNew);
        Resolver.AddFunction(HostModuleName, "??3@YAXPEAX@Z", &CxxDelete);
        Resolver.AddFunction(HostModuleName, "??_U@YAPEAX_K@Z", &CxxNewArray);
        Resolver.AddFunction(HostModuleName, "??_V@YAXPEAX@Z", &CxxDeleteArray);

        Resolver.AddFunction(HostModuleName, "??3@YAXPEAX_K@Z", &CxxDeleteSized);
        Resolver.AddFunction(HostModuleName, "??_V@YAXPEAX_K@Z", &CxxDeleteArraySized);

        Resolver.AddFunction(HostModuleName, "??2@YAPEAX_KAEBUnothrow_t@std@@@Z", &CxxNewNoThrow);
        Resolver.AddFunction(HostModuleName, "??_U@YAPEAX_KAEBUnothrow_t@std@@@Z", &CxxNewArrayNoThrow);

        Resolver.AddData(HostModuleName, "?nothrow@std@@3Unothrow_t@1@B", StdNoThrowObject);
    }

    auto HostRuntime::RegisterMsvcRuntime(Loader::ImportResolver& Resolver) -> Foundation::Void
    {
        Resolver.AddFunction(HostModuleName, "__std_exception_destroy", &StdExceptionDestroy);
        Resolver.AddFunction(HostModuleName, "__std_exception_copy", &StdExceptionCopy);
        Resolver.AddFunction(HostModuleName, "_CxxThrowException", &CxxThrowExceptionShim);
        Resolver.AddFunction(HostModuleName, "__CxxFrameHandler4", &CxxFrameHandler4);

        Resolver.AddFunction(HostModuleName, "?_Xlength_error@std@@YAXPEBD@Z", &StdLengthError);
        Resolver.AddFunction(HostModuleName, "?_Xbad_function_call@std@@YAXXZ", &StdBadFunctionCall);
        Resolver.AddFunction(HostModuleName, "__std_type_info_compare", &StdTypeInfoCompare);

        Resolver.AddData(HostModuleName, "??_7type_info@@6B@", TypeInfoVftable);
    }

	auto HostRuntime::RegisterHostFunctions(Loader::ImportResolver& Resolver) -> Foundation::Void
	{
		Resolver.AddFunction(HostModuleName, "GetHostContext", &GetHostContext);
	}
}