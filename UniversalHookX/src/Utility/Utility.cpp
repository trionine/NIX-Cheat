#include "Utility.h"
#include <string>
#include <memory>
#include <algorithm>
#include <winternl.h>

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

BYTE LibUE4Search[] = {
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x44, 0x02, 0x34, 0x00, 0x00, 0x00, 0x00, 0xDF, 0x3E, 0x0A,
    0x00, 0x02, 0x00, 0x05, 0x34, 0x00, 0x20, 0x00, 0x0E, 0x00, 0x28, 0x00,
    0x17, 0x00, 0x16, 0x00, 0x06, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00
};

BYTE LibModuleSearch[] = {
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x30, 0xB0, 0x34, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x34, 0x00, 0x20, 0x00, 0x08, 0x00, 0x28, 0x00,
    0x18, 0x00, 0x17, 0x00, 0x06, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
    0x34, 0x00, 0x00, 0x00
};

BYTE ViewMatrixSearch[] = { 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01 };

DWORD Utility::BaseAddv = 0;

INT Utility::GetTrueProcessId() {
    INT ProcessId = 0;
    INT ThreadCount = 0;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    else {
        PROCESSENTRY32 ProcessEntry32;
        ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &ProcessEntry32)) {
            do {
                if (!_stricmp(ProcessEntry32.szExeFile, "aow_exe.exe") && ProcessEntry32.cntThreads > ThreadCount) {
                    ThreadCount = ProcessEntry32.cntThreads;
                    ProcessId = (INT)ProcessEntry32.th32ProcessID;
                }
            } while (Process32Next(hSnapshot, &ProcessEntry32));
        }

        CloseHandle(hSnapshot);
        return ProcessId;
    }

    return 0;
}

int SundaySearch(BYTE* bStartAddr, int dwSize, BYTE* bSearchData, DWORD dwSearchSize) {
    if (!bStartAddr || !bSearchData || dwSize <= 0 || dwSearchSize <= 0 || dwSearchSize > static_cast<DWORD>(dwSize)) {
        return -1;
    }

    int iIndex[256];
    for (int i = 0; i < 256; i++) {
        iIndex[i] = -1;
    }

    for (DWORD i = 0; i < dwSearchSize; i++) {
        iIndex[static_cast<BYTE>(bSearchData[i])] = static_cast<int>(dwSearchSize - i);
    }

    int i = 0;
    while (i <= dwSize - static_cast<int>(dwSearchSize)) {
        DWORD k;
        for (k = 0; k < dwSearchSize; k++) {
            if (bStartAddr[i + k] != bSearchData[k])
                break;
        }

        if (k == dwSearchSize) {
            return i;
        }

        if (i + static_cast<int>(dwSearchSize) >= dwSize) {
            return -1;
        }

        int shift = iIndex[static_cast<BYTE>(bStartAddr[i + dwSearchSize])];
        if (shift == -1)
            i += static_cast<int>(dwSearchSize) + 1;
        else
            i += shift;
    }

    return -1;
}

int MemFind(BYTE* buffer, int dwBufferSize, BYTE* bstr, DWORD dwStrLen) {
    if (!buffer || !bstr || dwBufferSize <= 0 || dwStrLen <= 0 || dwStrLen > static_cast<DWORD>(dwBufferSize)) {
        return -1;
    }

    for (DWORD i = 0; i <= static_cast<DWORD>(dwBufferSize) - dwStrLen; i++) {
        DWORD j;
        for (j = 0; j < dwStrLen; j++) {
            if (buffer[i + j] != bstr[j] && bstr[j] != '?')
                break;
        }

        if (j == dwStrLen)
            return static_cast<int>(i);
    }

    return -1;
}

