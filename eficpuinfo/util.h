#ifndef _UTIL_H_
#define _UTIL_H_

#include <efi.h>
#include <efilib.h>

#define CapacityOf(x) (sizeof(x) / sizeof((x)[0]))

UINTN StrNCatFmt(CHAR16 *Str, UINTN StrSize, const CHAR16 *fmt, ...);

void PrintError(const CHAR16 *String, EFI_STATUS Status);
void ExitApp(EFI_STATUS Status);

void StringToAscii(CHAR8 *dst, CHAR16 *src);

EFI_STATUS SetWatchdog(UINTN seconds);

#endif
