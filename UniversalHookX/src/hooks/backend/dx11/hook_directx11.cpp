#include "../../../../font/font_awesome.h"
#include "../../../backend.hpp"
#include "../../../console/console.hpp"
#define STB_IMAGE_IMPLEMENTATION
#ifdef ENABLE_BACKEND_DX11
#include <regex>
#include <Windows.h>
#include <objbase.h>
#include <codecvt>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <xlocbuf>
typedef int CASEID;
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../../../AimForm/AimForm.h"
#include "config/Config.h"
#include "font/LiteFont.h"
#include "font/font_awesome.cpp"
#include "font/font_awesome.h"
#include "mathnvec/MathV.h"
#include "data/Data.h"
#include "dependencies/imgui/imgui.h"
#include "dependencies/imgui/imgui_impl_dx11.h"
#include "dependencies/imgui/imgui_impl_opengl3.h"
#include "dependencies/imgui/imgui_impl_win32.h"
#include "dependencies/imgui/imgui_internal.h"
#include "dependencies/minhook/MinHook.h"
#include "menu/menu.hpp"
#include "setting/Setting.h"
#include "src/imgui_notify.h"
#include "stb_image.h"
#include "utils/utils.hpp"
#include "Teammate/Teammate.h"
#include "hooks.hpp"
#include "logo_bytes.h"
#include "hook_directx11.hpp"
#include <d3d11.h>
#include <memory>
#include <map>

#define NEWSINDEX 0
#define VISUALINDEX 1
#define AIMBOTINDEX 2
#define MEMORYINDEX 3
#define SETTINGINDEX 4
/* NIX-style tab indices (same as NIX new driver design) */
#define ITEMSINDEX 1
#define VEHICLESINDEX 2
#define AIMINGINDEX 4
#define ADMININDEX 5
#define SETTINGTABINDEX 7
#define PLAYERSINDEX 8
#define IM_PI 3.14159265358979323846f
#define RAD2DEG(x) ((float)(x) * (float)(180.f / IM_PI))
#define DEG2RAD(x) ((float)(x) * (float)(IM_PI / 180.f))
#define _STRINGIFY(str) #str
#define STRINGIFY(str) _STRINGIFY(str)

ImGuiIO &io = ImGui::GetIO();

static ID3D11Device *g_pd3dDevice = NULL;
static ID3D11DeviceContext *g_pd3dDeviceContext = NULL;
static ID3D11RenderTargetView *g_pd3dRenderTarget = NULL;
static IDXGISwapChain *g_pSwapChain = NULL;
static ID3D11ShaderResourceView* s_pNixLogoTexture = nullptr;
static int s_nixLogoW = 0, s_nixLogoH = 0;
/* Vehicle/weapon icons loaded from DLL folder/icons/ (e.g. Buggy.png, M416.png) */
struct CachedIcon { ID3D11ShaderResourceView* srv = nullptr; int w = 0; int h = 0; };
static std::map<std::string, CachedIcon> s_iconCache;
static std::string s_iconsBasePath;

void UpdateAndDrawParticles(ImVec2 menuPos, ImVec2 menuSize);


// Store hook target addresses for proper cleanup
static void *g_fnCreateSwapChain = NULL;
static void *g_fnCreateSwapChainForHwnd = NULL;
static void *g_fnCreateSwapChainForCoreWindow = NULL;
static void *g_fnCreateSwapChainForComposition = NULL;
static void *g_fnPresent = NULL;
static void *g_fnPresent1 = NULL;
static void *g_fnResizeBuffers = NULL;
static void *g_fnResizeBuffers1 = NULL;
static bool g_bHooksCreated = false;

static void CleanupDeviceD3D11();
static void CleanupRenderTarget();
static void RenderImGui_DX11(IDXGISwapChain *pSwapChain);

bool isInitStyle = false;
bool isInitWinsize = false;
static vector<ARNearest> ARNearestCache = {};
static vector<ARNearest> ARNearestList = {};
static char Keyinput[65536] = "";
static char Chatinput[65536] = "";
static auto Flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
static string compilation_date = (string)(__DATE__);
static string compilation_time = (string)(__TIME__);
static INT IndexTabs = 0;
static HWND s_GameWindow = nullptr;
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#define WDA_NONE 0
typedef BOOL (WINAPI* PFN_SetWindowDisplayAffinity)(HWND, DWORD);
static PFN_SetWindowDisplayAffinity s_pSetWindowDisplayAffinity = nullptr;
static HWND s_OverlayWindow = nullptr;
static IDXGISwapChain* g_pOverlaySwapChain = nullptr;
static int s_OverlayFrameDelay = 0;
static bool s_OverlayFailed = false;
static ID3D11RenderTargetView* g_pOverlayRenderTarget = nullptr;
static bool s_OverlayClassRegistered = false;
static int s_OverlayW = 0, s_OverlayH = 0;
static INT Publicchaty = 0;
static INT Randintaim = 0;
static bool IsOpenLogin = false;
static bool Fake_Namecache = true;
static bool IsExitHack = false;
static bool IsExitHackCenter = false;
static bool IsLoginCenter = false;
static bool FirstTimeEnemyAround = true;
static bool FirstTimeGameinformation = true;
static bool FirstTimeCreateRandom = true;
static bool FirstTimeInitImgui = true;
static bool isLogin = true;
static const ImColor _randombush[] = {
    ImColor(255, 255, 0, 255),   // Mau vang , //duoi la 0
    ImColor(255, 100, 255, 255), // Mau Hong nhat , //duoi la 1
    ImColor(255, 0, 150, 255),   // Mau Hong dam, //duoi la 2
    ImColor(200, 255, 200, 255), // Mau Xanh lo, //duoi la 3
    ImColor(200, 255, 255, 255), // Mau Xanh ngoc, //duoi la 4
    ImColor(255, 155, 0, 255),   // Mau cam, //duoi la 5
    ImColor(255, 155, 100, 255), // Mau cam nhat, //duoi la 6
    ImColor(255, 50, 0, 255),    // Mau do, //duoi la 7
    ImColor(240, 170, 210, 255), // Mau da, //duoi la 8
    ImColor(255, 0, 150, 255),   // Mau Hong dam, //duoi la 9
};
static bool CreateDeviceD3D11(HWND hWnd)
{
    // Create the D3DDevice
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;

    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &g_pSwapChain, &g_pd3dDevice, nullptr, nullptr);
    if (hr != S_OK)
    {
        return false;
    }

    return true;
}
static void CreateRenderTarget(IDXGISwapChain *pSwapChain)
{
    ID3D11Texture2D *pBackBuffer = NULL;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer)
    {
        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);

        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = static_cast<DXGI_FORMAT>(Utils::GetCorrectDXGIFormat(sd.BufferDesc.Format));
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &desc, &g_pd3dRenderTarget);
        pBackBuffer->Release();
    }
}

static bool EnsureOverlayWindowImpl()
{
    HWND gameHwnd = s_GameWindow ? s_GameWindow : ScreenInfo::hWindow;
    if (!gameHwnd || !IsWindow(gameHwnd) || !g_pd3dDevice)
        return false;
    RECT gr = {};
    GetWindowRect(gameHwnd, &gr);
    int gw = gr.right - gr.left, gh = gr.bottom - gr.top;
    if (gw <= 0 || gh <= 0)
        return false;
    if (s_OverlayWindow && IsWindow(s_OverlayWindow))
    {
        if (gw != s_OverlayW || gh != s_OverlayH)
        {
            if (g_pOverlayRenderTarget) { g_pOverlayRenderTarget->Release(); g_pOverlayRenderTarget = nullptr; }
            if (g_pOverlaySwapChain) { g_pOverlaySwapChain->Release(); g_pOverlaySwapChain = nullptr; }
            DestroyWindow(s_OverlayWindow);
            s_OverlayWindow = nullptr;
        }
        else
        {
            SetWindowPos(s_OverlayWindow, HWND_TOPMOST, gr.left, gr.top, gw, gh, SWP_NOACTIVATE);
            return true;
        }
    }
    if (!s_OverlayClassRegistered)
    {
        WNDCLASSEXW wc = { sizeof(wc), 0, DefWindowProcW, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"BradyOverlay", nullptr };
        if (!RegisterClassExW(&wc))
            return false;
        s_OverlayClassRegistered = true;
    }
    int w = gw, h = gh;
    s_OverlayWindow = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST, L"BradyOverlay", L"", WS_POPUP | WS_VISIBLE, gr.left, gr.top, w, h, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!s_OverlayWindow)
        return false;
    SetLayeredWindowAttributes(s_OverlayWindow, RGB(0, 0, 1), 0, LWA_COLORKEY);
    IDXGIDevice* pDXGIDevice = nullptr;
    if (FAILED(g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pDXGIDevice))))
        return false;
    IDXGIAdapter* pAdapter = nullptr;
    if (FAILED(pDXGIDevice->GetAdapter(&pAdapter))) { pDXGIDevice->Release(); return false; }
    IDXGIFactory* pFactory = nullptr;
    if (FAILED(pAdapter->GetParent(IID_PPV_ARGS(&pFactory)))) { pAdapter->Release(); pDXGIDevice->Release(); return false; }
    DXGI_SWAP_CHAIN_DESC od = {};
    od.BufferCount = 2;
    od.BufferDesc.Width = w;
    od.BufferDesc.Height = h;
    od.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    od.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    od.OutputWindow = s_OverlayWindow;
    od.SampleDesc.Count = 1;
    od.Windowed = TRUE;
    od.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    if (FAILED(pFactory->CreateSwapChain(g_pd3dDevice, &od, &g_pOverlaySwapChain)))
    {
        pFactory->Release(); pAdapter->Release(); pDXGIDevice->Release();
        DestroyWindow(s_OverlayWindow); s_OverlayWindow = nullptr;
        return false;
    }
    pFactory->Release(); pAdapter->Release(); pDXGIDevice->Release();
    ID3D11Texture2D* pOverlayBack = nullptr;
    if (FAILED(g_pOverlaySwapChain->GetBuffer(0, IID_PPV_ARGS(&pOverlayBack))))
    {
        g_pOverlaySwapChain->Release(); g_pOverlaySwapChain = nullptr;
        DestroyWindow(s_OverlayWindow); s_OverlayWindow = nullptr;
        return false;
    }
    D3D11_RENDER_TARGET_VIEW_DESC rtd = {};
    rtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    if (FAILED(g_pd3dDevice->CreateRenderTargetView(pOverlayBack, &rtd, &g_pOverlayRenderTarget)))
    {
        pOverlayBack->Release();
        g_pOverlaySwapChain->Release(); g_pOverlaySwapChain = nullptr;
        DestroyWindow(s_OverlayWindow); s_OverlayWindow = nullptr;
        return false;
    }
    pOverlayBack->Release();
    s_OverlayW = w;
    s_OverlayH = h;
    ShowWindow(s_OverlayWindow, SW_SHOWNOACTIVATE);
    return true;
}

static bool EnsureOverlayWindow()
{
    if (s_OverlayFailed)
        return false;
    if (!EnsureOverlayWindowImpl())
        return false;
    return true;
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain *, UINT, UINT)> oPresent;
static HRESULT WINAPI hkPresent(IDXGISwapChain *pSwapChain,
                                UINT SyncInterval,
                                UINT Flags)
{
    if (!g_Running)
        return oPresent(pSwapChain, SyncInterval, Flags);

    RenderImGui_DX11(pSwapChain);

    UINT si = (Setting::Memory::bUnlockFPS) ? 0u : SyncInterval;
    return oPresent(pSwapChain, si, Flags);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain *, UINT, UINT, const DXGI_PRESENT_PARAMETERS *)> oPresent1;
static HRESULT WINAPI hkPresent1(IDXGISwapChain *pSwapChain,
                                 UINT SyncInterval,
                                 UINT PresentFlags,
                                 const DXGI_PRESENT_PARAMETERS *pPresentParameters)
{
    if (!g_Running)
        return oPresent1(pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
    RenderImGui_DX11(pSwapChain);

    UINT si = (Setting::Memory::bUnlockFPS) ? 0u : SyncInterval;
    return oPresent1(pSwapChain, si, PresentFlags, pPresentParameters);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain *, UINT, UINT, UINT, DXGI_FORMAT, UINT)> oResizeBuffers;
static HRESULT WINAPI hkResizeBuffers(IDXGISwapChain *pSwapChain,
                                      UINT BufferCount,
                                      UINT Width,
                                      UINT Height,
                                      DXGI_FORMAT NewFormat,
                                      UINT SwapChainFlags)
{
    if (!g_Running)
        return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    CleanupRenderTarget();

    return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain *, UINT, UINT, UINT, DXGI_FORMAT, UINT, const UINT *, IUnknown *const *)> oResizeBuffers1;
static HRESULT WINAPI hkResizeBuffers1(IDXGISwapChain *pSwapChain,
                                       UINT BufferCount,
                                       UINT Width,
                                       UINT Height,
                                       DXGI_FORMAT NewFormat,
                                       UINT SwapChainFlags,
                                       const UINT *pCreationNodeMask,
                                       IUnknown *const *ppPresentQueue)
{
    if (!g_Running)
        return oResizeBuffers1(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags, pCreationNodeMask, ppPresentQueue);
    CleanupRenderTarget();

    return oResizeBuffers1(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags, pCreationNodeMask, ppPresentQueue);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory *, IUnknown *, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **)> oCreateSwapChain;
static HRESULT WINAPI hkCreateSwapChain(IDXGIFactory *pFactory,
                                        IUnknown *pDevice,
                                        DXGI_SWAP_CHAIN_DESC *pDesc,
                                        IDXGISwapChain **ppSwapChain)
{
    if (!g_Running)
        return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
    CleanupRenderTarget();

    return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory *, IUnknown *, HWND, const DXGI_SWAP_CHAIN_DESC1 *, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *, IDXGIOutput *, IDXGISwapChain1 **)> oCreateSwapChainForHwnd;
static HRESULT WINAPI hkCreateSwapChainForHwnd(IDXGIFactory *pFactory,
                                               IUnknown *pDevice,
                                               HWND hWnd,
                                               const DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                               const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
                                               IDXGIOutput *pRestrictToOutput,
                                               IDXGISwapChain1 **ppSwapChain)
{
    if (!g_Running)
        return oCreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
    CleanupRenderTarget();

    return oCreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory *, IUnknown *, IUnknown *, const DXGI_SWAP_CHAIN_DESC1 *, IDXGIOutput *, IDXGISwapChain1 **)> oCreateSwapChainForCoreWindow;
static HRESULT WINAPI hkCreateSwapChainForCoreWindow(IDXGIFactory *pFactory,
                                                     IUnknown *pDevice,
                                                     IUnknown *pWindow,
                                                     const DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                                     IDXGIOutput *pRestrictToOutput,
                                                     IDXGISwapChain1 **ppSwapChain)
{
    if (!g_Running)
        return oCreateSwapChainForCoreWindow(pFactory, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
    CleanupRenderTarget();

    return oCreateSwapChainForCoreWindow(pFactory, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory *, IUnknown *, const DXGI_SWAP_CHAIN_DESC1 *, IDXGIOutput *, IDXGISwapChain1 **)> oCreateSwapChainForComposition;
static HRESULT WINAPI hkCreateSwapChainForComposition(IDXGIFactory *pFactory,
                                                      IUnknown *pDevice,
                                                      const DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                                      IDXGIOutput *pRestrictToOutput,
                                                      IDXGISwapChain1 **ppSwapChain)
{
    if (!g_Running)
        return oCreateSwapChainForComposition(pFactory, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
    CleanupRenderTarget();

    return oCreateSwapChainForComposition(pFactory, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}
float floattest = 0;

bool LoadTextureFromMemory(const unsigned char* buffer, int len, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    if (!buffer || len <= 0 || !g_pd3dDevice || !out_srv || !out_width || !out_height) return false;
    int w = 0, h = 0;
    unsigned char* image_data = stbi_load_from_memory(buffer, len, &w, &h, NULL, 4);
    if (!image_data) return false;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = w * 4;
    subResource.SysMemSlicePitch = 0;
    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
    stbi_image_free(image_data);
    if (FAILED(hr) || !pTexture) return false;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();
    if (FAILED(hr)) return false;
    *out_width = w;
    *out_height = h;
    return true;
}
bool LoadTextureFromFile(const char *filename, ID3D11ShaderResourceView **out_srv, int *out_width, int *out_height)
{
    int image_width = 0;
    int image_height = 0;
    unsigned char *image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    ID3D11Texture2D *pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();
    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}
/* Sanitize display name to icon filename (no spaces/special chars). */
static std::string IconKeyToFilename(const std::string& key)
{
    std::string s = key;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == ' ' || s[i] == '-') s[i] = '_';
        else if (s[i] == '(' || s[i] == ')') { s.erase(i, 1); i--; }
    }
    return s;
}
/* Load vehicle/weapon icon from icons folder (DLL dir + "icons\\" + key + ".png"). Cached per key. */
static CachedIcon* GetOrLoadIcon(const std::string& key)
{
    if (key.empty() || !g_pd3dDevice) return nullptr;
    std::string filename = IconKeyToFilename(key);
    if (filename.empty()) return nullptr;
    
    auto it = s_iconCache.find(key);
    if (it != s_iconCache.end())
        return it->second.srv ? &it->second : nullptr;

    // 1. Try primary path (DLL dir)
    std::string path = s_iconsBasePath + filename + ".png";
    CachedIcon ci = {};
    if (LoadTextureFromFile(path.c_str(), &ci.srv, &ci.w, &ci.h) && ci.srv)
    {
        s_iconCache[key] = ci;
        return &s_iconCache[key];
    }

    // 2. Try Fallback path (C:\icons\)
    std::string fallbackPath = "C:\\icons\\" + filename + ".png";
    if (LoadTextureFromFile(fallbackPath.c_str(), &ci.srv, &ci.w, &ci.h) && ci.srv)
    {
        s_iconCache[key] = ci;
        return &s_iconCache[key];
    }

    s_iconCache[key] = ci;
    return nullptr;
}
BOOL IsExits(string namefile)
{
    std::ifstream infile(namefile);
    BOOL bReturn = infile.good();
    infile.close();
    return bReturn;
}
std::vector<std::string> StrSplit(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}
namespace DX11
{
    void Hook(HWND hwnd)
    {
        // CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UpdateImage,0,0,0);
        if (!CreateDeviceD3D11(GetConsoleWindow()))
        {
            return;
        }
        if (g_pd3dDevice)
        {
            Menu::InitializeContext(hwnd);

            // Hook
            IDXGIDevice *pDXGIDevice = NULL;
            g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

            IDXGIAdapter *pDXGIAdapter = NULL;
            pDXGIDevice->GetAdapter(&pDXGIAdapter);

            IDXGIFactory *pIDXGIFactory = NULL;
            pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

            if (!pIDXGIFactory)
            {
                pDXGIAdapter->Release();
                pDXGIDevice->Release();
                return;
            }

            void **pVTable = *reinterpret_cast<void ***>(g_pSwapChain);
            void **pFactoryVTable = *reinterpret_cast<void ***>(pIDXGIFactory);

            // Store hook targets globally for cleanup
            g_fnCreateSwapChain = pFactoryVTable[10];
            g_fnCreateSwapChainForHwnd = pFactoryVTable[15];
            g_fnCreateSwapChainForCoreWindow = pFactoryVTable[16];
            g_fnCreateSwapChainForComposition = pFactoryVTable[24];

            g_fnPresent = pVTable[8];
            g_fnPresent1 = pVTable[22];

            g_fnResizeBuffers = pVTable[13];
            g_fnResizeBuffers1 = pVTable[39];

            // Release COM objects in reverse order of creation
            if (pIDXGIFactory) {
                pIDXGIFactory->Release();
                pIDXGIFactory = NULL;
            }
            if (pDXGIAdapter) {
                pDXGIAdapter->Release();
                pDXGIAdapter = NULL;
            }
            if (pDXGIDevice) {
                pDXGIDevice->Release();
                pDXGIDevice = NULL;
            }

            CleanupDeviceD3D11();

            // Only create hooks if not already created (prevents issues on reload)
            if (!g_bHooksCreated)
            {
                MH_CreateHook(reinterpret_cast<void **>(g_fnCreateSwapChain), &hkCreateSwapChain, reinterpret_cast<void **>(&oCreateSwapChain));
                MH_CreateHook(reinterpret_cast<void **>(g_fnCreateSwapChainForHwnd), &hkCreateSwapChainForHwnd, reinterpret_cast<void **>(&oCreateSwapChainForHwnd));
                MH_CreateHook(reinterpret_cast<void **>(g_fnCreateSwapChainForCoreWindow), &hkCreateSwapChainForCoreWindow, reinterpret_cast<void **>(&oCreateSwapChainForCoreWindow));
                MH_CreateHook(reinterpret_cast<void **>(g_fnCreateSwapChainForComposition), &hkCreateSwapChainForComposition, reinterpret_cast<void **>(&oCreateSwapChainForComposition));

                MH_CreateHook(reinterpret_cast<void **>(g_fnPresent), &hkPresent, reinterpret_cast<void **>(&oPresent));
                MH_CreateHook(reinterpret_cast<void **>(g_fnPresent1), &hkPresent1, reinterpret_cast<void **>(&oPresent1));

                MH_CreateHook(reinterpret_cast<void **>(g_fnResizeBuffers), &hkResizeBuffers, reinterpret_cast<void **>(&oResizeBuffers));
                MH_CreateHook(reinterpret_cast<void **>(g_fnResizeBuffers1), &hkResizeBuffers1, reinterpret_cast<void **>(&oResizeBuffers1));

                g_bHooksCreated = true;
            }

            MH_EnableHook(g_fnCreateSwapChain);
            MH_EnableHook(g_fnCreateSwapChainForHwnd);
            MH_EnableHook(g_fnCreateSwapChainForCoreWindow);
            MH_EnableHook(g_fnCreateSwapChainForComposition);

            MH_EnableHook(g_fnPresent);
            MH_EnableHook(g_fnPresent1);

            MH_EnableHook(g_fnResizeBuffers);
            MH_EnableHook(g_fnResizeBuffers1);
        }
    }
    void Unhook()
    {
        // Disable and remove all hooks first
        if (g_bHooksCreated)
        {
            if (g_fnPresent) { MH_DisableHook(g_fnPresent); MH_RemoveHook(g_fnPresent); }
            if (g_fnPresent1) { MH_DisableHook(g_fnPresent1); MH_RemoveHook(g_fnPresent1); }
            if (g_fnResizeBuffers) { MH_DisableHook(g_fnResizeBuffers); MH_RemoveHook(g_fnResizeBuffers); }
            if (g_fnResizeBuffers1) { MH_DisableHook(g_fnResizeBuffers1); MH_RemoveHook(g_fnResizeBuffers1); }
            if (g_fnCreateSwapChain) { MH_DisableHook(g_fnCreateSwapChain); MH_RemoveHook(g_fnCreateSwapChain); }
            if (g_fnCreateSwapChainForHwnd) { MH_DisableHook(g_fnCreateSwapChainForHwnd); MH_RemoveHook(g_fnCreateSwapChainForHwnd); }
            if (g_fnCreateSwapChainForCoreWindow) { MH_DisableHook(g_fnCreateSwapChainForCoreWindow); MH_RemoveHook(g_fnCreateSwapChainForCoreWindow); }
            if (g_fnCreateSwapChainForComposition) { MH_DisableHook(g_fnCreateSwapChainForComposition); MH_RemoveHook(g_fnCreateSwapChainForComposition); }
            
            g_bHooksCreated = false;
        }

        // Reset hook target pointers
        g_fnCreateSwapChain = NULL;
        g_fnCreateSwapChainForHwnd = NULL;
        g_fnCreateSwapChainForCoreWindow = NULL;
        g_fnCreateSwapChainForComposition = NULL;
        g_fnPresent = NULL;
        g_fnPresent1 = NULL;
        g_fnResizeBuffers = NULL;
        g_fnResizeBuffers1 = NULL;

        // Reset original function pointers
        oPresent = NULL;
        oPresent1 = NULL;
        oResizeBuffers = NULL;
        oResizeBuffers1 = NULL;
        oCreateSwapChain = NULL;
        oCreateSwapChainForHwnd = NULL;
        oCreateSwapChainForCoreWindow = NULL;
        oCreateSwapChainForComposition = NULL;

        // Cleanup ImGui - proper shutdown sequence
        if (ImGui::GetCurrentContext())
        {
            // Shutdown renderer first, then platform
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        CleanupDeviceD3D11();
    }
} // namespace DX11
string Status;
string OutputApi;
bool IsPress = false;
bool IsFirstReadKey = true;
static void CleanupRenderTarget()
{
    if (g_pd3dRenderTarget)
    {
        g_pd3dRenderTarget->Release();
        g_pd3dRenderTarget = NULL;
    }
}
static void CleanupDeviceD3D11()
{
    CleanupRenderTarget();

    // Release COM objects in correct order
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = NULL;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = NULL;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
}
typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
void SuspendProcess(DWORD processId)
{
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
        GetModuleHandleA("ntdll"), "NtSuspendProcess");
    pfnNtSuspendProcess(processHandle);
    CloseHandle(processHandle);
}
typedef LONG(NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);
void ResumeProcess(DWORD processId)
{
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(
        GetModuleHandleA("ntdll"), "NtResumeProcess");
    pfnNtResumeProcess(processHandle);
    CloseHandle(processHandle);
}
void DX11::InsertNoti(ImGuiNotiType Type, int TimeShow, std::string Text, std::string Title)
{
    ImGuiToast toast(Type, TimeShow);
    if (Text != "")
        toast.set_content(Text.c_str());
    if (Title != "")
        toast.set_title(Title.c_str());
    ImGui::InsertNotification(toast);
}
const char *const KeyNames[] = {
    "Unknown",
    "LMouse",
    "RMouse",
    "CANCEL",
    "MBUTTON",
    "XBUTTON1",
    "XBUTTON2",
    "Unknown",
    "BACK",
    "TAB",
    "Unknown",
    "Unknown",
    "CLEAR",
    "RETURN",
    "Unknown",
    "Unknown",
    "SHIFT",
    "CONTROL",
    "MENU",
    "PAUSE",
    "CAPITAL",
    "KANA",
    "Unknown",
    "JUNJA",
    "FINAL",
    "KANJI",
    "Unknown",
    "ESCAPE",
    "CONVERT",
    "NONCONVERT",
    "ACCEPT",
    "MODECHANGE",
    "SPACE",
    "PRIOR",
    "NEXT",
    "END",
    "HOME",
    "LEFT",
    "UP",
    "RIGHT",
    "DOWN",
    "SELECT",
    "PRINT",
    "EXECUTE",
    "SNAPSHOT",
    "INSERT",
    "DELETE",
    "HELP",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "LWIN",
    "RWIN",
    "APPS",
    "Unknown",
    "SLEEP",
    "NUMPAD0",
    "NUMPAD1",
    "NUMPAD2",
    "NUMPAD3",
    "NUMPAD4",
    "NUMPAD5",
    "NUMPAD6",
    "NUMPAD7",
    "NUMPAD8",
    "NUMPAD9",
    "MULTIPLY",
    "ADD",
    "SEPARATOR",
    "SUBTRACT",
    "DECIMAL",
    "DIVIDE",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "NUMLOCK",
    "SCROLL",
    "OEM_NEC_EQUAL",
    "OEM_FJ_MASSHOU",
    "OEM_FJ_TOUROKU",
    "OEM_FJ_LOYA",
    "OEM_FJ_ROYA",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "LShift",
    "RShift",
    "LCtrl",
    "RCtrl",
    "LMENU",
    "RMENU"};
static const int KeyCodes[] = {
    0x0, // Undefined
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07, // Undefined
    0x08,
    0x09,
    0x0A, // Reserved
    0x0B, // Reserved
    0x0C,
    0x0D,
    0x0E, // Undefined
    0x0F, // Undefined
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16, // IME On
    0x17,
    0x18,
    0x19,
    0x1A, // IME Off
    0x1B,
    0x1C,
    0x1D,
    0x1E,
    0x1F,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2A,
    0x2B,
    0x2C,
    0x2D,
    0x2E,
    0x2F,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3A, // Undefined
    0x3B, // Undefined
    0x3C, // Undefined
    0x3D, // Undefined
    0x3E, // Undefined
    0x3F, // Undefined
    0x40, // Undefined
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A,
    0x4B,
    0x4C,
    0x4B,
    0x4E,
    0x4F,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5A,
    0x5B,
    0x5C,
    0x5D,
    0x5E, // Rservered
    0x5F,
    0x60, // Numpad1
    0x61, // Numpad2
    0x62, // Numpad3
    0x63, // Numpad4
    0x64, // Numpad5
    0x65, // Numpad6
    0x66, // Numpad7
    0x67, // Numpad8
    0x68, // Numpad8
    0x69, // Numpad9
    0x6A,
    0x6B,
    0x6C,
    0x6D,
    0x6E,
    0x6F,
    0x70, // F1
    0x71, // F2
    0x72, // F3
    0x73, // F4
    0x74, // F5
    0x75, // F6
    0x76, // F7
    0x77, // F8
    0x78, // F9
    0x79, // F10
    0x7A, // F11
    0x7B, // F12
    0x7C, // F13
    0x7D, // F14
    0x7E, // F15
    0x7F, // F16
    0x80, // F17
    0x81, // F18
    0x82, // F19
    0x83, // F20
    0x84, // F21
    0x85, // F22
    0x86, // F23
    0x87, // F24
    0x88, // Unkown
    0x89, // Unkown
    0x8A, // Unkown
    0x8B, // Unkown
    0x8C, // Unkown
    0x8D, // Unkown
    0x8E, // Unkown
    0x8F, // Unkown
    0x90,
    0x91,
    0x92, // OEM Specific
    0x93, // OEM Specific
    0x94, // OEM Specific
    0x95, // OEM Specific
    0x96, // OEM Specific
    0x97, // Unkown
    0x98, // Unkown
    0x99, // Unkown
    0x9A, // Unkown
    0x9B, // Unkown
    0x9C, // Unkown
    0x9D, // Unkown
    0x9E, // Unkown
    0x9F, // Unkown
    0xA0,
    0xA1,
    0xA2,
    0xA3,
    0xA4,
    0xA5};
VOID RandomINT()
{
    while (true)
    {
        if (Randintaim == 0)
            Randintaim = 1;
        else if (Randintaim == 1)
            Randintaim = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}
VOID FindPoint(VECTOR2 point, int screenwidth, int screenheight, int degrees)
{
    float x2 = screenwidth / 2;
    float y2 = screenheight / 2;
    float d = sqrt(pow((point.X - x2), 2) + (pow((point.Y - y2), 2))); // Distance
    float r = degrees / d;                                             // Segment ratio

    point.X = r * point.X + (1 - r) * x2; // find point that divides the segment
    point.Y = r * point.Y + (1 - r) * y2; // into the ratio (1-r):r
}
VOID DrawBorderChar(ImFont *pFont, FLOAT FontSize, const char *Text, FLOAT X, FLOAT Y, ImColor Color, bool Center = true, int timeout = 0, ImColor Border_Color = ImColor(0, 0, 0, 255))
{
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;

    bool chose = true;
    if (Center && chose)
    {
        ImVec2 TextSize = pFont->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Text);
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) + 1.0f, Y + 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) - 1.0f, Y - 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) + 1.0f, Y - 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) - 1.0f, Y + 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2(X - TextSize.x / 2.0f, Y), Color, Text);
    }
    else if (!Center && chose)
    {
        dl->AddText(pFont, FontSize, ImVec2(X - 1.0f, Y - 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2(X - 1.0f, Y + 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2(X + 1.0f, Y - 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2(X + 1.0f, Y + 1.0f), Border_Color, Text);
        dl->AddText(pFont, FontSize, ImVec2(X, Y), Color, Text);
    }
    else if (!chose && Center)
    {
        ImVec2 TextSize = pFont->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Text);
        dl->AddText(pFont, FontSize, ImVec2(X - TextSize.x / 2.0f, Y), Color, Text);
    }
    else if (!chose && !Center)
    {
        dl->AddText(pFont, FontSize, ImVec2(X, Y), Color, Text);
    }
}
VOID DrawBorderString(ImFont *pFont, FLOAT FontSize, string &Text, FLOAT X, FLOAT Y, ImColor Color, bool Center = true, int timeout = 0, ImColor Border_Color = ImColor(0, 0, 0, 255))
{
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;

    bool chose = Setting::BorderString;
    if (Center && chose)
    {
        ImVec2 TextSize = pFont->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) + 1.0f, Y + 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) - 1.0f, Y - 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) + 1.0f, Y - 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2((X - TextSize.x / 2.0f) - 1.0f, Y + 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2(X - TextSize.x / 2.0f, Y), Color, Text.c_str());
    }
    else if (!Center && chose)
    {
        dl->AddText(pFont, FontSize, ImVec2(X - 1.0f, Y - 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2(X - 1.0f, Y + 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2(X + 1.0f, Y - 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2(X + 1.0f, Y + 1.0f), Border_Color, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2(X, Y), Color, Text.c_str());
    }
    else if (!chose && Center)
    {
        ImVec2 TextSize = pFont->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Text.c_str());
        dl->AddText(pFont, FontSize, ImVec2(X - TextSize.x / 2.0f, Y), Color, Text.c_str());
    }
    else if (!chose && !Center)
    {
        dl->AddText(pFont, FontSize, ImVec2(X, Y), Color, Text.c_str());
    }
}
VOID DrawLineMenu(ImDrawList *drawlist, float x1, float y1, float x2, float y2, ImColor Color, float thickness)
{
    drawlist->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Color, thickness);
}
VOID CornerBoxMenu(ImDrawList *drawlist, int x, int y, int w, int h, int thickness, float cornered, ImColor color)
{
    // Left Top
    DrawLineMenu(drawlist, x, y, x + (w * cornered), y, color, thickness);
    DrawLineMenu(drawlist, x, y, x, y + (h * cornered), color, thickness);
    // Left Bottom
    DrawLineMenu(drawlist, x, y + h, x + (w * cornered), y + h, color, thickness);
    DrawLineMenu(drawlist, x, y + h, x, (y + h) - (h * cornered), color, thickness);
    // Right Top
    DrawLineMenu(drawlist, x + w, y, (x + w) - (w * cornered), y, color, thickness);
    DrawLineMenu(drawlist, x + w, y, x + w, y + (h * cornered), color, thickness);
    // Right Bottom
    DrawLineMenu(drawlist, x + w, y + h, (x + w) - (w * cornered), y + h, color, thickness);
    DrawLineMenu(drawlist, x + w, y + h, x + w, (y + h) - (h * cornered), color, thickness);
}
VOID DrawLine(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, ImColor Color, FLOAT Thickness = 1)
{
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X1, Y1), ImVec2(X2, Y2), Color, Thickness);
}
VOID DrawLineForeground(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, ImColor Color, FLOAT Thickness = 1)
{
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (dl) dl->AddLine(ImVec2(X1, Y1), ImVec2(X2, Y2), Color, Thickness);
}
VOID DrawCircleForeground(FLOAT X, FLOAT Y, FLOAT Radius, ImColor Color, FLOAT Thickness = 1)
{
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (dl) dl->AddCircle(ImVec2(X, Y), Radius, Color, 0, Thickness);
}
VOID DrawFilledRectangle(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, ImColor Color)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(X, Y), ImVec2(X - Width, Y - Height), Color);
}
VOID DrawFilledRectangle2(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, ImColor Color)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(X, Y), ImVec2(X + Width, Y + Height), Color);
}
VOID DrawRectangle(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, ImColor Color, FLOAT Thickness = 1)
{
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + Width, Y + Height), Color, 0, 0, Thickness);
}
VOID DrawRectangle2(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, ImColor Color, FLOAT Thickness = 1)
{
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(X, Y), ImVec2(X - Width, Y - Height), Color, 0, 0, Thickness);
}
VOID DrawCircle(FLOAT X, FLOAT Y, FLOAT Radius, ImColor Color, FLOAT Thickness = 1)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(X, Y), Radius, Color, 0, Thickness);
}
VOID DrawFOVRing(FLOAT fX, FLOAT fY, FLOAT fRadius, ImColor Color, FLOAT Thickness = 1.5f)
{
    float time = (float)ImGui::GetTime();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;

    // 1. Subtle Pulse (Brighter, less darkness)
    float pulse = (0.93f + 0.07f * sin(time * 3.0f));
    ImColor animatedCol = Color;
    animatedCol.Value.w *= pulse;

    // --- THREE CONCENTRIC RINGS ---

    // LAYER 3: INNER RING (7 Long Dashes)
    const int segments3 = 70;
    float step3 = 2.0f * IM_PI / segments3;
    float rot3 = time * 2.0f; // Increased speed (from 1.2f)
    float fRad3 = fRadius - 4.0f;

    for (int i = 0; i < segments3; i++) {
        if (i % 10 < 7) { // 7 long segments
            float a1 = i * step3 + rot3;
            float a2 = (i + 1) * step3 + rot3;
            dl->AddLine(ImVec2(fX + fRad3 * cos(a1), fY + fRad3 * sin(a1)), ImVec2(fX + fRad3 * cos(a2), fY + fRad3 * sin(a2)), animatedCol, 2.5f);
        }
    }
}
VOID DrawPremiumESP(ImFont* pFont, FLOAT X, FLOAT Y, FLOAT boxH, const std::string& Name, FLOAT Health, FLOAT HealthMax, FLOAT Distance, ImColor NameCol, bool IsBot)
{
    // Use Background list to avoid menu overlapping
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;

    bool showHealth = Setting::Player::Miscellanouse::Health;

    // 1. Draw Vertical Health Bar (Type 0)
    if (showHealth && Setting::Player::Miscellanouse::HealthID == 0) {
        float healthPercent = Health / (HealthMax > 0 ? HealthMax : 100.0f);
        if (healthPercent > 1.0f) healthPercent = 1.0f;
        if (healthPercent < 0.0f) healthPercent = 0.0f;

        ImColor healthCol = ImColor(0, 255, 50, 255); 
        if (healthPercent < 0.35f) healthCol = ImColor(255, 30, 30, 255); 
        else if (healthPercent < 0.75f) healthCol = ImColor(255, 180, 0, 255); 

        // Type 0: Vertical Bar (Safe Hack style)
        float barW = 3.0f;
        float barX = X - (boxH / 4) - 4.0f; 
        float barY_Bottom = Y + boxH; 
        
        dl->AddRectFilled(ImVec2(barX, Y), ImVec2(barX + barW, barY_Bottom), ImColor(0, 0, 0, 180), 0.0f);
        if (healthPercent > 0) {
            float fillH = boxH * healthPercent;
            dl->AddRectFilled(ImVec2(barX, barY_Bottom), ImVec2(barX + barW, barY_Bottom - fillH), healthCol, 0.0f);
        }
    }
}

