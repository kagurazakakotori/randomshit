#include "firmware.h"

void QueryFirmwareVendor(CHAR16 *Buffer, UINTN BufferSize)
{
    StrNCatFmt(Buffer, BufferSize, L"Vendor        : %s\n", ST->FirmwareVendor);
}

void QueryFirmwareVersion(CHAR16 *Buffer, UINTN BufferSize)
{
    StrNCatFmt(Buffer, BufferSize, L"Revision      : 0x%x\n", ST->FirmwareRevision);
}

void QueryFirmwareUefiSpecVer(CHAR16 *Buffer, UINTN BufferSize)
{
    UINT32 efiver = ST->Hdr.Revision;
    StrNCatFmt(Buffer, BufferSize, L"UEFI Spec Ver.: %d.%d\n", (efiver >> 16) & 0xffff, efiver & 0xffff);
}
