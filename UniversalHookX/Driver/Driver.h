#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
using namespace std;
namespace Driver {
    BOOL LoadDriver(PCHAR DriverPath, LPCSTR DisplayName);
    VOID UnloadDriver(LPCSTR DisplayName);
    VOID WriteResToDisk(std::string PathFile, std::string File_WITHARG);
} // namespace Driver