VOID DrawCircleFilled(FLOAT X, FLOAT Y, FLOAT Radius, ImColor Color, FLOAT Thickness = 1)
{
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(X, Y), Radius, Color, 0);
}
VOID DrawCircle3D(FLOAT x1, FLOAT y1, FLOAT z1, FLOAT radius, ImColor color, FLOAT thickness = 1)
{
    const int duong_thang = 50;
    float step = 2 * M_PI / duong_thang;
    VECTOR3 prev;
    VECTOR3 origin;
    origin.X = x1;
    origin.Y = y1;
    origin.Z = z1;
    VECTOR3 originvec3;
    originvec3.X = origin.X + radius;
    originvec3.Y = origin.Y;
    originvec3.Z = origin.Z;
    VECTOR3 ScreenOut;
    INT Distance;
    bool curValid = Algorithm::WorldToScreen(originvec3, ScreenOut, Data::ViewMatrixBase);
    prev.X = ScreenOut.X;
    prev.Y = ScreenOut.Y;
    prev.Z = ScreenOut.Z;
    for (int i = 1; i <= duong_thang; ++i)
    {
        float angle = i * step;
        VECTOR3 originew;
        originew.X = origin.X + radius * cos(angle);
        originew.Y = origin.Y + radius * sin(angle);
        originew.Z = origin.Z;
        VECTOR3 Curvec3;
        VECTOR3 ScreenOut1;
        bool nextValid = Algorithm::WorldToScreen(originew, ScreenOut1, Data::ViewMatrixBase);
        VECTOR3 cur;
        cur.X = ScreenOut1.X;
        cur.Y = ScreenOut1.Y;
        cur.Z = ScreenOut1.Z;
        if (curValid && nextValid)
        {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(prev.X, prev.Y), ImVec2(ScreenOut1.X, ScreenOut1.Y), color, thickness);
        }
        curValid = nextValid;
        prev = cur;
    }
}

VOID DrawRedZonePremium(FLOAT x1, FLOAT y1, FLOAT z1, FLOAT radius, ImColor color, FLOAT thickness)
{
    float time = (float)ImGui::GetTime();
    float pulseAlpha = (0.5f + 0.5f * sin(time * 3.0f));
    
    ImColor animatedColor = color;
    animatedColor.Value.w *= (0.7f + 0.3f * pulseAlpha);

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    const int segments = 60;
    float step = 2.0f * M_PI / segments;
    float rotation = time * 0.6f;

    VECTOR3 origin = { x1, y1, z1 };
    VECTOR3 p1, p2, s1, s2;

    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * step + rotation;
        float angle2 = (i + 1) * step + rotation;

        // Dash pattern
        if (i % 3 != 0) 
        {
            p1 = { origin.X + radius * cos(angle1), origin.Y + radius * sin(angle1), origin.Z };
            p2 = { origin.X + radius * cos(angle2), origin.Y + radius * sin(angle2), origin.Z };
            
            if (Algorithm::WorldToScreen(p1, s1, Data::ViewMatrixBase) && Algorithm::WorldToScreen(p2, s2, Data::ViewMatrixBase))
            {
                // Shadow for depth
                dl->AddLine(ImVec2(s1.X, s1.Y), ImVec2(s2.X, s2.Y), ImColor(0, 0, 0, 150), thickness + 1.5f);
                // Main Dash
                dl->AddLine(ImVec2(s1.X, s1.Y), ImVec2(s2.X, s2.Y), animatedColor, thickness);
            }
        }
    }
}
VOID DrawCenterZonePremium(VECTOR3 pos, ImColor color, FLOAT thickness)
{
    float time = (float)ImGui::GetTime();
    float pulseAlpha = (0.5f + 0.5f * sin(time * 2.0f));
    ImColor animatedColor = color;
    animatedColor.Value.w *= (0.4f + 0.6f * pulseAlpha);

    // 1. Rotating Rings on Ground
    const int segments = 40;
    float step = 2.0f * M_PI / segments;
    float rotation = time * 1.5f;
    float radius = 250.0f;

    VECTOR3 p1, p2, s1, s2;
    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * step + rotation;
        float angle2 = (i + 1) * step + rotation;

        p1 = { pos.X + radius * cos(angle1), pos.Y + radius * sin(angle1), pos.Z };
        p2 = { pos.X + radius * cos(angle2), pos.Y + radius * sin(angle2), pos.Z };

        if (Algorithm::WorldToScreen(p1, s1, Data::ViewMatrixBase) && Algorithm::WorldToScreen(p2, s2, Data::ViewMatrixBase))
        {
            DrawLine(s1.X, s1.Y, s2.X, s2.Y, animatedColor, thickness + 0.5f);
        }
    }

    // 2. Vertical Light Beam (Indicator)
    VECTOR3 beamBase = pos;
    VECTOR3 beamTop = { pos.X, pos.Y, pos.Z + 1000.f };
    VECTOR3 sBase, sTop;
    if (Algorithm::WorldToScreen(beamBase, sBase, Data::ViewMatrixBase) && Algorithm::WorldToScreen(beamTop, sTop, Data::ViewMatrixBase))
    {
        ImColor beamCol = color;
        beamCol.Value.w *= 0.3f; // Transparent beam
        DrawLine(sBase.X, sBase.Y, sTop.X, sTop.Y, beamCol, 1.0f);
    }
}
VOID Line(VECTOR3 origin, VECTOR3 dest, ImColor col, FLOAT Thickness = 1)
{
    DrawLine(origin.X, origin.Y, dest.X, dest.Y, col, Thickness);
}
VOID CornerBox(int x, int y, int w, int h, int thickness, float cornered, ImColor color)
{
    float llongcorner = w * cornered;
    // Left Top
    DrawLine(x, y, x + llongcorner, y, color, thickness);
    DrawLine(x, y, x, y + llongcorner, color, thickness);
    // Left Bottom
    DrawLine(x, y + h, x + llongcorner, y + h, color, thickness);
    DrawLine(x, y + h, x, (y + h) - llongcorner, color, thickness);
    // Right Top
    DrawLine(x + w, y, (x + w) - llongcorner, y, color, thickness);
    DrawLine(x + w, y, x + w, y + llongcorner, color, thickness);
    // Right Bottom
    DrawLine(x + w, y + h, (x + w) - llongcorner, y + h, color, thickness);
    DrawLine(x + w, y + h, x + w, (y + h) - llongcorner, color, thickness);
}
/* Unity-style vehicle HP bar (0-100): green >70, red 30-70, blue <30 */
static void VehicleDrawHPBar(float x, float y, float w, float h, float hpPct)
{
    if (hpPct > 70.f)
        DrawFilledRectangle2(x, y, w * hpPct / 100.f, h, ImColor(0.f, 1.f, 0.f, 1.f));
    else if (hpPct > 30.f)
        DrawFilledRectangle2(x, y, w * hpPct / 100.f, h, ImColor(1.f, 0.f, 0.f, 1.f));
    else if (hpPct > 0.f)
        DrawFilledRectangle2(x, y, w * hpPct / 100.f, h, ImColor(0.f, 0.f, 1.f, 1.f));
    CornerBox((int)x, (int)y, (int)w, (int)h, 1, 1.0f, ImColor(0.f, 0.f, 0.f, 0.5f));
}
/* Unity-style vehicle fuel bar (0-100) */
static void DrawTotalEnemiesRedBar()
{
    if (Data::NetDriver <= 0) return;
    if (Data::LocalbDead && !Setting::QoL::bESPWhenSpectating) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float density = 1.0f;
    static float rectPosX = 0.f;
    static float rectPosY = 70.0f;
    static float dragOffsetX = 0.f;
    static float dragOffsetY = 0.f;
    static bool isDragging = false;
    static bool posInit = false;
    if (!posInit) {
        rectPosX = (float)(ScreenInfo::Width / 2);
        posInit = true;
    }
    float rectWidth = Setting::SpecialFeatures::RedBarLength;
    float rectHeight = Setting::SpecialFeatures::RedBarHeight;
    if (ImGui::IsMouseClicked(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        if (mousePos.x >= rectPosX - rectWidth && mousePos.x <= rectPosX + rectWidth && mousePos.y >= rectPosY && mousePos.y <= rectPosY + rectHeight) {
            dragOffsetX = mousePos.x - rectPosX;
            dragOffsetY = mousePos.y - rectPosY;
            isDragging = true;
        }
    }
    if (isDragging && ImGui::IsMouseDragging(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        rectPosX = mousePos.x - dragOffsetX;
        rectPosY = mousePos.y - dragOffsetY;
    }
    if (ImGui::IsMouseReleased(0)) isDragging = false;

    int botCount = 0;
    for (const auto& a : Data::AActorList) { if (a.IsBot) botCount++; }
    int playerCount = (int)Data::AActorList.size() - botCount;

    std::string s = "  PLAYERS  ";
    s.append("(" + std::to_string(playerCount) + ")  ");
    s.append(" || ");
    s.append("  BOTS  ");
    s.append("(" + std::to_string(botCount) + ")");

    float fontSz = density * 18.5f;

    /* Dark red bar (no bright red) */
    draw->AddRectFilled(ImVec2(rectPosX - rectWidth, rectPosY), ImVec2(rectPosX + rectWidth, rectPosY + rectHeight), IM_COL32(158, 20, 20, 200));

    draw->AddRectFilledMultiColor(ImVec2(rectPosX - rectWidth - 40, rectPosY), ImVec2(rectPosX - rectWidth, rectPosY + rectHeight),
        IM_COL32(158, 20, 20, 0), IM_COL32(158, 20, 20, 200), IM_COL32(158, 20, 20, 200), IM_COL32(158, 20, 20, 0));
    draw->AddRectFilledMultiColor(ImVec2(rectPosX + rectWidth, rectPosY), ImVec2(rectPosX + rectWidth + 40, rectPosY + rectHeight),
        IM_COL32(158, 20, 20, 200), IM_COL32(158, 20, 20, 0), IM_COL32(158, 20, 20, 0), IM_COL32(158, 20, 20, 160));

    ImVec2 textSize = ImGui::CalcTextSize(s.c_str(), nullptr, false, -1.0f);
    /* Text position: user-adjustable left/right via RedBarTextOffset */
    float textX = rectPosX - (textSize.x * 0.5f) + Setting::SpecialFeatures::RedBarTextOffset;
    float textY = rectPosY + (rectHeight - textSize.y) * 0.5f;
    ImVec2 textPos = ImVec2(textX + 1.0f, textY);
    draw->AddText(nullptr, fontSz, textPos, IM_COL32(0, 0, 0, 90), s.c_str());
    draw->AddText(nullptr, fontSz, ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), s.c_str());
}

/* Left-side: F4 Aimbot text only (no background). Home = hide/show. Draggable when hack menu is open. */
static bool s_showF4AimbotLeftMenu = true;


static void DrawKeyHints()
{
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float y = ScreenInfo::Height - 80.f;
    const char* hints = "F4 = Aimbot | Home = Panel | INSERT = Menu";
    draw->AddText(nullptr, 14.f, ImVec2(ScreenInfo::Width * 0.5f - 120.f, y), IM_COL32(200, 200, 200, 220), hints);
}

static void DrawFPSOverlay()
{
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float fps = ImGui::GetIO().Framerate;
    char buf[32];
    snprintf(buf, sizeof(buf), "FPS: %.0f", fps);
    draw->AddText(nullptr, 18.f, ImVec2(ScreenInfo::Width - 70.f, 8.f), IM_COL32(0, 255, 0, 255), buf);
}

static void DrawRadar()
{
    if (Data::LocalbDead && !Setting::QoL::bESPWhenSpectating) return;
    if (Data::AActorList.empty() && Data::VehicleList.empty()) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    const float radarX = ScreenInfo::Width - 160.f;
    const float radarY = 40.f;
    const float radarSize = 120.f;
    const float scale = 0.08f;
    draw->AddRectFilled(ImVec2(radarX, radarY), ImVec2(radarX + radarSize, radarY + radarSize), IM_COL32(0, 0, 0, 140));
    draw->AddRect(ImVec2(radarX, radarY), ImVec2(radarX + radarSize, radarY + radarSize), IM_COL32(255, 255, 255, 180), 0.f, 0, 1.5f);
    float cx = radarX + radarSize * 0.5f;
    float cy = radarY + radarSize * 0.5f;
    for (const auto& a : Data::AActorList)
    {
        if (a.IsDead) continue;
        float dx = (a.Position.X - Data::LocalPosition.X) * scale;
        float dy = (a.Position.Y - Data::LocalPosition.Y) * scale;
        if (dx * dx + dy * dy > 3600.f) continue;
        float px = cx + dx;
        float py = cy - dy;
        if (px >= radarX + 2 && px <= radarX + radarSize - 2 && py >= radarY + 2 && py <= radarY + radarSize - 2)
            draw->AddCircleFilled(ImVec2(px, py), 2.f, IM_COL32(255, 80, 80, 255));
    }
    for (const auto& v : Data::VehicleList)
    {
        float dx = (v.Position.X - Data::LocalPosition.X) * scale;
        float dy = (v.Position.Y - Data::LocalPosition.Y) * scale;
        if (dx * dx + dy * dy > 3600.f) continue;
        float px = cx + dx;
        float py = cy - dy;
        if (px >= radarX + 2 && px <= radarX + radarSize - 2 && py >= radarY + 2 && py <= radarY + radarSize - 2)
            draw->AddCircleFilled(ImVec2(px, py), 2.5f, IM_COL32(80, 200, 255, 255));
    }
    draw->AddCircleFilled(ImVec2(cx, cy), 3.f, IM_COL32(0, 255, 0, 255));
}

static void DrawSafeZoneTimer()
{
    if (Data::NetDriver <= 0 || Data::CurCircleWave < 0) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    char buf[64];
    snprintf(buf, sizeof(buf), "Wave: %d | Circle: %.0f", Data::CurCircleWave, Data::CirclePain);
    draw->AddText(nullptr, 16.f, ImVec2(ScreenInfo::Width * 0.5f - 80.f, 8.f), IM_COL32(255, 220, 100, 255), buf);
}

static void DrawCustomCrosshair()
{
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float cx = ScreenInfo::Width * 0.5f;
    float cy = ScreenInfo::Height * 0.5f;
    float s = Setting::QoL::fCrosshairSize;
    ImU32 col = IM_COL32((int)(Setting::QoL::fCrosshairColor[0] * 255), (int)(Setting::QoL::fCrosshairColor[1] * 255), (int)(Setting::QoL::fCrosshairColor[2] * 255), (int)(Setting::QoL::fCrosshairColor[3] * 255));
    draw->AddLine(ImVec2(cx - s, cy), ImVec2(cx + s, cy), col, 2.f);
    draw->AddLine(ImVec2(cx, cy - s), ImVec2(cx, cy + s), col, 2.f);
}

static void DrawMatchStats()
{
    if (Data::NetDriver <= 0) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    char buf[96];
    if (Data::ElapsedTime < 60)
        snprintf(buf, sizeof(buf), "Alive: %d | Teams: %d | Time: %ds", Data::AlivePlayer, Data::AliveTeam, Data::ElapsedTime);
    else
        snprintf(buf, sizeof(buf), "Alive: %d | Teams: %d | Time: %d:%02d min", Data::AlivePlayer, Data::AliveTeam, Data::ElapsedTime / 60, Data::ElapsedTime % 60);
    draw->AddText(nullptr, 15.f, ImVec2(10.f, ScreenInfo::Height - 42.f), IM_COL32(180, 220, 255, 255), buf);
}

static void DrawPingOverlay()
{
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    draw->AddText(nullptr, 14.f, ImVec2(ScreenInfo::Width - 70.f, 28.f), IM_COL32(200, 200, 100, 255), "Ping: --");
}

static void DrawSessionTimer()
{
    if (Data::NetDriver <= 0) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    int m = Data::ElapsedTime / 60, s = Data::ElapsedTime % 60;
    char buf[32];
    snprintf(buf, sizeof(buf), "Session: %d:%02d", m, s);
    draw->AddText(nullptr, 15.f, ImVec2(10.f, 28.f), IM_COL32(150, 255, 150, 255), buf);
}

static void DrawCompass()
{
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float cx = ScreenInfo::Width * 0.5f;
    float y = 30.f;
    draw->AddText(nullptr, 18.f, ImVec2(cx - 8.f, y), IM_COL32(255, 255, 255, 255), "N");
    draw->AddLine(ImVec2(cx - 30.f, y + 20.f), ImVec2(cx + 30.f, y + 20.f), IM_COL32(200, 200, 200, 180), 1.f);
}

static void DrawWaypoint()
{
    if (!Setting::QoL::bWaypoint || (Setting::QoL::fWaypointX == 0.f && Setting::QoL::fWaypointY == 0.f && Setting::QoL::fWaypointZ == 0.f)) return;
    VECTOR3 wp = { Setting::QoL::fWaypointX, Setting::QoL::fWaypointY, Setting::QoL::fWaypointZ };
    VECTOR3 screen;
    if (!Algorithm::WorldToScreen(wp, screen, Data::ViewMatrixBase)) return;
    float dist = sqrtf(powf(Data::LocalPosition.X - wp.X, 2) + powf(Data::LocalPosition.Y - wp.Y, 2) + powf(Data::LocalPosition.Z - wp.Z, 2)) / 100.f;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    draw->AddCircleFilled(ImVec2(screen.X, screen.Y), 6.f, IM_COL32(255, 200, 0, 255));
    char buf[48];
    snprintf(buf, sizeof(buf), "%.0fm", dist);
    draw->AddText(nullptr, 14.f, ImVec2(screen.X - 15.f, screen.Y - 22.f), IM_COL32(255, 255, 0, 255), buf);
}

static void DrawSpectatorList()
{
    if (!Setting::QoL::bShowSpectatorList || Data::NetDriver <= 0) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float x = ScreenInfo::Width - 220.f;
    float y = 190.f;
    draw->AddRectFilled(ImVec2(x, y), ImVec2(x + 210.f, y + 60.f), IM_COL32(0, 0, 0, 180));
    draw->AddRect(ImVec2(x, y), ImVec2(x + 210.f, y + 60.f), IM_COL32(255, 200, 0, 200));
    draw->AddText(nullptr, 14.f, ImVec2(x + 5.f, y + 4.f), IM_COL32(255, 220, 100, 255), "Spectating you:");
    if (Data::SpectatorNames.empty())
        draw->AddText(nullptr, 13.f, ImVec2(x + 5.f, y + 26.f), IM_COL32(180, 180, 180, 255), "No one spectating");
    else
        for (size_t i = 0; i < Data::SpectatorNames.size() && i < 3; i++)
            draw->AddText(nullptr, 13.f, ImVec2(x + 5.f, y + 26.f + (float)(i * 18)), IM_COL32(255, 255, 200, 255), Data::SpectatorNames[i].c_str());
}

static void DrawKeybindList()
{
    if (!Setting::MenuExtras::bKeybindList || !Setting::ShowMenu) return;
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    float x = ScreenInfo::Width - 220.f;
    float y = 60.f;
    draw->AddRectFilled(ImVec2(x, y), ImVec2(x + 210.f, y + 120.f), IM_COL32(0, 0, 0, 160));
    draw->AddRect(ImVec2(x, y), ImVec2(x + 210.f, y + 120.f), IM_COL32(255, 255, 255, 150));
    draw->AddText(nullptr, 14.f, ImVec2(x + 5.f, y + 2.f), IM_COL32(255, 255, 0, 255), "F4 = Aimbot");
    draw->AddText(nullptr, 14.f, ImVec2(x + 5.f, y + 22.f), IM_COL32(200, 200, 255, 255), "Home = Panel");
    draw->AddText(nullptr, 14.f, ImVec2(x + 5.f, y + 42.f), IM_COL32(200, 255, 200, 255), "INSERT = Menu");
    draw->AddText(nullptr, 14.f, ImVec2(x + 5.f, y + 62.f), IM_COL32(255, 200, 200, 255), "F5 = Hide Items/Vehicle");
    draw->AddText(nullptr, 14.f, ImVec2(x + 5.f, y + 82.f), IM_COL32(255, 220, 180, 255), "F6 = Aimbot Type");
}

static void VehicleDrawOilBar(float x, float y, float w, float h, float fuelPct)
{
    if (fuelPct > 0.f)
        DrawFilledRectangle2(x, y, w * fuelPct / 100.f, h, ImColor(1.f, 1.f, 0.f, 1.f));
    CornerBox((int)x, (int)y, (int)w, (int)h, 1, 1.0f, ImColor(0.f, 0.f, 0.f, 0.5f));
}
/* Unity-style 3D box: draw one edge in world space */
static void BOX3d_LINE(VECTOR3 center, float x1, float y1, float z1, float x2, float y2, float z2, ImU32 col, float thickness)
{
    VECTOR3 p1 = { center.X + x1, center.Y + y1, center.Z + z1 };
    VECTOR3 p2 = { center.X + x2, center.Y + y2, center.Z + z2 };
    VECTOR3 s1, s2;
    if (!Algorithm::WorldToScreen(p1, s1, Data::ViewMatrixBase) || !Algorithm::WorldToScreen(p2, s2, Data::ViewMatrixBase))
        return;
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(s1.X, s1.Y), ImVec2(s2.X, s2.Y), col, thickness);
}
static void Draw3dBOX(VECTOR3 center, float w, float h, ImU32 col, float thickness = 1.f)
{
    const float padding = 10.f;
    BOX3d_LINE(center, -w, -w, 0, w, -w, 0, col, thickness);
    BOX3d_LINE(center, -w, -w, h, w, -w, h, col, thickness);
    BOX3d_LINE(center, -w, -w, 0, -w, -w, h, col, thickness);
    BOX3d_LINE(center, w, -w, 0, w, -w, h, col, thickness);
    BOX3d_LINE(center, w, w + padding, 0, w, w + padding, h, col, thickness);
    BOX3d_LINE(center, -w, w + padding, 0, -w, w + padding, h, col, thickness);
    BOX3d_LINE(center, w, w + padding, 0, -w, w + padding, 0, col, thickness);
    BOX3d_LINE(center, w, w + padding, h, -w, w + padding, h, col, thickness);
    BOX3d_LINE(center, -w, -w, 0, -w, w + padding, 0, col, thickness);
    BOX3d_LINE(center, w, w + padding, 0, w, -w, 0, col, thickness);
    BOX3d_LINE(center, -w, -w, h, -w, w + padding, h, col, thickness);
    BOX3d_LINE(center, w, w + padding, h, w, -w, h, col, thickness);
}
VOID DrawChat(const char *Chat, const char *Name, ImVec4 ColorUser, FLOAT PosYFix, ImVec4 ColorChat = ImColor(255, 255, 255, 255))
{
    ImGui::SetCursorPosY(PosYFix);
    float old_size_font = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.5f;
    ImGui::PushFont(ImGui::GetFont());
    ImGui::Text(ICON_FA_USER_CIRCLE);
    ImGui::GetFont()->Scale = old_size_font;
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ColorUser);
    ImGui::Text(Name);
    ImGui::PopStyleColor();
    ImGui::SetCursorPosX(27);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorChat);
    ImGui::Text(Chat);
    ImGui::PopStyleColor();
}
VOID DrawChatByList(vector<string> ChatList, const char *Name, ImVec4 ColorUser, FLOAT PosYFix, ImVec4 ColorChat = ImColor(255, 255, 255, 255))
{
    ImGui::SetCursorPosY(PosYFix);
    float old_size_font = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.5f;
    ImGui::PushFont(ImGui::GetFont());
    ImGui::Text(ICON_FA_USER_CIRCLE);
    ImGui::GetFont()->Scale = old_size_font;
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ColorUser);
    ImGui::Text(Name);
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text, ColorChat);
    for (std::string &ChatText : ChatList)
    {
        ImGui::SetCursorPosX(27);
        ImGui::Text(ChatText.c_str());
    }
    ImGui::PopStyleColor();
}
VOID ButtonSetID(const char *label, const ImVec2 &size_arg, int *Index, int Set)
{
    if (*Index == Set)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.26f, 0.26f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.26f, 0.26f, 0.26f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f)); /* selected tab: icon + text red */
    }
    if (ImGui::Button(label, size_arg))
        *Index = Set;
    if (*Index == Set)
        ImGui::PopStyleColor(4);
}
VOID OneLineDrawTable(const char *Label, bool *v, float *d, float Col[4])
{
    const char *word = "###ABCD";
    const char *word2 = "_COL";
    const char *word3 = "###BOOL_";
    char *Finalcheckbox = new char[strlen(word3) + strlen(Label) + 1];
    strcpy(Finalcheckbox, word3);
    strcat(Finalcheckbox, Label);
    char *FinalWord = new char[strlen(word) + strlen(Label) + 1];
    strcpy(FinalWord, word);
    strcat(FinalWord, Label);
    char *FinalWordCol = new char[strlen(FinalWord) + strlen(word2) + 1];
    strcpy(FinalWordCol, FinalWord);
    strcat(FinalWordCol, word2);
    ImGui::Checkbox(Finalcheckbox, v);
    ImGui::TableNextColumn();
    ImGui::Text(Label);
    ImGui::TableNextColumn();
    ImGui::ColorEdit4(FinalWordCol, Col, ImGuiColorEditFlags_NoInputs);
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::DragFloat(FinalWord, d, 1, 0, 30);
    ImGui::PopItemWidth();
    ImGui::TableNextColumn();
    delete[] Finalcheckbox;
    delete[] FinalWord;
    delete[] FinalWordCol;
}
VOID GetInputKey(int *k)
{
    for (int button = 0; button < 256; button++)
    {
        if (GetAsyncKeyState(button) & 1)
        {
            *(int *)k = button;
            continue;
        }
    }
}
VOID BindKey(int *Value, const ImVec2 &size_arg = ImVec2(0, 0), const char *Label = "")
{
    ImGui::Hotkey(Label, Value, size_arg);
}
VOID BindKeyA(int *Value, const ImVec2 &size_arg = ImVec2(0, 0), const char *Label = "")
{
    string textbindkey;
    if (*Value == 0x0)
        textbindkey = (Setting::LanguageVNM == true) ? (u8"(chưa cài đặt)") : ("(not install)");
    else
        textbindkey = KeyNames[*Value];
    if (Label != "")
    {
        ImGui::PushID(Label);
    }
    if (ImGui::Button((Setting::LanguageVNM == true) ? (u8"Nhấn vào đây để đổi phím") : ("Click here to bind key "), size_arg))
    {
        if (Label != "")
        {
            ImGui::PopID();
        }
        bool IsChose = false;
        // std::thread ThreadInput(GetInputKey, Value);
        // ThreadInput.join( );
        while (true)
        {
            Sleep(1);
            for (int button = 0; button < 256; button++)
            {
                if (GetAsyncKeyState(button) & 1)
                {
                    textbindkey = KeyNames[button];
                    IsChose = true;
                    *(int *)Value = button;
                    continue;
                }
            }
            if (IsChose)
                break;
        }
    };
    ImGui::SameLine();
    ImGui::Text(textbindkey.c_str());
}
VOID CheckBoxColored(const char *label, bool *v, ImVec4 color = ImVec4(1, 1, 1, 1))
{
    ImGui::PushStyleColor(ImGuiCol_CheckMark, color);
    ImGui::Checkbox(label, v);
    ImGui::PopStyleColor();
}
VOID HelpMarker(const char *desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
VOID RenderStringInputText2(int &String, string Variable, int i)
{
    char buffer[256]; // Buffer for the input text
    strncpy(buffer, std::to_string(String).c_str(), sizeof(buffer));
    std::string IdentiferID = "###" + Variable + std::to_string(i) + "Text";
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText(IdentiferID.c_str(), buffer, sizeof(buffer)))
    {
        string StringValue = buffer;
        String = std::stoi(StringValue);
    }
    ImGui::PopItemWidth();
}
VOID RenderStringInputText(string &String, string Variable, int i, bool IsPattern = false)
{
    char buffer[256]; // Buffer for the input text
    strncpy(buffer, String.c_str(), sizeof(buffer));
    if (IsPattern == true)
    {
        std::memset(buffer, '\0', sizeof(buffer));
        std::string strShowText = String.substr(0, 8);
        strncpy(buffer, strShowText.c_str(), sizeof(buffer));
    }
    std::string IdentiferID = "###" + Variable + std::to_string(i);
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText(IdentiferID.c_str(), buffer, sizeof(buffer)))
    {
        String = buffer;
    }
    ImGui::PopItemWidth();
}
VOID DrawTextCentered(const char *text)
{
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(text).x) / 2.f);
    ImGui::Text(text);
}
FVector RotatorToVector(const FRotator &Rotator)
{
    float RadPitch = Rotator.Pitch * (float)3.14159265358979323846 / 180.f;
    float RadYaw = Rotator.Yaw * (float)3.14159265358979323846 / 180.f;

    float X = std::cos(RadPitch) * std::cos(RadYaw);
    float Y = std::cos(RadPitch) * std::sin(RadYaw);
    float Z = std::sin(RadPitch);

    return {X, Y, Z};
}
FVector CalculateGrenadeDropPosition(const FVector &InitialPosition, const FVector &PlayerForwardVector, float Speed, float Time, float Gravity)
{
    float X = InitialPosition.X + Speed * Time * PlayerForwardVector.X;
    float Y = InitialPosition.Y + Speed * Time * PlayerForwardVector.Y;
    float Z = InitialPosition.Z + Speed * Time * PlayerForwardVector.Z - 0.5f * Gravity * std::pow(Time, 2);

    return {X, Y, Z};
}
VOID DrawItemBox(string StrName, string DrawText, float X1, float Y1)
{
#pragma region Rifles
    VECTOR2 Screen;
    Screen.X = X1;
    Screen.Y = Y1;
    if (StrName.find("M16A4") != std::string::npos && Setting::PickupItems::Rifles::M16A4)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::M16A4_Color[0], Setting::PickupItems::Rifles::M16A4_Color[1], Setting::PickupItems::Rifles::M16A4_Color[2], Setting::PickupItems::Rifles::M16A4_Color[3]));
    }
    if (StrName.find("SCARL") != std::string::npos && Setting::PickupItems::Rifles::SCARL)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::SCARL_Color[0], Setting::PickupItems::Rifles::SCARL_Color[1], Setting::PickupItems::Rifles::SCARL_Color[2], Setting::PickupItems::Rifles::SCARL_Color[3]));
    }
    if (StrName.find("M416") != std::string::npos && Setting::PickupItems::Rifles::M416)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::M416_Color[0], Setting::PickupItems::Rifles::M416_Color[1], Setting::PickupItems::Rifles::M416_Color[2], Setting::PickupItems::Rifles::M416_Color[3]));
    }
    if (StrName.find("FAMAS") != std::string::npos && Setting::PickupItems::Rifles::FAMAS)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::FAMAS_Color[0], Setting::PickupItems::Rifles::FAMAS_Color[1], Setting::PickupItems::Rifles::FAMAS_Color[2], Setting::PickupItems::Rifles::FAMAS_Color[3]));
    }
    if (StrName.find("QBZ") != std::string::npos && Setting::PickupItems::Rifles::QBZ)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::QBZ_Color[0], Setting::PickupItems::Rifles::QBZ_Color[1], Setting::PickupItems::Rifles::QBZ_Color[2], Setting::PickupItems::Rifles::QBZ_Color[3]));
    }
    if (StrName.find("G63C") != std::string::npos && Setting::PickupItems::Rifles::G63C)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::G63C_Color[0], Setting::PickupItems::Rifles::G63C_Color[1], Setting::PickupItems::Rifles::G63C_Color[2], Setting::PickupItems::Rifles::G63C_Color[3]));
    }
    if (StrName.find("AUG") != std::string::npos && Setting::PickupItems::Rifles::AUG)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::AUG_Color[0], Setting::PickupItems::Rifles::AUG_Color[1], Setting::PickupItems::Rifles::AUG_Color[2], Setting::PickupItems::Rifles::AUG_Color[3]));
    }
    if (StrName.find("AKM") != std::string::npos && Setting::PickupItems::Rifles::AKM)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::AKM_Color[0], Setting::PickupItems::Rifles::AKM_Color[1], Setting::PickupItems::Rifles::AKM_Color[2], Setting::PickupItems::Rifles::AKM_Color[3]));
    }
    if (StrName.find("M762") != std::string::npos && Setting::PickupItems::Rifles::M762)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::M762_Color[0], Setting::PickupItems::Rifles::M762_Color[1], Setting::PickupItems::Rifles::M762_Color[2], Setting::PickupItems::Rifles::M762_Color[3]));
    }
    if (StrName.find("HoneyBadger") != std::string::npos && Setting::PickupItems::Rifles::HoneyBadger)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::HoneyBadger_Color[0], Setting::PickupItems::Rifles::HoneyBadger_Color[1], Setting::PickupItems::Rifles::HoneyBadger_Color[2], Setting::PickupItems::Rifles::HoneyBadger_Color[3]));
    }
    if (StrName.find("GROZA") != std::string::npos && Setting::PickupItems::Rifles::GROZA)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::GROZA_Color[0], Setting::PickupItems::Rifles::GROZA_Color[1], Setting::PickupItems::Rifles::GROZA_Color[2], Setting::PickupItems::Rifles::GROZA_Color[3]));
    }
    if (StrName.find("MK14") != std::string::npos && Setting::PickupItems::Rifles::MK14)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::MK14_Color[0], Setting::PickupItems::Rifles::MK14_Color[1], Setting::PickupItems::Rifles::MK14_Color[2], Setting::PickupItems::Rifles::MK14_Color[3]));
    }
