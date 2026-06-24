#include <mutex>
#include <thread>
#include <vector>
#include <Windows.h>
#include <urlmon.h>
#include <winhttp.h>
#include <sddl.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <regex>
#include "hooks.hpp"

// #define DISABLE_LOGGING_CONSOLE 1
#include "backend/dx11/hook_directx11.hpp"
#include "../console/console.hpp"
#include "../menu/menu.hpp"
#include "../utils/utils.hpp"

#include "../dependencies/minhook/MinHook.h"
#include "../dependencies/imgui/imgui.h"

#include "../setting/Setting.h"
#include "../data/Data.h"
#include "../Utility/Utility.h"
#include "AimForm/AimForm.h"
#include "../../ThemidaSDK/Include/C/ThemidaSDK.h"
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")
ImFont *Fonts::MainFont;
ImFont *Fonts::FontAwesome;
ImFont *Fonts::UnicodeFont;
static HWND g_hWindow = NULL;
static std::mutex g_mReinitHooksGuard;
std::atomic<bool> g_Running = true;
bool Hooks::bShuttingDown = false;
bool Hooks::IsExitHack = false;
bool Auth::bLogin = true;

// Satisfy linker for ATL base module without linking atls.lib
namespace ATL {
    class CAtlBaseModule {
    public:
        CAtlBaseModule() throw() {}
        ~CAtlBaseModule() throw() {}
    };
    CAtlBaseModule _AtlBaseModule;
}

HWND ScreenInfo::hWindow;
DWORD ScreenInfo::Left;
DWORD ScreenInfo::Top;
DWORD ScreenInfo::Width;
DWORD ScreenInfo::Height;
HANDLE Utility::GameHandle = 0;
HANDLE OpenedProcessHandles::UpdateCache;
HANDLE OpenedProcessHandles::UpdateEntity;
HANDLE OpenedProcessHandles::UpdateViewMatrix;
HANDLE OpenedProcessHandles::MemoryAttach;
HANDLE OpenedProcessHandles::Aim;
HANDLE OpenedProcessHandles::Macro;
string Data::TotalUser = "";
string Data::UserKey = "";
string Data::UserTime = "";

static WNDPROC oWndProc;
static LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /*if (uMsg == WM_DESTROY) {
         HANDLE hHandle = CreateThread(NULL, 0, ReinitializeGraphicalHooks, hWnd, 0, NULL);
         if (hHandle != NULL)
             CloseHandle(hHandle);
     }*/
    if (uMsg == WM_KEYDOWN)
    {
        /*
         if (Setting::Vehicle::VehicleSkipkey == Setting::PickupItems::Visual::SkipItemsKey)
         {
             if (wParam == Setting::Vehicle::VehicleSkipkey || wParam == Setting::PickupItems::Visual::SkipItemsKey)
             {
                 if (Setting::Vehicle::VehicleSkip)
                 {
                     Setting::Vehicle::IsShowVehicle = !Setting::Vehicle::IsShowVehicle;
                 }
                 if (Setting::PickupItems::Visual::SkipItems)
                 {
                     Setting::PickupItems::Visual::IsShowItems = !Setting::PickupItems::Visual::IsShowItems;
                 }
             }
         }
         else
         {
             if (Setting::PickupItems::Visual::SkipItems)
             {
                 if (wParam == Setting::PickupItems::Visual::SkipItemsKey)
                 {
                     Setting::PickupItems::Visual::IsShowItems = !Setting::PickupItems::Visual::IsShowItems;
                 }
             }
             if (Setting::Vehicle::VehicleSkip)
             {
                 if (wParam = Setting::Vehicle::VehicleSkipkey)
                 {
                     Setting::Vehicle::IsShowVehicle = !Setting::Vehicle::IsShowVehicle;
                 }
             }
         }
         */
        if (wParam == VK_INSERT)
        {
            Setting::ShowMenu = !Setting::ShowMenu;
        }
    }
    if (wParam == VK_LBUTTON)
        Data::IsMouseClick = true;
    else
        Data::IsMouseClick = false;
    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
