#include "cpu.h"

#define EAX_VENDOR_STR 0x0
#define EAX_PROCESSOR_INFO 0x1
#define EAX_BASIC_FEATURE 0x1
#define EAX_CACHE 0x4
#define EAX_EXTENDED_FEATURE 0x7
#define EAX_VENDOR_FEATURE 0x80000001
#define EAX_MODEL_STR_1 0x80000002
#define EAX_MODEL_STR_2 0x80000003
#define EAX_MODEL_STR_3 0x80000004

#define ECX_DEFAULT 0
#define ECX_EXTENDED 1 // only used when EAX == 07h

#define VCPU_BIT 0x80000000
#define MMX_BIT 0x00800000
#define SSE_BIT 0x02000000
#define SSE2_BIT 0x04000000
#define SSE3_BIT 0x00000001
#define VMX_BIT 0x00000020
#define SSSE3_BIT 0x00000200
#define FMA3_BIT 0x00001000
#define SSE41_BIT 0x00080000
#define SSE42_BIT 0x00100000
#define AES_BIT 0x02000000
#define AVX_BIT 0x10000000
#define SGX_BIT 0x00000004
#define AVX2_BIT 0x00000020
#define AVX512F_BIT 0x00010000
#define SHA_BIT 0x20000000
#define EM64T_BIT 0x20000000
#define A3DNOW_BIT 0x08000000
#define SSE4A_BIT 0x00000040
#define XOP_BIT 0x00000800
#define FMA4_BIT 0x00010000

static UINT32 Eax;
static UINT32 Ebx;
static UINT32 Ecx;
static UINT32 Edx;

static void cpuid(uint32_t eax, uint32_t ecx)
{
    asm volatile(
        "cpuid"
        : "=a"(Eax), "=b"(Ebx), "=c"(Ecx), "=d"(Edx)
        : "a"(eax), "c"(ecx));

#ifdef EFI_DEBUG
    Print(L"Eax: 0x%08x , Ebx: 0x%08x, Ecx: 0x%08x, Edx: 0x%08x\n", Eax, Ebx, Ecx, Edx);
#endif
}

void QueryCPUVendor(CHAR16 *Buffer, UINTN BufferSize)
{
    CHAR8 Vendor[13];

    cpuid(EAX_VENDOR_STR, ECX_DEFAULT);
    *(UINT32 *)Vendor = Ebx;
    *(UINT32 *)(Vendor + 4) = Edx;
    *(UINT32 *)(Vendor + 8) = Ecx;
    Vendor[12] = '\0';

    StrNCatFmt(Buffer, BufferSize, L"Vendor        : %a\n", Vendor);
}

void QueryCPUName(CHAR16 *Buffer, UINTN BufferSize)
{
    CHAR8 Name[49] = {'\0'};

    cpuid(EAX_MODEL_STR_1, ECX_DEFAULT);
    *(UINT32 *)Name = Eax;
    *(UINT32 *)(Name + 4) = Ebx;
    *(UINT32 *)(Name + 8) = Ecx;
    *(UINT32 *)(Name + 12) = Edx;

    cpuid(EAX_MODEL_STR_2, ECX_DEFAULT);
    *(UINT32 *)(Name + 16) = Eax;
    *(UINT32 *)(Name + 20) = Ebx;
    *(UINT32 *)(Name + 24) = Ecx;
    *(UINT32 *)(Name + 28) = Edx;

    cpuid(EAX_MODEL_STR_3, ECX_DEFAULT);
    *(UINT32 *)(Name + 32) = Eax;
    *(UINT32 *)(Name + 36) = Ebx;
    *(UINT32 *)(Name + 40) = Ecx;
    *(UINT32 *)(Name + 44) = Edx;

    Name[48] = '\0';
    StrNCatFmt(Buffer, BufferSize, L"Name          : %a\n", Name);
}

void QueryCPUVersion(CHAR16 *Buffer, UINTN BufferSize)
{
    cpuid(EAX_PROCESSOR_INFO, ECX_DEFAULT);
    UINT32 family = (Eax >> 8) & 0x0f;
    UINT32 model = (Eax >> 4) & 0x0f;
    UINT32 stepping = Eax & 0x0f;
    // CPU-Z uses Ext.Base for Ext. Family and Ext. Model
    // For example, Ext = 8, Base = E, then 8E is displayed
    UINT32 ext_family = (Eax & 0x0ff00000) >> 16 | family;
    UINT32 ext_model = (Eax & 0x000f0000) >> 12 | model;
    CHAR16 is_vcpu = (Ecx & VCPU_BIT) ? 'Y' : 'N';

    StrNCatFmt(Buffer, BufferSize, L"Family        : %x\n", family);
    StrNCatFmt(Buffer, BufferSize, L"Ext. Family   : %x\n", ext_family);
    StrNCatFmt(Buffer, BufferSize, L"Model         : %x\n", model);
    StrNCatFmt(Buffer, BufferSize, L"Ext. Model    : %x\n", ext_model);
    StrNCatFmt(Buffer, BufferSize, L"Stepping      : %x\n", stepping);
    StrNCatFmt(Buffer, BufferSize, L"Virtual CPU   : %c\n", is_vcpu);
}

void QueryCPUInstructionSet(CHAR16 *Buffer, UINTN BufferSize)
{
    StrNCatFmt(Buffer, BufferSize, L"Instructions  :");

    cpuid(EAX_BASIC_FEATURE, ECX_DEFAULT);
    if (Edx & MMX_BIT)
        StrNCatFmt(Buffer, BufferSize, L" MMX");
    if (Edx & SSE_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSE");
    if (Edx & SSE2_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSE2");
    if (Ecx & SSE3_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSE3");
    if (Ecx & SSSE3_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSSE3");
    if (Ecx & SSE41_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSE4.1");
    if (Ecx & SSE42_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSE4.2");
    if (Ecx & VMX_BIT)
        StrNCatFmt(Buffer, BufferSize, L" VMX");
    if (Ecx & AES_BIT)
        StrNCatFmt(Buffer, BufferSize, L" AES");
    if (Ecx & FMA3_BIT)
        StrNCatFmt(Buffer, BufferSize, L" FMA3");
    if (Ecx & AVX_BIT)
        StrNCatFmt(Buffer, BufferSize, L" AVX");

    cpuid(EAX_EXTENDED_FEATURE, ECX_DEFAULT);
    if (Ebx & AVX2_BIT)
        StrNCatFmt(Buffer, BufferSize, L" AVX2");
    if (Ebx & AVX512F_BIT)
        StrNCatFmt(Buffer, BufferSize, L" AVX512F");
    if (Ebx & SHA_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SHA");
    if (Ebx & SGX_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SGX");

    cpuid(EAX_VENDOR_FEATURE, ECX_DEFAULT);
    if (Edx & EM64T_BIT)
        StrNCatFmt(Buffer, BufferSize, L" EM64T");
    if (Ecx & SSE4A_BIT)
        StrNCatFmt(Buffer, BufferSize, L" SSE4A");
    if (Ecx & XOP_BIT)
        StrNCatFmt(Buffer, BufferSize, L" XOP");
    if (Ecx & FMA4_BIT)
        StrNCatFmt(Buffer, BufferSize, L" FMA4");

    StrNCatFmt(Buffer, BufferSize, L"\n");
}