#pragma endregion
#pragma region SMG
    if (StrName.find("THOMPSON") != std::string::npos && Setting::PickupItems::SMG::THOMPSON)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::THOMPSON_Color[0], Setting::PickupItems::SMG::THOMPSON_Color[1], Setting::PickupItems::SMG::THOMPSON_Color[2], Setting::PickupItems::SMG::THOMPSON_Color[3]));
    }
    if (StrName.find("UMP45") != std::string::npos && Setting::PickupItems::SMG::UMP45)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::UMP45_Color[0], Setting::PickupItems::SMG::UMP45_Color[1], Setting::PickupItems::SMG::UMP45_Color[2], Setting::PickupItems::SMG::UMP45_Color[3]));
    }
    if (StrName.find("P90") != std::string::npos && Setting::PickupItems::SMG::P90)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::P90_Color[0], Setting::PickupItems::SMG::P90_Color[1], Setting::PickupItems::SMG::P90_Color[2], Setting::PickupItems::SMG::P90_Color[3]));
    }
    if (StrName.find("UZI") != std::string::npos && Setting::PickupItems::SMG::UZI)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::UZI_Color[0], Setting::PickupItems::SMG::UZI_Color[1], Setting::PickupItems::SMG::UZI_Color[2], Setting::PickupItems::SMG::UZI_Color[3]));
    }
    if (StrName.find("BIZON") != std::string::npos && Setting::PickupItems::SMG::BIZON)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::BIZON_Color[0], Setting::PickupItems::SMG::BIZON_Color[1], Setting::PickupItems::SMG::BIZON_Color[2], Setting::PickupItems::SMG::BIZON_Color[3]));
    }
    if (StrName.find("MP5K") != std::string::npos && Setting::PickupItems::SMG::MP5K)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::MP5K_Color[0], Setting::PickupItems::SMG::MP5K_Color[1], Setting::PickupItems::SMG::MP5K_Color[2], Setting::PickupItems::SMG::MP5K_Color[3]));
    }
    if (StrName.find("VECTOR") != std::string::npos && Setting::PickupItems::SMG::VECTOR)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::VECTOR_Color[0], Setting::PickupItems::SMG::VECTOR_Color[1], Setting::PickupItems::SMG::VECTOR_Color[2], Setting::PickupItems::SMG::VECTOR_Color[3]));
    }
#pragma endregion
#pragma region Sniper
    if (StrName.find("WIN94") != std::string::npos && Setting::PickupItems::Sniper::WIN94)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::WIN94_Color[0], Setting::PickupItems::Sniper::WIN94_Color[1], Setting::PickupItems::Sniper::WIN94_Color[2], Setting::PickupItems::Sniper::WIN94_Color[3]));
    }
    if (StrName.find("VSS") != std::string::npos && Setting::PickupItems::Sniper::VSS)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::VSS_Color[0], Setting::PickupItems::Sniper::VSS_Color[1], Setting::PickupItems::Sniper::VSS_Color[2], Setting::PickupItems::Sniper::VSS_Color[3]));
    }
    if (StrName.find("MINI14") != std::string::npos && Setting::PickupItems::Sniper::MINI14)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MINI14_Color[0], Setting::PickupItems::Sniper::MINI14_Color[1], Setting::PickupItems::Sniper::MINI14_Color[2], Setting::PickupItems::Sniper::MINI14_Color[3]));
    }
    if (StrName.find("QBU") != std::string::npos && Setting::PickupItems::Sniper::QBU)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::QBU_Color[0], Setting::PickupItems::Sniper::QBU_Color[1], Setting::PickupItems::Sniper::QBU_Color[2], Setting::PickupItems::Sniper::QBU_Color[3]));
    }
    if (StrName.find("MK12") != std::string::npos && Setting::PickupItems::Sniper::MK12)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MK12_Color[0], Setting::PickupItems::Sniper::MK12_Color[1], Setting::PickupItems::Sniper::MK12_Color[2], Setting::PickupItems::Sniper::MK12_Color[3]));
    }
    if (StrName.find("MK47") != std::string::npos && Setting::PickupItems::Sniper::MK47)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MK47_Color[0], Setting::PickupItems::Sniper::MK47_Color[1], Setting::PickupItems::Sniper::MK47_Color[2], Setting::PickupItems::Sniper::MK47_Color[3]));
    }
    if (StrName.find("SLR") != std::string::npos && Setting::PickupItems::Sniper::SLR)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::SLR_Color[0], Setting::PickupItems::Sniper::SLR_Color[1], Setting::PickupItems::Sniper::SLR_Color[2], Setting::PickupItems::Sniper::SLR_Color[3]));
    }
    if (StrName.find("SKS") != std::string::npos && Setting::PickupItems::Sniper::SKS)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::SKS_Color[0], Setting::PickupItems::Sniper::SKS_Color[1], Setting::PickupItems::Sniper::SKS_Color[2], Setting::PickupItems::Sniper::SKS_Color[3]));
    }
    if (StrName.find("KAR98") != std::string::npos && Setting::PickupItems::Sniper::KAR98)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::KAR98_Color[0], Setting::PickupItems::Sniper::KAR98_Color[1], Setting::PickupItems::Sniper::KAR98_Color[2], Setting::PickupItems::Sniper::KAR98_Color[3]));
    }
    if (StrName.find("MOSIN") != std::string::npos && Setting::PickupItems::Sniper::MOSIN)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MOSIN_Color[0], Setting::PickupItems::Sniper::MOSIN_Color[1], Setting::PickupItems::Sniper::MOSIN_Color[2], Setting::PickupItems::Sniper::MOSIN_Color[3]));
    }
    if (StrName.find("BOW") != std::string::npos && Setting::PickupItems::Sniper::BOW)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::BOW_Color[0], Setting::PickupItems::Sniper::BOW_Color[1], Setting::PickupItems::Sniper::BOW_Color[2], Setting::PickupItems::Sniper::BOW_Color[3]));
    }
    if (StrName.find("AWM") != std::string::npos && Setting::PickupItems::Sniper::AWM)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::AWM_Color[0], Setting::PickupItems::Sniper::AWM_Color[1], Setting::PickupItems::Sniper::AWM_Color[2], Setting::PickupItems::Sniper::AWM_Color[3]));
    }
    if (StrName.find("AMR") != std::string::npos && Setting::PickupItems::Sniper::AMR)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::AMR_Color[0], Setting::PickupItems::Sniper::AMR_Color[1], Setting::PickupItems::Sniper::AMR_Color[2], Setting::PickupItems::Sniper::AMR_Color[3]));
    }
#pragma endregion
#pragma region AutoMachine
    if (StrName.find("M249") != std::string::npos && Setting::PickupItems::AutoMachine::M249)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::AutoMachine::M249_Color[0], Setting::PickupItems::AutoMachine::M249_Color[1], Setting::PickupItems::AutoMachine::M249_Color[2], Setting::PickupItems::AutoMachine::M249_Color[3]));
    }
    if (StrName.find("MG3") != std::string::npos && Setting::PickupItems::AutoMachine::MG3)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::AutoMachine::MG3_Color[0], Setting::PickupItems::AutoMachine::MG3_Color[1], Setting::PickupItems::AutoMachine::MG3_Color[2], Setting::PickupItems::AutoMachine::MG3_Color[3]));
    }
    if (StrName.find("DP28") != std::string::npos && Setting::PickupItems::AutoMachine::DP28)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::AutoMachine::DP28_Color[0], Setting::PickupItems::AutoMachine::DP28_Color[1], Setting::PickupItems::AutoMachine::DP28_Color[2], Setting::PickupItems::AutoMachine::DP28_Color[3]));
    }
#pragma endregion
#pragma region ShotGun
    if (StrName.find("S686") != std::string::npos && Setting::PickupItems::ShotGun::S686)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::S686_Color[0], Setting::PickupItems::ShotGun::S686_Color[1], Setting::PickupItems::ShotGun::S686_Color[2], Setting::PickupItems::ShotGun::S686_Color[3]));
    }
    if (StrName.find("S1897") != std::string::npos && Setting::PickupItems::ShotGun::S1897)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::S1897_Color[0], Setting::PickupItems::ShotGun::S1897_Color[1], Setting::PickupItems::ShotGun::S1897_Color[2], Setting::PickupItems::ShotGun::S1897_Color[3]));
    }
    if (StrName.find("M1014") != std::string::npos && Setting::PickupItems::ShotGun::M1014)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::M1014_Color[0], Setting::PickupItems::ShotGun::M1014_Color[1], Setting::PickupItems::ShotGun::M1014_Color[2], Setting::PickupItems::ShotGun::M1014_Color[3]));
    }
    if (StrName.find("Neostead2000") != std::string::npos && Setting::PickupItems::ShotGun::Neostead2000)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::Neostead2000_Color[0], Setting::PickupItems::ShotGun::Neostead2000_Color[1], Setting::PickupItems::ShotGun::Neostead2000_Color[2], Setting::PickupItems::ShotGun::Neostead2000_Color[3]));
    }
    if (StrName.find("S12K") != std::string::npos && Setting::PickupItems::ShotGun::S12K)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::S12K_Color[0], Setting::PickupItems::ShotGun::S12K_Color[1], Setting::PickupItems::ShotGun::S12K_Color[2], Setting::PickupItems::ShotGun::S12K_Color[3]));
    }
    if (StrName.find("DBS") != std::string::npos && Setting::PickupItems::ShotGun::DBS)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::DBS_Color[0], Setting::PickupItems::ShotGun::DBS_Color[1], Setting::PickupItems::ShotGun::DBS_Color[2], Setting::PickupItems::ShotGun::DBS_Color[3]));
    }
#pragma endregion
#pragma region Pistols
    if (StrName.find("P1911") != std::string::npos && Setting::PickupItems::Pistols::P1911)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::P1911_Color[0], Setting::PickupItems::Pistols::P1911_Color[1], Setting::PickupItems::Pistols::P1911_Color[2], Setting::PickupItems::Pistols::P1911_Color[3]));
    }
    if (StrName.find("R45") != std::string::npos && Setting::PickupItems::Pistols::R45)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::R45_Color[0], Setting::PickupItems::Pistols::R45_Color[1], Setting::PickupItems::Pistols::R45_Color[2], Setting::PickupItems::Pistols::R45_Color[3]));
    }
    if (StrName.find("DesertEagle") != std::string::npos && Setting::PickupItems::Pistols::DesertEagle)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::DesertEagle_Color[0], Setting::PickupItems::Pistols::DesertEagle_Color[1], Setting::PickupItems::Pistols::DesertEagle_Color[2], Setting::PickupItems::Pistols::DesertEagle_Color[3]));
    }
    if (StrName.find("P92") != std::string::npos && Setting::PickupItems::Pistols::P92)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::P92_Color[0], Setting::PickupItems::Pistols::P92_Color[1], Setting::PickupItems::Pistols::P92_Color[2], Setting::PickupItems::Pistols::P92_Color[3]));
    }
    if (StrName.find("P18C") != std::string::npos && Setting::PickupItems::Pistols::P18C)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::P18C_Color[0], Setting::PickupItems::Pistols::P18C_Color[1], Setting::PickupItems::Pistols::P18C_Color[2], Setting::PickupItems::Pistols::P18C_Color[3]));
    }
    if (StrName.find("Vz61") != std::string::npos && Setting::PickupItems::Pistols::Vz61)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::Vz61_Color[0], Setting::PickupItems::Pistols::Vz61_Color[1], Setting::PickupItems::Pistols::Vz61_Color[2], Setting::PickupItems::Pistols::Vz61_Color[3]));
    }
    if (StrName.find("R1895") != std::string::npos && Setting::PickupItems::Pistols::R1895)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::R1895_Color[0], Setting::PickupItems::Pistols::R1895_Color[1], Setting::PickupItems::Pistols::R1895_Color[2], Setting::PickupItems::Pistols::R1895_Color[3]));
    }
    if (StrName.find("Flaregun") != std::string::npos && Setting::PickupItems::Pistols::Flaregun)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::Flaregun_Color[0], Setting::PickupItems::Pistols::Flaregun_Color[1], Setting::PickupItems::Pistols::Flaregun_Color[2], Setting::PickupItems::Pistols::Flaregun_Color[3]));
    }
#pragma endregion
#pragma region NeccessaryHealth
    if (StrName.find("Adrenaline") != std::string::npos && Setting::PickupItems::NeccessaryHealth::Adrenaline)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[0], Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[1], Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[2], Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[3]));
    }
    if (StrName.find("EnergyDrink") != std::string::npos && Setting::PickupItems::NeccessaryHealth::EnergyDrink)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[0], Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[1], Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[2], Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[3]));
    }
    if (StrName.find("Painkiller") != std::string::npos && Setting::PickupItems::NeccessaryHealth::Painkiller)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::Painkiller_Color[0], Setting::PickupItems::NeccessaryHealth::Painkiller_Color[1], Setting::PickupItems::NeccessaryHealth::Painkiller_Color[2], Setting::PickupItems::NeccessaryHealth::Painkiller_Color[3]));
    }
    if (StrName.find("FirstaidKit") != std::string::npos && Setting::PickupItems::NeccessaryHealth::FirstaidKit)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[0], Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[1], Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[2], Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[3]));
    }
    if (StrName.find("Medkit") != std::string::npos && Setting::PickupItems::NeccessaryHealth::Medkit)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::Medkit_Color[0], Setting::PickupItems::NeccessaryHealth::Medkit_Color[1], Setting::PickupItems::NeccessaryHealth::Medkit_Color[2], Setting::PickupItems::NeccessaryHealth::Medkit_Color[3]));
    }
#pragma endregion
#pragma region Melee
    if (StrName.find("Pan") != std::string::npos && Setting::PickupItems::Melee::Pan)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Melee::Pan_Color[0], Setting::PickupItems::Melee::Pan_Color[1], Setting::PickupItems::Melee::Pan_Color[2], Setting::PickupItems::Melee::Pan_Color[3]));
    }
#pragma endregion
#pragma region Bag
    if (StrName.find("BagLv1") != std::string::npos && Setting::PickupItems::Bag::BagLv1)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Bag::BagLv1_Color[0], Setting::PickupItems::Bag::BagLv1_Color[1], Setting::PickupItems::Bag::BagLv1_Color[2], Setting::PickupItems::Bag::BagLv1_Color[3]));
    }
    if (StrName.find("BagLv2") != std::string::npos && Setting::PickupItems::Bag::BagLv2)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Bag::BagLv2_Color[0], Setting::PickupItems::Bag::BagLv2_Color[1], Setting::PickupItems::Bag::BagLv2_Color[2], Setting::PickupItems::Bag::BagLv2_Color[3]));
    }
    if (StrName.find("BagLv3") != std::string::npos && Setting::PickupItems::Bag::BagLv3)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Bag::BagLv3_Color[0], Setting::PickupItems::Bag::BagLv3_Color[1], Setting::PickupItems::Bag::BagLv3_Color[2], Setting::PickupItems::Bag::BagLv3_Color[3]));
    }
#pragma endregion
#pragma region Armor
    if (StrName.find("ArmorLv1") != std::string::npos && Setting::PickupItems::Armor::ArmorLv1)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Armor::ArmorLv1_Color[0], Setting::PickupItems::Armor::ArmorLv1_Color[1], Setting::PickupItems::Armor::ArmorLv1_Color[2], Setting::PickupItems::Armor::ArmorLv1_Color[3]));
    }
    if (StrName.find("ArmorLv2") != std::string::npos && Setting::PickupItems::Armor::ArmorLv2)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Armor::ArmorLv2_Color[0], Setting::PickupItems::Armor::ArmorLv2_Color[1], Setting::PickupItems::Armor::ArmorLv2_Color[2], Setting::PickupItems::Armor::ArmorLv2_Color[3]));
    }
    if (StrName.find("ArmorLv3") != std::string::npos && Setting::PickupItems::Armor::ArmorLv3)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Armor::ArmorLv3_Color[0], Setting::PickupItems::Armor::ArmorLv3_Color[1], Setting::PickupItems::Armor::ArmorLv3_Color[2], Setting::PickupItems::Armor::ArmorLv3_Color[3]));
    }
#pragma endregion
#pragma region Helmet
    if (StrName.find("HelmetLv1") != std::string::npos && Setting::PickupItems::Helmet::HelmetLv1)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Helmet::HelmetLv1_Color[0], Setting::PickupItems::Helmet::HelmetLv1_Color[1], Setting::PickupItems::Helmet::HelmetLv1_Color[2], Setting::PickupItems::Helmet::HelmetLv1_Color[3]));
    }
    if (StrName.find("HelmetLv2") != std::string::npos && Setting::PickupItems::Helmet::HelmetLv2)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Helmet::HelmetLv2_Color[0], Setting::PickupItems::Helmet::HelmetLv2_Color[1], Setting::PickupItems::Helmet::HelmetLv2_Color[2], Setting::PickupItems::Helmet::HelmetLv2_Color[3]));
    }
    if (StrName.find("HelmetLv3") != std::string::npos && Setting::PickupItems::Helmet::HelmetLv3)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Helmet::HelmetLv3_Color[0], Setting::PickupItems::Helmet::HelmetLv3_Color[1], Setting::PickupItems::Helmet::HelmetLv3_Color[2], Setting::PickupItems::Helmet::HelmetLv3_Color[3]));
    }
#pragma endregion
#pragma region Accessory
    if (StrName.find("AREx") != std::string::npos && Setting::PickupItems::Accessory::AREx)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::AREx_Color[0], Setting::PickupItems::Accessory::AREx_Color[1], Setting::PickupItems::Accessory::AREx_Color[2], Setting::PickupItems::Accessory::AREx_Color[3]));
    }
    if (StrName.find("ARExQk") != std::string::npos && Setting::PickupItems::Accessory::ARExQk)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::ARExQk_Color[0], Setting::PickupItems::Accessory::ARExQk_Color[1], Setting::PickupItems::Accessory::ARExQk_Color[2], Setting::PickupItems::Accessory::ARExQk_Color[3]));
    }
    if (StrName.find("LargeCompensator") != std::string::npos && Setting::PickupItems::Accessory::LargeCompensator)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::LargeCompensator_Color[0], Setting::PickupItems::Accessory::LargeCompensator_Color[1], Setting::PickupItems::Accessory::LargeCompensator_Color[2], Setting::PickupItems::Accessory::LargeCompensator_Color[3]));
    }
    if (StrName.find("LargeSuppressor") != std::string::npos && Setting::PickupItems::Accessory::LargeSuppressor)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::LargeSuppressor_Color[0], Setting::PickupItems::Accessory::LargeSuppressor_Color[1], Setting::PickupItems::Accessory::LargeSuppressor_Color[2], Setting::PickupItems::Accessory::LargeSuppressor_Color[3]));
    }
    if (StrName.find("SniperCompensator") != std::string::npos && Setting::PickupItems::Accessory::SniperCompensator)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::SniperCompensator_Color[0], Setting::PickupItems::Accessory::SniperCompensator_Color[1], Setting::PickupItems::Accessory::SniperCompensator_Color[2], Setting::PickupItems::Accessory::SniperCompensator_Color[3]));
    }
    if (StrName.find("SniperSuppressor") != std::string::npos && Setting::PickupItems::Accessory::SniperSuppressor)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::SniperSuppressor_Color[0], Setting::PickupItems::Accessory::SniperSuppressor_Color[1], Setting::PickupItems::Accessory::SniperSuppressor_Color[2], Setting::PickupItems::Accessory::SniperSuppressor_Color[3]));
    }
    if (StrName.find("ThumbGrip") != std::string::npos && Setting::PickupItems::Accessory::ThumbGrip)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::ThumbGrip_Color[0], Setting::PickupItems::Accessory::ThumbGrip_Color[1], Setting::PickupItems::Accessory::ThumbGrip_Color[2], Setting::PickupItems::Accessory::ThumbGrip_Color[3]));
    }
    if (StrName.find("Angled") != std::string::npos && Setting::PickupItems::Accessory::Angled)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Angled_Color[0], Setting::PickupItems::Accessory::Angled_Color[1], Setting::PickupItems::Accessory::Angled_Color[2], Setting::PickupItems::Accessory::Angled_Color[3]));
    }
    if (StrName.find("LightGrip") != std::string::npos && Setting::PickupItems::Accessory::LightGrip)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::LightGrip_Color[0], Setting::PickupItems::Accessory::LightGrip_Color[1], Setting::PickupItems::Accessory::LightGrip_Color[2], Setting::PickupItems::Accessory::LightGrip_Color[3]));
    }
    if (StrName.find("HalfGrip") != std::string::npos && Setting::PickupItems::Accessory::HalfGrip)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::HalfGrip_Color[0], Setting::PickupItems::Accessory::HalfGrip_Color[1], Setting::PickupItems::Accessory::HalfGrip_Color[2], Setting::PickupItems::Accessory::HalfGrip_Color[3]));
    }
    if (StrName.find("Vertical") != std::string::npos && Setting::PickupItems::Accessory::Vertical)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Vertical_Color[0], Setting::PickupItems::Accessory::Vertical_Color[1], Setting::PickupItems::Accessory::Vertical_Color[2], Setting::PickupItems::Accessory::Vertical_Color[3]));
    }
    if (StrName.find("Reddot") != std::string::npos && Setting::PickupItems::Accessory::Reddot)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Reddot_Color[0], Setting::PickupItems::Accessory::Reddot_Color[1], Setting::PickupItems::Accessory::Reddot_Color[2], Setting::PickupItems::Accessory::Reddot_Color[3]));
    }
    if (StrName.find("Holo") != std::string::npos && Setting::PickupItems::Accessory::Holo)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Holo_Color[0], Setting::PickupItems::Accessory::Holo_Color[1], Setting::PickupItems::Accessory::Holo_Color[2], Setting::PickupItems::Accessory::Holo_Color[3]));
    }
    if (StrName.find("Scope2X") != std::string::npos && Setting::PickupItems::Accessory::Scope2X)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope2X_Color[0], Setting::PickupItems::Accessory::Scope2X_Color[1], Setting::PickupItems::Accessory::Scope2X_Color[2], Setting::PickupItems::Accessory::Scope2X_Color[3]));
    }
    if (StrName.find("Scope3X") != std::string::npos && Setting::PickupItems::Accessory::Scope3X)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope3X_Color[0], Setting::PickupItems::Accessory::Scope3X_Color[1], Setting::PickupItems::Accessory::Scope3X_Color[2], Setting::PickupItems::Accessory::Scope3X_Color[3]));
    }
    if (StrName.find("Scope4X") != std::string::npos && Setting::PickupItems::Accessory::Scope4X)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope4X_Color[0], Setting::PickupItems::Accessory::Scope4X_Color[1], Setting::PickupItems::Accessory::Scope4X_Color[2], Setting::PickupItems::Accessory::Scope4X_Color[3]));
    }
    if (StrName.find("Scope6X") != std::string::npos && Setting::PickupItems::Accessory::Scope6X)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope6X_Color[0], Setting::PickupItems::Accessory::Scope6X_Color[1], Setting::PickupItems::Accessory::Scope6X_Color[2], Setting::PickupItems::Accessory::Scope6X_Color[3]));
    }
    if (StrName.find("Scope8X") != std::string::npos && Setting::PickupItems::Accessory::Scope8X)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope8X_Color[0], Setting::PickupItems::Accessory::Scope8X_Color[1], Setting::PickupItems::Accessory::Scope8X_Color[2], Setting::PickupItems::Accessory::Scope8X_Color[3]));
    }
    if (StrName.find("M416Stock") != std::string::npos && Setting::PickupItems::Accessory::M416Stock)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::M416Stock_Color[0], Setting::PickupItems::Accessory::M416Stock_Color[1], Setting::PickupItems::Accessory::M416Stock_Color[2], Setting::PickupItems::Accessory::M416Stock_Color[3]));
    }
#pragma endregion
#pragma region Ammo
    if (StrName.find("7.62mm") != std::string::npos && Setting::PickupItems::Ammo::Ammo762)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo762_Color[0], Setting::PickupItems::Ammo::Ammo762_Color[1], Setting::PickupItems::Ammo::Ammo762_Color[2], Setting::PickupItems::Ammo::Ammo762_Color[3]));
    }
    if (StrName.find("45ACP") != std::string::npos && Setting::PickupItems::Ammo::Ammo45ACP)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo45ACP_Color[0], Setting::PickupItems::Ammo::Ammo45ACP_Color[1], Setting::PickupItems::Ammo::Ammo45ACP_Color[2], Setting::PickupItems::Ammo::Ammo45ACP_Color[3]));
    }
    if (StrName.find("5.56mm") != std::string::npos && Setting::PickupItems::Ammo::Ammo556)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo556_Color[0], Setting::PickupItems::Ammo::Ammo556_Color[1], Setting::PickupItems::Ammo::Ammo556_Color[2], Setting::PickupItems::Ammo::Ammo556_Color[3]));
    }
    if (StrName.find("9mm") != std::string::npos && Setting::PickupItems::Ammo::Ammo9)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo9_Color[0], Setting::PickupItems::Ammo::Ammo9_Color[1], Setting::PickupItems::Ammo::Ammo9_Color[2], Setting::PickupItems::Ammo::Ammo9_Color[3]));
    }
    if (StrName.find("300Magnum") != std::string::npos && Setting::PickupItems::Ammo::Ammo300Magnum)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo300Magnum_Color[0], Setting::PickupItems::Ammo::Ammo300Magnum_Color[1], Setting::PickupItems::Ammo::Ammo300Magnum_Color[2], Setting::PickupItems::Ammo::Ammo300Magnum_Color[3]));
    }
    if (StrName.find("12Guage") != std::string::npos && Setting::PickupItems::Ammo::Ammo12Guage)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo12Guage_Color[0], Setting::PickupItems::Ammo::Ammo12Guage_Color[1], Setting::PickupItems::Ammo::Ammo12Guage_Color[2], Setting::PickupItems::Ammo::Ammo12Guage_Color[3]));
    }
    if (StrName.find("FlareGun_Aimmo") != std::string::npos && Setting::PickupItems::Ammo::AmmoFlareGun)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::AmmoFlareGun_Color[0], Setting::PickupItems::Ammo::AmmoFlareGun_Color[1], Setting::PickupItems::Ammo::AmmoFlareGun_Color[2], Setting::PickupItems::Ammo::AmmoFlareGun_Color[3]));
    }
#pragma endregion
#pragma region Others
    if (StrName.find("AirDrop") != std::string::npos && Setting::PickupItems::Others::AirDrop)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::AirDrop_Color[0], Setting::PickupItems::Others::AirDrop_Color[1], Setting::PickupItems::Others::AirDrop_Color[2], Setting::PickupItems::Others::AirDrop_Color[3]));
    }
    if (StrName.find("AirDrop") != std::string::npos && Setting::PickupItems::Others::AirDrop)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::AirDrop_Color[0], Setting::PickupItems::Others::AirDrop_Color[1], Setting::PickupItems::Others::AirDrop_Color[2], Setting::PickupItems::Others::AirDrop_Color[3]));
    }
    if (StrName.find("DeathBox") != std::string::npos && Setting::PickupItems::Others::DeathBox)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::DeathBox_Color[0], Setting::PickupItems::Others::DeathBox_Color[1], Setting::PickupItems::Others::DeathBox_Color[2], Setting::PickupItems::Others::DeathBox_Color[3]));
    }
    if (StrName.find("Grenade") != std::string::npos && Setting::PickupItems::Others::Grenade)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Grenade_Color[0], Setting::PickupItems::Others::Grenade_Color[1], Setting::PickupItems::Others::Grenade_Color[2], Setting::PickupItems::Others::Grenade_Color[3]));
    }
    if (StrName.find("Smoke") != std::string::npos && Setting::PickupItems::Others::Smoke)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Smoke_Color[0], Setting::PickupItems::Others::Smoke_Color[1], Setting::PickupItems::Others::Smoke_Color[2], Setting::PickupItems::Others::Smoke_Color[3]));
    }
    if (StrName.find("Molotof") != std::string::npos && Setting::PickupItems::Others::Molotof)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Molotof_Color[0], Setting::PickupItems::Others::Molotof_Color[1], Setting::PickupItems::Others::Molotof_Color[2], Setting::PickupItems::Others::Molotof_Color[3]));
    }
    if (StrName.find("Apple") != std::string::npos && Setting::PickupItems::Others::Apple)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Apple_Color[0], Setting::PickupItems::Others::Apple_Color[1], Setting::PickupItems::Others::Apple_Color[2], Setting::PickupItems::Others::Apple_Color[3]));
    }
    if (StrName.find("Gascan") != std::string::npos && Setting::PickupItems::Others::Gascan)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Gascan_Color[0], Setting::PickupItems::Others::Gascan_Color[1], Setting::PickupItems::Others::Gascan_Color[2], Setting::PickupItems::Others::Gascan_Color[3]));
    }
    if (StrName.find("TokenShop") != std::string::npos && Setting::PickupItems::Others::TokenShop)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::TokenShop_Color[0], Setting::PickupItems::Others::TokenShop_Color[1], Setting::PickupItems::Others::TokenShop_Color[2], Setting::PickupItems::Others::TokenShop_Color[3]));
    }
    if (StrName.find("SecurityCard") != std::string::npos && Setting::PickupItems::Others::SecurityCard)
    {
        DrawBorderString(Fonts::MainFont, 15, DrawText, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::SecurityCard_Color[0], Setting::PickupItems::Others::SecurityCard_Color[1], Setting::PickupItems::Others::SecurityCard_Color[2], Setting::PickupItems::Others::SecurityCard_Color[3]));
    }
#pragma endregion
}


DWORD WINAPI UnloadThread(LPVOID)
{
    g_Running = false;

    for (HANDLE& hHandle : Data::HandleList)
    {
        if (hHandle)
        {
            WaitForSingleObject(hHandle, INFINITE);
            CloseHandle(hHandle);
        }
    }
    Data::HandleList.clear();

    // H::Free();

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    Sleep(100); 

    FreeLibraryAndExitThread(Cheat::Module, 0);
    return 0;
}



void AngleVectors(const FVector angles, FVector *forward)
{
    float sp, sy, cp, cy;
    sp = sin(DEG2RAD(angles.X));
    sy = sin(DEG2RAD(angles.Y));
    cp = cos(DEG2RAD(angles.X));
    cy = cos(DEG2RAD(angles.Y));
    forward->X = cp * cy;
    forward->Y = cp * sy;
    forward->Z = -sp;
}
VOID SetDataClipBoard(std::string Data)
{
    const char *output = Data.c_str();
    const size_t len = strlen(output) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), output, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}
//
std::unordered_map<int, std::string> skinTextCache;
bool isSkinTextLoaded = false;
std::unordered_map<std::string, bool> editStates;
bool showSearchWindow = false;
char searchQuery[256] = "";
std::vector<std::pair<int, std::string>> searchResults;
#include <winhttp.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "winhttp.lib")
std::string HttpsWebRequestPostx(const std::string& domain, const std::string& url_path, const std::string& data) {
    std::string response;
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    // Use WinHttpOpen to initialize a session
    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        return "";
    }

    // Specify an HTTP server
    std::wstring wideDomain(domain.begin(), domain.end());
    hConnect = WinHttpConnect(hSession, wideDomain.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Create an HTTP request handle
    std::wstring widePath(url_path.begin(), url_path.end());
    hRequest = WinHttpOpenRequest(hConnect, L"POST", widePath.c_str(),
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Set headers
    LPCWSTR headers = L"Content-Type: application/x-www-form-urlencoded\r\nCache-Control: no-cache";
    if (!WinHttpAddRequestHeaders(hRequest, headers, -1L, WINHTTP_ADDREQ_FLAG_ADD)) {
    }

    // For simplicity, we're not verifying the SSL certificate here (similar to CURLOPT_SSL_VERIFYPEER=0)
    DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
        SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
        SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
        SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

    // Send the request
    if (!WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)data.c_str(), data.size(),
        data.size(), 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // End the request
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Read the response
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    std::vector<char> buffer;

    do {
        // Check for available data
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            break;
        }

        if (!dwSize) break;

        // Allocate space for the buffer
        buffer.resize(dwSize + 1);

        // Read the data
        if (!WinHttpReadData(hRequest, (LPVOID)buffer.data(), dwSize, &dwDownloaded)) {
            break;
        }

        // Append to response
        response.append(buffer.data(), dwDownloaded);

    } while (dwSize > 0);

    // Clean up
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return response;
}
// Per-checkbox animation state keyed by ImGuiID
static std::unordered_map<ImGuiID, float> s_cbHoverAlpha;  // smooth lerp
static std::unordered_map<ImGuiID, float> s_cbShimmerX;    // shimmer sweep X

// Custom Animated Parallelogram Checkbox
bool NixCheckbox(const char* label, bool* v) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    const float height = 12.0f;
    const float width  = 20.0f;
    const ImVec2 pos = window->DC.CursorPos;
    const float total_width = width + 10.0f + (label_size.x > 0 ? style.ItemInnerSpacing.x + label_size.x : 0);
    const ImRect total_bb(pos, ImVec2(pos.x + total_width, pos.y + height));

    ImRect total_bb_with_spacing = total_bb;
    total_bb_with_spacing.Max.y += 3.0f; // Reduced vertical space between checkboxes
    ImGui::ItemSize(total_bb_with_spacing, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }

    float dt = ImGui::GetIO().DeltaTime;
    float T  = (float)ImGui::GetTime();
    bool  on = *v;

    // 4. Smooth Color Lerp
    float& hA = s_cbHoverAlpha[id];
    hA = hovered ? ImMin(hA + dt * 10.0f, 1.0f)
                 : ImMax(hA - dt * 10.0f, 0.0f);

    const float slant = 3.5f;
    ImVec2 p0(pos.x + slant,         pos.y);
    ImVec2 p1(pos.x + width + slant, pos.y);
    ImVec2 p2(pos.x + width,         pos.y + height);
    ImVec2 p3(pos.x,                 pos.y + height);
    ImVec2 pts[4] = { p0, p1, p2, p3 };

    // --- Background (lerped) ---
    ImU32 bgCol;
    if (on) {
        bgCol = ImColor(210, 20, 20, 255);
    } else {
        int r = (int)(25 + hA * 25);
        int gg = (int)(25 + hA * 25);
        int b = (int)(30 + hA * 30);
        bgCol = IM_COL32(r, gg, b, 230);
    }
    window->DrawList->AddConvexPolyFilled(pts, 4, bgCol);

    // 1. Pulse Glow Border (when ON)
    if (on) {
        float pulse  = (sinf(T * 3.5f) + 1.0f) * 0.5f;
        int   gAlpha = (int)(80 + pulse * 160);
        window->DrawList->AddPolyline(pts, 4, ImColor(255, 40, 40, gAlpha), ImDrawFlags_Closed, 1.8f);
        // outer glow
        ImVec2 gp[4] = {
            ImVec2(p0.x-1,p0.y-1), ImVec2(p1.x+1,p1.y-1),
            ImVec2(p2.x+1,p2.y+1), ImVec2(p3.x-1,p3.y+1)
        };
        window->DrawList->AddPolyline(gp, 4, ImColor(255, 20, 20, gAlpha/4), ImDrawFlags_Closed, 1.0f);
    } else {
        int bA = (int)(40 + hA * 50);
        window->DrawList->AddPolyline(pts, 4, IM_COL32(110, 110, 130, bA), ImDrawFlags_Closed, 1.0f);
    }

    // (Left-Edge Indicator Bar removed)

    // 2. Hover Shimmer Sweep
    if (hA > 0.01f) {
        float& sX = s_cbShimmerX[id];
        if (hovered) {
            sX += dt * 130.0f;
            if (sX > width + slant + 15.0f) sX = -15.0f;
        }
        float lx = pos.x + sX;
        if (lx > pos.x - 8 && lx < pos.x + width + slant + 8) {
            window->DrawList->AddLine(
                ImVec2(lx + 4, pos.y),
                ImVec2(lx - 4, pos.y + height),
                ImColor(255, 255, 255, (int)(hA * 0.20f * 255)), 5.0f
            );
        }
    }

    // --- Label (brightness lerped) ---
    if (label_size.x > 0.0f) {
        int tA = on ? 255 : (int)(185 + hA * 70);
        window->DrawList->AddText(
            ImVec2(pos.x + width + slant + 8.0f, pos.y + (height - label_size.y) * 0.5f),
            IM_COL32(255, 255, 255, tA), label
        );
    }

    return pressed;
}