struct TargetFinderParams
{
    std::wstring wndClassName = L"";
    std::wstring wndName = L"";
    std::vector<HWND> hwnds;
};
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    TargetFinderParams &params = *(TargetFinderParams *)lParam;
    unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;
    wchar_t className[255] = L"";
    GetClassNameW(hwnd, className, 255);
    std::wstring classNameWstr = className;
    if (params.wndClassName != L"")
    {
        if (params.wndClassName == classNameWstr)
        {
            ++satisfiedCriteria;
        }
        else
        {
            ++unSatisfiedCriteria;
        }
    }
    wchar_t windowName[255] = L"";
    GetWindowTextW(hwnd, windowName, 255);
    std::wstring windowNameWstr = windowName;
    if (params.wndName == windowNameWstr || L"TXMenuWindow" == windowNameWstr)
    {
        ++unSatisfiedCriteria;
    }
    else
    {

        ++satisfiedCriteria;
    }
    if (!satisfiedCriteria)
        return TRUE;
    if (unSatisfiedCriteria)
        return TRUE;
    params.hwnds.push_back(hwnd);
    return TRUE;
}
std::vector<HWND> TargetFinder(TargetFinderParams params)
{
    EnumWindows(EnumWindowsCallback, (LPARAM)&params);
    return params.hwnds;
}
std::vector<HWND> GetTargetHWND(std::wstring WindowName, std::wstring ClassName)
{
    std::vector<HWND> WindowList = {};
    HWND current_hwnd = NULL;
    TargetFinderParams params;
    params.wndClassName = ClassName;
    params.wndName = WindowName;
    std::vector<HWND> hwnds = TargetFinder(params);
    return hwnds;
}
size_t write_callbacsk(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
std::wstring get_utf16(const std::string &str, int codepage)
{
    if (str.empty())
        return std::wstring();
    int sz = MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), 0, 0);
    std::wstring res(sz, 0);
    MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), &res[0], sz);
    return res;
}
string HttpsWebRequestPostOld(string domain, string url, string dat)
{
    LPSTR data = const_cast<char *>(dat.c_str());
    ;
    DWORD data_len = strlen(data);
    wstring sdomain = get_utf16(domain, CP_UTF8);
    wstring surl = get_utf16(url, CP_UTF8);
    string response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL bResults = FALSE;
    HINTERNET hSession = NULL,
              hConnect = NULL,
              hRequest = NULL;
    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME,
                           WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession)
        hConnect = WinHttpConnect(hSession, sdomain.c_str(),
                                  INTERNET_DEFAULT_HTTP_PORT, 0);
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, L"POST", surl.c_str(),
                                      NULL, WINHTTP_NO_REFERER,
                                      WINHTTP_DEFAULT_ACCEPT_TYPES,
                                      0);
    LPCWSTR additionalHeaders = L"Content-Type: application/x-www-form-urlencoded\r\n";
    DWORD headersLength = -1;
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,
                                      additionalHeaders,
                                      headersLength,
                                      (LPVOID)data,
                                      data_len,
                                      data_len,
                                      0);
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (bResults)
    {
        do
        {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                printf("Error %u in WinHttpQueryDataAvailable.\n",
                       GetLastError());
            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer)
            {
                printf("Out of memory\n");
                dwSize = 0;
            }
            else
            {
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                                     dwSize, &dwDownloaded))
                    printf("Error %u in WinHttpReadData.\n", GetLastError());
                else
                    response = response + string(pszOutBuffer);
                delete[] pszOutBuffer;
            }
        } while (dwSize > 0);
    }
    if (!bResults)
        printf("Error %d has occurred.\n", GetLastError());
    if (hRequest)
        WinHttpCloseHandle(hRequest);
    if (hConnect)
        WinHttpCloseHandle(hConnect);
    if (hSession)
        WinHttpCloseHandle(hSession);
    return response;
}
void CloseMessageBox(HWND hwnd)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(180000)); // 180 giay = 3 phut
    PostMessage(hwnd, WM_CLOSE, 0, 0);
    exit(0);
    ExitThread(0);
}

