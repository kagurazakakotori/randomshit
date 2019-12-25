#include "util.h"

UINTN StrNCatFmt(CHAR16 *Str, UINTN StrSize, const CHAR16 *fmt, ...)
{
    UINTN ret;
    va_list args;
    va_start(args, fmt);
    ret = VSPrint(Str + StrLen(Str), StrSize, fmt, args);
    va_end(args);
    return ret;
}

void PrintError(const CHAR16 *String, EFI_STATUS Status)
{
    CHAR16 Buffer[64];
    StatusToString(Buffer, Status);
    Print(L"%s: %s\n", String, Buffer);
}

void ExitApp(EFI_STATUS Status)
{
    Exit(Status, 0, NULL);
}

void StringToAscii(CHAR8 *dst, CHAR16 *src)
{
    UINTN length = StrLen(src);
    for (UINTN i = 0; i < length; i++)
    {
        dst[i] = (CHAR8)(src[i] & 0xff);
    }
}

EFI_STATUS SetWatchdog(UINTN seconds)
{
    EFI_STATUS rc;
    rc = uefi_call_wrapper(BS->SetWatchdogTimer, 4, seconds, 0x1ffff, 0, NULL);
    if (EFI_ERROR(rc))
    {
        PrintError(L"SetWatchdog", rc);
    }
    return rc;
}