bool NixArcSlider(const char* label, int* v, int v_min, int v_max) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(180, 140);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    if (held) {
        ImVec2 mouse = ImGui::GetIO().MousePos;
        ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.85f);
        float angle = atan2f(mouse.y - center.y, mouse.x - center.x);
        
        // Map angle to 0-1. Arc goes from approx -2.8 to -0.3 radians.
        float t = (angle + 2.8f) / 2.5f; 
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        *v = v_min + (int)(t * (v_max - v_min));
        ImGui::MarkItemEdited(id);
    }

    float value_f = (float)(*v - v_min) / (float)(v_max - v_min);
    ImDrawList* draw_list = window->DrawList;
    ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.85f);
    float radius = size.x * 0.45f;

    // Background arc (dark)
    draw_list->PathArcTo(center, radius, IM_PI + 0.3f, 2.0f * IM_PI - 0.3f, 40);
    draw_list->PathStroke(ImColor(35, 35, 40, 255), 0, 10.0f);

    // Active arc (red)
    draw_list->PathArcTo(center, radius, IM_PI + 0.3f, (IM_PI + 0.3f) + value_f * (IM_PI - 0.6f), 40);
    draw_list->PathStroke(ImColor(210, 20, 20, 255), 0, 10.0f);

    // Handle (glowing dot)
    float handle_angle = (IM_PI + 0.3f) + value_f * (IM_PI - 0.6f);
    ImVec2 handle_pos = ImVec2(center.x + cosf(handle_angle) * radius, center.y + sinf(handle_angle) * radius);
    
    // Glow and handle
    draw_list->AddCircleFilled(handle_pos, 10.0f, ImColor(210, 20, 20, 60));
    draw_list->AddCircleFilled(handle_pos, 7.0f, ImColor(210, 20, 20, 255));
    draw_list->AddCircleFilled(handle_pos, 3.0f, ImColor(255, 100, 100, 255));

    // Percentage Text
    char buf[32];
    sprintf(buf, "%d%%", (int)(value_f * 100.0f));
    ImVec2 text_size = ImGui::CalcTextSize(buf);
    draw_list->AddText(ImVec2(center.x - text_size.x * 0.5f, center.y - text_size.y * 1.2f), ImColor(210, 20, 20, 255), buf);

    // Label at top
    ImVec2 label_size = ImGui::CalcTextSize(label);
    draw_list->AddText(ImVec2(center.x - label_size.x * 0.5f, pos.y), ImColor(255, 255, 255, 255), label);

    return held;
}

/* PREMIUM PARTICLE SYSTEM */
struct NixParticle {
    ImVec2 pos;
    ImVec2 vel;
    float alpha;
    float size;
};
static std::vector<NixParticle> nixParticles;
void UpdateAndDrawParticles(ImVec2 menuPos, ImVec2 menuSize) {
    if (!Setting::Animation::bMovingParticles) return;
    
    if (nixParticles.empty()) {
        for (int i = 0; i < 60; i++) {
            nixParticles.push_back({ 
                ImVec2((float)(rand() % (int)menuSize.x), (float)(rand() % (int)menuSize.y)), 
                ImVec2((float)(rand() % 100 - 50) / 70.f, (float)(rand() % 100 - 50) / 70.f), 
                (float)(rand() % 100) / 100.f,
                (float)(rand() % 20 + 10) / 10.f
            });
        }
    }

    ImDrawList* draw = ImGui::GetWindowDrawList();
    float deltaTime = ImGui::GetIO().DeltaTime * Setting::Animation::fAnimationSpeed;

    for (int i = 0; i < (int)nixParticles.size(); i++) {
        auto& p = nixParticles[i];
        p.pos.x += p.vel.x * deltaTime * 60.f;
        p.pos.y += p.vel.y * deltaTime * 60.f;

        if (p.pos.x < 0) p.pos.x = menuSize.x;
        if (p.pos.x > menuSize.x) p.pos.x = 0;
        if (p.pos.y < 0) p.pos.y = menuSize.y;
        if (p.pos.y > menuSize.y) p.pos.y = 0;

        ImVec2 screenPos = ImVec2(menuPos.x + p.pos.x, menuPos.y + p.pos.y);
        
        if (Setting::Animation::iParticleMode == 0) { // Plexus
            for (int j = i + 1; j < (int)nixParticles.size(); j++) {
                auto& p2 = nixParticles[j];
                float dx = p.pos.x - p2.pos.x;
                float dy = p.pos.y - p2.pos.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 80.f) {
                    float lineAlpha = (1.0f - (dist / 80.f)) * 0.3f;
                    draw->AddLine(screenPos, ImVec2(menuPos.x + p2.pos.x, menuPos.y + p2.pos.y), ImColor(1.0f, 1.0f, 1.0f, lineAlpha));
                }
            }
        }
        
        draw->AddCircleFilled(screenPos, p.size, ImColor(1.0f, 1.0f, 1.0f, 0.4f));
    }
}

