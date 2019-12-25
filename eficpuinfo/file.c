#include "file.h"

EFI_FILE_HANDLE OpenFile(CHAR16 *FileName, UINT64 Mode, UINT64 Attributes)
{
    EFI_STATUS rc;
    EFI_FILE_IO_INTERFACE *FIO;

    rc = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiSimpleFileSystemProtocolGuid, NULL, &FIO);
    if (EFI_ERROR(rc))
    {
        PrintError(L"Openfile::LocateProtocol", rc);
        ExitApp(rc);
    }

    EFI_FILE_PROTOCOL *FileRoot;
    rc = uefi_call_wrapper(FIO->OpenVolume, 2, FIO, &FileRoot);
    if (EFI_ERROR(rc))
    {
        PrintError(L"Openfile::OpenVolume", rc);
        ExitApp(rc);
    }

    EFI_FILE_PROTOCOL *File;
    rc = uefi_call_wrapper(FileRoot->Open, 5, FileRoot, &File, FileName, Mode, Attributes);
    if (EFI_ERROR(rc))
    {
        PrintError(L"Openfile::Open", rc);
        ExitApp(rc);
    }

    rc = CloseFile(FileRoot);
    if (EFI_ERROR(rc))
    {
        PrintError(L"Openfile::CloseVolume", rc);
        ExitApp(rc);
    }

    return File;
}

EFI_STATUS CloseFile(EFI_FILE_HANDLE File)
{
    EFI_STATUS rc;
    rc = uefi_call_wrapper(File->Close, 1, File);
    return rc;
}

EFI_STATUS ReadFile(EFI_FILE_HANDLE File, void *Buffer, UINTN BufferSize)
{
    EFI_STATUS rc;
    rc = uefi_call_wrapper(File->Read, 3, File, &BufferSize, Buffer);
    return rc;
}

EFI_STATUS WriteFile(EFI_FILE_HANDLE File, void *Buffer, UINTN BufferSize)
{
    EFI_STATUS rc;
    rc = uefi_call_wrapper(File->Write, 3, File, &BufferSize, Buffer);
    return rc;
}