BOOL Utility::NormalMemSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet) {
    if (!bSearchData || nSearchSize <= 0 || dwStartAddr >= dwEndAddr || !Utility::GameHandle) {
        return FALSE;
    }

    MEMORY_BASIC_INFORMATION mbi;
    std::vector<MEMORY_REGION> m_vMemoryRegion;
    DWORD_PTR dwAddress = dwStartAddr;

    while (VirtualQueryEx(Utility::GameHandle, reinterpret_cast<LPCVOID>(dwAddress), &mbi, sizeof(mbi)) &&
        (dwAddress < dwEndAddr) &&
        ((dwAddress + mbi.RegionSize) > dwAddress)) {

        if ((mbi.State == MEM_COMMIT) &&
            ((mbi.Protect & PAGE_GUARD) == 0) &&
            (mbi.Protect != PAGE_NOACCESS) &&
            ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)) {

            MEMORY_REGION mData = { 0 };
            mData.dwBaseAddr = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress);
            mData.dwMemorySize = mbi.RegionSize;
            m_vMemoryRegion.push_back(mData);
        }

        dwAddress = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress) + mbi.RegionSize;
    }

    vRet.clear();

    for (const auto& mData : m_vMemoryRegion) {
        std::unique_ptr<BYTE[]> pCurrMemoryData;
        DWORD_PTR dwNumberOfBytesRead = 0;

        if (bIsCurrProcess) {
            pCurrMemoryData.reset(reinterpret_cast<BYTE*>(mData.dwBaseAddr));
            dwNumberOfBytesRead = mData.dwMemorySize;
        }
        else {
            pCurrMemoryData = std::make_unique<BYTE[]>(mData.dwMemorySize);
            if (!ReadProcessMemory(Utility::GameHandle, reinterpret_cast<LPCVOID>(mData.dwBaseAddr),
                pCurrMemoryData.get(), mData.dwMemorySize, &dwNumberOfBytesRead) ||
                dwNumberOfBytesRead == 0) {
                continue;
            }
        }

        DWORD_PTR dwOffset = 0;
        int iOffset = -1;

        do {
            if (iSearchMode == 0 || iSearchMode == 2) {
                iOffset = MemFind(pCurrMemoryData.get() + dwOffset,
                    static_cast<int>(dwNumberOfBytesRead - dwOffset),
                    bSearchData, nSearchSize);
            }
            else if (iSearchMode == 1) {
                iOffset = SundaySearch(pCurrMemoryData.get() + dwOffset,
                    static_cast<int>(dwNumberOfBytesRead - dwOffset),
                    bSearchData, nSearchSize);
            }

            if (iOffset != -1) {
                dwOffset += iOffset;
                vRet.push_back(dwOffset + mData.dwBaseAddr);
                dwOffset += nSearchSize;

                if (iSearchMode == 2) {
                    break;
                }
            }
        } while (iOffset != -1 && dwOffset < dwNumberOfBytesRead - nSearchSize);
    }

    return TRUE;
}

BOOL Utility::MemSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet) {
    if (!bSearchData || nSearchSize <= 0 || dwStartAddr >= dwEndAddr || !Utility::GameHandle) {
        return FALSE;
    }

    MEMORY_BASIC_INFORMATION mbi;
    std::vector<MEMORY_REGION> m_vMemoryRegion;
    DWORD_PTR dwAddress = dwStartAddr;

    while (VirtualQueryEx(Utility::GameHandle, reinterpret_cast<LPCVOID>(dwAddress), &mbi, sizeof(mbi)) &&
        (dwAddress < dwEndAddr) &&
        ((dwAddress + mbi.RegionSize) > dwAddress)) {

        if ((mbi.State == MEM_COMMIT) &&
            ((mbi.Protect & PAGE_GUARD) == 0) &&
            (mbi.Protect != PAGE_NOACCESS) &&
            ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)) {

            MEMORY_REGION mData = { 0 };
            mData.dwBaseAddr = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress);
            mData.dwMemorySize = mbi.RegionSize;
            m_vMemoryRegion.push_back(mData);
        }
        dwAddress = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress) + mbi.RegionSize;
    }

    vRet.clear();
    BOOL IsFound = FALSE;

    for (const auto& mData : m_vMemoryRegion) {
        if (IsFound && iSearchMode == 1) {
            break;
        }

        auto pCurrMemoryData = std::make_unique<BYTE[]>(mData.dwMemorySize);
        DWORD_PTR dwNumberOfBytesRead = 0;

        if (!ReadProcessMemory(Utility::GameHandle, reinterpret_cast<LPCVOID>(mData.dwBaseAddr),
            pCurrMemoryData.get(), mData.dwMemorySize, &dwNumberOfBytesRead) ||
            dwNumberOfBytesRead == 0) {
            continue;
        }

        DWORD_PTR dwOffset = 0;
        int iOffset = MemFind(pCurrMemoryData.get(), static_cast<int>(dwNumberOfBytesRead),
            bSearchData, nSearchSize);

        while (iOffset != -1) {
            dwOffset += iOffset;
            vRet.push_back(dwOffset + mData.dwBaseAddr);
            dwOffset += nSearchSize;
            iOffset = MemFind(pCurrMemoryData.get() + dwOffset,
                static_cast<int>(dwNumberOfBytesRead - dwOffset - nSearchSize),
                bSearchData, nSearchSize);
            IsFound = TRUE;
        }
    }

    return TRUE;
}

