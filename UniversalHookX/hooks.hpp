#pragma once
#undef _HAS_STD_BYTE
#include <Windows.h>
#include <string>
#include <iostream>
#include "src/dependencies/imgui/imgui.h"
#include <atomic>

extern std::atomic<bool> g_Running;
namespace Hooks {
    DWORD WINAPI Init(LPVOID lpParam) ;
	void Free( );
    void FreeRender( );
	extern bool bShuttingDown;
    extern bool IsExitHack;
} // namespace Hooks
namespace ScreenInfo
{
    extern HWND hWindow;
    extern DWORD Left;
    extern DWORD Top;
    extern DWORD Width;
    extern DWORD Height;
}
namespace Fonts
{
    extern ImFont* MainFont;
    extern ImFont* FontAwesome;
    extern ImFont* UnicodeFont;
}
namespace Auth
{
    extern void SendWebHook(std::string WebHookID, std::string WebHookToken, std::string Description, std::string Title, std::string Color);
    extern bool CheckKey(std::string key, std::string& output);
    extern bool Init( );
    extern bool bAdmin;
    extern bool bLogin;
}// ohh 
namespace H = Hooks;