void DrawOldMenu()
{
    static float s_menuX = 0.f, s_menuY = 0.f;
    static bool s_menuPosValid = false;
    float defX = ScreenInfo::Width * 0.14f;
    float defY = ScreenInfo::Height * 0.18f;
    if (Setting::MenuExtras::bSaveMenuPosition && s_menuPosValid)
        ImGui::SetNextWindowPos(ImVec2(s_menuX, s_menuY), ImGuiCond_FirstUseEver);
    else
        ImGui::SetNextWindowPos(ImVec2(defX, defY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(810, 540));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10);
    DWORD nixFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
   

    ImGui::Begin("NIX Menu", &Setting::ShowMenu, nixFlags);
    if (Setting::Animation::bRainbowBorder) ImGui::PopStyleColor();

    const ImVec2 p = ImGui::GetWindowPos();
    if (Setting::MenuExtras::bSaveMenuPosition) { s_menuX = p.x; s_menuY = p.y; s_menuPosValid = true; }
    ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
    /* Load logo + resolve icons folder from DLL directory */
    if (g_pd3dDevice)
    {
        wchar_t dllPath[MAX_PATH] = {};
        HMODULE hDll = (HMODULE)Cheat::Module;
        if (hDll && GetModuleFileNameW(hDll, dllPath, MAX_PATH))
        {
            std::wstring dir(dllPath);
            size_t last = dir.find_last_of(L"\\/");
            if (last != std::wstring::npos) dir.resize(last + 1);
            if (!s_pNixLogoTexture)
            {
                LoadTextureFromMemory(s_nixLogoEmbed, (int)sizeof(s_nixLogoEmbed), &s_pNixLogoTexture, &s_nixLogoW, &s_nixLogoH);
            }
            if (s_iconsBasePath.empty())
            {
                std::wstring iconsDir = dir + L"icons\\";
                char iconsPathA[MAX_PATH] = {};
                WideCharToMultiByte(CP_UTF8, 0, iconsDir.c_str(), -1, iconsPathA, MAX_PATH, NULL, NULL);
                s_iconsBasePath = iconsPathA;
            }
        }
    }
    const ImVec2 nixMin(p.x, p.y), nixMax(p.x + 750.f, p.y + 500.f);
    UpdateAndDrawParticles(nixMin, ImVec2(750.f, 500.f));

    ImGui::SetCursorPos(ImVec2(200, 70));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::BeginChild("##NIXContent", ImVec2(530, 410), false, ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleColor();
   
    
    if (IndexTabs == 0)
    {
        /* NIX design: ESP tab - Editor (left) + Color Edit & Other (right) */
        ImGui::BeginChild("###Left_Child", ImVec2(230, 460), false, ImGuiWindowFlags_NoScrollbar);
        {
            // ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Shifted up
            ImGui::Indent(15.0f); // Shifter checkboxes to the right

            NixCheckbox("SHOW DISTANCE", &Setting::Player::Distance); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW NAME", &Setting::Player::Name); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW WEAPONS", &Setting::Player::Weapon); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW LINES", &Setting::Player::SnapLines); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW TEAM ID", &Setting::Player::Miscellanouse::TeamIDCol); ImGui::Dummy(ImVec2(0, 4.0f));

            if (NixCheckbox("SHOW SKELETON BOX", &Setting::Player::Box)) {
                if (Setting::Player::Box) Setting::Player::BoxType = 1;
            } ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW SKELETON", &Setting::Player::Skeleton); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW HEALTH BAR", &Setting::Player::Miscellanouse::Health); ImGui::Dummy(ImVec2(0, 4.0f));

            NixCheckbox("SHOW CENTRE ZONE", &Setting::Player::ZonePrediction); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW CENTRE ZONE", &Setting::Player::ZonePrediction); ImGui::Dummy(ImVec2(0, 4.0f));
            if (Cheat::ModuleLoaded == true && (Data::IsFreeUser == -1 || Data::IsFreeUser == 0)) {
                NixCheckbox("VISIBLE CHECK", &Setting::Player::VisibleCheck); ImGui::Dummy(ImVec2(0, 4.0f));
            }
            NixCheckbox("SHOW TOTAL ENEMY (count)", &Setting::InfoOverlay); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW ALERTS", &Setting::Alerts::OpenAllAlert); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("ENEMY AIMING WARNING", &Setting::Player::AimStatus); ImGui::Dummy(ImVec2(0, 4.0f));
            NixCheckbox("SHOW GRENADE LINE", &Setting::Alerts::GrenadeAlert_Trajectory); ImGui::Dummy(ImVec2(0, 4.0f));

            ImGui::Unindent(25.0f);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("###Right_Child", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
        {
            // ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Shifted further up
            ImGui::PushItemWidth(120.f); // Shorter sliders/combos
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 1)); // Makes sliders/combos thinner vertically

            if (Setting::Player::Miscellanouse::Health) {
                ImGui::Text("Health Type");
                ImGui::SameLine(120);
                
                auto HealthBtn = [&](const char* icon, int id, const char* tip) {
                    bool active = (Setting::Player::Miscellanouse::HealthID == id);
                    if (active) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                    if (ImGui::Button(icon, ImVec2(35, 25))) Setting::Player::Miscellanouse::HealthID = id;
                    if (active) ImGui::PopStyleColor();
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip(tip);
                };
                
                HealthBtn(ICON_FA_GRIP_LINES_VERTICAL, 0, "Vertical Layout");
                ImGui::SameLine();
                HealthBtn(ICON_FA_GRIP_LINES, 1, "Horizontal Layout");
            }
            // SHOW BOX AS combo removed per request
            if (Setting::Player::SnapLines) {
                ImGui::SliderFloat("LINE THICKNESS", &Setting::Player::SnapLines_ThickNess, 0.05f, 5.0f);
                ImGui::ColorEdit4("LINE COLOR", Setting::Player::SnapLines_Color, ImGuiColorEditFlags_NoInputs);
            }

            if (Setting::Player::Skeleton)
            {
                const char* bonesModes[] = { "Default", "Team ID" };
                ImGui::Combo("BONES MODE", &Setting::Player::SkeletonType, bonesModes, IM_ARRAYSIZE(bonesModes));
            }

            ImGui::Checkbox("GAME INFORMATION", &Setting::SpecialFeatures::GameInformation);
            ImGui::ColorEdit4("BONES COLOR", Setting::Player::Skeleton_Color, ImGuiColorEditFlags_NoInputs);
            if (Setting::Player::Box)
                ImGui::ColorEdit4("BOX COLOR", Setting::Player::Box_Color, ImGuiColorEditFlags_NoInputs);

            ImGui::SliderFloat("NAME SIZE", &Setting::Player::Name_ThickNess, 10.0f, 40.0f, "%.0f");
            ImGui::SliderFloat("BONE THICKNESS", &Setting::Player::Skeleton_ThickNess, 1.0f, 5.0f, "%.1f");
            
            if (Setting::Alerts::GrenadeAlert_Trajectory)
                ImGui::ColorEdit4("GRENADE LINE COLOR", Setting::Alerts::GrenadeAlert_Trajectory_Color, ImGuiColorEditFlags_NoInputs);

            /* 🧭 NIX STEALTH CORE (Relocated to ESP) */
            ImGui::Dummy(ImVec2(0, 20)); // Increased space to shift down
            {
                static int s_perfSmoothness = 50; 
                float childWidth = ImGui::GetContentRegionAvail().x;
                ImGui::SetCursorPosX((childWidth - 180) * 0.5f); 
                if (NixArcSlider("NIX STEALTH CORE", &s_perfSmoothness, 0, 100))
                {
                    if (s_perfSmoothness < 33) {
                        Setting::UpdateTick = 30; Setting::UpdateModuleTick = 150; Setting::Memory::bUnlockFPS = false; Setting::SpecialFeatures::GameFPS = 60.f;
                    } else if (s_perfSmoothness < 66) {
                        Setting::UpdateTick = 18; Setting::UpdateModuleTick = 100; Setting::Memory::bUnlockFPS = true; Setting::SpecialFeatures::GameFPS = 90.f;
                    } else {
                        Setting::UpdateTick = 8; Setting::UpdateModuleTick = 50; Setting::Memory::bUnlockFPS = true; Setting::SpecialFeatures::GameFPS = 120.f;
                    }
                }
                ImGui::SetCursorPosX((childWidth - ImGui::CalcTextSize(s_perfSmoothness < 33 ? "MODE: LOW" : s_perfSmoothness < 66 ? "MODE: MEDIUM" : "MODE: HIGH").x) * 0.5f);
                ImGui::TextColored(ImVec4(1, 0, 0, 1), s_perfSmoothness < 33 ? "MODE: LOW" : s_perfSmoothness < 66 ? "MODE: MEDIUM" : "MODE: HIGH");
            }
            ImGui::Separator();
            ImGui::PopStyleVar(); // Pop FramePadding
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
    }
    if (IndexTabs == 1)
    {
        static int item_sub_tab = 0;
        const char* categories[] = { 
            "GENERAL", 
            "RIFLES", 
            "SMG / LMG", 
            "SNIPER / DMR", 
            "SHOTGUN / PISTOL",
            "SCOPES", 
            "ACCESSORIES", 
            "HEALTH / GEAR", 
            "AMMO / OTHERS" 
        };

        // Left sidebar for categories (Premium Parallelogram Design)
        ImGui::BeginChild("###Items_Sidebar", ImVec2(175, 460), false, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));
            const char* icons[] = {
                ICON_FA_CUBES, ICON_FA_CROSSHAIRS, ICON_FA_BOLT, ICON_FA_EYE, ICON_FA_GAVEL,
                ICON_FA_SEARCH, ICON_FA_COGS, ICON_FA_MEDKIT, ICON_FA_LIST
            };

            for (int i = 0; i < IM_ARRAYSIZE(categories); i++)
            {
                bool active = (item_sub_tab == i);
                ImDrawList* draw = ImGui::GetWindowDrawList();
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 size(160, 28);
                float slant = 8.0f;

                ImGui::InvisibleButton(categories[i], size);
                bool hovered = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) item_sub_tab = i;

                // Parallelogram Vertices
                ImVec2 p1(pos.x + slant, pos.y);
                ImVec2 p2(pos.x + size.x + slant, pos.y);
                ImVec2 p3(pos.x + size.x, pos.y + size.y);
                ImVec2 p4(pos.x, pos.y + size.y);
                ImVec2 pts[4] = { p1, p2, p3, p4 };

                ImU32 bgColor = active ? ImColor(200, 20, 20, 255) : (hovered ? ImColor(60, 60, 70, 200) : ImColor(30, 30, 35, 180));
                draw->AddConvexPolyFilled(pts, 4, bgColor);

                // Icon & Text
                ImVec2 hexCenter(pos.x + slant + 15, pos.y + size.y * 0.5f);

                // Icon & Text
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Ensure standard font
                draw->AddText(ImVec2(hexCenter.x - 6, hexCenter.y - 7), ImColor(255, 255, 255, 255), icons[i]);
                draw->AddText(ImVec2(pos.x + slant + 35, pos.y + (size.y - ImGui::CalcTextSize(categories[i]).y) * 0.5f), 
                              active ? ImColor(255, 255, 255) : ImColor(180, 180, 180), categories[i]);
                ImGui::PopFont();
            }
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Right content area
        ImGui::BeginChild("###Items_Content", ImVec2(0, 460), false, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::Dummy(ImVec2(0, 10.0f)); // Top spacing
            ImGui::Indent(25.0f); // Shift to the right
            if (item_sub_tab == 0) // GENERAL (Now contains master switches)
            {
                NixCheckbox("SHOW ITEM LOOT", &Setting::PickupItems::Visual::IsShowItems);
                NixCheckbox("SHOW IN DEATH-BOX", &Setting::PickupItems::Visual::ShowItemInDeathBox);
                
                ImGui::Dummy(ImVec2(0, 10));
                ImGui::Text("DEATH-BOX COLOR");
                ImGui::ColorEdit4("###DeathBoxColor", Setting::Player::Box_Color, ImGuiColorEditFlags_NoInputs);
            }
            else if (item_sub_tab == 1) // RIFLES
            {
                NixCheckbox("AKM", &Setting::PickupItems::Rifles::AKM);
                NixCheckbox("M416", &Setting::PickupItems::Rifles::M416);
                NixCheckbox("SCAR-L", &Setting::PickupItems::Rifles::SCARL);
                NixCheckbox("M762", &Setting::PickupItems::Rifles::M762);
                NixCheckbox("GROZA", &Setting::PickupItems::Rifles::GROZA);
                NixCheckbox("AUG", &Setting::PickupItems::Rifles::AUG);
                NixCheckbox("FAMAS", &Setting::PickupItems::Rifles::FAMAS);
                NixCheckbox("M16A4", &Setting::PickupItems::Rifles::M16A4);
                NixCheckbox("MK14", &Setting::PickupItems::Rifles::MK14);
                NixCheckbox("QBZ", &Setting::PickupItems::Rifles::QBZ);
                NixCheckbox("G63C", &Setting::PickupItems::Rifles::G63C);
                NixCheckbox("HONEY BADGER", &Setting::PickupItems::Rifles::HoneyBadger);
            }
            else if (item_sub_tab == 2) // SMG / LMG
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "SMG");
                NixCheckbox("UMP45", &Setting::PickupItems::SMG::UMP45);
                NixCheckbox("UZI", &Setting::PickupItems::SMG::UZI);
                NixCheckbox("VECTOR", &Setting::PickupItems::SMG::VECTOR);
                NixCheckbox("THOMPSON", &Setting::PickupItems::SMG::THOMPSON);
                NixCheckbox("BIZON", &Setting::PickupItems::SMG::BIZON);
                NixCheckbox("MP5K", &Setting::PickupItems::SMG::MP5K);
                NixCheckbox("P90", &Setting::PickupItems::SMG::P90);
                // ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "LMG");
                NixCheckbox("M249", &Setting::PickupItems::AutoMachine::M249);
                NixCheckbox("MG3", &Setting::PickupItems::AutoMachine::MG3);
                NixCheckbox("DP-28", &Setting::PickupItems::AutoMachine::DP28);
            }
            else if (item_sub_tab == 3) // SNIPER / DMR
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "SNIPER");
                NixCheckbox("AWM", &Setting::PickupItems::Sniper::AWM);
                NixCheckbox("AMR", &Setting::PickupItems::Sniper::AMR);
                NixCheckbox("KAR98K", &Setting::PickupItems::Sniper::KAR98);
                NixCheckbox("MOSIN", &Setting::PickupItems::Sniper::MOSIN);
                NixCheckbox("CROSSBOW", &Setting::PickupItems::Sniper::BOW);
                // ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "DMR");
                NixCheckbox("MINI14", &Setting::PickupItems::Sniper::MINI14);
                NixCheckbox("SKS", &Setting::PickupItems::Sniper::SKS);
                NixCheckbox("SLR", &Setting::PickupItems::Sniper::SLR);
                NixCheckbox("QBU", &Setting::PickupItems::Sniper::QBU);
                NixCheckbox("MK12", &Setting::PickupItems::Sniper::MK12);
                NixCheckbox("VSS", &Setting::PickupItems::Sniper::VSS);
                NixCheckbox("MK47", &Setting::PickupItems::Sniper::MK47);
                NixCheckbox("WIN94", &Setting::PickupItems::Sniper::WIN94);
            }
            else if (item_sub_tab == 4) // SHOTGUN / PISTOL
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "SHOTGUN");
                NixCheckbox("S12K", &Setting::PickupItems::ShotGun::S12K);
                NixCheckbox("DBS", &Setting::PickupItems::ShotGun::DBS);
                NixCheckbox("S686", &Setting::PickupItems::ShotGun::S686);
                NixCheckbox("S1897", &Setting::PickupItems::ShotGun::S1897);
                NixCheckbox("M1014", &Setting::PickupItems::ShotGun::M1014);
                NixCheckbox("NS2000", &Setting::PickupItems::ShotGun::Neostead2000);
                // ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "PISTOL");
                NixCheckbox("FLARE GUN", &Setting::PickupItems::Pistols::Flaregun);
                NixCheckbox("DESERT EAGLE", &Setting::PickupItems::Pistols::DesertEagle);
                NixCheckbox("P1911", &Setting::PickupItems::Pistols::P1911);
                NixCheckbox("P92", &Setting::PickupItems::Pistols::P92);
                NixCheckbox("P18C", &Setting::PickupItems::Pistols::P18C);
                NixCheckbox("R45", &Setting::PickupItems::Pistols::R45);
                NixCheckbox("VZ61", &Setting::PickupItems::Pistols::Vz61);
                NixCheckbox("R1895", &Setting::PickupItems::Pistols::R1895);
            }
            else if (item_sub_tab == 5) // SCOPES
            {
                NixCheckbox("RED DOT", &Setting::PickupItems::Accessory::Reddot);
                NixCheckbox("HOLO", &Setting::PickupItems::Accessory::Holo);
                NixCheckbox("2x SCOPE", &Setting::PickupItems::Accessory::Scope2X);
                NixCheckbox("3x SCOPE", &Setting::PickupItems::Accessory::Scope3X);
                NixCheckbox("4x SCOPE", &Setting::PickupItems::Accessory::Scope4X);
                NixCheckbox("6x SCOPE", &Setting::PickupItems::Accessory::Scope6X);
                NixCheckbox("8x SCOPE", &Setting::PickupItems::Accessory::Scope8X);
            }
            else if (item_sub_tab == 6) // ACCESSORIES
            {
                NixCheckbox("AR EXTENDED MAG", &Setting::PickupItems::Accessory::AREx);
                NixCheckbox("AR EX-QUICK MAG", &Setting::PickupItems::Accessory::ARExQk);
                NixCheckbox("AR COMPENSATOR", &Setting::PickupItems::Accessory::LargeCompensator);
                NixCheckbox("AR SUPPRESSOR", &Setting::PickupItems::Accessory::LargeSuppressor);
                NixCheckbox("SNIPER COMP", &Setting::PickupItems::Accessory::SniperCompensator);
                NixCheckbox("SNIPER SUPP", &Setting::PickupItems::Accessory::SniperSuppressor);
                NixCheckbox("VERTICAL GRIP", &Setting::PickupItems::Accessory::Vertical);
                NixCheckbox("ANGLED GRIP", &Setting::PickupItems::Accessory::Angled);
                NixCheckbox("HALF GRIP", &Setting::PickupItems::Accessory::HalfGrip);
                NixCheckbox("LIGHT GRIP", &Setting::PickupItems::Accessory::LightGrip);
                NixCheckbox("THUMB GRIP", &Setting::PickupItems::Accessory::ThumbGrip);
                NixCheckbox("M416 STOCK", &Setting::PickupItems::Accessory::M416Stock);
            }
            else if (item_sub_tab == 7) // HEALTH / GEAR
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "HEALTH");
                NixCheckbox("MEDKIT", &Setting::PickupItems::NeccessaryHealth::Medkit);
                NixCheckbox("FIRST AID", &Setting::PickupItems::NeccessaryHealth::FirstaidKit);
                NixCheckbox("PAINKILLER", &Setting::PickupItems::NeccessaryHealth::Painkiller);
                NixCheckbox("ENERGY DRINK", &Setting::PickupItems::NeccessaryHealth::EnergyDrink);
                NixCheckbox("ADRENALINE", &Setting::PickupItems::NeccessaryHealth::Adrenaline);
                // ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "GEAR");
                NixCheckbox("ARMOR LV.1", &Setting::PickupItems::Armor::ArmorLv1);
                NixCheckbox("ARMOR LV.2", &Setting::PickupItems::Armor::ArmorLv2);
                NixCheckbox("ARMOR LV.3", &Setting::PickupItems::Armor::ArmorLv3);
                NixCheckbox("HELMET LV.1", &Setting::PickupItems::Helmet::HelmetLv1);
                NixCheckbox("HELMET LV.2", &Setting::PickupItems::Helmet::HelmetLv2);
                NixCheckbox("HELMET LV.3", &Setting::PickupItems::Helmet::HelmetLv3);
                NixCheckbox("BAG LV.1", &Setting::PickupItems::Bag::BagLv1);
                NixCheckbox("BAG LV.2", &Setting::PickupItems::Bag::BagLv2);
                NixCheckbox("BAG LV.3", &Setting::PickupItems::Bag::BagLv3);
            }
            else if (item_sub_tab == 8) // AMMO / OTHERS
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "AMMO");
                NixCheckbox("5.56mm", &Setting::PickupItems::Ammo::Ammo556);
                NixCheckbox("7.62mm", &Setting::PickupItems::Ammo::Ammo762);
                NixCheckbox("9mm", &Setting::PickupItems::Ammo::Ammo9);
                NixCheckbox(".45 ACP", &Setting::PickupItems::Ammo::Ammo45ACP);
                NixCheckbox("300 MAGNUM", &Setting::PickupItems::Ammo::Ammo300Magnum);
                NixCheckbox("12 GUAGE", &Setting::PickupItems::Ammo::Ammo12Guage);
                NixCheckbox("FLARE AMMO", &Setting::PickupItems::Ammo::AmmoFlareGun);
                // ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "OTHERS");
                NixCheckbox("AIR DROP", &Setting::PickupItems::Others::AirDrop);
                NixCheckbox("DEATH BOX", &Setting::PickupItems::Others::DeathBox);
                NixCheckbox("GRENADE", &Setting::PickupItems::Others::Grenade);
                NixCheckbox("SMOKE", &Setting::PickupItems::Others::Smoke);
                NixCheckbox("MOLOTOV", &Setting::PickupItems::Others::Molotof);
                NixCheckbox("TOKEN SHOP", &Setting::PickupItems::Others::TokenShop);
            }
            ImGui::Unindent(25.0f);
        }
        ImGui::EndChild();
    }
    if (IndexTabs == 2)
    {
        /* NIX design: VEHICLES tab */
        ImGui::BeginChild("Car Setting", ImVec2(0, 0), false);
        {
            ImGui::Dummy(ImVec2(0, 20.0f)); // Shift down
            ImGui::Indent(15.0f); // Shift right

            NixCheckbox("SHOW VEHICLES", &Setting::Vehicle::Vehicle);
            if (Setting::Vehicle::Vehicle) {
                ImGui::Dummy(ImVec2(0, 10.0f)); // Spacing
                ImGui::ColorEdit4("VEHICLE TEXT COLOR", Setting::Vehicle::Vehicle_Color, ImGuiColorEditFlags_NoInputs);

                ImGui::Dummy(ImVec2(0, 10.0f)); // Spacing
                NixCheckbox("SHOW VEHICLE DISTANCE", &Setting::Vehicle::Vehicle_Distance);

                ImGui::Dummy(ImVec2(0, 10.0f)); // Spacing
                ImGui::PushItemWidth(200.0f); // Compact sliders
                ImGui::SliderInt("MAX DISTANCE", &Setting::Vehicle::Vehicle_MaxDistance, 100, 1500, "%d");

                ImGui::Dummy(ImVec2(0, 10.0f)); // Spacing
                ImGui::SliderFloat("TEXT SIZE", &Setting::Vehicle::Vehicle_ThickNess, 10.f, 24.f, "%.0f");
                ImGui::PopItemWidth();
            }
            ImGui::Unindent(15.0f);
        }
        ImGui::EndChild();
    }
    if (IndexTabs == AIMINGINDEX)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8);
        ImGui::BeginChild("###Main_Child", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::BeginChild("###Main_Settings", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
            {
                NixCheckbox("Aimbot", &Setting::AimAlternative);
                ImGui::SameLine();
                NixCheckbox("Macro", &Setting::Macro);

                ImGui::Dummy(ImVec2(0.0f, 15.0f)); // Space between master switches and tabs
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0)); // Hide tab bar line
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0, 0, 0, 0));
                if (ImGui::BeginTabBar("###Left_TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoSeparator))
                {
                    if (Setting::AimAlternative)
                    {
                        if (ImGui::BeginTabItem("Aimbot###Aimbot"))
                        {
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"Vị trí aimbot") : (u8"Target aimbot"));
                            ImGui::SameLine(180.0f);
                            ImGui::PushItemWidth(180.0f);
                            {
                                const char *TextToChose[] = {(Setting::LanguageVNM == true) ? (u8"Đầu") : ("Head"), (Setting::LanguageVNM == true) ? (u8"Cổ") : ("Neck"), (Setting::LanguageVNM == true) ? (u8"Ngực") : ("Chest"), (Setting::LanguageVNM == true) ? (u8"Gần tâm nhất") : ("Nearest crosshair"), (Setting::LanguageVNM == true) ? (u8"Gần tâm nhất (Low headshot)") : ("Nearest crosshair (Low headshot)")};
                                const char *Combo_Show = TextToChose[Setting::TargetAim];
                                if (ImGui::BeginCombo("###Target_Aim", Combo_Show, ImGuiComboFlags_HeightSmall))
                                {
                                    for (int n = 0; n < IM_ARRAYSIZE(TextToChose); n++)
                                    {
                                        const bool is_selected = (Setting::TargetAim == n);
                                        if (ImGui::Selectable(TextToChose[n], is_selected))
                                            Setting::TargetAim = n;
                                        if (is_selected)
                                            ImGui::SetItemDefaultFocus();
                                    }
                                    ImGui::EndCombo();
                                }
                            }
                            ImGui::PopItemWidth();

                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"Phím aimbot") : (u8"Key aimbot"));
                            ImGui::SameLine(180.0f);
                            ImGui::PushID(0);
                            BindKey(&Setting::KeyAim, ImVec2(180, 0));
                            ImGui::PopID();

                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"Loại aimbot") : (u8"Type aimbot"));
                            ImGui::SameLine(180.0f);
                            {
                                bool isSilent = (Setting::TypeAim == 0);
                                bool isMouse  = (Setting::TypeAim == 1);
                                bool isMemory = (Setting::TypeAim == 2);

                                if (NixCheckbox((Setting::LanguageVNM == true) ? (u8"Đạn đuổi") : ("Silent aim"), &isSilent) && isSilent)
                                    Setting::TypeAim = 0;

                                ImGui::SameLine();
                                if (NixCheckbox("Mouse event", &isMouse) && isMouse)
                                    Setting::TypeAim = 1;

                                ImGui::SameLine();
                                if (NixCheckbox("Memory", &isMemory) && isMemory)
                                    Setting::TypeAim = 2;
                            }

                            if (Setting::TypeAim == 1 || Setting::TypeAim == 2 || Setting::TypeAim == 3)
                            {
                                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                                ImGui::Text((Setting::LanguageVNM == true) ? (u8"Tốc độ aimbot") : (u8"Speed aimbot"));
                                ImGui::SameLine(180.0f);
                                ImGui::PushItemWidth(180.0f);
                                ImGui::SliderInt("###Aimbot_Speed", &Setting::SmoothAim, 0, 50);
                                ImGui::PopItemWidth();
                            }

                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"Giới hạn vùng aimbot") : (u8"Fov Size"));
                            ImGui::SameLine();
                            HelpMarker((Setting::LanguageVNM == true) ? (u8"Nếu giới hạn = 0, tính năng này sẽ hổng hoạt động") : (u8"Limited fov = 0, if you changed this features will not work"));
                            ImGui::SameLine(180.0f);
                            ImGui::PushItemWidth(180.0f);
                            ImGui::SliderInt("###FOV", &Setting::FovAim, 0, 1000);
                            ImGui::PopItemWidth();

                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Hiện vùng aimbot") : (u8"Show aimbot fov"), &Setting::Fov);
                            ImGui::SameLine();
                            NixCheckbox("Show Aim Line", &Setting::AimLine);
                            
                            if (Setting::Fov)
                            {
                                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                                ImGui::Text((Setting::LanguageVNM == true) ? (u8"Màu hiện vùng") : (u8"FOV color"));
                                ImGui::SameLine(240.0f);
                                ImGui::PushItemWidth(220.0f);
                                ImGui::ColorEdit4("###FOV_COLOR", Setting::Fov_Color);
                                ImGui::PopItemWidth();
                            }

                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem((Setting::LanguageVNM == true) ? (u8"Bộ lọc") : ("Filters")))
                        {
                            ImGui::Dummy(ImVec2(0.0f, 20.0f));
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Bỏ qua AI/Bot") : (u8"SKIP BOTS"), &Setting::SpecialFeatures::V3SkipBot);
                            ImGui::SameLine();
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Bỏ qua người bị gục") : (u8"SKIP KNOCKED"), &Setting::SpecialFeatures::V3SkipKnock);

                            if (Setting::SpecialFeatures::V3SkipKnock)
                            {
                                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                                ImGui::Text((Setting::LanguageVNM == true) ? (u8"- Phím bắn gục : ") : (u8"- Key to shoot knocked : "));
                                ImGui::SameLine(240.0f);
                                ImGui::PushID(1);
                                BindKey(&Setting::KeyShootKnock, ImVec2(220, 0));
                                ImGui::PopID();
                            }

                            ImGui::Dummy(ImVec2(0.0f, 20.0f));

                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Aimbot đón đầu") : (u8"Aimbot prediction"), &Setting::Predict);
                            if (Setting::Predict)
                            {
                                ImGui::SameLine(240.0f);
                                ImGui::PushItemWidth(220.0f);
                                ImGui::SliderFloat("###PredictAim", &Setting::PredictAim, 1, 3, "x%.3f");
                                ImGui::PopItemWidth();
                            }

                            ImGui::Dummy(ImVec2(0.0f, 20.0f));
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Giới hạn khoảng cách") : (u8"Limit distance track"), &Setting::AimLimitDistance);
                            if (Setting::AimLimitDistance)
                            {
                                ImGui::SameLine(240.0f);
                                ImGui::PushItemWidth(220.0f);
                                ImGui::SliderFloat("###AimLimitDistance", &Setting::AimLimitDistanceValue, 0, 500, "%.3fm");
                                ImGui::PopItemWidth();
                            }

                            ImGui::Dummy(ImVec2(0.0f, 20.0f));
                            if (Cheat::ModuleLoaded == true && (Data::IsFreeUser == -1 || Data::IsFreeUser == 0))
                            {
                                NixCheckbox((Setting::LanguageVNM == true) ? (u8"Check núp tường") : (u8"Visibile check"), &Setting::BulletCheckVisible);
                                if (Setting::BulletCheckVisible == true)
                                {
                                    ImGui::SameLine();
                                    NixCheckbox((Setting::LanguageVNM == true) ? (u8"Tự động bắn") : (u8"Auto fire"), &Setting::SpecialFeatures::AutoFire);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                            }

                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Khóa địch khi giữ phím aimbot") : (u8"Lock enemy when hold key aimbot"), &Setting::LockEnemyPressAim);
                            ImGui::Dummy(ImVec2(0.0f, 20.0f));

                            ImGui::EndTabItem();
                        }
                    }
                    if (Setting::Macro)
                    {
                        if (ImGui::BeginTabItem("Macro"))
                        {
                            ImGui::Dummy(ImVec2(0, 5.0f));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"Tỉ lệ kéo chuột") : (u8"Macro value"));
                            ImGui::SameLine();
                            ImGui::PushItemWidth(150.0f);
                            ImGui::SliderInt("###MacroAim", &Setting::MacroAim, 0, 10);
                            ImGui::PopItemWidth();
                            ImGui::Dummy(ImVec2(0, 10.0f));
                            
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Tự tap súng") : (u8"Auto tap"), &Setting::AutoTap);
                            ImGui::SameLine();
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Chỉ ghìm khi dùng aimbot") : (u8"Only active when use aimbot"), &Setting::MacroWithAim);
                            if (Setting::AutoTap)
                            {
                                ImGui::Dummy(ImVec2(0, 10.0f));
                                ImGui::Text((Setting::LanguageVNM == true) ? (u8"Delay mỗi tap (ms): ") : (u8"Delay by tap (ms): "));
                                ImGui::SameLine();
                                ImGui::PushItemWidth(150.0f);
                                ImGui::DragInt("###AutoTapDelay", &Setting::AutoTapDelay, 0, 0, 100);
                                ImGui::PopItemWidth();
                            }
                            ImGui::Dummy(ImVec2(0, 10.0f));
                            ImGui::EndTabItem();
                        }
                    }
                    if (Setting::TypeAim == 0)
                    {
                        if (ImGui::BeginTabItem((Setting::LanguageVNM == true) ? (u8"Đạn đuổi") : ("Bullet tracking")))
                        {
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"Take the gun to active bullet tracking, not need spam ammo,\n") : (u8"Take the gun to active bullet tracking, not need spam ammo,\n"));
                            ImGui::Text((Setting::LanguageVNM == true) ? (u8"if got problem with bullet track in game press P to fix.") : (u8"if got problem with bullet track in game press P to fix."));
                            
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Auto fix bullet track problem") : ("Auto fix bullet track problem"), &Setting::BulletTrack::AutoFix);
                            
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Enable aimbot") : ("Enable aimbot"), &Setting::BulletTrack::OpenAim);
                            
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            ImGui::Text("Z-Position");
                            
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            // ImGui::Separator(); // Removed black line
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));
                            NixCheckbox((Setting::LanguageVNM == true) ? (u8"Bypass FOV limit (BAN)") : ("Bypass FOV limit (BAN)"), &Setting::BulletTrack::BypassFovLitmit);
                            ImGui::EndTabItem();
                        }
                    }
                    ImGui::EndTabBar();
                }
                ImGui::PopStyleColor(5); // End Hide tab bar line
                ImGui::EndChild();
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
    }
    if (IndexTabs == ADMININDEX)
    {
        /* NIX design: ADMIN tab - risk features */
        ImGui::BeginChild("Magic a", ImVec2(300, 460), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Safe Features:");
        ImGui::Dummy(ImVec2(0, 5.0f));
        ImGui::Dummy(ImVec2(0, 10.0f));
        
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.0f, 1.f), "USE THIS FEATURES IN YOUR OWN RISK");
        ImGui::Dummy(ImVec2(0, 5.0f));
        ImGui::Dummy(ImVec2(0, 10.0f));
        
        NixCheckbox("FAST CAR (RISK)", &Setting::Memory::HighRiskFastCar);
        ImGui::Dummy(ImVec2(0, 10.0f));
        ImGui::Dummy(ImVec2(0, 5.0f));
        ImGui::Dummy(ImVec2(0, 10.0f));
        
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.f, 1.f), "iPad View (FOV zoom)");
        ImGui::Dummy(ImVec2(0, 5.0f));
        NixCheckbox("iPad View", &Setting::SpecialFeatures::IpadView);
        if (Setting::SpecialFeatures::IpadView)
        {
            ImGui::Text("FOV (80 = zoom out, 140 = zoom in)");
            ImGui::SliderInt("###IpadViewValue", &Setting::SpecialFeatures::IpadViewValue, 80, 140, "%d");
        }
        ImGui::Dummy(ImVec2(0, 10.0f));
        ImGui::Dummy(ImVec2(0, 5.0f));
        ImGui::Dummy(ImVec2(0, 10.0f));
        
        NixCheckbox("ESP when spectating", &Setting::QoL::bESPWhenSpectating);
        ImGui::Dummy(ImVec2(0, 10.0f));
        
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "ESP Style (Chams)");
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
        if (ImGui::BeginTabBar("ESPStyleTabs", ImGuiTabBarFlags_NoSeparator))
        {
            if (ImGui::BeginTabItem("Standard Skeleton")) {
                Setting::Player::SkeletonStyle = 0;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Neon Glow")) {
                Setting::Player::SkeletonStyle = 1;
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        ImGui::EndChild();
    }
    if (IndexTabs == SETTINGTABINDEX)
    {
        ImGui::Dummy(ImVec2(0, 20.0f)); // Shift everything down
        
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8);
        ImGui::BeginChild("###Main_Child", ImVec2(0, 0), false);
        {
            ImGui::PopStyleVar();
            
            ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0, 0, 0, 0));
            if (ImGui::BeginTabBar("SettingsTabs")) 
            {
                if (ImGui::BeginTabItem(ICON_FA_COG " GENERAL")) 
                {
                    ImGui::BeginChild("CONFIGS", ImVec2(295, 0), false);
                    {
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.f), "FIX ESP GLITCH");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        NixCheckbox("Border string (text outline)", &Setting::BorderString);
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        ImGui::Text("Update tick (Use 18ms)");
                        ImGui::SliderInt("##Updatetick", &Setting::UpdateTick, 8, 100, "%d ms");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        NixCheckbox("Enable name cache", &Fake_Namecache);
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        if (ImGui::Button("Flush name cache", ImVec2(-1, 0)))
                        {
                            Data::NamedCacheList.clear();
                            ImGui::InsertNotification({ImGuiToastType_Success, 2500, (Setting::LanguageVNM == 1) ? u8"Reloaded name cache" : "Reloaded name cache"});
                        }
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        if (ImGui::Button(" SAVE SETTINGS ", ImVec2(120.f, 30.f)))
                        {
                            config::SaveConfig("Custom_cfg");
                            ImGui::InsertNotification({ImGuiToastType_Success, 3000, (Setting::LanguageVNM == 1) ? u8"Settings Has Been Saved" : "Settings Has Been Saved"});
                        }
                        ImGui::SameLine();
                        if (ImGui::Button(" LOAD SETTING ", ImVec2(120.f, 30.f)))
                        {
                            if (config::LoadConfig("Custom_cfg"))
                                ImGui::InsertNotification({ImGuiToastType_Success, 3000, (Setting::LanguageVNM == 1) ? u8"Load Settings, Success" : "Load Settings, Success"});
                            else
                                ImGui::InsertNotification({ImGuiToastType_Error, 3000, (Setting::LanguageVNM == 1) ? u8"Failed To Find Last Settings" : "Failed To Find Last Settings"});
                        }
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("##SDQWDQs", ImVec2(295, 0), false);
                    {
                        ImGui::TextColored(ImVec4(0.8f, 0.f, 0.f, 1.f), "EXTRA SETTINGS");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
                        if (ImGui::Button("   GAME INFORMATION "))
                            Setting::SpecialFeatures::GameInformation = !Setting::SpecialFeatures::GameInformation;
                        ImGui::SameLine();
                        ImGui::TextColored(Setting::SpecialFeatures::GameInformation ? ImVec4(0.2f, 1.f, 0.2f, 1.f) : ImVec4(1.f, 0.2f, 0.2f, 1.f), Setting::SpecialFeatures::GameInformation ? " ON" : " OFF");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        
                        if (ImGui::Button("   ANTI-SCREEN CAPTURE "))
                        {
                            Setting::AntiScreenCapture = !Setting::AntiScreenCapture;
                            HWND hwnd = s_GameWindow ? s_GameWindow : ScreenInfo::hWindow;
                            if (hwnd && IsWindow(hwnd))
                            {
                                if (!s_pSetWindowDisplayAffinity)
                                    s_pSetWindowDisplayAffinity = (PFN_SetWindowDisplayAffinity)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowDisplayAffinity");
                                if (s_pSetWindowDisplayAffinity)
                                    s_pSetWindowDisplayAffinity(hwnd, Setting::AntiScreenCapture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
                            }
                        }
                        ImGui::SameLine();
                        ImGui::TextColored(Setting::AntiScreenCapture ? ImVec4(0.2f, 1.f, 0.2f, 1.f) : ImVec4(1.f, 0.2f, 0.2f, 1.f), Setting::AntiScreenCapture ? " ON" : " OFF");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        ImGui::PopStyleVar();
                        
                        if (ImGui::Button((Setting::LanguageVNM == true) ? (u8"Thoát hack") : (u8"Unload hack"), ImVec2(160, 38)))
                            IsExitHack = true;
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem(ICON_FA_MAGIC " ANIMATIONS")) 
                {
                    ImGui::BeginChild("ANIM_LEFT", ImVec2(530, 0), false);
                    {
                        ImGui::TextColored(ImColor(230, 20, 20, 255), "MENU EFFECTS");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        // Animation toggles removed as requested
                        ImGui::Dummy(ImVec2(0, 10));
                        
                        ImGui::Text("Animation Speed");
                        ImGui::SliderFloat("##AnimSpeed", &Setting::Animation::fAnimationSpeed, 0.1f, 3.0f, "%.1fx");
                        
                        ImGui::Dummy(ImVec2(0, 20));
                        
                        ImGui::TextColored(ImColor(230, 20, 20, 255), "PARTICLE SYSTEM");
                        ImGui::Dummy(ImVec2(0, 5.0f));
                        NixCheckbox("Enable Background Particles", &Setting::Animation::bMovingParticles);
                        
                        ImGui::Text("Particle Style: Plexus (Lines)");
                        // Mode selection and reset removed as requested
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
                ImGui::PopStyleColor(5);
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndChild(); // ##NIXContent
    /* Sidebar tabs: sab same column me, ESP bhi same row align */
    /* 🧭 PREMIUM SIDEBAR NAVIGATION */
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
    ImGui::SetCursorPos(ImVec2(37, 70)); // Shifted higher

    auto SidebarButton = [&](const char* icon, const char* label, int index, int targetIndex) {
        if (targetIndex < 0 || targetIndex >= 100) return;
        bool active = (index == targetIndex);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size(110, 22);
        float slant = 9.0f; // Slant amount

        // Interaction logic
        ImGui::InvisibleButton(label, size);
        bool hovered = ImGui::IsItemHovered();
        if (ImGui::IsItemClicked()) IndexTabs = targetIndex;

        // Vertices for Parallelogram
        ImVec2 p1(pos.x + slant, pos.y);
        ImVec2 p2(pos.x + size.x + slant, pos.y);
        ImVec2 p3(pos.x + size.x, pos.y + size.y);
        ImVec2 p4(pos.x, pos.y + size.y);
        ImVec2 points[4] = { p1, p2, p3, p4 };

        // Draw Background
        ImU32 bgColor;
        if (active) bgColor = ImColor(210, 20, 20, 255);
        else if (hovered) bgColor = ImColor(50, 50, 60, 240);
        else bgColor = ImColor(25, 25, 30, 230);

        draw->AddConvexPolyFilled(points, 4, bgColor);

        // Draw Border
        ImU32 borderColor = active ? ImColor(255, 255, 255, 90) : ImColor(100, 100, 120, 60);
        draw->AddPolyline(points, 4, borderColor, ImDrawFlags_Closed, 1.0f);

        // Draw Icon and Text
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImVec2 textPos(pos.x + 14, pos.y + (size.y - ImGui::GetFontSize()) * 0.5f);
        draw->AddText(textPos, ImColor(255, 255, 255, active ? 255 : 185), (std::string(icon) + "  " + label).c_str());
        ImGui::PopFont();

        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + size.y + 25)); // Increased gap for better look
    };

    SidebarButton(ICON_FA_EYE, "VISUALS ESP", IndexTabs, 0);
    SidebarButton(ICON_FA_CROSSHAIRS, "AIMBOT", IndexTabs, AIMINGINDEX);
    SidebarButton(ICON_FA_CUBE, "ITEMS", IndexTabs, ITEMSINDEX);
    SidebarButton(ICON_FA_CAR, "VEHICLES", IndexTabs, VEHICLESINDEX);
    SidebarButton(ICON_FA_USER_SECRET, "ADMIN", IndexTabs, ADMININDEX);
    SidebarButton(ICON_FA_COG, "SETTINGS", IndexTabs, SETTINGTABINDEX);

    ImGui::PopStyleVar(2);
    ImGui::End();
}
VECTOR3 Screen;
VECTOR2 ScreenVec2;
VECTOR2 Head;
VECTOR2 Chest;
VECTOR2 Pelvis;
VECTOR2 lSholder;
VECTOR2 rSholder;
VECTOR2 lElbow;
VECTOR2 rElbow;
VECTOR2 lWrist;
VECTOR2 rWrist;
VECTOR2 lThigh;
VECTOR2 rThigh;
VECTOR2 lKnee;
VECTOR2 rKnee;
VECTOR2 lAnkle;
VECTOR2 rAnkle;
VECTOR2 lFoot;
VECTOR2 rFoot;
VECTOR2 Root;
VECTOR3 HeadPos;
string TextToDraw;
int TargetID = 0;

float GetDPIScale()
{
    HDC screen = GetDC(0);
    // Sistem DPI değeri / standart DPI (96) => ölçek katsayısı
    float dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(0, screen);
    return dpi / 96.0f;  // 1.0f = 100%, 1.25f = 125%, vb.
}

void AutoScaleImGui()
{
    ImGuiIO& io = ImGui::GetIO();

    // DPI ölçeği
    float scale = GetDPIScale();

    // Uygula
    io.FontGlobalScale = scale;

    // UI elementlerini de ölçekle
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
}
std::string GetShortWeaponName(const std::string& weaponFullName)
{
    size_t firstUnderscore = weaponFullName.find('_');
    if (firstUnderscore != std::string::npos)
    {
        size_t secondUnderscore = weaponFullName.find('_', firstUnderscore + 1);
        if (secondUnderscore != std::string::npos)
        {
            size_t thirdUnderscore = weaponFullName.find('_', secondUnderscore + 1);
            if (thirdUnderscore != std::string::npos)
                return weaponFullName.substr(secondUnderscore + 1, thirdUnderscore - secondUnderscore - 1);
            else
                return weaponFullName.substr(secondUnderscore + 1);
        }
    }

    return weaponFullName;
}
std::string PlayerWeapon(std::string str)
{
    const char* cstr = str.c_str();

    if (strstr(cstr, "AWM")) return "AWM";
    if (strstr(cstr, "QBU")) return "QBU";
    if (strstr(cstr, "SLR")) return "SLR";
    if (strstr(cstr, "SKS")) return "SKS";
    if (strstr(cstr, "Mini14")) return "Mini14";
    if (strstr(cstr, "M24")) return "M24";
    if (strstr(cstr, "Kar98k")) return "Kar98k";
    if (strstr(cstr, "VSS")) return "VSS";
    if (strstr(cstr, "Win94")) return "Win94";
    if (strstr(cstr, "MK12")) return "MK12";
    if (strstr(cstr, "Mk14") || strstr(cstr, "WEP_Mk14")) return "Mk14";
    if (strstr(cstr, "AMR")) return "AMR";
    if (strstr(cstr, "Mosin")) return "Mosin";

    // Rifles
    if (strstr(cstr, "FAMAS")) return "FAMAS";
    if (strstr(cstr, "AUG")) return "AUG";
    if (strstr(cstr, "M762")) return "M762";
    if (strstr(cstr, "SCAR")) return "SCAR-L";
    if (strstr(cstr, "M416")) return "M416";
    if (strstr(cstr, "AN94")) return "ASM";
    if (strstr(cstr, "M16A4")) return "M16A4";
    if (strstr(cstr, "Mk47")) return "Mk47";
    if (strstr(cstr, "G36")) return "G36C";
    if (strstr(cstr, "QBZ")) return "QBZ";
    if (strstr(cstr, "AKM")) return "AKM";
    if (strstr(cstr, "Groza")) return "Groza";
    if (strstr(cstr, "HoneyBadger")) return "Honey Badger";
    if (strstr(cstr, "ACE32")) return "ACE32";

    // Machine Guns
    if (strstr(cstr, "P90")) return "P90";
    if (strstr(cstr, "PP19")) return "PP19";
    if (strstr(cstr, "TommyGun")) return "Tommy Gun";
    if (strstr(cstr, "MP5K")) return "MP5K";
    if (strstr(cstr, "UMP9")) return "UMP9";
    if (strstr(cstr, "Vector")) return "Vector";
    if (strstr(cstr, "Uzi")) return "Uzi";

    // Shotguns
    if (strstr(cstr, "S12K")) return "S12K";
    if (strstr(cstr, "DP12")) return "DP12";
    if (strstr(cstr, "S686")) return "S686";
    if (strstr(cstr, "S1897")) return "S1897";
    if (strstr(cstr, "SawedOff")) return "SawedOff";
    if (strstr(cstr, "M1014")) return "M1014";

    // Pistols
    if (strstr(cstr, "Flaregun")) return "Flaregun";
    if (strstr(cstr, "R1895")) return "R1895";
    if (strstr(cstr, "Vz61")) return "Vz61";
    if (strstr(cstr, "P92")) return "P92";
    if (strstr(cstr, "P18C")) return "P18C";
    if (strstr(cstr, "R45")) return "R45";
    if (strstr(cstr, "P1911")) return "P1911";
    if (strstr(cstr, "DesertEagle")) return "DesertEagle";

    // Other Weapons
    if (strstr(cstr, "DP28")) return "DP28";
    if (strstr(cstr, "M249")) return "M249";
    if (strstr(cstr, "MG3")) return "MG3";
    if (strstr(cstr, "CrossBow")) return "CrossBow";
    if (strstr(cstr, "HuntingBowEA")) return "Explosive Bow";

    // Grenades
    if (strstr(cstr, "Shoulei")) return "Shoulei Grenade";
    if (strstr(cstr, "Smoke")) return "Smoke Grenade";
    if (strstr(cstr, "Burn")) return "Burn Grenade";

    // Melee Weapons
    if (strstr(cstr, "Dagger")) return "Dagger";
    if (strstr(cstr, "Machete")) return "Machete";

    if (strstr(cstr, "WEP") || strstr(cstr, "Pistol") ||
        strstr(cstr, "Rifle") || strstr(cstr, "Sniper") ||
        strstr(cstr, "Other") || strstr(cstr, "ShotGun") ||
        strstr(cstr, "MachineGun") || strstr(cstr, "Grenade")) {
        return GetShortWeaponName(str);
    }

    return str;
}
/* UnityHax-style: map full weapon class name to single icon character (for WeaponAsIcon). */
static std::string PlayerWeaponFont(const std::string& str)
{
    const char* c = str.c_str();
    if (strstr(c, "BP_Sniper_AWM_C")) return "C";
    if (strstr(c, "BP_Sniper_QBU_C")) return "j";
    if (strstr(c, "BP_Sniper_SLR_C")) return "(";
    if (strstr(c, "BP_Sniper_SKS_C")) return "E";
    if (strstr(c, "BP_Sniper_Mini14_C")) return "F";
    if (strstr(c, "BP_Sniper_M24_C")) return "I";
    if (strstr(c, "BP_Sniper_Kar98k_C")) return "r";
    if (strstr(c, "BP_Sniper_VSS_C")) return "N";
    if (strstr(c, "BP_Sniper_Win94_C")) return "l";
    if (strstr(c, "BP_Sniper_Mosin_C")) return "q";
    if (strstr(c, "BP_Sniper_MK12_C")) return "p";
    if (strstr(c, "BP_Sniper_Mk14_C") || strstr(c, "BP_WEP_Mk14")) return "R";
    if (strstr(c, "BP_Sniper_AMR_C")) return "D";
    if (strstr(c, "BP_Rifle_FAMAS_C")) return "k";
    if (strstr(c, "BP_Rifle_AUG_C")) return "n";
    if (strstr(c, "BP_Rifle_M762_C")) return "H";
    if (strstr(c, "BP_Rifle_SCAR_C")) return "J";
    if (strstr(c, "BP_Rifle_M416_C")) return "A";
    if (strstr(c, "BP_Rifle_HoneyBadger_C")) return "g";
    if (strstr(c, "BP_Rifle_M16A4_C")) return "m";
    if (strstr(c, "BP_Rifle_Mk47_C")) return "i";
    if (strstr(c, "BP_Rifle_G36_C")) return "y";
    if (strstr(c, "BP_Rifle_QBZ_C")) return "o";
    if (strstr(c, "BP_Rifle_AKM_C")) return "B";
    if (strstr(c, "BP_Rifle_Groza_C")) return "K";
    if (strstr(c, "BP_Rifle_ACE32_C")) return "z";
    if (strstr(c, "BP_Other_DP28_C")) return "L";
    if (strstr(c, "BP_Other_M249_C")) return "S";
    if (strstr(c, "BP_Other_MG3_C")) return "T";
    if (strstr(c, "BP_ShotGun_S12K_C")) return "U";
    if (strstr(c, "BP_ShotGun_DP12_C")) return "Y";
    if (strstr(c, "BP_ShotGun_S686_C")) return "W";
    if (strstr(c, "BP_ShotGun_S1897_C")) return "V";
    if (strstr(c, "BP_ShotGun_SawedOff_C")) return "X";
    if (strstr(c, "BP_ShotGun_Neostead2000_C")) return "s";
    if (strstr(c, "BP_ShotGun_M1014_C")) return "$";
    if (strstr(c, "BP_MachineGun_P90_C")) return "6";
    if (strstr(c, "BP_MachineGun_PP19_C")) return "7";
    if (strstr(c, "BP_MachineGun_TommyGun_C")) return "8";
    if (strstr(c, "BP_MachineGun_MP5K_C")) return "x";
    if (strstr(c, "BP_MachineGun_UMP9_C")) return "9";
    if (strstr(c, "BP_MachineGun_Vector_C")) return "Q";
    if (strstr(c, "BP_MachineGun_Uzi_C")) return "G";
    if (strstr(c, "BP_Pistol_Flaregun_C")) return "O";
    if (strstr(c, "BP_Pistol_R1895_C")) return ">";
    if (strstr(c, "BP_Pistol_Vz61_C")) return "@";
    if (strstr(c, "BP_Pistol_P92_C")) return "=";
    if (strstr(c, "BP_Pistol_P18C_C")) return "Z";
    if (strstr(c, "BP_Pistol_R45_C")) return ")";
    if (strstr(c, "BP_Pistol_P1911_C")) return "<";
    if (strstr(c, "BP_Pistol_DesertEagle_C")) return "W";
    if (strstr(c, "BP_Other_CrossBow_C") || strstr(c, "CrossbowBorderland")) return "P";
    if (strstr(c, "BP_Grenade_Shoulei")) return "b";
    if (strstr(c, "BP_Grenade_Smoke")) return "e";
    if (strstr(c, "BP_Grenade_Burn")) return "c";
    if (strstr(c, "BP_Grenade_Stun")) return "d";
    if (strstr(c, "BP_WEP_Pan_C")) return "p";
    return "";
}
#include "wininet.h"
#pragma comment (lib , "wininet.lib")
bool DownloadLib(const char* baseUrl, const char* filepath) {
    char fullUrl[512];
    std::time_t t = std::time(nullptr);
    sprintf_s(fullUrl, sizeof(fullUrl), "%s?ts=%lld", baseUrl, static_cast<long long>(t));
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return false;
    HINTERNET hFile = InternetOpenUrlA(
        hInternet,
        fullUrl,
        NULL,
        0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE,
        0
    );
    if (!hFile) {
        InternetCloseHandle(hInternet);
        return false;
    }
    const DWORD bufferSize = 4096;
    char buffer[bufferSize];
    DWORD bytesRead;
    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open()) {
        InternetCloseHandle(hFile);
        InternetCloseHandle(hInternet);
        return false;
    }
    while (InternetReadFile(hFile, buffer, bufferSize, &bytesRead) && bytesRead > 0) {
        out.write(buffer, bytesRead);
    }
    out.close();
    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);
    return true;
}
std::array<uint8_t, 4> BranchWithLink(uint32_t bl_address, uint32_t target_address)
{
    uint32_t pc = bl_address + 8;
    int32_t offset = static_cast<int32_t>(target_address - pc);
    uint32_t bl_offset = (offset / 4) & 0xFFFFFF;
    uint32_t bl_instruction = 0xEB000000 | bl_offset;

    return {
        static_cast<uint8_t>(bl_instruction & 0xFF),
        static_cast<uint8_t>((bl_instruction >> 8) & 0xFF),
        static_cast<uint8_t>((bl_instruction >> 16) & 0xFF),
        static_cast<uint8_t>((bl_instruction >> 24) & 0xFF)
    };
}
inline bool SafeWriteProtected(DWORD BaseAddress, PVOID data, SIZE_T length)
{
    if (BaseAddress == 0 || BaseAddress < 0x1000 || data == nullptr || length == 0)
    {
        return false;
    }

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQueryEx(Utility::GameHandle, (LPVOID)BaseAddress, &mbi, sizeof(mbi)) == 0)
    {
        return false;
    }

    if (mbi.State != MEM_COMMIT)
    {
        return false;
    }

    DWORD OldProtect;
    if (!VirtualProtectEx(Utility::GameHandle, (LPVOID)BaseAddress, length, PAGE_EXECUTE_READWRITE, &OldProtect))
    {
        return false;
    }

    ULONG written = 0;
    NTSTATUS status = NtWriteVirtualMemory(Utility::GameHandle, (LPVOID)BaseAddress, data, length, &written);

    VirtualProtectEx(Utility::GameHandle, (LPVOID)BaseAddress, length, OldProtect, &OldProtect);

    if (status != 0 || written != length)
    {
        return false;
    }

    return true;
}
int ApplyCommand(std::string command)
{
    command.insert(0, "/C ");

    SHELLEXECUTEINFOA ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = "cmd.exe";
    ShExecInfo.lpParameters = command.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    if (ShellExecuteExA(&ShExecInfo) == FALSE)
        return -1;

    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    DWORD rv;
    GetExitCodeProcess(ShExecInfo.hProcess, &rv);
    CloseHandle(ShExecInfo.hProcess);

    return rv;
}
static void RenderImGui_DX11(IDXGISwapChain* pSwapChain)
{

    if (!ImGui::GetIO().BackendRendererUserData)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice))))
        {
            g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
        }
    }

    if (!H::bShuttingDown)
    {
        if (!g_pd3dRenderTarget)
        {
            CreateRenderTarget(pSwapChain);
        }

        if (ImGui::GetCurrentContext() && g_pd3dRenderTarget)
        {
            RECT TempRect;
            POINT TempPoint;
            ZeroMemory(&TempRect, sizeof(RECT));
            ZeroMemory(&TempPoint, sizeof(POINT));
            GetClientRect(ScreenInfo::hWindow, &TempRect);
            ScreenInfo::Left = TempRect.left;
            ScreenInfo::Top = TempRect.top;
            ScreenInfo::Width = TempRect.right - TempRect.left;
            ScreenInfo::Height = TempRect.bottom - TempRect.top;
            if (FirstTimeInitImgui)
            {
               
                // NIX internal hack style (dark/red theme - full SetupImGuiStyle from NIX NEW-DRIVER)
                ImGuiStyle& style = ImGui::GetStyle();
                style.Alpha = 1.0f;
                style.WindowPadding = ImVec2(8.0f, 8.0f);
                style.WindowRounding = 0.0f;
                style.WindowBorderSize = 1.0f;
                style.WindowMinSize = ImVec2(32.0f, 32.0f);
                style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
                style.WindowMenuButtonPosition = ImGuiDir_Left;
                style.ChildRounding = 0.0f;
                style.ChildBorderSize = 1.0f;
                style.PopupRounding = 0.0f;
                style.PopupBorderSize = 1.0f;
                style.FramePadding = ImVec2(4.0f, 3.0f);
                style.FrameRounding = 0.0f;
                style.FrameBorderSize = 1.0f;
                style.ItemSpacing = ImVec2(8.0f, 4.0f);
                style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
                style.CellPadding = ImVec2(4.0f, 2.0f);
                style.IndentSpacing = 21.0f;
                style.ColumnsMinSpacing = 6.0f;
                style.ScrollbarSize = 14.0f;
                style.ScrollbarRounding = 9.0f;
                style.GrabMinSize = 10.0f;
                style.GrabRounding = 0.0f;
                style.TabRounding = 0.0f;
                style.TabBorderSize = 1.0f;
                style.TabMinWidthForCloseButton = 0.0f;
                style.ColorButtonPosition = ImGuiDir_Right;
                style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
                style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
                ImVec4* colors = style.Colors;
                colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.72941178f, 0.74901962f, 0.73725492f, 1.0f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
                colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.07843137f, 0.07843137f, 0.07843137f, 0.94f);
                colors[ImGuiCol_Border] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.54f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17647059f, 0.17647059f, 0.17647059f, 0.40f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 0.67f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 0.65f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 0.67f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784f, 0.01960784f, 0.01960784f, 0.53f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30980393f, 0.30980393f, 0.30980393f, 1.0f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40784314f, 0.40784314f, 0.40784314f, 1.0f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 1.0f);
                colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.38039216f, 0.38039216f, 1.0f);
                colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.54f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_Header] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.27058825f, 0.27058825f, 0.27058825f, 1.0f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.35294119f, 0.35294119f, 0.35294119f, 1.0f);
                colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
                colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.78f);
                colors[ImGuiCol_SeparatorActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(0.20784314f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.38039216f, 0.38039216f, 1.0f);
                colors[ImGuiCol_Tab] = ImVec4(0.07843137f, 0.07843137f, 0.07843137f, 0.40f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.35294119f, 0.35294119f, 0.35294119f, 1.0f);
                colors[ImGuiCol_TabActive] = ImVec4(0.19607843f, 0.19607843f, 0.19607843f, 1.0f);
                colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.97f);
                colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.63137257f, 0.44705883f, 1.0f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.56470588f, 0.0f, 1.0f);
                colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18823530f, 0.18823530f, 0.18823530f, 1.0f);
                colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
                colors[ImGuiCol_TableBorderLight] = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
                colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 0.98f, 0.89f, 0.06f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(0.18431373f, 0.39607844f, 0.85882354f, 0.90f);
                colors[ImGuiCol_DragDropTarget] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                ImGuiIO &io = ImGui::GetIO();
                ImFontConfig CustomFont2;
                CustomFont2.MergeMode = true;
                CustomFont2.FontDataOwnedByAtlas = true;
                CustomFont2.PixelSnapH = true;
                ImFontConfig UnicodeConfig;
                UnicodeConfig.PixelSnapH = true;
                UnicodeConfig.MergeMode = false;
                UnicodeConfig.OversampleH = 1;
                UnicodeConfig.OversampleV = 1;
                static const ImWchar ranges[] = { 0x0020, static_cast<ImWchar>(0x10FFFF), 0 };
                float baseFontSize = 23;                         // 13.0f is the size of the default font. Change to the font size you use.
                float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
                io.Fonts->AddFontFromMemoryTTF(LiteFont_char, sizeof(LiteFont_char), 15.5, 0, ranges);
                ImGui::MergeIconsWithLatestFont(15.5);

                // Use Tahoma as a fallback for Arabic/Farsi/Chinese/Russian player names. Tahoma is universally available on Windows.
                std::string fontPath = "C:\\Windows\\Fonts\\tahoma.ttf";

                Fonts::MainFont = io.Fonts->AddFontFromMemoryTTF(LiteFont_char, sizeof(LiteFont_char), 18, &CustomFont2, ranges);
                Fonts::FontAwesome = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, sizeof(font_awesome_data), iconFontSize, &CustomFont2, ranges);
                Fonts::UnicodeFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16, &UnicodeConfig, ranges);
                if (!Fonts::UnicodeFont) Fonts::UnicodeFont = Fonts::MainFont; // Fallback just in case

                FirstTimeInitImgui = false;
            }
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            /* Apply settings-tab transparency and font size (working) */
            {
                ImGuiStyle& style = ImGui::GetStyle();
                int mt = Setting::MenuTransparency;
                int bt = Setting::BackgroundTransparency;
                float fs = Setting::MenuFontSize;
                if (mt < 0) mt = 0; if (mt > 255) mt = 255;
                if (bt < 0) bt = 0; if (bt > 255) bt = 255;
                if (fs < 8.f) fs = 8.f; if (fs > 30.f) fs = 30.f;
                style.Alpha = (float)mt / 255.f;
                style.Colors[ImGuiCol_WindowBg].w = (float)bt / 255.f;
                style.Colors[ImGuiCol_ChildBg].w = (float)bt / 255.f;
                ImGui::GetIO().FontGlobalScale = fs / 18.f;
            }
            if (Setting::bOverlayHidden)
            {
                ImGui::EndFrame();
                ImGui::RenderNotifications();
                ImGui::Render();
                if (!s_pSetWindowDisplayAffinity)
                    s_pSetWindowDisplayAffinity = (PFN_SetWindowDisplayAffinity)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowDisplayAffinity");
                HWND hwndMain = s_GameWindow ? s_GameWindow : ScreenInfo::hWindow;
                if (hwndMain && IsWindow(hwndMain) && s_pSetWindowDisplayAffinity)
                    s_pSetWindowDisplayAffinity(hwndMain, Setting::AntiScreenCapture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pd3dRenderTarget, NULL);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                return;
            }
            {
                bool ToSkip = true;
                if (isLogin)
                {
                    if (Cheat::UploadPic == true)
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
                        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
                        ImGui::SetNextWindowPos(ImVec2(0, ScreenInfo::Height / 2));
                        ImGui::Begin("", 0, Flags);
                        ImGui::PopStyleColor();
                        ImGui::PopStyleVar();
                        ImGui::Text((Setting::LanguageVNM == true) ? (u8"Bạn có chắc muốn upload hình?") : ("Are you sure want to upload picture?"));
                        if (ImGui::Button("OK", ImVec2(ImGui::GetWindowSize().x / 2 - 15, 0)))
                        {
                            Cheat::UploadPic = false;
                            Cheat::UploadPicture = 1;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button((Setting::LanguageVNM == true) ? (u8"Hủy bỏ") : (u8"Cancel"), ImVec2(-1, 0)))
                        {
                            Cheat::UploadPic = false;
                            Cheat::UploadPicture = 2;
                        }
                        ImGui::End();
                    }

                    if (Setting::ShowMenu)
                    {
                        DrawOldMenu();
                    }

                    // Handle hack unload (Button or END key)
                    if (IsExitHack || (GetAsyncKeyState(VK_END) & 1))
                    {
                        H::bShuttingDown = true;
                        U::UnloadDLL();
                    }

                    if (Setting::InfoOverlay)
                    {
                        DrawTotalEnemiesRedBar();
                    }
                    if (Setting::QoL::bShowSpectatorList && Data::NetDriver > 0) DrawSpectatorList();


                    if (Data::NetDriver > 0)
                    {
                        if (GetAsyncKeyState(VK_F5) & 1)
                        {
                            if (Setting::Vehicle::IsShowVehicle == false || Setting::PickupItems::Visual::IsShowItems == false)
                            {
                                Setting::Vehicle::IsShowVehicle = true;
                                Setting::PickupItems::Visual::IsShowItems = true;
                            }
                            else
                            {
                                Setting::Vehicle::IsShowVehicle = false;
                                Setting::PickupItems::Visual::IsShowItems = false;
                            }
                            if (Setting::Vehicle::IsShowVehicle == true && Setting::PickupItems::Visual::IsShowItems == true)
                            {
                                ImGui::InsertNotification({ImGuiToastType_Info, 2500, (Setting::LanguageVNM == 1) ? u8"Chế độ ẩn Item + Xe: Off" : "Hide all Item + Vehicle: Off"});
                            }
                            else if (Setting::Vehicle::IsShowVehicle == false && Setting::PickupItems::Visual::IsShowItems == false)
                            {
                                ImGui::InsertNotification({ImGuiToastType_Info, 2500, (Setting::LanguageVNM == 1) ? u8"Chế độ ẩn Item + Xe: On" : "Hide all Item + Vehicle: On"});
                            }
                        }
                        if (GetAsyncKeyState(VK_F6) & 1)
                        {
                            if (Setting::TypeAim == 0)
                                Setting::TypeAim = 1;
                            else if (Setting::TypeAim == 1)
                                Setting::TypeAim = 0;

                            if (Setting::TypeAim == 0 || Setting::TypeAim == 1)
                            {
                                ImGui::InsertNotification({ ImGuiToastType_Warning, 3000, "Aimbot has been Enabled" });
                            }
                        }
                    }
                    if (Setting::SpecialFeatures::GameInformation) {
                        static float pulseTimer = 0.0f;
                        if (Data::NetDriver > 0 && (!Data::LocalbDead || Setting::QoL::bESPWhenSpectating)) {
                            // Pulsing Logic
                            pulseTimer += ImGui::GetIO().DeltaTime * 2.5f;
                            float pulseAlpha = (sin(pulseTimer) + 1.0f) * 0.5f;
                            ImVec4 accentRed = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                            ImVec4 pulseColor = ImVec4(1.0f, 0.2f, 0.2f, 0.4f + (pulseAlpha * 0.6f));

                            // --- PREMIUM GLASS DASHBOARD ---
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12);
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 12));
                            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.04f, 0.07f, 0.65f)); // Glass Transparency
                            ImGui::PushStyleColor(ImGuiCol_Border, (Data::TotalCheater > 0) ? accentRed : ImVec4(1, 1, 1, 0.15f));
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.5f);

                            ImGuiIO& io = ImGui::GetIO();
                            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.02f, io.DisplaySize.y * 0.05f), ImGuiCond_FirstUseEver);

                            ImGui::Begin("##NIXLOOP_CORE", nullptr, 
                                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

                            // 🔴 HEADER
                            ImGui::TextColored(pulseColor, ICON_FA_SHIELD_ALT);
                            ImGui::SameLine();
                            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Using primary bold font
                            ImGui::TextColored(ImVec4(1, 1, 1, 1), "NIXLOOP CORE SYSTEM");
                            ImGui::PopFont();
                            
                            ImGui::Spacing();
                            ImGui::PushStyleColor(ImGuiCol_Separator, accentRed);
                            ImGui::Separator();
                            ImGui::PopStyleColor();
                            ImGui::Spacing();

                            // STATS LIST (Premium Descriptive Text)
                            auto PremiumStat = [&](const char* icon, const char* label, int val, ImVec4 valColor) {
                                ImGui::TextColored(accentRed, icon);
                                ImGui::SameLine(35);
                                ImGui::Text("%s", label);
                                ImGui::SameLine(130); // Reduced from 165 for tighter look
                                ImGui::TextColored(valColor, "%d", val);
                            };

                            PremiumStat(ICON_FA_USER, "Alive :", Data::AlivePlayer, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
                            PremiumStat(ICON_FA_USER_COG, "Real :", Data::PlayerNum, ImVec4(0.3f, 0.8f, 1.0f, 1.0f));
                            PremiumStat(ICON_FA_USERS, "Teams :", Data::AliveTeam, ImVec4(1, 1, 1, 1));
                            

                            ImGui::Spacing();
                            ImGui::PushStyleColor(ImGuiCol_Separator, accentRed);
                            ImGui::Separator();
                            ImGui::PopStyleColor();
                            ImGui::Spacing();
                            
                            // STATUS / TIME
                            if (!Data::LocalbDead) {
                                ImGui::TextColored(accentRed, ICON_FA_CLOCK);
                                ImGui::SameLine(35);
                                ImGui::Text("Match Time :");
                                ImGui::SameLine(130);
                                if (Data::ElapsedTime < 60)
                                    ImGui::TextColored(ImVec4(1, 0.8f, 0.2f, 1), "%d sec", Data::ElapsedTime);
                                else
                                    ImGui::TextColored(ImVec4(1, 0.8f, 0.2f, 1), "%d:%02d min", Data::ElapsedTime / 60, Data::ElapsedTime % 60);
                            } else {
                                ImGui::TextColored(accentRed, ICON_FA_EXCLAMATION_TRIANGLE);
                                ImGui::SameLine(35);
                                ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Connected to Lobby");
                            }

                            ImGui::End();
                            ImGui::PopStyleVar(3);
                            ImGui::PopStyleColor(2);
                        }
                    }
                    if (Setting::Watermark)
                    {
                        ImGui::SetNextWindowPos({1400, 100});
                        string TYPEVIP = "Custom-Hax | Type key : VIP";
                        string TYPEFREE = "Custom-Hax | Type key : FREE";
                        string TYPEADMIN = "Custom-Hax | Type key : ADMIN";
                        ImGui::Begin("####Watermark", &ToSkip, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
                        if (Auth::bAdmin == false)
                        {
                            ImGui::Text(TYPEVIP.c_str());
                        }
                        if (Auth::bAdmin == true)
                        {
                            ImGui::Text(TYPEADMIN.c_str());
                        }
                        ImGui::End();
                    }

                    if (Data::NetDriver > 0 && (!Data::LocalbDead || Setting::QoL::bESPWhenSpectating))
                    {
                    int Distance = 0;
                    float HeightOfName = 0;
                    ImColor ColTeamID;
                    ImColor ColAActor;
                    for (Item &Item : Data::ItemList)
                    {
                        if (!Algorithm::WorldToScreen(Item.Position, Screen, Data::ViewMatrixBase))
                            continue;
                        Distance = sqrt(pow(Data::LocalPosition.X - Item.Position.X, 2) + pow(Data::LocalPosition.Y - Item.Position.Y, 2) + pow(Data::LocalPosition.Z - Item.Position.Z, 2)) / 100;
                        TextToDraw = Item.Name;
                        if (Setting::SpecialFeatures::ArNearest && (Data::IsFreeUser == -1 || Data::IsFreeUser == 0))
                        {
                            if (Item.Name == "M16A4" || Item.Name == "SCARL" || Item.Name == "M416" || Item.Name == "FAMAS" || Item.Name == "QBZ" || Item.Name == "G63C" || Item.Name == "AUG" || Item.Name == "AKM" || Item.Name == "M762" || Item.Name == "HoneyBadger" || Item.Name == "GROZA")
                            {
                                ARNearest WepReturn;
                                WepReturn.Distance = Distance;
                                WepReturn.Position = Screen;
                                ARNearestCache.push_back(WepReturn);
                            }
                        }
                        if (Setting::PickupItems::Visual::IsShowItems)
                        {
                            TextToDraw = TextToDraw + " [" + to_string((int)Distance) + " M]";
#pragma region Rifles
                            if (Item.Name == "M16A4" && Setting::PickupItems::Rifles::M16A4)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::M16A4_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::M16A4_Color[0], Setting::PickupItems::Rifles::M16A4_Color[1], Setting::PickupItems::Rifles::M16A4_Color[2], Setting::PickupItems::Rifles::M16A4_Color[3]));
                            }
                            if (Item.Name == "SCARL" && Setting::PickupItems::Rifles::SCARL)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::SCARL_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::SCARL_Color[0], Setting::PickupItems::Rifles::SCARL_Color[1], Setting::PickupItems::Rifles::SCARL_Color[2], Setting::PickupItems::Rifles::SCARL_Color[3]));
                            }
                            if (Item.Name == "M416" && Setting::PickupItems::Rifles::M416)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::M416_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::M416_Color[0], Setting::PickupItems::Rifles::M416_Color[1], Setting::PickupItems::Rifles::M416_Color[2], Setting::PickupItems::Rifles::M416_Color[3]));
                            }
                            if (Item.Name == "FAMAS" && Setting::PickupItems::Rifles::FAMAS)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::FAMAS_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::FAMAS_Color[0], Setting::PickupItems::Rifles::FAMAS_Color[1], Setting::PickupItems::Rifles::FAMAS_Color[2], Setting::PickupItems::Rifles::FAMAS_Color[3]));
                            }
                            if (Item.Name == "QBZ" && Setting::PickupItems::Rifles::QBZ)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::QBZ_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::QBZ_Color[0], Setting::PickupItems::Rifles::QBZ_Color[1], Setting::PickupItems::Rifles::QBZ_Color[2], Setting::PickupItems::Rifles::QBZ_Color[3]));
                            }
                            if (Item.Name == "G63C" && Setting::PickupItems::Rifles::G63C)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::G63C_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::G63C_Color[0], Setting::PickupItems::Rifles::G63C_Color[1], Setting::PickupItems::Rifles::G63C_Color[2], Setting::PickupItems::Rifles::G63C_Color[3]));
                            }
                            if (Item.Name == "AUG" && Setting::PickupItems::Rifles::AUG)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::AUG_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::AUG_Color[0], Setting::PickupItems::Rifles::AUG_Color[1], Setting::PickupItems::Rifles::AUG_Color[2], Setting::PickupItems::Rifles::AUG_Color[3]));
                            }
                            if (Item.Name == "AKM" && Setting::PickupItems::Rifles::AKM)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::AKM_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::AKM_Color[0], Setting::PickupItems::Rifles::AKM_Color[1], Setting::PickupItems::Rifles::AKM_Color[2], Setting::PickupItems::Rifles::AKM_Color[3]));
                            }
                            if (Item.Name == "M762" && Setting::PickupItems::Rifles::M762)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::M762_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::M762_Color[0], Setting::PickupItems::Rifles::M762_Color[1], Setting::PickupItems::Rifles::M762_Color[2], Setting::PickupItems::Rifles::M762_Color[3]));
                            }
                            if (Item.Name == "HoneyBadger" && Setting::PickupItems::Rifles::HoneyBadger)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::HoneyBadger_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::HoneyBadger_Color[0], Setting::PickupItems::Rifles::HoneyBadger_Color[1], Setting::PickupItems::Rifles::HoneyBadger_Color[2], Setting::PickupItems::Rifles::HoneyBadger_Color[3]));
                            }
                            if (Item.Name == "GROZA" && Setting::PickupItems::Rifles::GROZA)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::GROZA_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::GROZA_Color[0], Setting::PickupItems::Rifles::GROZA_Color[1], Setting::PickupItems::Rifles::GROZA_Color[2], Setting::PickupItems::Rifles::GROZA_Color[3]));
                            }
                            if (Item.Name == "MK14" && Setting::PickupItems::Rifles::MK14)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Rifles::MK14_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Rifles::MK14_Color[0], Setting::PickupItems::Rifles::MK14_Color[1], Setting::PickupItems::Rifles::MK14_Color[2], Setting::PickupItems::Rifles::MK14_Color[3]));
                            }
