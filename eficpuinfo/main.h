#ifndef _MAIN_H_
#define _MAIN_H_

#include <efi.h>
#include <efilib.h>
#include "firmware.h"
#include "cpu.h"
#include "file.h"
#include "util.h"

#define GLOBAL_BUFFER_CAPACITY 4096

CHAR16 GlobalBuffer[GLOBAL_BUFFER_CAPACITY];

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab);

void ClearScreen();
void PromptSave();

EFI_STATUS SaveToFile(CHAR16 *FileName);

void QueryFirmwareInfo(CHAR16 *Buffer, UINTN BufferSize);
void QueryCPUInfo(CHAR16 *Buffer, UINTN BufferSize);

#endif