std::string get_hwid()
{
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return "none";

    DWORD dwSize = 0;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
    if (dwSize == 0)
    {
        CloseHandle(hToken);
        return "none";
    }

    PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwSize);
    if (!pTokenUser)
    {
        CloseHandle(hToken);
        return "none";
    }

    if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize))
    {
        free(pTokenUser);
        CloseHandle(hToken);
        return "none";
    }

    LPSTR pszSid = NULL;
    if (!ConvertSidToStringSidA(pTokenUser->User.Sid, &pszSid))
    {
        free(pTokenUser);
        CloseHandle(hToken);
        return "none";
    }

    std::string hwid = pszSid;
    LocalFree(pszSid);
    free(pTokenUser);
    CloseHandle(hToken);
    return hwid;
}
bool isValidTimestamp(const std::string &timestamp)
{
    std::regex pattern("^\\d+$"); // Regular expression pattern for numeric digits
    return std::regex_match(timestamp, pattern);
}
bool isInteger(const std::string &str)
{
    if (str.empty())
    {
        return false;
    }
    size_t i = 0;
    if (str[0] == '-' || str[0] == '+')
    {
        i = 1;
    }
    for (; i < str.length(); i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}
void Auth::SendWebHook(std::string WebHookID, std::string WebHookToken, std::string Description, std::string Title, std::string Color)
{
    HINTERNET hSession = WinHttpOpen(L"Fontesie/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS,
                                     0);

    HINTERNET hConnect = WinHttpConnect(hSession,
                                        L"discordapp.com",
                                        INTERNET_DEFAULT_HTTPS_PORT,
                                        0);
    std::string WebAPI = "/api/webhooks/" + WebHookID + "/" + WebHookToken;
    wstring temp = wstring(WebAPI.begin(), WebAPI.end());
    LPCWSTR wideString = temp.c_str();
    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
                                            L"POST",
                                            wideString,
                                            NULL,
                                            WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_SECURE);

    std::string request_body = "{\"username\": \"Custom BOT\",\"content\": null,\"embeds\": [{\"title\": \"" + Title + "\",\"description\": \"" + Description + "\",\"footer\": {\"text\": \"Custom DISCORD BOT\"},\"color\": " + Color + " }], \"attachments\": []}";

    BOOL bResults = WinHttpSendRequest(hRequest,
                                       L"Content-Type: application/json\r\n",
                                       (DWORD)-1L,
                                       (LPVOID)request_body.c_str(),
                                       (DWORD)request_body.length(),
                                       (DWORD)request_body.length(),
                                       0);

    if (bResults)
    {
        WinHttpReceiveResponse(hRequest, NULL);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}

bool Auth::bAdmin = 1;

bool FileExist(const std::string &name)
{
    FILE* file = nullptr;
    if (fopen_s(&file, name.c_str(), "r") == 0 && file != nullptr)
    {
        fclose(file);
        return true;
    }
    return false;
}
BOOL CALLBACK EnumWindowsProcX(HWND hWnd, LPARAM lParam)
{
    char Buffer[1000];
    GetClassNameA(hWnd, Buffer, 1000);

    string WindowClassName = Buffer;

    if (WindowClassName.find("TXGuiFoundation") != string::npos)
    {
        wchar_t Name[1000];
        GetWindowTextW(hWnd, Name, 1000);
        if (wcscmp(Name, L"TXMenuWindow") == 0)
        {
            return TRUE;
        }
        if (wcscmp(Name, L"") == 0)
        {
            return TRUE;
        }
        g_hWindow = FindWindowW(L"TXGuiFoundation", Name);
    }

    return TRUE;
}
namespace Hooks
{
    DWORD WINAPI Init(LPVOID lpParam) 
    {
        INT ProcessID = Utility::GetTrueProcessId();
        while (ProcessID == 0)
        {
            ProcessID = Utility::GetTrueProcessId();
            Sleep(30);
        }
         ::EnumWindows(::EnumWindowsProcX, 0);
         g_hWindow = ::FindWindowExW(g_hWindow, 0, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
         if (g_hWindow == 0)
         {
             ::MessageBoxA(0, "g_hWindow not found", "", 0);
             exit(0);
         }

         RECT Rect;
         GetWindowRect(g_hWindow, &Rect);
         ScreenInfo::Left = Rect.left;
         ScreenInfo::Top = Rect.top;
         ScreenInfo::Width = Rect.right - Rect.left;
         ScreenInfo::Height = Rect.bottom - Rect.top;
         ScreenInfo::hWindow = g_hWindow;
        
         ::ShowWindow(GetConsoleWindow( ), SW_HIDE);
        // Discord::Initialize( );
        // Discord::Update( );

        Utility::GameHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, ProcessID);

        //std::thread tfont([]()
        //                  {
        //    string WebFont = "https://Customhax.cc/DLL/UnicodeSupport.dll";
        //    string UnicodeFontPath = "C:\\UnicodeSupport.dll";
        //    if (!FileExist(UnicodeFontPath))
        //    {
        //        printf("Downloading font...\n");
        //        URLDownloadToFileA(NULL, WebFont.c_str(), UnicodeFontPath.c_str(), 0, NULL);
        //    } });
        std::thread tue4([]()
            { Data::LibUE4Addv = Utility::ReadMemoryEx<DWORD>(Offset::UE4Pointer);/*Utility::GetUE4();*/ });
  
        tue4.join();
        //tfont.join();

        if (Data::LibUE4Addv == 0)
        {
            MessageBoxA(0, "libue4.so not found", "", 0);
            system("TIMEOUT 10");
            system("CLS");
            system("ECHO libue4.so not found.");

            {
                exit(0);
            }
        }

        /*
        if (Data::LibUE4Addv != 0 && Setting::Memory::bUnlockGraphics)
        {
            Data::ApplyGraphicsPatch();
        }
        */

        Data::ViewMatrix = Data::LibUE4Addv + Offset::ViewMatrix;
        Setting::LanguageVNM = false;

        if (::Utility::ReadMemoryEx<bool>(0x9339400) != true)
        {
            Cheat::IsNeedModule = true;
        }
        else
        {
            Cheat::ModuleLoaded = true;
        }

      
        OpenedProcessHandles::UpdateCache = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Data::UpdateCache, 0, 0, 0);
        OpenedProcessHandles::UpdateEntity = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Data::UpdateEntity, 0, 0, 0);
        OpenedProcessHandles::UpdateViewMatrix = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Data::UpdateViewMatrix, 0, 0, 0);
        Data::MemoryAttach();
        OpenedProcessHandles::Aim = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AimForm::Aim, 0, 0, 0);
        OpenedProcessHandles::Macro = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AimForm::Macro, 0, 0, 0);
       
        Data::HandleList.push_back(OpenedProcessHandles::UpdateCache);
        Data::HandleList.push_back(OpenedProcessHandles::UpdateEntity);
        Data::HandleList.push_back(OpenedProcessHandles::UpdateViewMatrix);
        Data::HandleList.push_back(OpenedProcessHandles::Aim);
        Data::HandleList.push_back(OpenedProcessHandles::Macro);

        RenderingBackend_t eRenderingBackend = U::GetRenderingBackend();
        switch (eRenderingBackend)
        {
        case DIRECTX11:
            DX11::Hook(g_hWindow);
            break;
        case OPENGL:
            MessageBoxA(NULL, "Please Change your render to DirectX !", "Error", MB_ICONERROR | MB_OK);
            break;
        }
        oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
    }
    void Free()
    {
        if (oWndProc)
            SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);

        DX11::Unhook();   // ImGui + D3D11

        MH_DisableHook(MH_ALL_HOOKS);
    }

    void FreeRender()
    {
        if (oWndProc)
        {
            SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        RenderingBackend_t eRenderingBackend = U::GetRenderingBackend();
        switch (eRenderingBackend)
        {
            break;
        case DIRECTX11:
            DX11::Unhook();
            break;
        }
    }
}
