#ifndef _FILE_H_
#define _FILE_H_

#include <efi.h>
#include <efilib.h>
#include "util.h"

EFI_FILE_HANDLE OpenFile(CHAR16 *FileName, UINT64 Mode, UINT64 Attributes);
EFI_STATUS CloseFile(EFI_FILE_HANDLE File);

EFI_STATUS ReadFile(EFI_FILE_HANDLE File, void *Buffer, UINTN BufferSize);
EFI_STATUS WriteFile(EFI_FILE_HANDLE File, void *Buffer, UINTN BufferSize);

#endif
