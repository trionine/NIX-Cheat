#include <Windows.h>
#include <iostream>
#include <thread>

#include "console/console.hpp"

#include "hooks/hooks.hpp"
#include "utils/utils.hpp"
#include "./setting/Setting.h"
#include "dependencies/minhook/MinHook.h"
HINSTANCE selfModuleHandle;

DWORD WINAPI OnProcessAttach(LPVOID lpParam);
DWORD WINAPI OnProcessDetach(LPVOID lpParam);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    selfModuleHandle = hinstDLL;
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        U::SetRenderingBackend(DIRECTX11);
        Cheat::Module = reinterpret_cast<HMODULE>(hinstDLL);
        HANDLE hHandle = CreateThread(NULL, 0, OnProcessAttach, hinstDLL, 0, NULL);
        if (hHandle != NULL) {
            CloseHandle(hHandle);
        }
    }
    return TRUE;
}

DWORD WINAPI OnProcessAttach(LPVOID lpParam) {
    Console::Alloc( );
    MH_Initialize( );
    H::Init(lpParam);
    return 0;
}