#pragma endregion
#pragma region SMG
                            if (Item.Name == "THOMPSON" && Setting::PickupItems::SMG::THOMPSON)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::THOMPSON_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::THOMPSON_Color[0], Setting::PickupItems::SMG::THOMPSON_Color[1], Setting::PickupItems::SMG::THOMPSON_Color[2], Setting::PickupItems::SMG::THOMPSON_Color[3]));
                            }
                            if (Item.Name == "UMP45" && Setting::PickupItems::SMG::UMP45)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::UMP45_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::UMP45_Color[0], Setting::PickupItems::SMG::UMP45_Color[1], Setting::PickupItems::SMG::UMP45_Color[2], Setting::PickupItems::SMG::UMP45_Color[3]));
                            }
                            if (Item.Name == "P90" && Setting::PickupItems::SMG::P90)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::P90_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::P90_Color[0], Setting::PickupItems::SMG::P90_Color[1], Setting::PickupItems::SMG::P90_Color[2], Setting::PickupItems::SMG::P90_Color[3]));
                            }
                            if (Item.Name == "UZI" && Setting::PickupItems::SMG::UZI)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::UZI_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::UZI_Color[0], Setting::PickupItems::SMG::UZI_Color[1], Setting::PickupItems::SMG::UZI_Color[2], Setting::PickupItems::SMG::UZI_Color[3]));
                            }
                            if (Item.Name == "BIZON" && Setting::PickupItems::SMG::BIZON)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::BIZON_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::BIZON_Color[0], Setting::PickupItems::SMG::BIZON_Color[1], Setting::PickupItems::SMG::BIZON_Color[2], Setting::PickupItems::SMG::BIZON_Color[3]));
                            }
                            if (Item.Name == "MP5K" && Setting::PickupItems::SMG::MP5K)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::MP5K_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::MP5K_Color[0], Setting::PickupItems::SMG::MP5K_Color[1], Setting::PickupItems::SMG::MP5K_Color[2], Setting::PickupItems::SMG::MP5K_Color[3]));
                            }
                            if (Item.Name == "VECTOR" && Setting::PickupItems::SMG::VECTOR)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::SMG::VECTOR_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::SMG::VECTOR_Color[0], Setting::PickupItems::SMG::VECTOR_Color[1], Setting::PickupItems::SMG::VECTOR_Color[2], Setting::PickupItems::SMG::VECTOR_Color[3]));
                            }
#pragma endregion
#pragma region Sniper
                            if (Item.Name == "WIN94" && Setting::PickupItems::Sniper::WIN94)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::WIN94_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::WIN94_Color[0], Setting::PickupItems::Sniper::WIN94_Color[1], Setting::PickupItems::Sniper::WIN94_Color[2], Setting::PickupItems::Sniper::WIN94_Color[3]));
                            }
                            if (Item.Name == "VSS" && Setting::PickupItems::Sniper::VSS)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::VSS_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::VSS_Color[0], Setting::PickupItems::Sniper::VSS_Color[1], Setting::PickupItems::Sniper::VSS_Color[2], Setting::PickupItems::Sniper::VSS_Color[3]));
                            }
                            if (Item.Name == "MINI14" && Setting::PickupItems::Sniper::MINI14)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::MINI14_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MINI14_Color[0], Setting::PickupItems::Sniper::MINI14_Color[1], Setting::PickupItems::Sniper::MINI14_Color[2], Setting::PickupItems::Sniper::MINI14_Color[3]));
                            }
                            if (Item.Name == "QBU" && Setting::PickupItems::Sniper::QBU)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::QBU_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::QBU_Color[0], Setting::PickupItems::Sniper::QBU_Color[1], Setting::PickupItems::Sniper::QBU_Color[2], Setting::PickupItems::Sniper::QBU_Color[3]));
                            }
                            if (Item.Name == "MK12" && Setting::PickupItems::Sniper::MK12)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::MK12_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MK12_Color[0], Setting::PickupItems::Sniper::MK12_Color[1], Setting::PickupItems::Sniper::MK12_Color[2], Setting::PickupItems::Sniper::MK12_Color[3]));
                            }
                            if (Item.Name == "MK47" && Setting::PickupItems::Sniper::MK47)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::MK47_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MK47_Color[0], Setting::PickupItems::Sniper::MK47_Color[1], Setting::PickupItems::Sniper::MK47_Color[2], Setting::PickupItems::Sniper::MK47_Color[3]));
                            }
                            if (Item.Name == "SLR" && Setting::PickupItems::Sniper::SLR)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::SLR_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::SLR_Color[0], Setting::PickupItems::Sniper::SLR_Color[1], Setting::PickupItems::Sniper::SLR_Color[2], Setting::PickupItems::Sniper::SLR_Color[3]));
                            }
                            if (Item.Name == "SKS" && Setting::PickupItems::Sniper::SKS)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::SKS_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::SKS_Color[0], Setting::PickupItems::Sniper::SKS_Color[1], Setting::PickupItems::Sniper::SKS_Color[2], Setting::PickupItems::Sniper::SKS_Color[3]));
                            }
                            if (Item.Name == "KAR98" && Setting::PickupItems::Sniper::KAR98)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::KAR98_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::KAR98_Color[0], Setting::PickupItems::Sniper::KAR98_Color[1], Setting::PickupItems::Sniper::KAR98_Color[2], Setting::PickupItems::Sniper::KAR98_Color[3]));
                            }
                            if (Item.Name == "MOSIN" && Setting::PickupItems::Sniper::MOSIN)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::MOSIN_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::MOSIN_Color[0], Setting::PickupItems::Sniper::MOSIN_Color[1], Setting::PickupItems::Sniper::MOSIN_Color[2], Setting::PickupItems::Sniper::MOSIN_Color[3]));
                            }
                            if (Item.Name == "BOW" && Setting::PickupItems::Sniper::BOW)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::BOW_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::BOW_Color[0], Setting::PickupItems::Sniper::BOW_Color[1], Setting::PickupItems::Sniper::BOW_Color[2], Setting::PickupItems::Sniper::BOW_Color[3]));
                            }
                            if (Item.Name == "AWM" && Setting::PickupItems::Sniper::AWM)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::AWM_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::AWM_Color[0], Setting::PickupItems::Sniper::AWM_Color[1], Setting::PickupItems::Sniper::AWM_Color[2], Setting::PickupItems::Sniper::AWM_Color[3]));
                            }
                            if (Item.Name == "AMR" && Setting::PickupItems::Sniper::AMR)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Sniper::AMR_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Sniper::AMR_Color[0], Setting::PickupItems::Sniper::AMR_Color[1], Setting::PickupItems::Sniper::AMR_Color[2], Setting::PickupItems::Sniper::AMR_Color[3]));
                            }
#pragma endregion
#pragma region AutoMachine
                            if (Item.Name == "M249" && Setting::PickupItems::AutoMachine::M249)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::AutoMachine::M249_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::AutoMachine::M249_Color[0], Setting::PickupItems::AutoMachine::M249_Color[1], Setting::PickupItems::AutoMachine::M249_Color[2], Setting::PickupItems::AutoMachine::M249_Color[3]));
                            }
                            if (Item.Name == "MG3" && Setting::PickupItems::AutoMachine::MG3)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::AutoMachine::MG3_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::AutoMachine::MG3_Color[0], Setting::PickupItems::AutoMachine::MG3_Color[1], Setting::PickupItems::AutoMachine::MG3_Color[2], Setting::PickupItems::AutoMachine::MG3_Color[3]));
                            }
                            if (Item.Name == "DP28" && Setting::PickupItems::AutoMachine::DP28)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::AutoMachine::DP28_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::AutoMachine::DP28_Color[0], Setting::PickupItems::AutoMachine::DP28_Color[1], Setting::PickupItems::AutoMachine::DP28_Color[2], Setting::PickupItems::AutoMachine::DP28_Color[3]));
                            }
#pragma endregion
#pragma region ShotGun
                            if (Item.Name == "S686" && Setting::PickupItems::ShotGun::S686)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::ShotGun::S686_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::S686_Color[0], Setting::PickupItems::ShotGun::S686_Color[1], Setting::PickupItems::ShotGun::S686_Color[2], Setting::PickupItems::ShotGun::S686_Color[3]));
                            }
                            if (Item.Name == "S1897" && Setting::PickupItems::ShotGun::S1897)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::ShotGun::S1897_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::S1897_Color[0], Setting::PickupItems::ShotGun::S1897_Color[1], Setting::PickupItems::ShotGun::S1897_Color[2], Setting::PickupItems::ShotGun::S1897_Color[3]));
                            }
                            if (Item.Name == "M1014" && Setting::PickupItems::ShotGun::M1014)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::ShotGun::M1014_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::M1014_Color[0], Setting::PickupItems::ShotGun::M1014_Color[1], Setting::PickupItems::ShotGun::M1014_Color[2], Setting::PickupItems::ShotGun::M1014_Color[3]));
                            }
                            if (Item.Name == "Neostead2000" && Setting::PickupItems::ShotGun::Neostead2000)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::ShotGun::Neostead2000_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::Neostead2000_Color[0], Setting::PickupItems::ShotGun::Neostead2000_Color[1], Setting::PickupItems::ShotGun::Neostead2000_Color[2], Setting::PickupItems::ShotGun::Neostead2000_Color[3]));
                            }
                            if (Item.Name == "S12K" && Setting::PickupItems::ShotGun::S12K)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::ShotGun::S12K_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::S12K_Color[0], Setting::PickupItems::ShotGun::S12K_Color[1], Setting::PickupItems::ShotGun::S12K_Color[2], Setting::PickupItems::ShotGun::S12K_Color[3]));
                            }
                            if (Item.Name == "DBS" && Setting::PickupItems::ShotGun::DBS)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::ShotGun::DBS_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::ShotGun::DBS_Color[0], Setting::PickupItems::ShotGun::DBS_Color[1], Setting::PickupItems::ShotGun::DBS_Color[2], Setting::PickupItems::ShotGun::DBS_Color[3]));
                            }
#pragma endregion
#pragma region Pistols
                            if (Item.Name == "P1911" && Setting::PickupItems::Pistols::P1911)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::P1911_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::P1911_Color[0], Setting::PickupItems::Pistols::P1911_Color[1], Setting::PickupItems::Pistols::P1911_Color[2], Setting::PickupItems::Pistols::P1911_Color[3]));
                            }
                            if (Item.Name == "R45" && Setting::PickupItems::Pistols::R45)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::R45_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::R45_Color[0], Setting::PickupItems::Pistols::R45_Color[1], Setting::PickupItems::Pistols::R45_Color[2], Setting::PickupItems::Pistols::R45_Color[3]));
                            }
                            if (Item.Name == "DesertEagle" && Setting::PickupItems::Pistols::DesertEagle)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::DesertEagle_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::DesertEagle_Color[0], Setting::PickupItems::Pistols::DesertEagle_Color[1], Setting::PickupItems::Pistols::DesertEagle_Color[2], Setting::PickupItems::Pistols::DesertEagle_Color[3]));
                            }
                            if (Item.Name == "P92" && Setting::PickupItems::Pistols::P92)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::P92_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::P92_Color[0], Setting::PickupItems::Pistols::P92_Color[1], Setting::PickupItems::Pistols::P92_Color[2], Setting::PickupItems::Pistols::P92_Color[3]));
                            }
                            if (Item.Name == "P18C" && Setting::PickupItems::Pistols::P18C)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::P18C_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::P18C_Color[0], Setting::PickupItems::Pistols::P18C_Color[1], Setting::PickupItems::Pistols::P18C_Color[2], Setting::PickupItems::Pistols::P18C_Color[3]));
                            }
                            if (Item.Name == "Vz61" && Setting::PickupItems::Pistols::Vz61)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::Vz61_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::Vz61_Color[0], Setting::PickupItems::Pistols::Vz61_Color[1], Setting::PickupItems::Pistols::Vz61_Color[2], Setting::PickupItems::Pistols::Vz61_Color[3]));
                            }
                            if (Item.Name == "R1895" && Setting::PickupItems::Pistols::R1895)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::R1895_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::R1895_Color[0], Setting::PickupItems::Pistols::R1895_Color[1], Setting::PickupItems::Pistols::R1895_Color[2], Setting::PickupItems::Pistols::R1895_Color[3]));
                            }
                            if (Item.Name == "Flaregun" && Setting::PickupItems::Pistols::Flaregun)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Pistols::Flaregun_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Pistols::Flaregun_Color[0], Setting::PickupItems::Pistols::Flaregun_Color[1], Setting::PickupItems::Pistols::Flaregun_Color[2], Setting::PickupItems::Pistols::Flaregun_Color[3]));
                            }
#pragma endregion
#pragma region NeccessaryHealth
                            if (Item.Name == "Adrenaline" && Setting::PickupItems::NeccessaryHealth::Adrenaline)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::NeccessaryHealth::Adrenaline_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[0], Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[1], Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[2], Setting::PickupItems::NeccessaryHealth::Adrenaline_Color[3]));
                            }
                            if (Item.Name == "EnergyDrink" && Setting::PickupItems::NeccessaryHealth::EnergyDrink)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::NeccessaryHealth::EnergyDrink_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[0], Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[1], Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[2], Setting::PickupItems::NeccessaryHealth::EnergyDrink_Color[3]));
                            }
                            if (Item.Name == "Painkiller" && Setting::PickupItems::NeccessaryHealth::Painkiller)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::NeccessaryHealth::Painkiller_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::Painkiller_Color[0], Setting::PickupItems::NeccessaryHealth::Painkiller_Color[1], Setting::PickupItems::NeccessaryHealth::Painkiller_Color[2], Setting::PickupItems::NeccessaryHealth::Painkiller_Color[3]));
                            }
                            if (Item.Name == "FirstaidKit" && Setting::PickupItems::NeccessaryHealth::FirstaidKit)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::NeccessaryHealth::FirstaidKit_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[0], Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[1], Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[2], Setting::PickupItems::NeccessaryHealth::FirstaidKit_Color[3]));
                            }
                            if (Item.Name == "Medkit" && Setting::PickupItems::NeccessaryHealth::Medkit)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::NeccessaryHealth::Medkit_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::NeccessaryHealth::Medkit_Color[0], Setting::PickupItems::NeccessaryHealth::Medkit_Color[1], Setting::PickupItems::NeccessaryHealth::Medkit_Color[2], Setting::PickupItems::NeccessaryHealth::Medkit_Color[3]));
                            }
#pragma endregion
#pragma region Melee
                            if (Item.Name == "Pan" && Setting::PickupItems::Melee::Pan)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Melee::Pan_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Melee::Pan_Color[0], Setting::PickupItems::Melee::Pan_Color[1], Setting::PickupItems::Melee::Pan_Color[2], Setting::PickupItems::Melee::Pan_Color[3]));
                            }
#pragma endregion
#pragma region Bag
                            if (Item.Name == "BagLv1" && Setting::PickupItems::Bag::BagLv1)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Bag::BagLv1_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Bag::BagLv1_Color[0], Setting::PickupItems::Bag::BagLv1_Color[1], Setting::PickupItems::Bag::BagLv1_Color[2], Setting::PickupItems::Bag::BagLv1_Color[3]));
                            }
                            if (Item.Name == "BagLv2" && Setting::PickupItems::Bag::BagLv2)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Bag::BagLv2_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Bag::BagLv2_Color[0], Setting::PickupItems::Bag::BagLv2_Color[1], Setting::PickupItems::Bag::BagLv2_Color[2], Setting::PickupItems::Bag::BagLv2_Color[3]));
                            }
                            if (Item.Name == "BagLv3" && Setting::PickupItems::Bag::BagLv3)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Bag::BagLv3_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Bag::BagLv3_Color[0], Setting::PickupItems::Bag::BagLv3_Color[1], Setting::PickupItems::Bag::BagLv3_Color[2], Setting::PickupItems::Bag::BagLv3_Color[3]));
                            }
#pragma endregion
#pragma region Armor
                            if (Item.Name == "ArmorLv1" && Setting::PickupItems::Armor::ArmorLv1)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Armor::ArmorLv1_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Armor::ArmorLv1_Color[0], Setting::PickupItems::Armor::ArmorLv1_Color[1], Setting::PickupItems::Armor::ArmorLv1_Color[2], Setting::PickupItems::Armor::ArmorLv1_Color[3]));
                            }
                            if (Item.Name == "ArmorLv2" && Setting::PickupItems::Armor::ArmorLv2)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Armor::ArmorLv2_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Armor::ArmorLv2_Color[0], Setting::PickupItems::Armor::ArmorLv2_Color[1], Setting::PickupItems::Armor::ArmorLv2_Color[2], Setting::PickupItems::Armor::ArmorLv2_Color[3]));
                            }
                            if (Item.Name == "ArmorLv3" && Setting::PickupItems::Armor::ArmorLv3)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Armor::ArmorLv3_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Armor::ArmorLv3_Color[0], Setting::PickupItems::Armor::ArmorLv3_Color[1], Setting::PickupItems::Armor::ArmorLv3_Color[2], Setting::PickupItems::Armor::ArmorLv3_Color[3]));
                            }
#pragma endregion
#pragma region Helmet
                            if (Item.Name == "HelmetLv1" && Setting::PickupItems::Helmet::HelmetLv1)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Helmet::HelmetLv1_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Helmet::HelmetLv1_Color[0], Setting::PickupItems::Helmet::HelmetLv1_Color[1], Setting::PickupItems::Helmet::HelmetLv1_Color[2], Setting::PickupItems::Helmet::HelmetLv1_Color[3]));
                            }
                            if (Item.Name == "HelmetLv2" && Setting::PickupItems::Helmet::HelmetLv2)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Helmet::HelmetLv2_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Helmet::HelmetLv2_Color[0], Setting::PickupItems::Helmet::HelmetLv2_Color[1], Setting::PickupItems::Helmet::HelmetLv2_Color[2], Setting::PickupItems::Helmet::HelmetLv2_Color[3]));
                            }
                            if (Item.Name == "HelmetLv3" && Setting::PickupItems::Helmet::HelmetLv3)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Helmet::HelmetLv3_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Helmet::HelmetLv3_Color[0], Setting::PickupItems::Helmet::HelmetLv3_Color[1], Setting::PickupItems::Helmet::HelmetLv3_Color[2], Setting::PickupItems::Helmet::HelmetLv3_Color[3]));
                            }
#pragma endregion
#pragma region Accessory
                            if (Item.Name == "AREx" && Setting::PickupItems::Accessory::AREx)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::AREx_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::AREx_Color[0], Setting::PickupItems::Accessory::AREx_Color[1], Setting::PickupItems::Accessory::AREx_Color[2], Setting::PickupItems::Accessory::AREx_Color[3]));
                            }
                            if (Item.Name == "ARExQk" && Setting::PickupItems::Accessory::ARExQk)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::ARExQk_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::ARExQk_Color[0], Setting::PickupItems::Accessory::ARExQk_Color[1], Setting::PickupItems::Accessory::ARExQk_Color[2], Setting::PickupItems::Accessory::ARExQk_Color[3]));
                            }
                            if (Item.Name == "LargeCompensator" && Setting::PickupItems::Accessory::LargeCompensator)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::LargeCompensator_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::LargeCompensator_Color[0], Setting::PickupItems::Accessory::LargeCompensator_Color[1], Setting::PickupItems::Accessory::LargeCompensator_Color[2], Setting::PickupItems::Accessory::LargeCompensator_Color[3]));
                            }
                            if (Item.Name == "LargeSuppressor" && Setting::PickupItems::Accessory::LargeSuppressor)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::LargeSuppressor_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::LargeSuppressor_Color[0], Setting::PickupItems::Accessory::LargeSuppressor_Color[1], Setting::PickupItems::Accessory::LargeSuppressor_Color[2], Setting::PickupItems::Accessory::LargeSuppressor_Color[3]));
                            }
                            if (Item.Name == "SniperCompensator" && Setting::PickupItems::Accessory::SniperCompensator)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::SniperCompensator_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::SniperCompensator_Color[0], Setting::PickupItems::Accessory::SniperCompensator_Color[1], Setting::PickupItems::Accessory::SniperCompensator_Color[2], Setting::PickupItems::Accessory::SniperCompensator_Color[3]));
                            }
                            if (Item.Name == "SniperSuppressor" && Setting::PickupItems::Accessory::SniperSuppressor)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::SniperSuppressor_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::SniperSuppressor_Color[0], Setting::PickupItems::Accessory::SniperSuppressor_Color[1], Setting::PickupItems::Accessory::SniperSuppressor_Color[2], Setting::PickupItems::Accessory::SniperSuppressor_Color[3]));
                            }
                            if (Item.Name == "ThumbGrip" && Setting::PickupItems::Accessory::ThumbGrip)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::ThumbGrip_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::ThumbGrip_Color[0], Setting::PickupItems::Accessory::ThumbGrip_Color[1], Setting::PickupItems::Accessory::ThumbGrip_Color[2], Setting::PickupItems::Accessory::ThumbGrip_Color[3]));
                            }
                            if (Item.Name == "Angled" && Setting::PickupItems::Accessory::Angled)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Angled_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Angled_Color[0], Setting::PickupItems::Accessory::Angled_Color[1], Setting::PickupItems::Accessory::Angled_Color[2], Setting::PickupItems::Accessory::Angled_Color[3]));
                            }
                            if (Item.Name == "LightGrip" && Setting::PickupItems::Accessory::LightGrip)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::LightGrip_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::LightGrip_Color[0], Setting::PickupItems::Accessory::LightGrip_Color[1], Setting::PickupItems::Accessory::LightGrip_Color[2], Setting::PickupItems::Accessory::LightGrip_Color[3]));
                            }
                            if (Item.Name == "HalfGrip" && Setting::PickupItems::Accessory::HalfGrip)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::HalfGrip_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::HalfGrip_Color[0], Setting::PickupItems::Accessory::HalfGrip_Color[1], Setting::PickupItems::Accessory::HalfGrip_Color[2], Setting::PickupItems::Accessory::HalfGrip_Color[3]));
                            }
                            if (Item.Name == "Vertical" && Setting::PickupItems::Accessory::Vertical)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Vertical_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Vertical_Color[0], Setting::PickupItems::Accessory::Vertical_Color[1], Setting::PickupItems::Accessory::Vertical_Color[2], Setting::PickupItems::Accessory::Vertical_Color[3]));
                            }
                            if (Item.Name == "Reddot" && Setting::PickupItems::Accessory::Reddot)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Reddot_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Reddot_Color[0], Setting::PickupItems::Accessory::Reddot_Color[1], Setting::PickupItems::Accessory::Reddot_Color[2], Setting::PickupItems::Accessory::Reddot_Color[3]));
                            }
                            if (Item.Name == "Holo" && Setting::PickupItems::Accessory::Holo)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Holo_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Holo_Color[0], Setting::PickupItems::Accessory::Holo_Color[1], Setting::PickupItems::Accessory::Holo_Color[2], Setting::PickupItems::Accessory::Holo_Color[3]));
                            }
                            if (Item.Name == "Scope2X" && Setting::PickupItems::Accessory::Scope2X)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Scope2X_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope2X_Color[0], Setting::PickupItems::Accessory::Scope2X_Color[1], Setting::PickupItems::Accessory::Scope2X_Color[2], Setting::PickupItems::Accessory::Scope2X_Color[3]));
                            }
                            if (Item.Name == "Scope3X" && Setting::PickupItems::Accessory::Scope3X)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Scope3X_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope3X_Color[0], Setting::PickupItems::Accessory::Scope3X_Color[1], Setting::PickupItems::Accessory::Scope3X_Color[2], Setting::PickupItems::Accessory::Scope3X_Color[3]));
                            }
                            if (Item.Name == "Scope4X" && Setting::PickupItems::Accessory::Scope4X)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Scope4X_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope4X_Color[0], Setting::PickupItems::Accessory::Scope4X_Color[1], Setting::PickupItems::Accessory::Scope4X_Color[2], Setting::PickupItems::Accessory::Scope4X_Color[3]));
                            }
                            if (Item.Name == "Scope6X" && Setting::PickupItems::Accessory::Scope6X)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Scope6X_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope6X_Color[0], Setting::PickupItems::Accessory::Scope6X_Color[1], Setting::PickupItems::Accessory::Scope6X_Color[2], Setting::PickupItems::Accessory::Scope6X_Color[3]));
                            }
                            if (Item.Name == "Scope8X" && Setting::PickupItems::Accessory::Scope8X)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::Scope8X_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::Scope8X_Color[0], Setting::PickupItems::Accessory::Scope8X_Color[1], Setting::PickupItems::Accessory::Scope8X_Color[2], Setting::PickupItems::Accessory::Scope8X_Color[3]));
                            }
                            if (Item.Name == "M416Stock" && Setting::PickupItems::Accessory::M416Stock)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Accessory::M416Stock_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Accessory::M416Stock_Color[0], Setting::PickupItems::Accessory::M416Stock_Color[1], Setting::PickupItems::Accessory::M416Stock_Color[2], Setting::PickupItems::Accessory::M416Stock_Color[3]));
                            }
