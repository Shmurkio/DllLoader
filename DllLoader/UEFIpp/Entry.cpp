#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Dll.hpp>

static constexpr const char* TestNames[]
{
    "Success",
    "TestCrt",
    "TestNewDelete",
    "TestStringVectorAlgorithm",
    "TestBasicStlTypes",
    "TestSharedWeakPtr",
    "TestVariantAny",
    "TestDequeList",
    "TestAssociativeContainers"
};

/*
UEFIppHost.lib / UEFIppHost.def

Requirements:
- UefiDll\Lib must contain:
    * UEFIppHost.def
    * UEFIppHost.lib

If UEFIppHost.lib is missing, the DLL cannot import the runtime symbols
required by the MSVC STL, CRT, and C++ runtime support layers.

To generate UEFIppHost.lib:

    lib /def:UEFIppHost.def /machine:x64 /out:UEFIppHost.lib

Import Resolution:
- Every symbol exported by UEFIppHost.def must be implemented and registered
  by DllLoader through the ImportResolver.
- Missing symbols will cause DLL loading to fail.
- The loader prints the unresolved import name to the console to aid debugging.

Extending the Runtime:
- Additional CRT, STL, or custom host functions can be exposed by adding
  entries to UEFIppHost.def and registering implementations in HostRuntime.

Test DLL:
- Check DllMain.cpp in the UefiDll project for the C++ test code executed by
  the loaded DLL.
*/
auto Main([[maybe_unused]] const Vector<String>& Args) -> Foundation::Bool
{
    Dll::Runtime::OnFuncCall.Subscribe([](StringView FunctionName)
    {
        Stream::Out::Serial
            << "[DLL Runtime] " << FunctionName << Stream::Endl;
    });

    Dll::Loader::ImportResolver Resolver{};
    Dll::Runtime::HostRuntime::Register(Resolver);

    Stream::FileInputStream DllFile{ "UefiDll.dll" };

    Dll::Loader::PeLoader Loader{};

    if (!Loader.Load(DllFile.Buffer(), Resolver))
    {
        Stream::Out::Serial
            << "Failed to load DLL" << Stream::Endl;
        return false;
    }

    Stream::Out::Serial
        << "DLL loaded at: 0x" << Loader.Base() << " (" << Loader.Size() << " bytes)" << Stream::Endl;

    using DllMainFn = auto (*)() -> int;
    auto DllMain = Foundation::Cast::Auto<DllMainFn>(Loader.Export("DllMain"));

    if (!DllMain)
    {
        Stream::Out::Serial
            << "DllMain export not found" << Stream::Endl;
        return false;
    }

    const auto Status = DllMain();

    if (Status == 0)
    {
        Stream::Out::Serial
            << "========================================" << Stream::Endl
            << "DLL runtime validation successful" << Stream::Endl
            << "========================================" << Stream::Endl
            << "Working features:" << Stream::Endl
            << Stream::Endl

            << "[CRT]" << Stream::Endl
            << "  malloc / free" << Stream::Endl
            << "  calloc / realloc" << Stream::Endl
            << "  memcpy / memmove / memset / memcmp" << Stream::Endl
            << "  strlen / wcslen" << Stream::Endl
            << "  strcmp / strncmp" << Stream::Endl
            << "  wcscmp / wcsncmp" << Stream::Endl
            << "  strcpy / strncpy" << Stream::Endl
            << "  wcscpy / wcsncpy" << Stream::Endl
            << Stream::Endl

            << "[C++ Runtime]" << Stream::Endl
            << "  operator new / delete" << Stream::Endl
            << "  operator new[] / delete[]" << Stream::Endl
            << "  std::nothrow new" << Stream::Endl
            << "  sized delete" << Stream::Endl
            << Stream::Endl

            << "[STL]" << Stream::Endl
            << "  std::string" << Stream::Endl
            << "  std::vector" << Stream::Endl
            << "  std::unique_ptr" << Stream::Endl
            << "  std::shared_ptr" << Stream::Endl
            << "  std::weak_ptr" << Stream::Endl
            << "  std::optional" << Stream::Endl
            << "  std::variant" << Stream::Endl
            << "  std::any" << Stream::Endl
            << "  std::function" << Stream::Endl
            << "  std::array" << Stream::Endl
            << "  std::pair" << Stream::Endl
            << "  std::tuple" << Stream::Endl
            << "  std::deque" << Stream::Endl
            << "  std::list" << Stream::Endl
            << "  std::set" << Stream::Endl
            << "  std::unordered_set" << Stream::Endl
            << "  std::map" << Stream::Endl
            << "  std::unordered_map" << Stream::Endl
            << "  std::sort" << Stream::Endl
            << Stream::Endl

            << "[MSVC Runtime]" << Stream::Endl
            << "  std::type_info helpers" << Stream::Endl
            << "  std::exception construction/destruction stubs" << Stream::Endl
            << "  STL throw-path stubs" << Stream::Endl
            << "  _CxxThrowException -> terminate stub" << Stream::Endl;
    }
	else if (Status > 0 && Status < sizeof(TestNames) / sizeof(TestNames[0]))
	{
		Stream::Out::Serial
			<< "DLL runtime validation failed in test: " << TestNames[Status] << Stream::Endl;
	}
	else
	{
		Stream::Out::Serial
			<< "DLL runtime validation failed with unknown error code: " << Status << Stream::Endl;
	}

    Loader.Unload();

    return Status == 0;
}