BOOL MemSearchFast(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet) {
    if (!bSearchData || nSearchSize <= 0 || dwStartAddr >= dwEndAddr) {
        return FALSE;
    }

    constexpr DWORD dwBufferSize = 0x100000;
    auto pCurrMemoryData = std::make_unique<BYTE[]>(dwBufferSize);
    MEMORY_BASIC_INFORMATION mbi;
    std::vector<MEMORY_REGION> m_vMemoryRegion;
    DWORD_PTR dwAddress = dwStartAddr;

    HANDLE hProcess = bIsCurrProcess ? GetCurrentProcess() : Utility::GameHandle;
    if (!hProcess) {
        return FALSE;
    }

    while (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(dwAddress), &mbi, sizeof(mbi)) &&
        (dwAddress < dwEndAddr) &&
        ((dwAddress + mbi.RegionSize) > dwAddress)) {

        if ((mbi.State == MEM_COMMIT) &&
            ((mbi.Protect & PAGE_GUARD) == 0) &&
            (mbi.Protect != PAGE_NOACCESS) &&
            ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)) {

            MEMORY_REGION mData = { 0 };
            mData.dwBaseAddr = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress);
            mData.dwMemorySize = mbi.RegionSize;
            m_vMemoryRegion.push_back(mData);
        }
        dwAddress = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress) + mbi.RegionSize;
    }

    vRet.clear();
    BOOL bFoundAddress = FALSE;

    for (const auto& mData : m_vMemoryRegion) {
        if (bFoundAddress && iSearchMode == 1) {
            break;
        }

        DWORD_PTR bytesProcessed = 0;
        while (bytesProcessed < mData.dwMemorySize) {
            DWORD chunkSize = (std::min)(dwBufferSize, static_cast<DWORD>(mData.dwMemorySize - bytesProcessed));
            DWORD_PTR dwNumberOfBytesRead = 0;

            ZeroMemory(pCurrMemoryData.get(), dwBufferSize);

            if (!ReadProcessMemory(hProcess,
                reinterpret_cast<LPCVOID>(mData.dwBaseAddr + bytesProcessed),
                pCurrMemoryData.get(), chunkSize, &dwNumberOfBytesRead) ||
                dwNumberOfBytesRead == 0) {
                break;
            }

            DWORD_PTR dwOffset = 0;
            int iOffset = MemFind(pCurrMemoryData.get(), static_cast<int>(dwNumberOfBytesRead),
                bSearchData, nSearchSize);

            while (iOffset != -1) {
                dwOffset += iOffset;
                vRet.push_back(dwOffset + mData.dwBaseAddr + bytesProcessed);
                dwOffset += nSearchSize;
                iOffset = MemFind(pCurrMemoryData.get() + dwOffset,
                    static_cast<int>(dwNumberOfBytesRead - dwOffset - nSearchSize),
                    bSearchData, nSearchSize);

                if (iSearchMode == 1 && !vRet.empty()) {
                    bFoundAddress = TRUE;
                    break;
                }
            }

            if (bFoundAddress && iSearchMode == 1) {
                break;
            }

            bytesProcessed += chunkSize;
        }
    }

    return TRUE;
}

BOOL Utility::MemSearchSlow(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet) {
    return NormalMemSearch(bSearchData, nSearchSize, dwStartAddr, dwEndAddr, bIsCurrProcess, 0, vRet);
}

int MemFindSpecial2(BYTE* buffer, int dwBufferSize, DWORD dwSearchValue) {
    if (!buffer || dwBufferSize < 4) {
        return -1;
    }

    for (int i = 0; i <= dwBufferSize - 4; i++) {
        DWORD value = *reinterpret_cast<DWORD*>(buffer + i);
        if (value == dwSearchValue)
            return i;
    }
    return -1;
}

