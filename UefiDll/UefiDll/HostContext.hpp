#pragma once

#include <cstdint>

using EfiStatus = std::uint64_t;
using EfiHandle = void*;
using EfiEvent = void*;
using EfiTpl = std::uint64_t;
using EfiPhysicalAddress = std::uint64_t;
using EfiVirtualAddress = std::uint64_t;

struct EfiSimpleTextOutputProtocol
{
    void* Reset;

    EfiStatus(*OutputString)(EfiSimpleTextOutputProtocol* This, const wchar_t* String);

    void* TestString;
    void* QueryMode;
    void* SetMode;
    void* SetAttribute;
    void* ClearScreen;
    void* SetCursorPosition;
    void* EnableCursor;

    void* Mode;
};

struct EfiTableHeader
{
    std::uint64_t Signature;
    std::uint32_t Revision;
    std::uint32_t HeaderSize;
    std::uint32_t Crc32;
    std::uint32_t Reserved;
};

struct EfiBootServices
{
    EfiTableHeader Header;

    void* RaiseTpl;
    void* RestoreTpl;

    void* AllocatePages;
    void* FreePages;
    void* GetMemoryMap;
    void* AllocatePool;
    void* FreePool;

    void* CreateEvent;
    void* SetTimer;
    void* WaitForEvent;
    void* SignalEvent;
    void* CloseEvent;
    void* CheckEvent;

    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    void* HandleProtocol;
    void* Reserved;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;

    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;

    void* GetNextMonotonicCount;
    void* Stall;
    void* SetWatchdogTimer;

    void* ConnectController;
    void* DisconnectController;

    void* OpenProtocol;
    void* CloseProtocol;
    void* OpenProtocolInformation;

    void* ProtocolsPerHandle;
    void* LocateHandleBuffer;
    void* LocateProtocol;
    void* InstallMultipleProtocolInterfaces;
    void* UninstallMultipleProtocolInterfaces;

    void* CalculateCrc32;

    void* CopyMem;
    void* SetMem;

    void* CreateEventEx;
};

struct EfiRuntimeServices
{
    EfiTableHeader Header;

    void* GetTime;
    void* SetTime;
    void* GetWakeupTime;
    void* SetWakeupTime;

    void* SetVirtualAddressMap;
    void* ConvertPointer;

    void* GetVariable;
    void* GetNextVariableName;
    void* SetVariable;

    void* GetNextHighMonotonicCount;
    void* ResetSystem;

    void* UpdateCapsule;
    void* QueryCapsuleCapabilities;

    void* QueryVariableInfo;
};

struct EfiSystemTable
{
    EfiTableHeader Header;

    void* FirmwareVendor;
    unsigned int FirmwareRevision;

    void* ConsoleInHandle;
    void* ConIn;

    void* ConsoleOutHandle;
    EfiSimpleTextOutputProtocol* ConOut;

    void* StandardErrorHandle;
    void* StdErr;

    EfiRuntimeServices* RuntimeServices;
    EfiBootServices* BootServices;

    unsigned long long NumberOfTableEntries;
    void* ConfigurationTable;
};

struct HostContext
{
    EfiSystemTable* SystemTable{};
    EfiBootServices* BootServices{};
    EfiRuntimeServices* RuntimeServices{};
};

extern "C" auto GetHostContext() -> HostContext*;