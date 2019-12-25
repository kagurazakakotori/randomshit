#ifndef _CPU_H_
#define _CPU_H_

#include <efi.h>
#include <efilib.h>
#include "util.h"

void QueryCPUVendor(CHAR16 *Buffer, UINTN BufferSize);
void QueryCPUName(CHAR16 *Buffer, UINTN BufferSize);
void QueryCPUVersion(CHAR16 *Buffer, UINTN BufferSize);
void QueryCPUInstructionSet(CHAR16 *Buffer, UINTN BufferSize);

#endif
