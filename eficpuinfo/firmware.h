#ifndef _FIRMWARE_H_
#define _FIRMWARE_H_

#include <efi.h>
#include <efilib.h>
#include "util.h"

void QueryFirmwareVendor(CHAR16 *Buffer, UINTN BufferSize);
void QueryFirmwareVersion(CHAR16 *Buffer, UINTN BufferSize);
void QueryFirmwareUefiSpecVer(CHAR16 *Buffer, UINTN BufferSize);

#endif