BOOL MemSearchValue4Bytes(DWORD dwSearchValue, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, std::vector<DWORD_PTR>& vRet) {
    if (dwStartAddr >= dwEndAddr) {
        return FALSE;
    }

    MEMORY_BASIC_INFORMATION mbi;
    DWORD_PTR dwAddress = dwStartAddr;
    HANDLE hProcess = bIsCurrProcess ? GetCurrentProcess() : Utility::GameHandle;

    if (!hProcess) {
        return FALSE;
    }

    vRet.clear();

    while (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(dwAddress), &mbi, sizeof(mbi)) &&
        (dwAddress < dwEndAddr) &&
        (mbi.RegionSize > 0)) {

        if ((mbi.State == MEM_COMMIT) &&
            ((mbi.Protect & PAGE_GUARD) == 0) &&
            (mbi.Protect != PAGE_NOACCESS) &&
            ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)) {

            auto pCurrMemoryData = std::make_unique<BYTE[]>(mbi.RegionSize);
            DWORD_PTR dwNumberOfBytesRead = 0;

            if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(mbi.BaseAddress),
                pCurrMemoryData.get(), mbi.RegionSize, &dwNumberOfBytesRead) &&
                dwNumberOfBytesRead >= 4) {

                DWORD_PTR dwOffset = 0;
                while (dwOffset <= dwNumberOfBytesRead - 4) {
                    int matchOffset = MemFindSpecial2(pCurrMemoryData.get() + dwOffset,
                        static_cast<int>(dwNumberOfBytesRead - dwOffset),
                        dwSearchValue);
                    if (matchOffset == -1) {
                        break;
                    }
                    DWORD_PTR dwMatchAddr = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress) + dwOffset + matchOffset;
                    vRet.push_back(dwMatchAddr);
                    dwOffset += matchOffset + 4;
                }
            }
        }
        dwAddress = reinterpret_cast<DWORD_PTR>(mbi.BaseAddress) + mbi.RegionSize;
    }

    return TRUE;
}

int GetNumberOfDigits(DWORD value) {
    int digits = 0;
    do {
        digits++;
        value /= 10;
    } while (value != 0);
    return digits;
}

size_t Utility::ReadStringA(HANDLE hProc, void* pAddress, char* pOut, size_t MaxLength) {
    if (!hProc || !pAddress || !pOut || MaxLength == 0) {
        return 0;
    }

    SIZE_T Read;
    if (!ReadProcessMemory(hProc, pAddress, pOut, MaxLength, &Read) || !Read) {
        return 0;
    }

    size_t Ret = 0;
    while (Ret < MaxLength) {
        if (!pOut[Ret]) {
            return Ret;
        }
        Ret++;
    }
    return MaxLength;
}

bool Utility::ScanPatternNew4Bytes(const std::string& pattern, DWORD Byte, int ScanMod) {
    std::string cleanPattern;
    for (char c : pattern) {
        if (std::isxdigit(static_cast<unsigned char>(c))) {
            cleanPattern.push_back(c);
        }
    }

    if (cleanPattern.length() % 2 != 0) {
        return false;
    }

    std::vector<BYTE> patternBytes;
    for (size_t i = 0; i < cleanPattern.length(); i += 2) {
        BYTE byte = static_cast<BYTE>(std::strtoul(cleanPattern.substr(i, 2).c_str(), nullptr, 16));
        patternBytes.push_back(byte);
    }

    std::vector<DWORD_PTR> results;
    MemSearchFast(patternBytes.data(), patternBytes.size(), 0x000000, 0x00007fffffffffff, FALSE, ScanMod, results);

    if (results.empty()) {
        Utility::MemSearchSlow(patternBytes.data(), patternBytes.size(), 0xE000000, 0xE63BA958, FALSE, 0, results);
    }

    if (!results.empty()) {
        for (DWORD_PTR address : results) {
            WriteProcessMemory(Utility::GameHandle, reinterpret_cast<BYTE*>(address), &Byte, sizeof(Byte), NULL);
        }
        return true;
    }

    return false;
}

bool Utility::ScanHelmet(DWORD Search, DWORD Byte, DWORD ByteCheck, DWORD ByteVerify) {
    std::vector<DWORD_PTR> OutputAddv;
    bool bReturn = false;

    MemSearchValue4Bytes(Search, 0xE000000, 0xE63BA958, FALSE, OutputAddv);

    if (!OutputAddv.empty()) {
        for (DWORD_PTR& Addv : OutputAddv) {
            auto BytesCheck = Utility::ReadMemoryEx<DWORD>(Addv + ByteCheck);
            if (BytesCheck == ByteVerify) {
                WriteProcessMemory(Utility::GameHandle, reinterpret_cast<BYTE*>(Addv), &Byte, sizeof(Byte), NULL);
                bReturn = true;
            }
        }
    }
    return bReturn;
}

