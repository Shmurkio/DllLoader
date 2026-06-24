#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Dll.hpp>

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
	(or run BuildLib.bat if the vcvars64.bat path matches your Visual Studio installation)

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
    Dll::Runtime::OnTrace.Subscribe(
    [](const Dll::Runtime::TraceInfo& Info)
    {
        Stream::Out::Serial
            << "[DLL " << Dll::Runtime::TraceCategoryName(Info.Category) << "][" << Dll::Runtime::TraceLevelName(Info.Level) << "] ";

        if (!Info.FunctionName.Empty())
        {
            Stream::Out::Serial
                << Info.FunctionName << ":" << Info.LineNumber << " ";
        }

        Stream::Out::Serial << Info.Message;

        if (!Info.Module.Empty() || !Info.Symbol.Empty())
        {
            Stream::Out::Serial
                << " " << Info.Module << "!" << Info.Symbol;
        }

        if (Info.Address)
        {
            Stream::Out::Serial
                << " -> 0x" << Stream::Hexadecimal << Info.Address << Stream::Decimal;
        }

        if (Info.Rip)
        {
            Stream::Out::Serial
                << " RIP=0x" << Stream::Hexadecimal << Info.Rip;

            if (Info.Rva)
            {
                Stream::Out::Serial
                    << " RVA=0x" << Info.Rva;
            }

            if (!Info.SectionName.Empty())
            {
                Stream::Out::Serial
                    << " Section=" << Info.SectionName;
            }

            Stream::Out::Serial << Stream::Decimal;
        }

        Stream::Out::Serial << Stream::Endl;
    });

    Dll::Loader::ImportResolver Resolver{};

    Dll::Runtime::InitializeHostContext(&UEFI::Context::SystemTable(), &UEFI::Context::BootServices(), &UEFI::Context::RuntimeServices());

    Dll::Runtime::HostRuntime::Register(Resolver);

    Stream::FileInputStream DllFile{ "UefiDll.dll" };

    Dll::Loader::PeLoader Loader{};

    if (!Loader.Load(DllFile.Buffer(), Resolver))
    {
        Stream::Out::Serial
            << "Failed to load DLL" << Stream::Endl;
        return false;
    }

    Dll::Runtime::SetTraceImageContext(Loader.Base(), Loader.Size());
    Dll::Runtime::SetCurrentImageBase(Loader.Base());

    Stream::Out::Serial
        << "DLL loaded at: 0x" << Loader.Base() << " (" << Loader.Size() << " bytes)" << Stream::Endl;

    using DllMainFn = auto (*)() -> int;
    auto DllMain = Foundation::Cast::Auto<DllMainFn>(Loader.Export("DllMain"));

    if (!DllMain)
    {
        Stream::Out::Serial
            << "DllMain export not found" << Stream::Endl;

        Dll::Runtime::ClearTraceImageContext();
        Loader.Unload();
        return false;
    }

    const auto Status = DllMain();

    Dll::Runtime::ClearTraceImageContext();
    Loader.Unload();

    Stream::Out::Serial
        << "DllMain returned: " << Status << Stream::Endl;

    return true;
}