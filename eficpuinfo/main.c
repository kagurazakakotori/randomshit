#include "main.h"

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab)
{
    InitializeLib(image_handle, systab);
    ClearScreen();
    
    SetWatchdog(10);

    // Prepare Global Buffer
    ZeroMem(GlobalBuffer, sizeof(GlobalBuffer));
    QueryFirmwareInfo(GlobalBuffer, sizeof(GlobalBuffer));
    QueryCPUInfo(GlobalBuffer, sizeof(GlobalBuffer));

    Print(L"%s\n", GlobalBuffer);

    SetWatchdog(0);
    PromptSave();

    // Set watchdog back to 5 min
    SetWatchdog(300);
    Print(L"\n");

    return EFI_SUCCESS;
}

void ClearScreen()
{
    EFI_STATUS rc;
    rc = uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    if (EFI_ERROR(rc))
    {
        PrintError(L"ClearScreen", rc);
        ExitApp(rc);
    }
}

void PromptSave()
{
    CHAR16 Buffer[128];

    Input(L"Save to file?[y/N] ", Buffer, CapacityOf(Buffer));
    Print(L"\n");
    if (Buffer[0] == L'Y' || Buffer[0] == L'y')
    {
        // Prompt file name
        Input(L"Filename: [cpuinfo.txt] ", Buffer, sizeof(Buffer));
        Print(L"\n");

        EFI_STATUS rc;
        CHAR16 *FileName = (StrLen(Buffer) == 0) ? L"cpuinfo.txt" : Buffer;

        rc = SaveToFile(FileName);
        if (rc == EFI_SUCCESS)
        {
            Print(L"Saved to file %s\n", FileName);
        }
    }
}

EFI_STATUS SaveToFile(CHAR16 *FileName)
{
    EFI_STATUS rc;
    EFI_FILE_HANDLE File = OpenFile(FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_ARCHIVE);

    CHAR8 OutputBuffer[GLOBAL_BUFFER_CAPACITY];
    StringToAscii(OutputBuffer, GlobalBuffer);

    rc = WriteFile(File, OutputBuffer, strlena(OutputBuffer));
    if (EFI_ERROR(rc))
    {
        PrintError(L"WriteFile", rc);
        ExitApp(rc);
    }

    rc = CloseFile(File);
    if (EFI_ERROR(rc))
    {
        PrintError(L"CloseFile", rc);
        ExitApp(rc);
    }

    return EFI_SUCCESS;
}

void QueryFirmwareInfo(CHAR16 *Buffer, UINTN BufferSize)
{
    StrNCatFmt(Buffer, BufferSize, L"********  Firmware Information  ********\n");
    QueryFirmwareVendor(Buffer, BufferSize);
    QueryFirmwareVersion(Buffer, BufferSize);
    QueryFirmwareUefiSpecVer(Buffer, BufferSize);
    StrNCatFmt(Buffer, BufferSize, L"\n");
}

void QueryCPUInfo(CHAR16 *Buffer, UINTN BufferSize)
{
    StrNCatFmt(Buffer, BufferSize, L"********    CPU Information     ********\n");
    QueryCPUVendor(Buffer, BufferSize);
    QueryCPUName(Buffer, BufferSize);
    QueryCPUVersion(Buffer, BufferSize);
    QueryCPUInstructionSet(Buffer, BufferSize);
    StrNCatFmt(Buffer, BufferSize, L"\n");
}