DWORD Utility::GetViewMatrixBase(DWORD StartAddress, DWORD EndAddress) {
    if (StartAddress >= EndAddress || !Utility::GameHandle) {
        return 0;
    }

    DWORD ViewMatrixBase = 0;
    std::vector<DWORD_PTR> FoundedBase;

    Utility::MemSearch(ViewMatrixSearch, sizeof(ViewMatrixSearch), StartAddress, EndAddress, 0, 0, FoundedBase);

    for (DWORD_PTR base : FoundedBase) {
        DWORD Cand = static_cast<DWORD>(base) - 0x20;
        DWORD Eng = Utility::ReadMemoryEx<DWORD>(Cand);

        if (!Eng) continue;

        Eng += 0x20;
        DWORD Tmp = Utility::ReadMemoryEx<DWORD>(Eng);

        if (!Tmp) continue;

        Tmp += 0x200;

        float v1, v2, v3, v4;
        v1 = Utility::ReadMemoryEx<float>(Tmp + 0x8);
        v2 = Utility::ReadMemoryEx<float>(Tmp + 0x18);
        v3 = Utility::ReadMemoryEx<float>(Tmp + 0x28);
        v4 = Utility::ReadMemoryEx<float>(Tmp + 0x38);

        if (v1 == 0 && v2 == 0 && v3 == 0 && v4 >= 3) {
            ViewMatrixBase = Cand;
            break;
        }
    }

    return ViewMatrixBase;
}

DWORD Utility::GetUE4(DWORD StartAddress, DWORD EndAddress) {
    if (StartAddress >= EndAddress) {
        return 0;
    }

    std::vector<DWORD_PTR> FoundedAddv;
    Utility::MemSearch(LibUE4Search, sizeof(LibUE4Search), StartAddress, EndAddress, 0, 0, FoundedAddv);

    if (!FoundedAddv.empty()) {
        return static_cast<DWORD>(FoundedAddv[0]);
    }
    return 0;
}

DWORD Utility::GetModule(DWORD StartAddress, DWORD EndAddress) {
    if (StartAddress >= EndAddress) {
        return 0;
    }

    std::vector<DWORD_PTR> FoundedAddv;
    Utility::MemSearch(LibModuleSearch, sizeof(LibModuleSearch), StartAddress, EndAddress, 0, 0, FoundedAddv);

    if (!FoundedAddv.empty()) {
        return static_cast<DWORD>(FoundedAddv[0]);
    }
    return 0;
}

std::string Utility::GameVersion() {
    if (!Utility::GameHandle) {
        return "";
    }

    BYTE Search[] = { 0x63, 0x6F, 0x6D, 0x2E };
    std::vector<DWORD_PTR> Addrs;

    MemSearch(Search, sizeof(Search), 0x30000000, 0x40600000, 0, 0, Addrs);

    if (Addrs.empty()) {
        return "";
    }

    char Buf[30] = { 0 };
    for (auto& add : Addrs) {
        SIZE_T bytesRead = 0;

        if (NtReadVirtualMemory(Utility::GameHandle,
            reinterpret_cast<PVOID>(add),
            Buf,
            sizeof(Buf) - 1,
            &bytesRead) == STATUS_SUCCESS)
        {
            Buf[sizeof(Buf) - 1] = '\0';

            if (strstr(Buf, "com.vng.pubgmobile") ||
                strstr(Buf, "com.tencent.ig") ||
                strstr(Buf, "com.rekoo.pubgm") ||
                strstr(Buf, "com.pubg.krmobile"))
            {
                return Buf;
            }
        }
    }


    return "";
}

VOID Utility::ChangeClickability(BOOL CanClick, HWND hWindow) {
    if (!hWindow || !IsWindow(hWindow)) {
        return;
    }

    LONG Style = GetWindowLong(hWindow, GWL_EXSTYLE);

    if (CanClick) {
        Style &= ~WS_EX_LAYERED;
        SetWindowLong(hWindow, GWL_EXSTYLE, Style);
        SetForegroundWindow(hWindow);
    }
    else {
        Style |= WS_EX_LAYERED;
        SetWindowLong(hWindow, GWL_EXSTYLE, Style);
    }
}