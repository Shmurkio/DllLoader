#include <UEFIpp/UEFIpp.hpp>
#include <Dll/Dll.hpp>

static auto LoadStageName(Dll::Loader::LoadStage Stage) -> StringView
{
    switch (Stage)
    {
    case Dll::Loader::LoadStage::Begin: return "Begin";
    case Dll::Loader::LoadStage::AllocateImage: return "AllocateImage";
    case Dll::Loader::LoadStage::CopyHeaders: return "CopyHeaders";
    case Dll::Loader::LoadStage::CopySections: return "CopySections";
    case Dll::Loader::LoadStage::ApplyRelocations: return "ApplyRelocations";
    case Dll::Loader::LoadStage::ResolveImports: return "ResolveImports";
    case Dll::Loader::LoadStage::Finished: return "Finished";
    case Dll::Loader::LoadStage::Unload: return "Unload";
    default: return "Unknown";
    }
}

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
    // Log DLL loader stage changes
    Dll::Loader::OnLoadEvent.Subscribe([](const Dll::Loader::LoadEventInfo& Info)
    {
        Stream::Out::Serial
            << "[DLL Loader] Stage=" << LoadStageName(Info.Stage) << " Base=0x" << Info.Base << " Size=" << Info.Size << Stream::Endl;
    });

    // Log DLL loader failures
    Dll::Loader::OnLoadFailed.Subscribe([](const Dll::Loader::LoadFailedInfo& Info)
    {
        Stream::Out::Serial
            << "[DLL Loader][Failed] Stage=" << LoadStageName(Info.Stage) << ": " << Info.Reason << Stream::Endl;
    });

    // Log resolved imports
    Dll::Loader::OnImportResolved.Subscribe([](const Dll::Loader::ImportResolvedInfo& Info)
    {
        Stream::Out::Serial
            << "[DLL Loader] Resolved import " << Info.Module << "!" << Info.Name  << " -> 0x" << Info.Address << Stream::Endl;
    });

    // Log unresolved/unsupported imports
    Dll::Loader::OnImportFailed.Subscribe([](const Dll::Loader::ImportFailedInfo& Info)
    {
        Stream::Out::Serial
            << "[DLL Loader][Import Failed] " << Info.Module << "!" << Info.Name << Stream::Endl;
    });

    // Log DLL imported function invocations
    Dll::Runtime::OnFuncCall.Subscribe([](StringView FunctionName)
    {
        Stream::Out::Serial
            << "[DLL Runtime] " << FunctionName << Stream::Endl;
    });

    // Log DLL runtime failures and unimplemented paths
    Dll::Runtime::OnUnhandledCall.Subscribe([](const Dll::Runtime::UnhandledCallInfo& Info)
    {
        Stream::Out::Serial
            << "[DLL Runtime][Unhandled] " << Info.FunctionName << ": " << Info.Reason << Stream::Endl;
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

    Dll::Runtime::SetCurrentImageBase(Loader.Base());

    const auto Status = DllMain();

    Loader.Unload();

	Stream::Out::Serial
		<< "DllMain returned: " << Status << Stream::Endl;

    return true;
}