#pragma endregion
#pragma region Ammo
                            if (Item.Name == "7.62mm" && Setting::PickupItems::Ammo::Ammo762)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::Ammo762_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo762_Color[0], Setting::PickupItems::Ammo::Ammo762_Color[1], Setting::PickupItems::Ammo::Ammo762_Color[2], Setting::PickupItems::Ammo::Ammo762_Color[3]));
                            }
                            if (Item.Name == "45ACP" && Setting::PickupItems::Ammo::Ammo45ACP)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::Ammo45ACP_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo45ACP_Color[0], Setting::PickupItems::Ammo::Ammo45ACP_Color[1], Setting::PickupItems::Ammo::Ammo45ACP_Color[2], Setting::PickupItems::Ammo::Ammo45ACP_Color[3]));
                            }
                            if (Item.Name == "5.56mm" && Setting::PickupItems::Ammo::Ammo556)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::Ammo556_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo556_Color[0], Setting::PickupItems::Ammo::Ammo556_Color[1], Setting::PickupItems::Ammo::Ammo556_Color[2], Setting::PickupItems::Ammo::Ammo556_Color[3]));
                            }
                            if (Item.Name == "9mm" && Setting::PickupItems::Ammo::Ammo9)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::Ammo9_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo9_Color[0], Setting::PickupItems::Ammo::Ammo9_Color[1], Setting::PickupItems::Ammo::Ammo9_Color[2], Setting::PickupItems::Ammo::Ammo9_Color[3]));
                            }
                            if (Item.Name == "300Magnum" && Setting::PickupItems::Ammo::Ammo300Magnum)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::Ammo300Magnum_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo300Magnum_Color[0], Setting::PickupItems::Ammo::Ammo300Magnum_Color[1], Setting::PickupItems::Ammo::Ammo300Magnum_Color[2], Setting::PickupItems::Ammo::Ammo300Magnum_Color[3]));
                            }
                            if (Item.Name == "12Guage" && Setting::PickupItems::Ammo::Ammo12Guage)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::Ammo12Guage_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::Ammo12Guage_Color[0], Setting::PickupItems::Ammo::Ammo12Guage_Color[1], Setting::PickupItems::Ammo::Ammo12Guage_Color[2], Setting::PickupItems::Ammo::Ammo12Guage_Color[3]));
                            }
                            if (Item.Name == "FlareGun_Aimmo" && Setting::PickupItems::Ammo::AmmoFlareGun)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Ammo::AmmoFlareGun_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Ammo::AmmoFlareGun_Color[0], Setting::PickupItems::Ammo::AmmoFlareGun_Color[1], Setting::PickupItems::Ammo::AmmoFlareGun_Color[2], Setting::PickupItems::Ammo::AmmoFlareGun_Color[3]));
                            }
#pragma endregion
#pragma region Others
                            if (Item.Name == "AirDrop" && Setting::PickupItems::Others::AirDrop)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::AirDrop_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::AirDrop_Color[0], Setting::PickupItems::Others::AirDrop_Color[1], Setting::PickupItems::Others::AirDrop_Color[2], Setting::PickupItems::Others::AirDrop_Color[3]));
                                if (Distance >= 100)
                                    continue;
                                if (Item.boxItem.size() > 0 && Setting::PickupItems::Visual::ShowItemInDeathBox)
                                {
                                    for (int i = 0; i < Item.boxItem.size(); i++)
                                    {
                                        DrawItemBox(Item.boxItem[i], Item.boxItem[i], Screen.X, Screen.Y - 16.5 * (i + 1));
                                    }
                                }
                            }
                            if (Item.Name == "AirDrop" && Setting::PickupItems::Others::AirDrop)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::AirDrop_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::AirDrop_Color[0], Setting::PickupItems::Others::AirDrop_Color[1], Setting::PickupItems::Others::AirDrop_Color[2], Setting::PickupItems::Others::AirDrop_Color[3]));
                                if (Distance >= 100)
                                    continue;
                                if (Item.boxItem.size() > 0 && Setting::PickupItems::Visual::ShowItemInDeathBox)
                                {
                                    for (int i = 0; i < Item.boxItem.size(); i++)
                                    {
                                        DrawItemBox(Item.boxItem[i], Item.boxItem[i], Screen.X, Screen.Y - 16.5 * (i + 1));
                                    }
                                }
                            }
                            if (Item.Name == "DeathBox" && Setting::PickupItems::Others::DeathBox)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::DeathBox_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::DeathBox_Color[0], Setting::PickupItems::Others::DeathBox_Color[1], Setting::PickupItems::Others::DeathBox_Color[2], Setting::PickupItems::Others::DeathBox_Color[3]));
                                if (Distance >= 100)
                                    continue;
                                if (Item.boxItem.size() > 0 && Setting::PickupItems::Visual::ShowItemInDeathBox)
                                {
                                    for (int i = 0; i < Item.boxItem.size(); i++)
                                    {
                                        DrawItemBox(Item.boxItem[i], Item.boxItem[i], Screen.X, Screen.Y - 16.5 * (i + 1));
                                    }
                                }
                            }
                            if (Item.Name == "Grenade" && Setting::PickupItems::Others::Grenade)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::Grenade_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Grenade_Color[0], Setting::PickupItems::Others::Grenade_Color[1], Setting::PickupItems::Others::Grenade_Color[2], Setting::PickupItems::Others::Grenade_Color[3]));
                            }
                            if (Item.Name == "Smoke" && Setting::PickupItems::Others::Smoke)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::Smoke_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Smoke_Color[0], Setting::PickupItems::Others::Smoke_Color[1], Setting::PickupItems::Others::Smoke_Color[2], Setting::PickupItems::Others::Smoke_Color[3]));
                            }
                            if (Item.Name == "Molotof" && Setting::PickupItems::Others::Molotof)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::Molotof_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Molotof_Color[0], Setting::PickupItems::Others::Molotof_Color[1], Setting::PickupItems::Others::Molotof_Color[2], Setting::PickupItems::Others::Molotof_Color[3]));
                            }
                            if (Item.Name == "Apple" && Setting::PickupItems::Others::Apple)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::Apple_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Apple_Color[0], Setting::PickupItems::Others::Apple_Color[1], Setting::PickupItems::Others::Apple_Color[2], Setting::PickupItems::Others::Apple_Color[3]));
                            }
                            if (Item.Name == "Gascan" && Setting::PickupItems::Others::Gascan)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::Gascan_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::Gascan_Color[0], Setting::PickupItems::Others::Gascan_Color[1], Setting::PickupItems::Others::Gascan_Color[2], Setting::PickupItems::Others::Gascan_Color[3]));
                            }
                            if (Item.Name == "TokenShop" && Setting::PickupItems::Others::TokenShop)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::TokenShop_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::TokenShop_Color[0], Setting::PickupItems::Others::TokenShop_Color[1], Setting::PickupItems::Others::TokenShop_Color[2], Setting::PickupItems::Others::TokenShop_Color[3]));
                            }
                            if (Item.Name == "SecurityCard" && Setting::PickupItems::Others::SecurityCard)
                            {
                                DrawBorderString(Fonts::MainFont, Setting::PickupItems::Others::SecurityCard_ThickNess, TextToDraw, Screen.X, Screen.Y, ImColor(Setting::PickupItems::Others::SecurityCard_Color[0], Setting::PickupItems::Others::SecurityCard_Color[1], Setting::PickupItems::Others::SecurityCard_Color[2], Setting::PickupItems::Others::SecurityCard_Color[3]));
                            }
