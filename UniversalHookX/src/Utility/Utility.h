#pragma once
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <TlHelp32.h>
#include <vector>
#include <memory>
#pragma comment(lib, "ntdll.lib")
extern "C" NTSTATUS NTAPI NtReadVirtualMemory(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    SIZE_T BufferSize,
    PSIZE_T NumberOfBytesRead
);
EXTERN_C NTSTATUS NTAPI NtWriteVirtualMemory(HANDLE, PVOID, PVOID, ULONG, PULONG);
EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
EXTERN_C NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);
typedef struct _MEMORY_REGION {
    DWORD_PTR dwBaseAddr;
    DWORD_PTR dwMemorySize;
} MEMORY_REGION, * PMEMORY_REGION;
namespace Utility {
    extern HANDLE GameHandle;
    extern INT GamePID;
    extern DWORD BaseAddv;
    INT GetTrueProcessId();
    VOID ChangeClickability(BOOL CanClick, HWND hWindow);
    BOOL NormalMemSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet);
    BOOL MemSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet);
    BOOL MemSearchSlow(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet);
    DWORD GetViewMatrixBase(DWORD StartAddress = 0x26000000, DWORD EndAddress = 0x90000000);
    DWORD GetUE4(DWORD StartAddress = 0x25000000, DWORD EndAddress = 0x60000000);
    DWORD GetModule(DWORD StartAddress = 0x26000000, DWORD EndAddress = 0xB0000000);
    std::string GameVersion();
    size_t ReadStringA(HANDLE hProc, void* pAddress, char* pOut, size_t MaxLength);
    bool ScanPatternNew4Bytes(const std::string& pattern, DWORD Byte, int ScanMod);
    bool ScanHelmet(DWORD Search, DWORD Byte, DWORD ByteCheck, DWORD ByteVerify);

    template <typename T>
    T ReadMemoryEx(DWORD BaseAddress) {
        if (!GameHandle || BaseAddress == 0) return T();
        T Buffer;
        NTSTATUS status = NtReadVirtualMemory(GameHandle, (PVOID)BaseAddress, &Buffer, sizeof(Buffer), nullptr);
        if (!NT_SUCCESS(status)) return T();
        return Buffer;
    }

    template <typename T>
    void WriteMemoryEx(DWORD BaseAddress, T WriteValue) {
        if (!GameHandle || BaseAddress == 0) return;
        NtWriteVirtualMemory(GameHandle, (LPVOID)BaseAddress, &WriteValue, sizeof(WriteValue), nullptr);
    }

    template <typename T>
    void WriteMemoryVector(T BaseAddress, T WriteValue) {
        if (!GameHandle || BaseAddress == 0) return;
        NtWriteVirtualMemory(GameHandle, (T)BaseAddress, &WriteValue, sizeof(WriteValue), nullptr);
    }

    inline void WriteProtected(DWORD BaseAddress, PVOID data, SIZE_T length)
    {
        if (!GameHandle || BaseAddress == 0 || !data || length == 0) return;
        MEMORY_BASIC_INFORMATION buffer;
        ULONG written = 0;
        unsigned long OldProtect;
        unsigned long OldProtect2;
        if (VirtualProtectEx(GameHandle, (LPVOID)BaseAddress, length, PAGE_EXECUTE_READWRITE, &OldProtect)) {
            NtWriteVirtualMemory(GameHandle, (LPVOID)BaseAddress, data, length, &written);
            VirtualProtectEx(GameHandle, (LPVOID)BaseAddress, length, OldProtect, &OldProtect2);
        }
    }

    template <typename T>
    void WriteMemoryExProtected(DWORD BaseAddress, T WriteValue)
    {
        if (!GameHandle || BaseAddress == 0) return;
        DWORD OldProtect;
        if (VirtualProtectEx(GameHandle, (LPVOID)BaseAddress, sizeof(WriteValue), PAGE_EXECUTE_READWRITE, &OldProtect)) {
            NtWriteVirtualMemory(GameHandle, (LPVOID)BaseAddress, &WriteValue, sizeof(WriteValue), nullptr);
            VirtualProtectEx(GameHandle, (LPVOID)BaseAddress, sizeof(WriteValue), OldProtect, nullptr);
        }
    }

    inline void WriteFloatAsDouble(DWORD BaseAddress, float value)
    {
        double dval = 0.0;
        std::memcpy(&dval, &value, sizeof(float));
        Utility::WriteMemoryExProtected<double>(BaseAddress, dval);
    }

    inline void WriteProtectedRaw(DWORD BaseAddress, PVOID data, SIZE_T length)
    {
        if (!GameHandle || BaseAddress == 0 || !data || length == 0) return;
        MEMORY_BASIC_INFORMATION buffer;
        ULONG written = 0;
        unsigned long OldProtect;
        unsigned long OldProtect2;
        if (VirtualProtectEx(GameHandle, (LPVOID)BaseAddress, length, PAGE_EXECUTE_READWRITE, &OldProtect)) {
            NtWriteVirtualMemory(GameHandle, (LPVOID)BaseAddress, data, length, &written);
            VirtualProtectEx(GameHandle, (LPVOID)BaseAddress, length, OldProtect, &OldProtect2);
        }
    }

    template <typename T>
    inline void WriteProtectedEx(DWORD BaseAddress, const T& value)
    {
        WriteProtectedRaw(BaseAddress, (PVOID)&value, sizeof(T));
    }
}