#pragma endregion
                        }
                    }
                    for (Vehicle &Vehicle : Data::VehicleList)
                    {
#pragma region VehicleRender
                        if (Setting::Vehicle::Vehicle)
                        {
                            if (Setting::Vehicle::VehicleSkip == true && Setting::Vehicle::IsShowVehicle == false && Vehicle.bIsEngineStarted == false)
                                continue;
                            if (!Algorithm::WorldToScreenPlayer(Vehicle.Position, Screen, Data::ViewMatrixBase))
                                continue;
                            Distance = sqrt(pow(Data::LocalPosition.X - Vehicle.Position.X, 2) + pow(Data::LocalPosition.Y - Vehicle.Position.Y, 2) + pow(Data::LocalPosition.Z - Vehicle.Position.Z, 2)) / 100;
                            if (Distance < 3 || Distance > (float)Setting::Vehicle::Vehicle_MaxDistance)
                                continue;
                            /* Unity-style: name + distance; optional vehicle icon from icons folder */
                            TextToDraw = Vehicle.Name;
                            if (Setting::Vehicle::Vehicle_Distance)
                                TextToDraw = TextToDraw + " [" + std::to_string((int)Distance) + " M]";
                            ImColor CarCol = ImColor(Setting::Vehicle::Vehicle_Color[0], Setting::Vehicle::Vehicle_Color[1], Setting::Vehicle::Vehicle_Color[2], Setting::Vehicle::Vehicle_Color[3]);
                            if (Setting::Vehicle::VehicleChange && Vehicle.driving)
                                CarCol = ImColor(Setting::Vehicle::VehicleChange_Color[0], Setting::Vehicle::VehicleChange_Color[1], Setting::Vehicle::VehicleChange_Color[2], Setting::Vehicle::VehicleChange_Color[3]);
                            /* When Icons+Text (VehType 0), draw vehicle icon from icons folder if present */
                            if (Setting::Vehicle::Vehicle_VehType == 0 && !Vehicle.Name.empty())
                            {
                                CachedIcon* pVehIcon = GetOrLoadIcon(Vehicle.Name);
                                if (pVehIcon && pVehIcon->srv && pVehIcon->w > 0 && pVehIcon->h > 0)
                                {
                                    const float iconSize = 32.f;
                                    float ix = Screen.X - iconSize * 0.5f;
                                    float iy = Screen.Y - iconSize - 4.f;
                                    ImDrawList* dl = ImGui::GetBackgroundDrawList();
                                    dl->AddImage((ImTextureID)pVehIcon->srv, ImVec2(ix, iy), ImVec2(ix + iconSize, iy + iconSize), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                                }
                            }

                             DrawBorderString(Fonts::MainFont, Setting::Vehicle::Vehicle_ThickNess, TextToDraw, Screen.X, Screen.Y, CarCol);
                            float fuelPct = Vehicle.phantram >= 0.f && Vehicle.phantram <= 100.f ? Vehicle.phantram : (float)Vehicle.Fuel;
                            float hpPct = (float)Vehicle.HP;
                            if (hpPct > 100.f) hpPct = 100.f;
                            if (hpPct < 0.f) hpPct = 0.f;
                            /* Fuel by VehType: 0=Icons+Text, 1=Bar, 2=Text only */
                            if (Setting::Vehicle::Vehicle_Fuel)
                            {
                                if (Setting::Vehicle::Vehicle_VehType == 1)
                                    VehicleDrawOilBar(Screen.X - 5, Screen.Y + 31, 80.f, 4.f, fuelPct);
                            }
                            /* HP by VehType */
                            if (Setting::Vehicle::Vehicle_HP)
                            {
                                if (Setting::Vehicle::Vehicle_VehType == 1)
                                    VehicleDrawHPBar(Screen.X - 5, Screen.Y + 23, 80.f, 4.f, hpPct);
                            }
                            /* Direction line (Unity: only when driving) */
                            if (Setting::Vehicle::Vehicle_DirectionLine && Vehicle.driving)
                            {
                                float radPitch = Vehicle.ReplicatedMovement.Rotation.Pitch * 3.14159265358979323846f / 180.f;
                                float radYaw = Vehicle.ReplicatedMovement.Rotation.Yaw * 3.14159265358979323846f / 180.f;
                                float dx = std::cos(radPitch) * std::cos(radYaw);
                                float dy = std::cos(radPitch) * std::sin(radYaw);
                                float dz = std::sin(radPitch);
                                float len = Setting::Vehicle::VehicleLineDirectionSize;
                                VECTOR3 startPos = Vehicle.Position; startPos.Z += 100.f;
                                VECTOR3 endPos = { startPos.X + dx * len, startPos.Y + dy * len, startPos.Z + dz * len };
                                VECTOR3 screenStart, screenEnd;
                                if (Algorithm::WorldToScreen(startPos, screenStart, Data::ViewMatrixBase) && Algorithm::WorldToScreen(endPos, screenEnd, Data::ViewMatrixBase))
                                {
                                    ImDrawList* dl = ImGui::GetBackgroundDrawList();
                                    dl->AddLine(ImVec2(screenStart.X, screenStart.Y), ImVec2(screenEnd.X, screenEnd.Y), (ImU32)CarCol, 2.f);
                                    dl->AddCircleFilled(ImVec2(screenEnd.X, screenEnd.Y), 4.f, (ImU32)CarCol);
                                }
                            }
                            /* Low HP warning: 3D circle (Unity-style) */
                            if (Setting::Vehicle::Vehicle_Warning && Vehicle.HP <= 20 && Vehicle.HP >= 0)
                            {
                                float distM = Distance * 100.f;
                                if (distM <= 500.f)
                                    DrawCircle3D(Vehicle.Position.X, Vehicle.Position.Y, Vehicle.Position.Z, 500.f, ImColor(255, 0, 0, 255), 2.f);
                                else
                                    DrawCircle3D(Vehicle.Position.X, Vehicle.Position.Y, Vehicle.Position.Z, 500.f, ImColor(0, 255, 0, 255), 2.f);
                            }
                            /* 3D box (Unity-style 240x225) or skip for 2D */
                            if (Setting::Vehicle::Vehicle_Box)
                                Draw3dBOX(Vehicle.Position, 240.f, 225.f, (ImU32)CarCol, 1.5f);
                            /* Wheel damage (Unity-style text) */
                            if (Setting::Vehicle::Vehicle_WheelDamage)
                            {
                                int wheelDmg = 0;
                                if (Vehicle.Wheel.FirstWheel <= 0.f) wheelDmg++;
                                if (Vehicle.Wheel.SecondWheel <= 0.f) wheelDmg++;
                                if (Vehicle.Wheel.ThirdWheel <= 0.f) wheelDmg++;
                                if (Vehicle.Wheel.FourthWheel <= 0.f) wheelDmg++;
                                if (wheelDmg > 0)
                                {
                                    std::string wheelStr = "Wheel Damage : " + std::to_string(wheelDmg);
                                    ImColor wheelCol = wheelDmg >= 4 ? ImColor(255, 69, 0, 255) : CarCol;
                                    DrawBorderString(Fonts::MainFont, Setting::Vehicle::Vehicle_ThickNess, wheelStr, Screen.X, Screen.Y + 40, wheelCol);
                                }
                            }
                        }
#pragma endregion VehicleRender
                    }
                    if (Setting::Alerts::OpenAllAlert && Setting::Alerts::GrenadeAlert_Trajectory)
                    {
                        for (int i = 0; i < Data::BombPositionList.size(); i++)
                        {
                            if (Data::BombPositionList.at(i).PositionVec.size() > 0)
                            {
                                VECTOR3 sPosition;
                                VECTOR3 lPosition;
                                for (int j = 1; j < Data::BombPositionList.at(i).PositionVec.size(); j++)
                                {
                                    if (Algorithm::WorldToScreen(Data::BombPositionList.at(i).PositionVec.at(j), sPosition, Data::ViewMatrixBase) && Algorithm::WorldToScreen(Data::BombPositionList.at(i).PositionVec.at(j - 1), lPosition, Data::ViewMatrixBase))
                                    {
                                        DrawLine(sPosition.X, sPosition.Y, lPosition.X, lPosition.Y, ImColor(Setting::Alerts::GrenadeAlert_Trajectory_Color[0], Setting::Alerts::GrenadeAlert_Trajectory_Color[1], Setting::Alerts::GrenadeAlert_Trajectory_Color[2], Setting::Alerts::GrenadeAlert_Trajectory_Color[3]), 3);
                                    }
                                }
                            }
                        }
                    }

                    for (Alerts &Alerts : Data::AlertsList)
                    {
                        if (!Setting::Alerts::OpenAllAlert)
                            break;
                        const char *wepIcons;
                        if (!Algorithm::WorldToScreen(Alerts.Position, Screen, Data::ViewMatrixBase))
                            continue;
                        Distance = sqrt(pow(Data::LocalPosition.X - Alerts.Position.X, 2) + pow(Data::LocalPosition.Y - Alerts.Position.Y, 2) + pow(Data::LocalPosition.Z - Alerts.Position.Z, 2)) / 100;
                        ImColor Color;
                        if (Alerts.Name == "Grenade" && Distance <= 6.85f)
                            Color = ImColor(255, 0, 0, 255);
                        if (Alerts.Name == "Molotuf" && Distance <= 4.5f)
                            Color = ImColor(255, 0, 0, 255);
                        TextToDraw = to_string((INT)Alerts.TimeLeft);
                        if (Setting::Alerts::OpenAllAlert)
                        {
                            if (Setting::Alerts::GrenadeAlert && Alerts.Name == "Grenade")
                            {
                                Color = ImColor(Setting::Alerts::GrenadeAlert_Color[0], Setting::Alerts::GrenadeAlert_Color[1], Setting::Alerts::GrenadeAlert_Color[2], Setting::Alerts::GrenadeAlert_Color[3]);
                                if (Distance <= 6.85f)
                                    Color = ImColor(255, 0, 0, 255);
                                wepIcons = ("\xef\x87\xa2");
                                FVector Velocity(80, 640, 300);

                                // float v0x = std::cos(Data::GlobalCameraCache.Rotation.Pitch);
                                // float v0y = std::sin(Data::GlobalCameraCache.Rotation.Pitch);
                                // float v0z = -std::cos(Data::GlobalCameraCache.Rotation.Yaw);
                                // int timeleft = 7;
                                // float Gravity = 9.8f;
                                // float posX = (1 * sin(2 * Data::GlobalCameraCache.Rotation.Pitch) * sin(2 * Data::GlobalCameraCache.Rotation.Pitch)) / Gravity;
                                // float posY = v0y * timeleft - (1 / 2) * Gravity * (timeleft * timeleft);
                                // float posZ = v0z * timeleft;
                                //
                                VECTOR3 LastPos;
                                VECTOR3 LastPosScreen;
                                // LastPos.X = x;
                                // LastPos.Y = y;
                                // LastPos.Z = z;

                                // FRotator PlayerRotation = Data::GlobalCameraCache.Rotation;
                                // FVector PlayerForwardVector = RotatorToVector(PlayerRotation);
                                // FVector TransformVector = FVector(Alerts.Position.X, Alerts.Position.Y, Alerts.Position.Z);
                                // float x = Velocity.X * Alerts.TimeLeft;
                                // float y = (Velocity.Y * Alerts.TimeLeft) - (10.f * std::pow(Alerts.TimeLeft, 2) / 2);
                                // float z = Velocity.Z * Alerts.TimeLeft;
                                //
                                // LastPos.X = TransformVector.X + x;
                                // LastPos.Y = TransformVector.Y + y;
                                // LastPos.Z = TransformVector.Z + z;
                                // printf("PosX : %f PosY : %f PosZ : %f AtTime : %f \n",LastPos.X,LastPos.Y,LastPos.Z,(float)Alerts.TimeLeft);
                                // Algorithm::WorldToScreen(LastPos, LastPosScreen, Data::ViewMatrixBase);
                                // DrawLine(ScreenInfo::Width / 2, 0, LastPosScreen.X, LastPosScreen.Y, ImColor(255, 0, 0, 255), 1);

                                // FVector Drop = CalculateGrenadeDropPosition(FVector(Alerts.Position.X, Alerts.Position.Y, Alerts.Position.Z), PlayerForwardVector,10,Alerts.TimeLeft,9.8f);
                                // LastPos.X = Alerts.Position.X + 10 * Alerts.TimeLeft * std::cos(Data::GlobalCameraCache.Rotation.Pitch);
                                // LastPos.Y = Alerts.Position.Y + 10 * Alerts.TimeLeft - std::sin(Data::GlobalCameraCache.Rotation.Pitch) - 0.5f * 9.8f * std::pow(Alerts.TimeLeft, 2);
                                // LastPos.Z = Alerts.Position.Z + 10 * Alerts.TimeLeft * std::cos(Data::GlobalCameraCache.Rotation.Pitch) -  0.5f * 9.8f * std::pow(Alerts.TimeLeft,2);
                                // Algorithm::WorldToScreen(LastPos, LastPosScreen, Data::ViewMatrixBase);
                                // DrawLine(ScreenInfo::Width / 2, 0, LastPosScreen.X, LastPosScreen.Y, ImColor(255, 0, 0, 255), 1);

                                /*float Gravity = 9.8f;
                                float PitchDegree = Data::GlobalCameraCache.Rotation.Pitch;
                                float v0x = 10 * std::cos(PitchDegree);
                                float v0y = 10 * std::sin(PitchDegree);
                                float v0z = sqrt(10 * 10 - v0x * v0x);

                                float t = 7;
                                float x = v0x * 7;
                                float y = v0y * t + v0y * v0y / 4.9 - 4.9 * t * t / 2;
                                float z = v0z * t;
                                VECTOR3 LastPost;
                                LastPost.X = x;
                                LastPost.Y = y;
                                LastPost.Z = z;*/

                                DrawCircle3D(Alerts.Position.X, Alerts.Position.Y, Alerts.Position.Z, 650, Color, Setting::Alerts::GrenadeAlert_ThickNess); // grenade = 600
                                DrawBorderChar(Fonts::FontAwesome, 35, wepIcons, Screen.X, Screen.Y, ImColor(255, 255, 0, 255), true, 0, ImColor(255, 0, 0, 255));
                                // Algorithm::WorldToScreen(LastPost, Screen, Data::ViewMatrixBase);

                                DrawBorderString(Fonts::MainFont, 20, TextToDraw, Screen.X, Screen.Y, ImColor(0, 0, 0, 255), true);
                            }

                            if (Setting::Alerts::GrenadeAlert && Alerts.Name == "Molotuf")
                            {

                                Color = ImColor(Setting::Alerts::MolotufAlert_Color[0], Setting::Alerts::MolotufAlert_Color[1], Setting::Alerts::MolotufAlert_Color[2], Setting::Alerts::MolotufAlert_Color[3]);
                                if (Distance <= 4.5f)
                                    Color = ImColor(255, 0, 0, 255);
                                wepIcons = ("\xef\x81\xad");
                                DrawCircle3D(Alerts.Position.X, Alerts.Position.Y, Alerts.Position.Z, 435, Color, Setting::Alerts::MolotufAlert_ThickNess); // Molotuf = 435
                                DrawBorderChar(Fonts::FontAwesome, 35, wepIcons, Screen.X, Screen.Y - 4, ImColor(255, 255, 0, 255), true, 0, ImColor(255, 0, 0, 255));
                                DrawBorderString(Fonts::MainFont, 20, TextToDraw, Screen.X, Screen.Y + 4, ImColor(0, 0, 0, 255), true);
                            }
                            if (Setting::Alerts::SmokeAlert && Alerts.Name == "Smoke")
                            {
                                Color = ImColor(Setting::Alerts::SmokeAlert_Color[0], Setting::Alerts::SmokeAlert_Color[1], Setting::Alerts::SmokeAlert_Color[2], Setting::Alerts::SmokeAlert_Color[3]);
                                wepIcons = ("\xef\x92\xad");
                                DrawCircle3D(Alerts.Position.X, Alerts.Position.Y, Alerts.Position.Z, 300, Color, Setting::Alerts::SmokeAlert_ThickNess); 
                                DrawBorderChar(Fonts::FontAwesome, 35, wepIcons, Screen.X, Screen.Y, ImColor(255, 255, 0, 255), true, 0, ImColor(255, 0, 0, 255));
                                DrawBorderString(Fonts::MainFont, 20, TextToDraw, Screen.X, Screen.Y, ImColor(0, 0, 0, 255), true);
                            }
                            if (Setting::Alerts::RedzoneAlert && Alerts.Name == "Redzone")
                            {
                                Color = ImColor(Setting::Alerts::RedzoneAlert_Color[0], Setting::Alerts::RedzoneAlert_Color[1], Setting::Alerts::RedzoneAlert_Color[2], Setting::Alerts::RedzoneAlert_Color[3]);
                                if (Distance <= 10.f)
                                    Color = ImColor(255, 0, 0, 255);
                                DrawCircle3D(Alerts.Position.X, Alerts.Position.Y, Alerts.Position.Z, 1000, Color, Setting::Alerts::RedzoneAlert_ThickNess); // Redzone = 500
                            }
                        }
                    }

                    for (AActor &AActor : Data::AActorList)
                    {
                        float iExtraBoxX = 3.f;
                        float iExtraBoxY = 5.5f;
                        TargetID += 1;
                        if (Data::LocalbDead && Setting::QoL::bESPWhenSpectating && AActor.TeamID == Data::MyTeamID)
                            continue;
                        HeightOfName = 2.0f;
                        Screen.X = 0;
                        Screen.Y = 0;
                        Distance = sqrt(pow(Data::LocalPosition.X - AActor.Position.X, 2) + pow(Data::LocalPosition.Y - AActor.Position.Y, 2) + pow(Data::LocalPosition.Z - AActor.Position.Z, 2)) / 100;
                        try
                        {
                            ColTeamID = _randombush[AActor.TeamID % 7];
                        }
                        catch (exception)
                        {
                            ColTeamID = ImColor(255, 255, 255, 255);
                        }
                        if (Setting::ESPExtras::bDistanceColor)
                        {
                            float t = (Distance - Setting::ESPExtras::fDistanceNear) / (Setting::ESPExtras::fDistanceFar - Setting::ESPExtras::fDistanceNear);
                            if (t < 0.f) t = 0.f; if (t > 1.f) t = 1.f;
                            ColTeamID = ImColor(1.f - t, t, 0.2f, 1.f);
                        }
                        if (!Algorithm::WorldToScreenPlayer(AActor.Position, Screen, Data::ViewMatrixBase))
                            continue;

                        // Spectate stability: reject garbage projections during camera transition
                        if (Screen.X < -(float)ScreenInfo::Width || Screen.X > (float)ScreenInfo::Width * 2.f ||
                            Screen.Y < -(float)ScreenInfo::Height || Screen.Y > (float)ScreenInfo::Height * 2.f)
                            continue;

                        // Apply user-defined ESP position offsets
                        Screen.X += Setting::Player::ESP_OffsetX;
                        Screen.Y += Setting::Player::ESP_OffsetY;
                        if (Setting::ESPExtras::bFOVFilterESP && Setting::Fov)
                        {
                            float dx = Screen.X - ScreenInfo::Width * 0.5f;
                            float dy = Screen.Y - ScreenInfo::Height * 0.5f;
                            if (sqrtf(dx*dx + dy*dy) > Setting::FovAim + 20.f) continue;
                        }
                        double nowSec = GetTickCount64() / 1000.0;
                        {
                            INT prevBullet = 0;
                            auto itPrev = Data::ActorLastCurBullet.find(AActor.Address);
                            if (itPrev != Data::ActorLastCurBullet.end()) prevBullet = itPrev->second;
                            if (AActor.CurMaxBullet > 0 && AActor.CurBullet < prevBullet)
                                Data::ThreatUntilTime[AActor.Address] = nowSec + 2.0;
                            Data::ActorLastCurBullet[AActor.Address] = AActor.CurBullet;
                        }
                        bool usePriorityColor = false;
                        ImColor priorityCol;
                        if (Setting::ESPExtras::bThreatHighlight)
                        {
                            auto itThreat = Data::ThreatUntilTime.find(AActor.Address);
                            if (itThreat != Data::ThreatUntilTime.end() && itThreat->second > nowSec)
                            {
                                usePriorityColor = true;
                                priorityCol = ImColor(Setting::ESPExtras::ThreatHighlight_Color[0], Setting::ESPExtras::ThreatHighlight_Color[1], Setting::ESPExtras::ThreatHighlight_Color[2], Setting::ESPExtras::ThreatHighlight_Color[3]);
                            }
                        }
                        if (!usePriorityColor && Setting::ESPExtras::bRecentlyShotHighlight && AActor.Address == Data::LastHitActorAddress && (nowSec - Data::LastHitTime) < (double)Setting::ESPExtras::fRecentlyShotDurationSec)
                        {
                            usePriorityColor = true;
                            priorityCol = ImColor(Setting::ESPExtras::RecentlyShotHighlight_Color[0], Setting::ESPExtras::RecentlyShotHighlight_Color[1], Setting::ESPExtras::RecentlyShotHighlight_Color[2], Setting::ESPExtras::RecentlyShotHighlight_Color[3]);
                        }
                        if (Setting::AimAlternative)
                        {
                            if (Setting::AimLine)
                            {
                                if (AimForm::BestTargetID == TargetID)
                                {
                                    ImColor AimLineDraw = ImColor(Setting::AimLine_Color[0], Setting::AimLine_Color[1], Setting::AimLine_Color[2], Setting::AimLine_Color[3]);
                                    if (Setting::AimLineColTeamID)
                                        AimLineDraw = ColTeamID;
                                    if (GetAsyncKeyState(Setting::KeyAim) & 0x8000 && Setting::AimLineChange)
                                        AimLineDraw = ImColor(Setting::AimLineChangeCol_Color[0], Setting::AimLineChangeCol_Color[1], Setting::AimLineChangeCol_Color[2], Setting::AimLineChangeCol_Color[3]);
                                    if (Setting::TargetAim == 0 || Setting::TargetAim == 1 || Setting::TargetAim == 2)
                                    {
                                        DrawLine(ScreenInfo::Width / 2, ScreenInfo::Height, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2, AimLineDraw, Setting::AimLine_ThickNess);
                                    }
                                    if (Setting::TargetAim == 3 || Setting::TargetAim == 4)
                                    {
                                        if (AimForm::ScreenBullet.X > 0 && AimForm::ScreenBullet.Y > 0)
                                            DrawLine(ScreenInfo::Width / 2, ScreenInfo::Height, AimForm::ScreenBullet.X, AimForm::ScreenBullet.Y, AimLineDraw, Setting::AimLine_ThickNess);
                                    }
                                }
                            }
                        }
                        if (Setting::Player::Details == true)
                        {

                            HeadPos = AActor.HeadPos;
                            HeadPos.Z += 7;

                            float curHealth = AActor.Health;
                            float maxHealth = AActor.HealthMax;
                            if (AActor.Health == 0 && !AActor.IsDead) {
                                curHealth = AActor.NearDeathBreath;
                                maxHealth = AActor.BreathMax;
                            }
                            if (Setting::Player::SnapLines)
                            {
                                if (usePriorityColor)
                                    ColAActor = priorityCol;
                                else if (Setting::Player::VisibleCheck)
                                {
                                    if (AActor.IsVisible == 1)
                                        ColAActor = ImColor(Setting::Player::VisibleCheck_Color[0], Setting::Player::VisibleCheck_Color[1], Setting::Player::VisibleCheck_Color[2], Setting::Player::VisibleCheck_Color[3]);
                                    else
                                        ColAActor = ImColor(Setting::Player::VisibleCheckPart2_Color[0], Setting::Player::VisibleCheckPart2_Color[1], Setting::Player::VisibleCheckPart2_Color[2], Setting::Player::VisibleCheckPart2_Color[3]);
                                }
                                else
                                {
                                    if (Setting::Player::SnapLinesType == 0)
                                        ColAActor = ImColor(Setting::Player::SnapLines_Color[0], Setting::Player::SnapLines_Color[1], Setting::Player::SnapLines_Color[2], Setting::Player::SnapLines_Color[3]);
                                    if (Setting::Player::SnapLinesType == 1)
                                        ColAActor = ColTeamID;
                                }
                                Algorithm::WorldToScreenBone(Data::ViewMatrixBase, HeadPos, Head);
                                DrawLine(ScreenInfo::Width / 2, 0, Screen.X, Screen.Y, ColAActor, Setting::Player::SnapLines_ThickNess);
                            }
                            if (Setting::ESPExtras::bSnaplines)
                            {
                                ImColor snapCol = usePriorityColor ? priorityCol : (Setting::Player::VisibleCheck ? ((AActor.IsVisible == 1) ? ImColor(Setting::Player::VisibleCheck_Color[0], Setting::Player::VisibleCheck_Color[1], Setting::Player::VisibleCheck_Color[2], Setting::Player::VisibleCheck_Color[3]) : ImColor(Setting::Player::VisibleCheckPart2_Color[0], Setting::Player::VisibleCheckPart2_Color[1], Setting::Player::VisibleCheckPart2_Color[2], Setting::Player::VisibleCheckPart2_Color[3])) : ColTeamID);
                                DrawLine(ScreenInfo::Width / 2, ScreenInfo::Height, Screen.X, Screen.Y, snapCol, Setting::ESPExtras::fSkeletonThickness > 0 ? Setting::ESPExtras::fSkeletonThickness : 1.f);
                            }
                             // Skeleton rendering moved to Pass 2 for better visibility in rushes
                            if (Setting::Player::Miscellanouse::Health)
                            {
                                if (Setting::Player::Miscellanouse::HealthID == 0)
                                {
                                    // float HPTHICKNESS = 3;
                                    // DrawFilledRectangle(Screen.X - Screen.Z / 4 - iExtraBoxX - 1, Screen.Y + Screen.Z + iExtraBoxY, HPTHICKNESS, Screen.Z + iExtraBoxY * 2, ImVec4(0, 0, 0, 255));
                                }
                            }
                            if (Setting::Player::Box)
                            {
                                if (usePriorityColor)
                                    ColAActor = priorityCol;
                                else if (Setting::Player::VisibleCheck)
                                {
                                    if (AActor.IsVisible == 1)
                                        ColAActor = ImColor(Setting::Player::VisibleCheck_Color[0], Setting::Player::VisibleCheck_Color[1], Setting::Player::VisibleCheck_Color[2], Setting::Player::VisibleCheck_Color[3]);
                                    else
                                        ColAActor = ImColor(Setting::Player::VisibleCheckPart2_Color[0], Setting::Player::VisibleCheckPart2_Color[1], Setting::Player::VisibleCheckPart2_Color[2], Setting::Player::VisibleCheckPart2_Color[3]);
                                }
                                else
                                {
                                    if (Setting::Player::BoxTypeColor == 0)
                                        ColAActor = ImColor(Setting::Player::Box_Color[0], Setting::Player::Box_Color[1], Setting::Player::Box_Color[2], Setting::Player::Box_Color[3]);
                                    if (Setting::Player::BoxTypeColor == 1)
                                        ColAActor = ColTeamID;
                                }
                                if (Setting::Player::BoxType == 0)
                                {
                                    DrawRectangle(Screen.X - Screen.Z / 4 - iExtraBoxX, Screen.Y - iExtraBoxY, Screen.Z / 2 + iExtraBoxX * 2, Screen.Z + iExtraBoxY * 2, ColAActor, Setting::Player::Box_ThickNess);
                                }
                                if (Setting::Player::BoxType == 1)
                                {
                                    CornerBox(Screen.X - Screen.Z / 4 - iExtraBoxX, Screen.Y - iExtraBoxY, Screen.Z / 2 + iExtraBoxX * 2, Screen.Z + iExtraBoxY * 2, Setting::Player::Box_ThickNess, 0.2, ColAActor);
                                }
                            }
                            if (Setting::Player::Name || Setting::Player::Distance || Setting::Player::Miscellanouse::Health)
                            {
                                using convert_type = std::codecvt_utf8<wchar_t>;
                                std::wstring_convert<convert_type, wchar_t> converter;
                                std::string converted_str = converter.to_bytes(AActor.Name);
                                
                                std::string nameToUse = converted_str;
                                if (AActor.IsAI) nameToUse = "AI Bot";
                                else if (AActor.IsBot) nameToUse = "Bot";


                                ImColor NameCol = ImColor(Setting::Player::Name_Color[0], Setting::Player::Name_Color[1], Setting::Player::Name_Color[2], Setting::Player::Name_Color[3]);
                                
                                // Premium ESP (Type 0 Health Bar)
                                DrawPremiumESP(Fonts::UnicodeFont, Screen.X, Screen.Y - iExtraBoxY, Screen.Z + iExtraBoxY * 2, nameToUse, curHealth, maxHealth, (float)Distance, NameCol, AActor.IsBot || AActor.IsAI);
                                
                                // Name at top (Above skeleton)
                                if (Setting::Player::Name) {
                                    float nameY = (Screen.Y - iExtraBoxY) - 20.0f;
                                    ImColor borderCol = ImColor(Setting::Player::NameBorder_Color[0], Setting::Player::NameBorder_Color[1], Setting::Player::NameBorder_Color[2], Setting::Player::NameBorder_Color[3]);
                                    DrawBorderString(Fonts::UnicodeFont, Setting::Player::Name_ThickNess, nameToUse, Screen.X, nameY, NameCol, true, 0, borderCol);
                                }
                            }
                            if (Setting::Player::Weapon)
                            {
                                ImColor WeaponCol;
                                if (AActor.CurBullet == 0 && AActor.CurMaxBullet == 0)
                                    WeaponCol = ImColor(0, 255, 0, 255);
                                else
                                    WeaponCol = ImColor(Setting::Player::Weapon_Color[0], Setting::Player::Weapon_Color[1], Setting::Player::Weapon_Color[2], Setting::Player::Weapon_Color[3]);
                                if (Setting::Player::WeaponAsIcon)
                                {
                                    std::string weaponShort = PlayerWeapon(AActor.PlayerWeapon);
                                    CachedIcon* pWeaponIcon = GetOrLoadIcon(weaponShort);
                                    if (pWeaponIcon && pWeaponIcon->srv && pWeaponIcon->w > 0 && pWeaponIcon->h > 0)
                                    {
                                        const float iconSize = 32.f;
                                        float ix = Screen.X - iconSize * 0.5f;
                                        float iy = Screen.Y - 32.f - iExtraBoxY - iconSize;
                                        ImDrawList* dl = ImGui::GetBackgroundDrawList();
                                        dl->AddImage((ImTextureID)pWeaponIcon->srv, ImVec2(ix, iy), ImVec2(ix + iconSize, iy + iconSize), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                                    }
                                    else
                                    {
                                        std::string iconStr = PlayerWeaponFont(AActor.PlayerWeapon);
                                        if (!iconStr.empty())
                                            DrawBorderString(Fonts::MainFont, 32.f, iconStr, Screen.X, Screen.Y - 32.f - iExtraBoxY, WeaponCol);
                                        else
                                        {
                                            TextToDraw = weaponShort;
                                            // Bright Yellow for clear visibility
                                            if (WeaponCol.Value.x == Setting::Player::Weapon_Color[0]) 
                                                WeaponCol = ImColor(255, 230, 20, 255); 
                                                
                                            DrawBorderString(Fonts::MainFont, Setting::Player::Weapon_ThickNess, TextToDraw, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName, WeaponCol);
                                            HeightOfName += Setting::Player::Weapon_ThickNess + 2.0f;
                                        }
                                    }
                                }
                                else
                                {
                                    TextToDraw = PlayerWeapon(AActor.PlayerWeapon);
                                    if (TextToDraw == "Fist" || TextToDraw.empty())
                                        WeaponCol = ImColor(0, 255, 0, 255);
                                    else if (TextToDraw.find("BP") != std::string::npos)
                                        WeaponCol = ImColor(255, 255, 255, 255);
                                    // Ammo display removed as per user request
                                    // if (AActor.CurMaxBullet > 0)
                                    //    TextToDraw += " " + std::to_string(AActor.CurBullet) + "/" + std::to_string(AActor.CurMaxBullet);
                                    // Premium Yellow for clear rush visibility
                                    if (WeaponCol.Value.x == Setting::Player::Weapon_Color[0] && TextToDraw != "Fist" && TextToDraw.find("BP") == std::string::npos) 
                                        WeaponCol = ImColor(255, 230, 20, 255); 

                                    DrawBorderString(Fonts::MainFont, Setting::Player::Weapon_ThickNess, TextToDraw, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName, WeaponCol);
                                    HeightOfName += Setting::Player::Weapon_ThickNess + 2.0f;
                                }
                            }

                            if (Setting::Player::Distance)
                            {
                                std::string distStr = "[" + std::to_string((int)Distance) + " M]";
                                float distY = (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName;
                                
                                DrawBorderString(Fonts::MainFont, 17.5f, distStr, Screen.X, distY, ImColor(255, 255, 255, 255));
                                HeightOfName += 17.5f + 2.0f;

                                // Text Health below Distance (Safe Hack style)
                                if (Setting::Player::Miscellanouse::Health && Setting::Player::Miscellanouse::HealthID == 1) {
                                    std::string healthStr = "HP: " + std::to_string((int)curHealth);
                                    ImColor healthCol = ImColor(0, 255, 50, 255); 
                                    if ((curHealth/maxHealth) < 0.35f) healthCol = ImColor(255, 30, 30, 255); 
                                    else if ((curHealth/maxHealth) < 0.75f) healthCol = ImColor(255, 180, 0, 255); 

                                    float healthY = (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName;
                                    DrawBorderString(Fonts::MainFont, Setting::Player::Miscellanouse::Health_ThickNess, healthStr, Screen.X, healthY, healthCol);
                                    HeightOfName += Setting::Player::Miscellanouse::Health_ThickNess + 2.0f;
                                }
                            }
                            if (Setting::Player::Miscellanouse::PlayerUID)
                            {
                                TextToDraw = AActor.PlayerUID;
                                DrawBorderString(Fonts::MainFont, Setting::Player::Miscellanouse::PlayerUID_ThickNess, TextToDraw, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName, ImColor(Setting::Player::Miscellanouse::PlayerUID_Color[0], Setting::Player::Miscellanouse::PlayerUID_Color[1], Setting::Player::Miscellanouse::PlayerUID_Color[2], Setting::Player::Miscellanouse::PlayerUID_Color[3]));
                                HeightOfName += Setting::Player::Miscellanouse::PlayerUID_ThickNess;
                                HeightOfName += 1;
                            }

                            // Health text logic moved to DrawPremiumESP
                            /*
                            if (AActor.BeRevivedNum != 0) {
                                char PhanTram[24];
                                sprintf(PhanTram, "%.1f", AActor.BeRevivedNum);

                            }
                            TextToDraw = to_string(AActor.BeRevivedNum);
                            DrawBorderString(Fonts::MainFont, Setting::Player::AimStatus_ThickNess, TextToDraw, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName, ImColor(255, 165, 0), true, 0);
                            HeightOfName += Setting::Player::AimStatus_ThickNess;
                            HeightOfName += 1;*/
                         
                            if (Setting::Player::AimStatus)
                            {
                                if (AActor.Aimming == "Aiming")
                                {
                                    // --- Premium Screen Center Warning ---
                                    ImDrawList* dl = ImGui::GetBackgroundDrawList();
                                    float screenW = (float)ScreenInfo::Width;
                                    float screenH = (float)ScreenInfo::Height;
                                    
                                    std::string warningText = (Setting::LanguageVNM) ? (u8"ĐỊCH ĐANG NGẮM BẮN BẠN") : ("ENEMY IS AIMING AT YOU");
                                    float fontSize = 24.0f;
                                    ImVec2 textSize = Fonts::MainFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, warningText.c_str());
                                    
                                    float boxH = 42.0f;
                                    float boxW = textSize.x + 75.0f;
                                    float boxX = screenW / 2.0f - boxW / 2.0f;
                                    float boxY = screenH * 0.145f - 20.0f;
                                    float chamfer = boxH / 2.0f;
                                    
                                    // Hexagon points calculation
                                    ImVec2 pts[6];
                                    pts[0] = ImVec2(boxX + chamfer, boxY);
                                    pts[1] = ImVec2(boxX + boxW - chamfer, boxY);
                                    pts[2] = ImVec2(boxX + boxW, boxY + boxH / 2.0f);
                                    pts[3] = ImVec2(boxX + boxW - chamfer, boxY + boxH);
                                    pts[4] = ImVec2(boxX + chamfer, boxY + boxH);
                                    pts[5] = ImVec2(boxX, boxY + boxH / 2.0f);

                                    // 1. Background (Premium Dark Glassmorphism Hexagon)
                                    dl->AddConvexPolyFilled(pts, 6, ImColor(15, 15, 15, 220));
                                    
                                    // 2. Pulsing Glow Border (Premium Orange)
                                    ImColor pulseCol = (Randintaim == 0) ? ImColor(255, 140, 0, 255) : ImColor(255, 165, 0, 255);
                                    dl->AddPolyline(pts, 6, pulseCol, ImDrawFlags_Closed, 2.0f);
                                    
                                    // 3. Icon (FontAwesome Alert Triangle)
                                    const char* icon = "\xef\x81\xb1";
                                    ImVec2 iconSize = Fonts::FontAwesome->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, icon);
                                    float iconX = boxX + 18.0f;
                                    float centerY = boxY + boxH / 2.0f;
                                    dl->AddText(Fonts::FontAwesome, fontSize, ImVec2(iconX, centerY - iconSize.y/2.0f), pulseCol, icon);
                                    
                                    // 4. Warning Text (White)
                                    dl->AddText(Fonts::MainFont, fontSize - 4.0f, ImVec2(iconX + 35.0f, centerY - textSize.y/2.0f), ImColor(240, 240, 240, 255), warningText.c_str());

                                    // --- Premium ESP Indicator (On Player Box) ---
                                    std::string tagText = "AIMING";
                                    ImVec2 tagSize = Fonts::MainFont->CalcTextSizeA(13.0f, FLT_MAX, 0.0f, tagText.c_str());
                                    float tagW = tagSize.x + 12.0f;
                                    float tagH = tagSize.y + 4.0f;
                                    float tagX = Screen.X - tagW * 0.5f;
                                    float tagY = (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName;
                                    
                                    // Tag Background (Premium Grey)
                                    dl->AddRectFilled(ImVec2(tagX, tagY), ImVec2(tagX + tagW, tagY + tagH), ImColor(45, 45, 45, 220), 4.0f);
                                    // Tag Border (Orange alert highlight)
                                    dl->AddRect(ImVec2(tagX, tagY), ImVec2(tagX + tagW, tagY + tagH), ImColor(255, 165, 0, 255), 4.0f, 0, 1.2f);
                                    // Tag Text
                                    dl->AddText(Fonts::MainFont, 13.0f, ImVec2(tagX + 6.0f, tagY + 2.0f), ImColor(255, 255, 255, 255), tagText.c_str());
                                    
                                    HeightOfName += tagH + 3.0f;
                                }
                            }
                            // if (AActor.IsHiddenFOW == true) {
                            //     TextToDraw = "HIDDEN";
                            //     DrawBorderString(Fonts::MainFont, Setting::Player::Name_ThickNess, TextToDraw, Screen.X, Screen.Y + Screen.Z + HeightOfName, ImColor(255,0,0,255));
                            //     HeightOfName += Setting::Player::Name_ThickNess;
                            // }
                            /*if (AActor.bAutoDestroyWhenFinished == 62 || AActor.IsHiddenFOW)
                            {
                                TextToDraw = "HIDDEN";
                                DrawBorderString(Fonts::MainFont, Setting::Player::Name_ThickNess, TextToDraw, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName, ImColor(255, 0, 0, 255));
                                HeightOfName += Setting::Player::Name_ThickNess;
                                HeightOfName += 1;
                            }*/
                            if (Setting::SpecialFeatures::DevMode)
                            {
                                TextToDraw = "X : " + std::to_string(AActor.Position.X) + " Y: " + std::to_string(AActor.Position.Y) + " Z: " + std::to_string(AActor.Position.Z);
                                DrawBorderString(Fonts::MainFont, Setting::Player::Name_ThickNess, TextToDraw, Screen.X, (Screen.Y - iExtraBoxY) + Screen.Z + iExtraBoxY * 2 + HeightOfName, ImColor(255, 255, 255, 255));
                                HeightOfName += Setting::Player::Name_ThickNess;
                                HeightOfName += 1;
                            }
                        }
                    }

                    // --- PASS 2: SKELETON RENDERING (Ensures Skeletons are on TOP of all other ESP elements) ---
                    if (Setting::Player::Details == true && Setting::Player::Skeleton == true)
                    {
                        for (AActor &AActor : Data::AActorList)
                        {
                            if (Data::LocalbDead && Setting::QoL::bESPWhenSpectating && AActor.TeamID == Data::MyTeamID)
                                continue;
                            
                            Screen.X = 0; Screen.Y = 0; Screen.Z = 0;
                            if (!Algorithm::WorldToScreenPlayer(AActor.Position, Screen, Data::ViewMatrixBase))
                                continue;
                            if (Screen.X < -(float)ScreenInfo::Width || Screen.X > (float)ScreenInfo::Width * 2.f ||
                                Screen.Y < -(float)ScreenInfo::Height || Screen.Y > (float)ScreenInfo::Height * 2.f)
                                continue;

                            Screen.X += Setting::Player::ESP_OffsetX;
                            Screen.Y += Setting::Player::ESP_OffsetY;
                            if (Setting::ESPExtras::bFOVFilterESP && Setting::Fov)
                            {
                                float dx = Screen.X - ScreenInfo::Width * 0.5f;
                                float dy = Screen.Y - ScreenInfo::Height * 0.5f;
                                if (sqrtf(dx*dx + dy*dy) > Setting::FovAim + 20.f) continue;
                            }

                            HeadPos = AActor.HeadPos;
                            HeadPos.Z += 7;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, HeadPos, Head)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.ChestPos, Chest)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.PelvisPos, Pelvis)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.lSholderPos, lSholder)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.rSholderPos, rSholder)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.lElbowPos, lElbow)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.rElbowPos, rElbow)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.lWristPos, lWrist)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.rWristPos, rWrist)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.lThighPos, lThigh)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.rThighPos, rThigh)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.lKneePos, lKnee)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.rKneePos, rKnee)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.lAnklePos, lAnkle)) continue;
                            if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.rAnklePos, rAnkle)) continue;

                            ImColor skCol;
                            bool usePriority = false;
                            double nowSec = GetTickCount64() / 1000.0;
                            if (Setting::ESPExtras::bThreatHighlight) {
                                auto it = Data::ThreatUntilTime.find(AActor.Address);
                                if (it != Data::ThreatUntilTime.end() && it->second > nowSec) {
                                    usePriority = true;
                                    skCol = ImColor(Setting::ESPExtras::ThreatHighlight_Color[0], Setting::ESPExtras::ThreatHighlight_Color[1], Setting::ESPExtras::ThreatHighlight_Color[2], Setting::ESPExtras::ThreatHighlight_Color[3]);
                                }
                            }
                            if (!usePriority && Setting::ESPExtras::bRecentlyShotHighlight && AActor.Address == Data::LastHitActorAddress && (nowSec - Data::LastHitTime) < (double)Setting::ESPExtras::fRecentlyShotDurationSec) {
                                usePriority = true;
                                skCol = ImColor(Setting::ESPExtras::RecentlyShotHighlight_Color[0], Setting::ESPExtras::RecentlyShotHighlight_Color[1], Setting::ESPExtras::RecentlyShotHighlight_Color[2], Setting::ESPExtras::RecentlyShotHighlight_Color[3]);
                            }

                            if (!usePriority) {
                                if (AActor.Health == 0 && !AActor.IsDead) skCol = ImColor(255, 0, 0, 255);
                                else if (Setting::Player::VisibleCheck) {
                                    if (AActor.IsVisible == 1) skCol = ImColor(Setting::Player::VisibleCheck_Color[0], Setting::Player::VisibleCheck_Color[1], Setting::Player::VisibleCheck_Color[2], Setting::Player::VisibleCheck_Color[3]);
                                    else skCol = ImColor(Setting::Player::VisibleCheckPart2_Color[0], Setting::Player::VisibleCheckPart2_Color[1], Setting::Player::VisibleCheckPart2_Color[2], Setting::Player::VisibleCheckPart2_Color[3]);
                                } else {
                                    if (Setting::Player::SkeletonType == 0) skCol = ImColor(Setting::Player::Skeleton_Color[0], Setting::Player::Skeleton_Color[1], Setting::Player::Skeleton_Color[2], Setting::Player::Skeleton_Color[3]);
                                    else try { skCol = _randombush[AActor.TeamID % 7]; } catch(...) { skCol = ImColor(255, 255, 255, 255); }
                                }
                            }

                            float boneradius = Screen.Z / 12;
                            if (boneradius <= 0.7f) boneradius = 0.7f;
                            float skThick = Setting::Player::Skeleton_ThickNess;

                            if (Setting::Player::SkeletonStyle == 1) // Neon Glow
                            {
                                ImColor glowCol = ImColor(skCol.Value.x, skCol.Value.y, skCol.Value.z, skCol.Value.w * 0.3f);
                                float glowThick = skThick + 6.0f;
                                DrawCircleForeground(Head.X, Head.Y, boneradius + 1.5f, glowCol, glowThick);
                                DrawLineForeground(Chest.X, Chest.Y, Pelvis.X, Pelvis.Y, glowCol, glowThick);
                                DrawLineForeground(Chest.X, Chest.Y, lSholder.X, lSholder.Y, glowCol, glowThick);
                                DrawLineForeground(Chest.X, Chest.Y, rSholder.X, rSholder.Y, glowCol, glowThick);
                                DrawLineForeground(lSholder.X, lSholder.Y, lElbow.X, lElbow.Y, glowCol, glowThick);
                                DrawLineForeground(rSholder.X, rSholder.Y, rElbow.X, rElbow.Y, glowCol, glowThick);
                                DrawLineForeground(lElbow.X, lElbow.Y, lWrist.X, lWrist.Y, glowCol, glowThick);
                                DrawLineForeground(rElbow.X, rElbow.Y, rWrist.X, rWrist.Y, glowCol, glowThick);
                                DrawLineForeground(Pelvis.X, Pelvis.Y, lThigh.X, lThigh.Y, glowCol, glowThick);
                                DrawLineForeground(Pelvis.X, Pelvis.Y, rThigh.X, rThigh.Y, glowCol, glowThick);
                                DrawLineForeground(lThigh.X, lThigh.Y, lKnee.X, lKnee.Y, glowCol, glowThick);
                                DrawLineForeground(rThigh.X, rThigh.Y, rKnee.X, rKnee.Y, glowCol, glowThick);
                                DrawLineForeground(lKnee.X, lKnee.Y, lAnkle.X, lAnkle.Y, glowCol, glowThick);
                                DrawLineForeground(rKnee.X, rKnee.Y, rAnkle.X, rAnkle.Y, glowCol, glowThick);

                                ImColor midCol = ImColor(skCol.Value.x, skCol.Value.y, skCol.Value.z, skCol.Value.w * 0.6f);
                                float midThick = skThick + 2.5f;
                                DrawCircleForeground(Head.X, Head.Y, boneradius + 0.5f, midCol, midThick);
                                DrawLineForeground(Chest.X, Chest.Y, Pelvis.X, Pelvis.Y, midCol, midThick);
                                DrawLineForeground(Chest.X, Chest.Y, lSholder.X, lSholder.Y, midCol, midThick);
                                DrawLineForeground(Chest.X, Chest.Y, rSholder.X, rSholder.Y, midCol, midThick);
                                DrawLineForeground(lSholder.X, lSholder.Y, lElbow.X, lElbow.Y, midCol, midThick);
                                DrawLineForeground(rSholder.X, rSholder.Y, rElbow.X, rElbow.Y, midCol, midThick);
                                DrawLineForeground(lElbow.X, lElbow.Y, lWrist.X, lWrist.Y, midCol, midThick);
                                DrawLineForeground(rElbow.X, rElbow.Y, rWrist.X, rWrist.Y, midCol, midThick);
                                DrawLineForeground(Pelvis.X, Pelvis.Y, lThigh.X, lThigh.Y, midCol, midThick);
                                DrawLineForeground(Pelvis.X, Pelvis.Y, rThigh.X, rThigh.Y, midCol, midThick);
                                DrawLineForeground(lThigh.X, lThigh.Y, lKnee.X, lKnee.Y, midCol, midThick);
                                DrawLineForeground(rThigh.X, rThigh.Y, rKnee.X, rKnee.Y, midCol, midThick);
                                DrawLineForeground(lKnee.X, lKnee.Y, lAnkle.X, lAnkle.Y, midCol, midThick);
                                DrawLineForeground(rKnee.X, rKnee.Y, rAnkle.X, rAnkle.Y, midCol, midThick);
                            }

                            DrawCircleForeground(Head.X, Head.Y, boneradius, skCol, skThick);
                            DrawLineForeground(Chest.X, Chest.Y, Pelvis.X, Pelvis.Y, skCol, skThick);
                            DrawLineForeground(Chest.X, Chest.Y, lSholder.X, lSholder.Y, skCol, skThick);
                            DrawLineForeground(Chest.X, Chest.Y, rSholder.X, rSholder.Y, skCol, skThick);
                            DrawLineForeground(lSholder.X, lSholder.Y, lElbow.X, lElbow.Y, skCol, skThick);
                            DrawLineForeground(rSholder.X, rSholder.Y, rElbow.X, rElbow.Y, skCol, skThick);
                            DrawLineForeground(lElbow.X, lElbow.Y, lWrist.X, lWrist.Y, skCol, skThick);
                            DrawLineForeground(rElbow.X, rElbow.Y, rWrist.X, rWrist.Y, skCol, skThick);
                            DrawLineForeground(Pelvis.X, Pelvis.Y, lThigh.X, lThigh.Y, skCol, skThick);
                            DrawLineForeground(Pelvis.X, Pelvis.Y, rThigh.X, rThigh.Y, skCol, skThick);
                            DrawLineForeground(lThigh.X, lThigh.Y, lKnee.X, lKnee.Y, skCol, skThick);
                            DrawLineForeground(rThigh.X, rThigh.Y, rKnee.X, rKnee.Y, skCol, skThick);
                            DrawLineForeground(lKnee.X, lKnee.Y, lAnkle.X, lAnkle.Y, skCol, skThick);
                            DrawLineForeground(rKnee.X, rKnee.Y, rAnkle.X, rAnkle.Y, skCol, skThick);
                        }
                    }
                    TargetID = 0;

                    for (AActor &AActor : Data::AActorList)
                    {
                        Screen.X = 0;
                        Screen.Y = 0;
                        Distance = sqrt(pow(Data::LocalPosition.X - AActor.Position.X, 2) + pow(Data::LocalPosition.Y - AActor.Position.Y, 2) + pow(Data::LocalPosition.Z - AActor.Position.Z, 2)) / 100;
                        try
                        {
                            ColTeamID = _randombush[AActor.TeamID % 7];
                        }
                        catch (exception)
                        {
                            ColTeamID = ImColor(255, 255, 255, 255);
                        }
                        if (Setting::Player::Details == true && Setting::Player::Radar == true)
                        {
                            FVector2D EntityPos = MathV::WorldToRadar(Data::GlobalCameraCache.Rotation, Data::GlobalCameraCache.Location, FVector(AActor.Position.X, AActor.Position.Y, AActor.Position.Z), FVector2D(0, 0), FVector2D(ScreenInfo::Width, ScreenInfo::Height));
                            int radar_range = Setting::Player::Radar_Distance;
                            int x, y;
                            x = EntityPos.X;
                            y = EntityPos.Y;
                            auto angle = FVector();
                            auto Forward = FVector((float)(ScreenInfo::Width / 2) - x, (float)(ScreenInfo::Height / 2) - y, 0.f);
                            MathV::VectorAnglesRadar(Forward, angle);
                            const auto angle_yaw_rad = DEG2RAD(angle.Y + 180.f);
                            const auto new_point_x = (ScreenInfo::Width / 2) + (radar_range) / 2 * 8 * cosf(angle_yaw_rad);
                            const auto new_point_y = (ScreenInfo::Height / 2) + (radar_range) / 2 * 8 * sinf(angle_yaw_rad);
                            std::array<FVector, 3> points{
                                FVector(new_point_x - ((90) / 4 + 3.5f) / 2, new_point_y - ((radar_range) / 4 + 3.5f) / 2, 0.f),
                                FVector(new_point_x + ((90) / 4 + 3.5f) / 4, new_point_y, 0.f),
                                FVector(new_point_x - ((90) / 4 + 3.5f) / 2, new_point_y + ((radar_range) / 4 + 3.5f) / 2, 0.f) };
                            MathV::RotateTriangle(points, angle.Y + 180.f);
                            if (!Algorithm::WorldToScreen(AActor.Position, Screen, Data::ViewMatrixBase))
                            {
                                if (Distance <= 139)
                                {
                                    auto now = steady_clock::now();
                                    auto ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

                                    if ((ms / 800) % 2 == 0)
                                    {
                                        if (Setting::Player::VisibleCheck)
                                        {
                                            if (AActor.IsVisible == 1)
                                            {
                                                DrawCircle(
                                                    (points.at(0).X + points.at(1).X + points.at(2).X) / 3,
                                                    (points.at(0).Y + points.at(1).Y + points.at(2).Y) / 3,
                                                    13,
                                                    ImColor(
                                                        Setting::Player::VisibleCheck_Color[0],
                                                        Setting::Player::VisibleCheck_Color[1],
                                                        Setting::Player::VisibleCheck_Color[2],
                                                        Setting::Player::VisibleCheck_Color[3]
                                                    ),
                                                    11
                                                );
                                            }
                                            else
                                            {
                                                DrawCircle(
                                                    (points.at(0).X + points.at(1).X + points.at(2).X) / 3,
                                                    (points.at(0).Y + points.at(1).Y + points.at(2).Y) / 3,
                                                    13,
                                                    ColTeamID,
                                                    11
                                                );
                                            }
                                        }
                                        else
                                        {
                                            DrawCircle((points.at(0).X + points.at(1).X + points.at(2).X) / 3, (points.at(0).Y + points.at(1).Y + points.at(2).Y) / 3, 13, ColTeamID, 11);
                                        }
                                    }
                                }
                                if (Distance >= 140)
                                {
                                    if (Setting::Player::VisibleCheck)
                                    {
                                        ImColor color = ColTeamID;

                                        if (AActor.IsVisible == 1)
                                        {
                                            color = ImColor(
                                                Setting::Player::VisibleCheck_Color[0],
                                                Setting::Player::VisibleCheck_Color[1],
                                                Setting::Player::VisibleCheck_Color[2],
                                                Setting::Player::VisibleCheck_Color[3]
                                            );
                                        }

                                        DrawCircleFilled(
                                            (points.at(0).X + points.at(1).X + points.at(2).X) / 3,
                                            (points.at(0).Y + points.at(1).Y + points.at(2).Y) / 3,
                                            9,
                                            color,
                                            1
                                        );
                                    }
                                }
                            }
                        }
                    }
                    /*
                    if(true) {
                        std::vector<VECTOR3> HisttoryPos;
                        HisttoryPos.clear( );
                        FVector Velocity(Cheat::Test1, Cheat::Test2, Cheat::Test3);
                        VECTOR3 PreviousPosition;
                        PreviousPosition.X = Data::LocalHeadPos.X;
                        PreviousPosition.Y = Data::LocalHeadPos.Y;
                        PreviousPosition.Z = Data::LocalHeadPos.Z;
                        HisttoryPos.push_back(PreviousPosition);
                        for (float i = 0; i < 10; i += 0.01f) {
                            float simulationtime = i * 6;
                            FVector Displacement;
                            VECTOR3 LastPosSSS;
                            VECTOR3 PrevPosScreen;
                            VECTOR3 CurposScreen;
                            Displacement.X = std::cos(Data::GlobalCameraCache.Rotation.Pitch) * Velocity.X * simulationtime;
                            Displacement.Y = std::sin(Data::GlobalCameraCache.Rotation.Pitch) * Velocity.Y * simulationtime - (9.8f * simulationtime * simulationtime / 2);
                            Displacement.Z = Velocity.Z * simulationtime;
                            LastPosSSS.X = Data::LocalHeadPos.X + Displacement.X;
                            LastPosSSS.Y = Data::LocalHeadPos.Y + Displacement.Y;
                            LastPosSSS.Z = Data::LocalHeadPos.Z + Displacement.Z;
                            HisttoryPos.push_back(LastPosSSS);
                            // Algorithm::WorldToScreen(PreviousPosition, PrevPosScreen, Data::ViewMatrixBase);
                            // Algorithm::WorldToScreen(LastPosSSS, CurposScreen, Data::ViewMatrixBase);
                            // DrawLine(PrevPosScreen.X, PrevPosScreen.Y, CurposScreen.X, PreviousPosition.Y, ImColor(255, 0, 0, 255), 1);
                            // PreviousPosition = LastPosSSS;
                        }
                        for (int i = 1; i < HisttoryPos.size( ); i++) {
                            VECTOR3 PrevPos;
                            VECTOR3 NextPos;
                            Algorithm::WorldToScreen(HisttoryPos.at(i - 1), PrevPos, Data::ViewMatrixBase);
                            Algorithm::WorldToScreen(HisttoryPos.at(i), NextPos, Data::ViewMatrixBase);
                            DrawLine(PrevPos.X, PrevPos.Y, NextPos.X, NextPos.Y, ImColor(255, 0, 255, 255), 3);
                        }
                    }
                    */
                    if (Setting::Player::ZonePrediction)
                    {
                        if (Data::NetDriver > 0)
                        {
                            // Fix Ground Clipping by adding Z offset to the label position
                            VECTOR3 labelWorldPos = { Data::NextZonePosition.X, Data::NextZonePosition.Y, Data::NextZonePosition.Z + 150.f };
                            
                            if (Algorithm::WorldToScreen(labelWorldPos, Screen, Data::ViewMatrixBase))
                            {
                                Distance = sqrt(pow(Data::LocalPosition.X - Data::NextZonePosition.X, 2) + pow(Data::LocalPosition.Y - Data::NextZonePosition.Y, 2) + pow(Data::LocalPosition.Z - Data::NextZonePosition.Z, 2)) / 100;
                                TextToDraw = "Zone [" + to_string((int)Distance) + " M]";
                                
                                ImColor zoneCol = ImColor(Setting::Player::ZonePrediction_Color[0], Setting::Player::ZonePrediction_Color[1], Setting::Player::ZonePrediction_Color[2], Setting::Player::ZonePrediction_Color[3]);
                                
                                // Draw Floating Label
                                DrawBorderString(Fonts::MainFont, Setting::Player::ZonePrediction_ThickNess, TextToDraw, Screen.X, Screen.Y, zoneCol);
                                
                                // Floating Indicator (Small circle and line, removed the large 3D circle as requested)
                                DrawCircle(Screen.X, Screen.Y - 20, 4, zoneCol, 1);
                                DrawLine(Screen.X, Screen.Y - 15, Screen.X, Screen.Y - 5, zoneCol, 1);
                            }
                        }
                    }
                    if (Setting::SpecialFeatures::ArNearest)
                    {
                        ARNearestList = ARNearestCache;
                        int LargetDistance = 300;
                        VECTOR3 ScreenDraw;
                        ScreenDraw.X = 0;
                        ScreenDraw.Y = 0;
                        for (ARNearest &ARNearest : ARNearestList)
                        {
                            if (ARNearest.Distance < LargetDistance)
                            {
                                LargetDistance = ARNearest.Distance;
                                ScreenDraw = ARNearest.Position;
                            }
                        }
                        if (ScreenDraw.X > 0 && ScreenDraw.Y > 0 && ARNearestList.size() > 0)
                        {
                            DrawLine(ScreenInfo::Width / 2, ScreenInfo::Height, ScreenDraw.X, ScreenDraw.Y, ImColor(255, 0, 0, 255), 3);
                        }
                        ARNearestCache.clear();
                    }
                    if (Setting::SpecialFeatures::DevMode)
                    {
                        for (DevMode &DevMode : Data::DevModeList)
                        {
                            if (!Algorithm::WorldToScreen(DevMode.Position, Screen, Data::ViewMatrixBase))
                                continue;
                            if (Screen.X <= 0 || Screen.Y <= 0)
                                continue;
                            DrawBorderString(Fonts::MainFont, 15, DevMode.EntityName, Screen.X, Screen.Y, ImColor(255, 255, 255, 255));
                        }
                    }
                    if (Setting::SpecialFeatures::DangerMode)
                    {
                        for (VECTOR3 &Vec3 : Data::BulletList)
                        {
                            if (!Algorithm::WorldToScreen(Vec3, Screen, Data::ViewMatrixBase))
                                continue;
                            // DrawRectangle(Screen.X, Screen.Y, Screen.Z / 2, Screen.Z, ImColor(255, 0, 0, 255), 2);
                            DrawCircleFilled(Screen.X, Screen.Y, 2.f, ImColor(255, 0, 0, 255));
                        }
                    }
                    }
                    if (Data::NetDriver > 0)
                    {
                        if (Setting::AimAlternative && Setting::Fov && AimForm::BestTargetID > 0)
                        {
                            DrawFOVRing(ScreenInfo::Width / 2, ScreenInfo::Height / 2, (float)Setting::FovAim, ImColor(Setting::Fov_Color[0], Setting::Fov_Color[1], Setting::Fov_Color[2], Setting::Fov_Color[3]), 1.5f);
                        }
                    }
                }
                
                ImGui::RenderNotifications();
                ImGui::Render();
                /* Anti-Screen Capture: apply WDA to game window so it is excluded from capture/OBS when ON (UnityHax-style). */
                if (!s_pSetWindowDisplayAffinity)
                    s_pSetWindowDisplayAffinity = (PFN_SetWindowDisplayAffinity)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowDisplayAffinity");
                HWND hwndMain = s_GameWindow ? s_GameWindow : ScreenInfo::hWindow;
                if (hwndMain && IsWindow(hwndMain) && s_pSetWindowDisplayAffinity)
                    s_pSetWindowDisplayAffinity(hwndMain, Setting::AntiScreenCapture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pd3dRenderTarget, NULL);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            }
        }
    }
}

#else
#include <Windows.h>
namespace DX11
{
    void Hook(HWND hwnd) { LOG("\n"); }
    void Unhook() {}
} // namespace DX11
#endif

