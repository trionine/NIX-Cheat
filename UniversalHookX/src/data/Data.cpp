#include "Data.h"
#include "../setting/Setting.h"
#include "../../src/hooks/backend/dx11/hook_directx11.hpp"
#include "../../src/hooks/hooks.hpp"
#include "../../Teammate/Teammate.h"
#include <ole2.h>
#include "FarsiType.h"
#include <olectl.h>
#include <wininet.h>
static int MapSkinLobbyId(int id)
{
    if (id == 0) return 0;
    static const std::unordered_map<std::string, std::string> SkinLobbyMap = {
        {"403003", "1407366"},
        {"1406748", "1407392"},
        {"2200101", "12211301"},
        {"2200201", "12213201"},
        {"2200301", "12209901"},
        {"2001001", "2002941"},
        {"501001", "1501003639"},
        {"501002", "1501003639"},
        {"501003", "1501003639"},
        {"202408052", "202408058"},
    };
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", id);
    auto it = SkinLobbyMap.find(buf);
    if (it != SkinLobbyMap.end())
        return atoi(it->second.c_str());
    return id;
}
INT Data::AlivePlayer = 0;
INT Data::AliveTeam = 0;
INT Data::PlayerNum = 0;
INT Data::ElapsedTime = 0;
float Data::CirclePain = 0.f;
int Data::CurCircleWave = 0;
INT Data::IsFreeUser = 0;
INT Data::EntityCount = 0;
INT Data::GameID = 0;
INT Data::LastGameID = 0;
INT Data::TotalCheater = 0;
FLOAT Data::LocalTravelled = 0;
FLOAT Data::LocalTravelSpeed = 0;
DWORD Data::ViewMatrix = 0;
DWORD Data::LibUE4Addv = 0;
DWORD Data::PlayerKey = 0;
DWORD Data::LocalPlayer = 0;
DWORD Data::NetDriver = 0;
DWORD Data::STPlayerController = 0;
// DWORD Data::PlayerController = 0;
DWORD Data::CurrentReloadWeapon = 0;
FMinimalViewInfo Data::GlobalCameraCache;
GameData Data::pGameData;
SkeletonRead Data::skeData;
BOOL Data::LocalbIsWeaponFiring;
BOOL Data::LocalbDead;
BOOL Data::IsMouseClick = false;
BOOL Data::ResetMenu = false;
VECTOR3 Data::NextZonePosition;
VECTOR3 Data::LocalPosition;
VECTOR3 Data::LocalHeadPos;
D3DMATRIX1 Data::ViewMatrixBase;
DWORD Data::LastHitActorAddress = 0;
double Data::LastHitTime = 0.0;
std::map<DWORD, INT> Data::ActorLastCurBullet;
std::map<DWORD, double> Data::ThreatUntilTime;
DWORD Data::MyTeamID = 0;
string Data::LocalUID = "";
UINT uWorld = 0;
UINT gNames = 0;
HANDLE hHandleAttachment;
vector<AActor> Data::AActorList = {};
vector<Vehicle> Data::VehicleList = {};
vector<Item> Data::ItemList = {};
vector<Alerts> Data::AlertsList = {};
vector<GrenadeTime> Data::GrenadeTimeList = {};
vector<DevMode> Data::DevModeList = {};
vector<VECTOR3> Data::BulletList = {};
vector<NamedCache> Data::NamedCacheList = {};
vector<HANDLE> Data::HandleList = {};
vector<BombPosition> Data::BombPositionList = {};
vector<string> Data::LocalChatList = {};
vector<std::string> Data::SpectatorNames = {};
vector<_Mod_Skin_Effect_Class> Data::bModEffectList = {};
vector<CachedPosition> LastPlayerPosition = {};
vector<docxuong> DocXuongList = {};
vector<checknup> CheckNupList = {};
std::vector<TempEsp> TempCacheActorAdrress = {};
std::vector<TempEsp> TempActorAdrress = {};
std::vector<string> CheaterList = {};
std::vector<string> TeammateCheaterList = {};
std::vector<string> TeammateList = {};
std::vector<SpecialName> SpecialNameList = {};
static std::wstring WideEmptyString = L"";
bool IsInitUID = false;
bool IsCheaterRequest = false;
bool IsShowedTotalCheater = false;
DWORD BaseModule = 0x0;
DWORD BaseModuleMSG = 0x0;
DWORD BaseModuleCar = 0x0;
DWORD BaseModuleDeadBox = 0x0;
DWORD BaseModuleSkeletonRead = 0x0;
DWORD BaseOffset = 0xCC4D4;
DWORD BaseOffsetMSG = BaseOffset + 0x4;
DWORD BaseOffsetCar = BaseOffset + 0xC;
DWORD BaseOffsetDeadBox = BaseOffset + 0x10;
DWORD BaseOffsetSkeletonRead = BaseOffset + 0x14;
VOID ModCarThread()
{
 
}

bool saveBitmap(LPCSTR filename, HBITMAP bmp, HPALETTE pal)
{
    bool result = false;
    PICTDESC pd;

    pd.cbSizeofstruct = sizeof(PICTDESC);
    pd.picType = PICTYPE_BITMAP;
    pd.bmp.hbitmap = bmp;
    pd.bmp.hpal = pal;

    LPPICTURE picture;
    HRESULT res = OleCreatePictureIndirect(&pd, IID_IPicture, false,
                                           reinterpret_cast<void **>(&picture));

    if (!SUCCEEDED(res))
        return false;

    LPSTREAM stream;
    res = CreateStreamOnHGlobal(0, true, &stream);

    if (!SUCCEEDED(res))
    {
        picture->Release();
        return false;
    }

    LONG bytes_streamed;
    res = picture->SaveAsFile(stream, true, &bytes_streamed);

    HANDLE file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, 0,
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (!SUCCEEDED(res) || !file)
    {
        stream->Release();
        picture->Release();
        return false;
    }

    HGLOBAL mem = 0;
    GetHGlobalFromStream(stream, &mem);
    LPVOID data = GlobalLock(mem);

    DWORD bytes_written;

    result = !!WriteFile(file, data, bytes_streamed, &bytes_written, 0);
    result &= (bytes_written == static_cast<DWORD>(bytes_streamed));

    GlobalUnlock(mem);
    CloseHandle(file);

    stream->Release();
    picture->Release();

    return result;
}
bool screenCapturePart(int x, int y, int w, int h, LPCSTR fname)
{
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;
    if (saveBitmap(fname, hBitmap, hpal))
        return true;
    return false;
}
bool IsOnTeammateA(std::string PlayerID)
{
    if (Teammate::TeammateList.size() > 0)
    {
        for (int i = 0; i < Teammate::TeammateList.size(); i++)
        {
            if (PlayerID == Teammate::TeammateList[i])
                return true;
        }
    }
    return false;
}
size_t write_callbacask(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
{
    strValue = strDefaultValue;
    WCHAR szBuffer[512];
    DWORD dwBufferSize = sizeof(szBuffer);
    ULONG nError;
    nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
    if (ERROR_SUCCESS == nError)
    {
        strValue = szBuffer;
    }
    return nError;
}
std::string GetTempPathMobilePC()
{
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Tencent\\MobileGamePC", 0, KEY_READ, &hKey);
    bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
    bool bDoesNotExistsSpecifically(lRes == ERROR_FILE_NOT_FOUND);
    std::wstring strValueOfBinDir;
    GetStringRegKey(hKey, L"sf", strValueOfBinDir, L"bad");
    std::string returnday(strValueOfBinDir.begin(), strValueOfBinDir.end());
    ;
    return returnday;
}
std::map<std::wstring, std::wstring> CharsMap{
    {L"Ū", L" "},
    {L"ē", L" "},
    {L"Ē", L" "},
    {L"ō", L" "},
    {L"Ř", L" "},
    {L"ř", L" "},
    {L"ŗ", L" "},
    {L"Ŗ", L" "},
    {L"ŕ", L" "},
    {L"Ŕ", L" "},
    {L"Ů", L" "},
    {L"Ŭ", L" "},
    {L"ŭ", L" "},
    {L"ų", L" "},
    {L"ů", L" "},
    {L"Ű", L" "},
    {L"Ŵ", L" "},
    {L"ŵ", L" "},
    {L"Ŷ", L" "},
    {L"ŷ", L" "},
    {L"ī", L" "},
    {L"ū", L" "},
    {L"į", L" "},
    {L"Ġ", L" "},
    {L"ġ", L" "},
};

std::vector<std::string> SplitStr(std::string s, std::string delimiter)
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
std::wstring StringReplaceAll(std::wstring StringToScan, const std::wstring &Start, const std::wstring &End)
{
    size_t start_pos = 0;
    while ((start_pos = StringToScan.find(Start, start_pos)) != std::wstring::npos)
    {
        StringToScan.replace(start_pos, Start.length(), End);
        start_pos += End.length();
    }
    return StringToScan;
}
std::wstring ArrangeThem(std::wstring StringToArrange)
{
    WideEmptyString = StringToArrange;
    for (size_t i = 0; i < StringToArrange.size(); i++)
    {
        for (auto const &FaCharInWord : CharsMap)
        {
            if (FaCharInWord.first == std::wstring(1, StringToArrange[i]))
                WideEmptyString = StringReplaceAll(WideEmptyString, std::wstring(1, StringToArrange[i]), FaCharInWord.second);
        }
    }
    return WideEmptyString;
}
std::wstring read_unicode(const std::uintptr_t address, std::size_t size)
{
    const auto buffer = std::make_unique<wchar_t[]>(size);
    ReadProcessMemory(Utility::GameHandle, (LPVOID)address, buffer.get(), size * 2, 0);
    return std::wstring(buffer.get());
}
BOOL FileExits(string namefile)
{
    std::ifstream infile(namefile);
    BOOL bReturn = infile.good();
    infile.close();
    return bReturn;
}
BOOL IsNamedCache(DWORD Addv, NamedCache *Cache)
{
    if (Addv == 0)
        return false;
    if (Data::NamedCacheList.size() == 0)
        return false;
    for (int i = 0; i < Data::NamedCacheList.size(); i++)
    {
        if (Addv == Data::NamedCacheList[i].Address)
        {
            *Cache = Data::NamedCacheList[i];
            return true;
        }
    }
    return false;
}
BOOL IsPlayer(std::string type)
{
    if (type == ("BP_PlayerCharacter_Dinosaur_C") || type == ("BP_PlayerPawn_Dinosaur_Livik_C") || type == ("BP_PlayerPawn_C") || type == ("BP_PlayerPawn_AI_C") || type == ("BP_PlayerPawn_FM_C") || type == ("BP_PlayerPawn_FM_Bot_C") || type == ("BP_PlayerCharacter_SlayTheBot_AI_C") || type == ("BP_PlayerCharacter_TeamDeathAllMatch_C") || type == ("BP_PlayerCharacter_FourMaps_TeamDeathAllMatch_C") || type == ("BP_PlayerPawn_FM_AI_C") || type == ("BP_PlayerPawn_SI_C") || type == ("BP_PlayerPawn_ST_C") || type == ("BP_PlayerPawn_ST_AI_C") || type == ("BP_PlayerPawn_Heavy_C") || type == ("BP_PlayerPawn_XT_C") || type == ("BP_PlayerPawn_TDM_TPP_8v8_C") || type == ("BP_PlayerPawn_TDM_TPP_C") || type == ("BP_PlayerCharacter_BRTDM_C") || type == ("BP_PlayerCharacter_SanHok_BRTDM_C") || type == ("BP_PlayerCharacter_FourMap_BRTDM_C") || type == ("BP_PlayerCharacter_EastAsiaRolePlay_C") || type == ("BP_PlayerCharacter_Borderland_C") || type == ("BP_PlayerCharacter_EastAsia_LivikRolePlay_C") || type == ("BP_PlayerCharacter_Sink_Revival_C") || type == ("BP_PlayerPawn_Sink_AI_C") || type == ("BP_PlayerPawn_HT_AI_C") || type == ("BP_PlayerCharacter_ZNQ5th_C") || type == ("BP_PlayerCharacter_ZNQ5th_Livik_C") || type == ("BP_PlayerCharacter_CreativeMode_C") || type == ("BP_PlayerCharacter_ZNQ5thRolePlay_C") || type == ("BP_PlayerCharacter_ZNQ5thRolePlay_Livik_C"))
        return true;
    if (type.find("BP_PlayerPawn") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerCharacter") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlanET_FakePlayer") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlayerPawn_Infec") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_FM_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_Rune_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlayerPawn_Infec_InvisibleZombie_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlayerPawn_Infec_Human_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_ZombieBase_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_TDM_TPP_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_FM_Bot_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerCharacter_SlayTheBot_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlanET_FakePlayer_AIPawn_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_SI_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_Heavy_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlayerPawn_Infec_Revenger_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlayerPawn_Infec_NormalZombie_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlayerPawn_Infec_KingZombie_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerCharacter_FourMaps_ZNQ4th_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerCharacter_PlanX_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("PlanX_PlayerCharacter_AI_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerCharacter_PlanA_C") != string::npos)
    {
        return TRUE;
    }
    if (type.find("BP_PlayerPawn_ST_AI_C") != string::npos)
    {
        return TRUE;
    }
    return false;
}
BOOL IsVehicle(std::string type)
{
    if (type == ("BP_VH_Buggy_2_C") || type == ("BP_VH_Buggy_C") || type == ("BP_VH_Buggy_3_C") || type == ("BP_VH_Buggy_4_C") || type == ("BP_VH_Buggy_5_C") || type == ("BP_VH_Buggy_6_C") || type == ("VH_Dacia_C") || type == ("VH_Dacia_2_C") || type == ("VH_Dacia_3_C") || type == ("VH_Dacia_4_C") || type == ("VH_UAZ01_C") || type == ("VH_UAZ02_C") || type == ("VH_UAZ03_C") || type == ("PickUp_01_C") || type == ("PickUp_02_C") || type == ("PickUp_03_C") || type == ("PickUp_04_C") || type == ("PickUp_05_C") || type == ("PickUp_06_C") || type == ("PickUp_07_C") || type == ("PickUp_08_C") || type == ("PickUp_09_C") || type == ("PickUp_10_C") || type == ("VH_MiniBus_01_C") || type == ("VH_MiniBus_02_C") || type == ("VH_MiniBus_03_C") || type == ("Mirado_open_1_C") || type == ("Mirado_open_2_C") || type == ("Mirado_open_3_C") || type == ("Mirado_open_4_C") || type == ("Mirado_open_Golden_C") || type == ("Mirado_close_1_C") || type == ("Mirado_close_2_C") || type == ("Mirado_close_3_C") || type == ("Mirado_close_4_C") || type == ("VH_Scooter1_C") || type == ("VH_Scooter2_C") || type == ("VH_Scooter3_C") || type == ("Rony_1_C") || type == ("Rony_2_C") || type == ("Rony_3_C") || type == ("LadaNiva_01_C") || type == ("LadaNiva_02_C") || type == ("VH_Motorcycle_C") || type == ("VH_Motorcycle_1_C") || type == ("VH_MotorcycleCart_C") || type == ("VH_MotorcycleCart_1_C") || type == ("VH_Motorglider_C") || type == ("VH_PG117_C") || type == ("AquaRail_1_C") || type == ("water_Plane") || type == ("VH_Snowbike_C") || type == ("VH_Snowbile_C") || type == ("VH_CoupeRB_1_C") || type == ("BP_VH_Bigfoot_C") || type == ("VH_BRDM_C") || type == ("BP_VH_Tuk_1_C") || type == ("VH_UTV_C") || type == ("UH60_Vehicle_C") || type == ("ArmedUAZ_C") || type == ("ArmedPickupFloat_low_C") || type == ("ArmedBuggyFloat_low_C") || type == ("ArmedDacia_low_C") || type == ("AH6_Vehicle_C") || type == ("BP_Bike_Pickable_C") || type == ("BP_MediumTank_C") || type == ("BP_VH_Buggy_SI_C") || type == ("BP_VH_Tuk_1_SI_C") || type == ("BP_Bike_WithRack_SI_C") || type == ("VH_CoupeRB_1_SI_C") || type == ("VH_Dacia_SI_C") || type == ("VH_MotorcycleCart_SI_C") || type == ("VH_Motorcycle_SI_C") || type == ("VH_Motorglider_SI_C") || type == ("VH_PG117_SI_C") || type == ("VH_UAZ01_SI_C") || type == ("Mirado_open_1_SI_C") || type == ("VH_Snowmobile_C") || type.find("BP_RaptorVehicle_C") != std::string::npos || type.find("Pterosaur") != std::string::npos || type.find("TyrannosaurusRex") != std::string::npos || type.find("BP_TRexVehicle_C") != std::string::npos || type == "Rony_01_C" || type == "BP_CoupeRB_Base_C" || type == "VH_ATV1_C" || type == "VH_Scooter_C")
        return true;
    return false;
}
BOOL IsBullet(std::string type)
{
    if (type == ("BP_PlayerRifleBulletImpact_C"))
        return true;
    return false;
}
BOOL IsNextZone(std::string type)
{
    if (type == ("BP_radiation") || type == ("BP_radiation939") || type == ("BP_radiation_C"))
        return true;
    return false;
}
BOOL IsValidBomb(DWORD AddvBomb)
{
    if (Data::GrenadeTimeList.size() > 0)
    {
        for (int i = 0; i <= Data::GrenadeTimeList.size(); i++)
        {
            if (Data::GrenadeTimeList[i].Addv == AddvBomb)
                return true;
        }
    }
    return false;
}
BOOL IsHaveOnAddress(DWORD Addv, std::vector<DWORD> ListAddv)
{
    for (int i = 0; i < ListAddv.size(); i++)
    {
        if (Addv == ListAddv[i])
            return true;
    }
    return false;
}
BOOL SavedPosition(DWORD Addv, VECTOR3 Position)
{
    bool IsFoundPos = false;
    for (int i = 0; i < LastPlayerPosition.size(); i++)
    {
        if (LastPlayerPosition[i].Addv == Addv)
        {
            LastPlayerPosition[i].Position = Position;
            IsFoundPos = true;
            return TRUE;
        }
    }
    if (!IsFoundPos)
    {
        LastPlayerPosition.push_back({Addv, Position});
        return TRUE;
    }
    return FALSE;
}
BOOL isInteger(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}
VECTOR3 GetSavedPosition(DWORD Addv)
{
    for (int i = 0; i < LastPlayerPosition.size(); i++)
    {
        if (LastPlayerPosition[i].Addv == Addv)
        {
            return LastPlayerPosition[i].Position;
        }
    }
    return {0, 0, 0};
}
bool EditLastPosition(DWORD Addv, VECTOR3 Position)
{
    bool bEdit = false;
    for (int i = 0; i < LastPlayerPosition.size(); i++)
    {
        if (LastPlayerPosition[i].Addv == Addv)
        {
            LastPlayerPosition[i].Position = Position;
            bEdit = true;
        }
    }
    if (bEdit == false)
    {
        LastPlayerPosition.push_back({Addv, Position});
        bEdit = true;
    }
    return bEdit;
}
int IsCheater(std::string UIDPlayer)
{
    for (int i = 0; i < CheaterList.size(); i++)
    {
        if (CheaterList[i] == UIDPlayer)
            return CheaterUser;
    }
    /*
    for (int i = 0; i < TeammateCheaterList.size( ); i++) {
        if (TeammateCheaterList[i] == UIDPlayer)
            return CheaterTeammate;
    }
    */
    return NormalUser;
}
int IsSpecialName(std::string UIDPlayer)
{
    for (int i = 0; i < SpecialNameList.size(); i++)
    {
        if (SpecialNameList[i].PlayerUID == UIDPlayer)
            return i;
    }
    return -1;
}
int GetVisibleValue(DWORD Addv)
{
    return 0;
}
int GetDocXuongValue(DWORD Addv)
{
    for (int i = 0; i < DocXuongList.size(); i++)
    {
        if (DocXuongList[i].address == Addv)
            return i;
    }
    return -1;
}
BOOL IsTeammateRequested(std::string UIDPlayer)
{
    for (int i = 0; i < TeammateList.size(); i++)
    {
        if (TeammateList[i] == UIDPlayer)
            return true;
    }
    return false;
}
BOOL Adb_Cmd(string CmdCode)
{
    if (!CmdCode.empty())
    {
        string CmdCode2 = "/C " + CmdCode;
        std::wstring stemp = std::wstring(CmdCode2.begin(), CmdCode2.end());
        LPCWSTR sw = stemp.c_str();
        filesystem::path directoryPath = current_path();
        string CurrentDirectory = directoryPath.generic_string();
        std::wstring stemp2 = std::wstring(CurrentDirectory.begin(), CurrentDirectory.end());
        LPCWSTR sw2 = stemp2.c_str();
        SHELLEXECUTEINFO info1 = {0};
        info1.cbSize = sizeof(SHELLEXECUTEINFO);
        info1.fMask = SEE_MASK_NOCLOSEPROCESS;
        info1.hwnd = NULL;
        info1.lpVerb = NULL;
        info1.lpFile = "cmd.exe";
        info1.lpParameters = CmdCode2.c_str();
        info1.lpDirectory = CurrentDirectory.c_str();
        info1.nShow = SW_HIDE;
        info1.hInstApp = NULL;
        ShellExecuteEx(&info1);
        WaitForSingleObject(info1.hProcess, INFINITE);
        return true;
    }
    else
    {
        return false;
    }
}
INT IsScoped(DWORD AddvCope, std::vector<ScopeWeapon> ScopeWeaponList)
{
    if (ScopeWeaponList.size() == 0)
        return -1;
    for (int i = 0; i < ScopeWeaponList.size(); i++)
    {
        if (AddvCope == ScopeWeaponList[i].Addv)
            return i;
    }
    return -1;
}
namespace Decryption
{
    struct ActorsEncryption
    {
        uint32_t Enc_1;
        uint32_t Enc_2;
        uint32_t Enc_3;
        uint32_t Enc_4;
    };
    struct Encryption_Chunk
    {
        uint32_t val_1, val_2, val_3, val_4;
    };
    uint32_t DecryptActorsArray(uint32_t uLevel, int Actors_Offset, int EncryptedActors_Offset)
    {
        if (uLevel < 0x10000000)
            return 0;

        if (Utility::ReadMemoryEx<uint32_t>(uLevel + Actors_Offset) > 0)
        {
            return uLevel + Actors_Offset;
        }

        if (Utility::ReadMemoryEx<uint32_t>(uLevel + EncryptedActors_Offset) > 0)
        {
            // std::cout << "Method 2" << std::endl;
            return uLevel + EncryptedActors_Offset;
        }

        auto Encryption = Utility::ReadMemoryEx<ActorsEncryption>(uLevel + EncryptedActors_Offset + 0x0C);

        if (Encryption.Enc_1 > 0)
        {
            // std::cout << "Method 3" << std::endl;
            uint32_t DecryptedActorsPointer = 0;
            int DecCount = 4;
            int index = 0;
            while (index < DecCount)
            {
                auto ival = Utility::ReadMemoryEx<uint32_t>(Encryption.Enc_1 + 0x80 + (0x4 * index));
                DecryptedActorsPointer += Utility::ReadMemoryEx<uint8_t>(Encryption.Enc_1 + ival) << (0x8 * index++);
            }
            return DecryptedActorsPointer;
        }
        else if (Encryption.Enc_2 > 0)
        {
            auto Encrypted_Actors = Utility::ReadMemoryEx<uint32_t>(Encryption.Enc_2);
            if (Encrypted_Actors > 0)
            {
                // std::cout << "Method 4" << std::endl;
                return ((unsigned short)Encrypted_Actors - 0x400) & 0xFF00 | (unsigned char)(Encrypted_Actors - 0x04) | (Encrypted_Actors + 0xFC0000) & 0xFF0000 | (Encrypted_Actors - 0x4000000) & 0xFF000000;
            }
        }
        else if (Encryption.Enc_3 > 0)
        {
            auto Encrypted_Actors = Utility::ReadMemoryEx<uint32_t>(Encryption.Enc_3);
            if (Encrypted_Actors > 0)
            {
                // std::cout << "Method 5" << std::endl;
                return (Encrypted_Actors >> 0x18) | (Encrypted_Actors << (32 - 0x18));
            }
        }
        else if (Encryption.Enc_4 > 0)
        {
            auto Encrypted_Actors = Utility::ReadMemoryEx<uint32_t>(Encryption.Enc_4);
            if (Encrypted_Actors > 0)
            {
                // std::cout << "Method 6" << std::endl;
                return Encrypted_Actors ^ 0xCDCD00;
            }
        }
        return 0;
    }
} // namespace Decryption
std::string GetPlayerName(DWORD BaseAddress)
{
    char *String = new char[34];
    for (int i = 0; i < 34; i++)
    {
        String[i] = Utility::ReadMemoryEx<char>(BaseAddress + i * 2);
    }
    string Name = String;
    delete[] String;
    return Name;
}
std::string GetString(DWORD BaseAddress)
{
    char *String = new char[32];
    for (int i = 0; i < 32; i++)
    {
        String[i] = Utility::ReadMemoryEx<char>(BaseAddress + i);
    }
    string Result = String;
    delete[] String;
    return Result;
}
std::string GetEntityType3(DWORD GNames, DWORD Id)
{
    std::string Result = "";
    if (Id >= 0)
    {
        DWORD GName = Utility::ReadMemoryEx<DWORD>(GNames);
        DWORD NamePtr = Utility::ReadMemoryEx<DWORD>(GName + (Id / 0x4000) * 0x4);
        DWORD Name = Utility::ReadMemoryEx<DWORD>(NamePtr + (Id % 0x4000) * 0x4);
        BYTE Buffer[40];
        ReadProcessMemory(Utility::GameHandle, (LPCVOID)(Name + 0x8), Buffer, sizeof(Buffer), nullptr);
        char Data[40];
        memcpy(Data, Buffer, 40);
        Data[39] = '\0';
        Result = Data;
    }
    return Result;
}
std::string GetBoxItem(int str)
{
    if (str == 101002 && Setting::PickupItems::Rifles::M16A4)
        return "M16A4";
    if (str == 101003 && Setting::PickupItems::Rifles::SCARL)
        return "SCARL";
    if (str == 101004 && Setting::PickupItems::Rifles::M416)
        return "M416";
    if (str == 101100 && Setting::PickupItems::Rifles::FAMAS)
        return "FAMAS";
    if (str == 101007 && Setting::PickupItems::Rifles::QBZ)
        return "QBZ";
    if (str == 101010 && Setting::PickupItems::Rifles::G63C)
        return "G63C";
    if (str == 101006 && Setting::PickupItems::Rifles::AUG)
        return "AUG";
    if (str == 101001 && Setting::PickupItems::Rifles::AKM)
        return "AKM";
    if (str == 101008 && Setting::PickupItems::Rifles::M762)
        return "M762";
    if (str == 101012 && Setting::PickupItems::Rifles::HoneyBadger)
        return "HoneyBadger";
    if (str == 101005 && Setting::PickupItems::Rifles::GROZA)
        return "GROZA";
    if (str == 103007 && Setting::PickupItems::Rifles::MK14)
        return "MK14";
    if (str == 102004 && Setting::PickupItems::SMG::THOMPSON)
        return "THOMPSON";
    if (str == 102002 && Setting::PickupItems::SMG::UMP45)
        return "UMP45";
    if (str == 102105 && Setting::PickupItems::SMG::P90)
        return "P90";
    if (str == 102001 && Setting::PickupItems::SMG::UZI)
        return "UZI";
    if (str == 102005 && Setting::PickupItems::SMG::BIZON)
        return "BIZON";
    if (str == 102007 && Setting::PickupItems::SMG::MP5K)
        return "MP5K";
    if (str == 102003 && Setting::PickupItems::SMG::VECTOR)
        return "VECTOR";
    if (str == 103008 && Setting::PickupItems::Sniper::WIN94)
        return "WIN94";
    if (str == 103005 && Setting::PickupItems::Sniper::VSS)
        return "VSS";
    if (str == 103006 && Setting::PickupItems::Sniper::MINI14)
        return "MINI14";
    if (str == 103010 && Setting::PickupItems::Sniper::QBU)
        return "QBU";
    if (str == 103100 && Setting::PickupItems::Sniper::MK12)
        return "MK12";
    if (str == 101009 && Setting::PickupItems::Sniper::MK47)
        return "MK47";
    if (str == 103009 && Setting::PickupItems::Sniper::SLR)
        return "SLR";
    if (str == 103004 && Setting::PickupItems::Sniper::SKS)
        return "SKS";
    if (str == 103001 && Setting::PickupItems::Sniper::KAR98)
        return "KAR98";
    if (str == 103011 && Setting::PickupItems::Sniper::MOSIN)
        return "MOSIN";
    if (str == 107001 && Setting::PickupItems::Sniper::BOW)
        return "BOW";
    if (str == 103003 && Setting::PickupItems::Sniper::AWM)
        return "AWM";
    if (str == 103012 && Setting::PickupItems::Sniper::AMR)
        return "AMR";
    if (str == 105001 && Setting::PickupItems::AutoMachine::M249)
        return "M249";
    if (str == 105010 && Setting::PickupItems::AutoMachine::MG3)
        return "MG3";
    if (str == 105002 && Setting::PickupItems::AutoMachine::DP28)
        return "DP28";
    if (str == 104001 && Setting::PickupItems::ShotGun::S686)
        return "S686";
    if (str == 104002 && Setting::PickupItems::ShotGun::S1897)
        return "S1897";
    if (str == 104101 && Setting::PickupItems::ShotGun::M1014)
        return "M1014";
    if (str == 104102 && Setting::PickupItems::ShotGun::Neostead2000)
        return "Neostead2000";
    if (str == 104003 && Setting::PickupItems::ShotGun::S12K)
        return "S12K";
    if (str == 104004 && Setting::PickupItems::ShotGun::DBS)
        return "DBS";
    if (str == 106002 && Setting::PickupItems::Pistols::P1911)
        return "P1911";
    if (str == 106005 && Setting::PickupItems::Pistols::R45)
        return "R45";
    if (str == 106010 && Setting::PickupItems::Pistols::DesertEagle)
        return "DesertEagle";
    if (str == 106001 && Setting::PickupItems::Pistols::P92)
        return "P92";
    if (str == 106004 && Setting::PickupItems::Pistols::P18C)
        return "P18C";
    if (str == 106008 && Setting::PickupItems::Pistols::Vz61)
        return "Vz61";
    if (str == 106003 && Setting::PickupItems::Pistols::R1895)
        return "R1895";
    if (str == 106007 && Setting::PickupItems::Pistols::Flaregun)
        return "Flaregun";
    if (str == 601002 && Setting::PickupItems::NeccessaryHealth::Adrenaline)
        return "Adrenaline";
    if (str == 601001 && Setting::PickupItems::NeccessaryHealth::EnergyDrink)
        return "EnergyDrink";
    if (str == 601003 && Setting::PickupItems::NeccessaryHealth::Painkiller)
        return "Painkiller";
    if (str == 601005 && Setting::PickupItems::NeccessaryHealth::FirstaidKit)
        return "FirstaidKit";
    if (str == 601006 && Setting::PickupItems::NeccessaryHealth::Medkit)
        return "Medkit";
    if (str == 108004 && Setting::PickupItems::Melee::Pan)
        return "Pan";
    if (str == 501004 && Setting::PickupItems::Bag::BagLv1)
        return "BagLv1";
    if (str == 501005 && Setting::PickupItems::Bag::BagLv2)
        return "BagLv2";
    if (str == 501006 && Setting::PickupItems::Bag::BagLv3)
        return "BagLv3";
    if (str == 503001 && Setting::PickupItems::Armor::ArmorLv1)
        return "ArmorLv1";
    if (str == 503002 && Setting::PickupItems::Armor::ArmorLv2)
        return "ArmorLv2";
    if (str == 503003 && Setting::PickupItems::Armor::ArmorLv3)
        return "ArmorLv3";
    if (str == 502001 && Setting::PickupItems::Helmet::HelmetLv1)
        return "HelmetLv1";
    if (str == 502002 && Setting::PickupItems::Helmet::HelmetLv2)
        return "HelmetLv2";
    if (str == 502003 && Setting::PickupItems::Helmet::HelmetLv3)
        return "HelmetLv3";
    if (str == 204011 && Setting::PickupItems::Accessory::AREx)
        return "AREx";
    if (str == 204013 && Setting::PickupItems::Accessory::ARExQk)
        return "ARExQk";
    if (str == 201009 && Setting::PickupItems::Accessory::LargeCompensator)
        return "LargeCompensator";
    if (str == 201011 && Setting::PickupItems::Accessory::LargeSuppressor)
        return "LargeSuppressor";
    if (str == 201003 && Setting::PickupItems::Accessory::SniperCompensator)
        return "SniperCompensator";
    if (str == 201007 && Setting::PickupItems::Accessory::SniperSuppressor)
        return "SniperSuppressor";
    if (str == 202006 && Setting::PickupItems::Accessory::ThumbGrip)
        return "ThumbGrip";
    if (str == 202011 && Setting::PickupItems::Accessory::Angled)
        return "Angled";
    if (str == 202004 && Setting::PickupItems::Accessory::LightGrip)
        return "LightGrip";
    if (str == 202005 && Setting::PickupItems::Accessory::HalfGrip)
        return "HalfGrip";
    if (str == 202002 && Setting::PickupItems::Accessory::Vertical)
        return "Vertical";
    if (str == 203001 && Setting::PickupItems::Accessory::Reddot)
        return "Reddot";
    if (str == 203002 && Setting::PickupItems::Accessory::Holo)
        return "Holo";
    if (str == 203003 && Setting::PickupItems::Accessory::Scope2X)
        return "Scope2X";
    if (str == 203014 && Setting::PickupItems::Accessory::Scope3X)
        return "Scope3X";
    if (str == 203004 && Setting::PickupItems::Accessory::Scope4X)
        return "Scope4X";
    if (str == 203015 && Setting::PickupItems::Accessory::Scope6X)
        return "Scope6X";
    if (str == 203005 && Setting::PickupItems::Accessory::Scope8X)
        return "Scope8X";
    if (str == 205002 && Setting::PickupItems::Accessory::M416Stock)
        return "M416Stock";
    if (str == 302001 && Setting::PickupItems::Ammo::Ammo762)
        return "7.62mm";
    if (str == 305001 && Setting::PickupItems::Ammo::Ammo45ACP)
        return "45ACP";
    if (str == 303001 && Setting::PickupItems::Ammo::Ammo556)
        return "5.56mm";
    if (str == 301001 && Setting::PickupItems::Ammo::Ammo9)
        return "9mm";
    if (str == 306001 && Setting::PickupItems::Ammo::Ammo300Magnum)
        return "300Magnum";
    if (str == 304001 && Setting::PickupItems::Ammo::Ammo12Guage)
        return "12Guage";
    if (str == 308001 && Setting::PickupItems::Ammo::AmmoFlareGun)
        return "FlareGun_Aimmo";
    if (str == 602004 && Setting::PickupItems::Others::Grenade)
        return "Grenade";
    if (str == 602002 && Setting::PickupItems::Others::Smoke)
        return "Smoke";
    if (str == 602003 && Setting::PickupItems::Others::Molotof)
        return "Molotof";
    if (str == 602005 && Setting::PickupItems::Others::Apple)
        return "Apple";
    if (str == 603001 && Setting::PickupItems::Others::Gascan)
        return "Gascan";
    if (str == 3000324 && Setting::PickupItems::Others::TokenShop)
        return "TokenShop";
    if (str == 3000321 && Setting::PickupItems::Others::SecurityCard)
        return "SecurityCard";
    return "";
}
std::string StringToUtf8(const std::string &str)
{
    INT size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(), str.length(), NULL, 0);
    std::wstring utf16_str(size, '\0');
    MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(), str.length(), &utf16_str[0], size);
    INT utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_str.length(), NULL, 0, NULL, NULL);
    std::string utf8_str(utf8_size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_str.length(), &utf8_str[0], utf8_size, NULL, NULL);
    return utf8_str;
}
std::string IsItem(std::string str)
{
    if (str == "BP_Rifle_M16A4_Wrapper_C")
        return "M16A4";
    if (str == "BP_Rifle_SCAR_Wrapper_C")
        return "SCARL";
    if (str == "BP_Rifle_M416_Wrapper_C")
        return "M416";
    if (str == "BP_Rifle_FAMAS_Wrapper_C")
        return "FAMAS";
    if (str == "BP_Rifle_QBZ_Wrapper_C")
        return "QBZ";
    if (str == "BP_Rifle_G36_Wrapper_C")
        return "G63C";
    if (str == "BP_Rifle_AUG_Wrapper_C")
        return "AUG";
    if (str == "BP_Rifle_AKM_Wrapper_C")
        return "AKM";
    if (str == "BP_Rifle_M762_Wrapper_C")
        return "M762";
    if (str == "BP_Rifle_HoneyBadger_Wrapper_C")
        return "HoneyBadger";
    if (str == "BP_Rifle_Groza_Wrapper_C")
        return "GROZA";
    if (str == "BP_WEP_Mk14_Pickup_C")
        return "MK14";
    if (str == "BP_MachineGun_TommyGun_Wrapper_C")
        return "THOMPSON";
    if (str == "BP_MachineGun_UMP9_Wrapper_C")
        return "UMP45";
    if (str == "BP_MachineGun_P90_Wrapper_C")
        return "P90";
    if (str == "BP_MachineGun_Uzi_Wrapper_C")
        return "UZI";
    if (str == "BP_MachineGun_PP19_Wrapper_C")
        return "BIZON";
    if (str == "BP_MachineGun_MP5K_Wrapper_C")
        return "MP5K";
    if (str == "BP_MachineGun_Vector_Wrapper_C")
        return "VECTOR";
    if (str == "BP_Sniper_Win94_Wrapper_C")
        return "WIN94";
    if (str == "BP_Sniper_VSS_Wrapper_C")
        return "VSS";
    if (str == "BP_Sniper_Mini14_Wrapper_C")
        return "MINI14";
    if (str == "BP_Sniper_QBU_Wrapper_C")
        return "QBU";
    if (str == "BP_Sniper_MK12_Wrapper_C")
        return "MK12";
    if (str == "BP_Rifle_Mk47_Wrapper_C")
        return "MK47";
    if (str == "BP_Sniper_SLR_Wrapper_C")
        return "SLR";
    if (str == "BP_Sniper_SKS_Wrapper_C")
        return "SKS";
    if (str == "BP_Sniper_Kar98k_Wrapper_C")
        return "KAR98";
    if (str == "BP_Sniper_Mosin_Wrapper_C")
        return "MOSIN";
    if (str == "BP_Other_CrossBow_Wrapper_C")
        return "BOW";
    if (str == "BP_Sniper_AWM_Wrapper_C")
        return "AWM";
    if (str == "BP_Sniper_AMR_Wrapper")
        return "AMR";
    if (str == "BP_Other_M249_Wrapper_C")
        return "M249";
    if (str == "BP_Other_MG3_Wrapper_C")
        return "MG3";
    if (str == "BP_Other_DP28_Wrapper_C")
        return "DP28";
    if (str == "BP_ShotGun_S686_Wrapper_C")
        return "S686";
    if (str == "BP_ShotGun_S1897_Wrapper_C")
        return "S1897";
    if (str == "BP_ShotGun_M1014_Wrapper_C")
        return "M1014";
    if (str == "BP_ShotGun_Neostead2000_Wrapper_C")
        return "Neostead2000";
    if (str == "BP_ShotGun_S12K_Wrapper_C")
        return "S12K";
    if (str == "BP_ShotGun_DP12_Wrapper_C")
        return "DBS";
    if (str == "BP_Pistol_P1911_Wrapper_C")
        return "P1911";
    if (str == "BP_Pistol_R45_Wrapper_C")
        return "R45";
    if (str == "BP_Pistol_DesertEagle_Wrapper_C")
        return "DesertEagle";
    if (str == "BP_Pistol_P92_Wrapper_C")
        return "P92";
    if (str == "BP_Pistol_P18C_Wrapper_C")
        return "P18C";
    if (str == "BP_Pistol_Vz61_Wrapper_C")
        return "Vz61";
    if (str == "BP_Pistol_R1895_Wrapper_C")
        return "R1895";
    if (str == "BP_Pistol_Flaregun_Wrapper_C")
        return "Flaregun";
    if (str == "Injection_Pickup_C")
        return "Adrenaline";
    if (str == "Drink_Pickup_C")
        return "EnergyDrink";
    if (str == "Pills_Pickup_C")
        return "Painkiller";
    if (str == "Firstaid_Pickup_C")
        return "FirstaidKit";
    if (str == "FirstAidbox_Pickup_C")
        return "Medkit";
    if (str == "BP_WEP_Pan_Pickup_C")
        return "Pan";
    if (str == "PickUp_BP_Bag_Lv1_C")
        return "BagLv1";
    if (str == "PickUp_BP_Bag_Lv2_C")
        return "BagLv2";
    if (str == "PickUp_BP_Bag_Lv3_C")
        return "BagLv3";
    if (str == "PickUp_BP_Bag_Lv1_B_C")
        return "BagLv1";
    if (str == "PickUp_BP_Bag_Lv2_B_C")
        return "BagLv2";
    if (str == "PickUp_BP_Bag_Lv3_B_C")
        return "BagLv3";
    if (str == "PickUp_BP_Armor_Lv1_C")
        return "ArmorLv1";
    if (str == "PickUp_BP_Armor_Lv2_C")
        return "ArmorLv2";
    if (str == "PickUp_BP_Armor_Lv3_C")
        return "ArmorLv3";
    if (str == "PickUp_BP_Armor_Lv1_B_C")
        return "ArmorLv1";
    if (str == "PickUp_BP_Armor_Lv2_B_C")
        return "ArmorLv2";
    if (str == "PickUp_BP_Armor_Lv3_B_C")
        return "ArmorLv3";
    if (str == "PickUp_BP_Helmet_Lv1_C")
        return "HelmetLv1";
    if (str == "PickUp_BP_Helmet_Lv2_C")
        return "HelmetLv2";
    if (str == "PickUp_BP_Helmet_Lv3_C")
        return "HelmetLv3";
    if (str == "PickUp_BP_Helmet_Lv1_B_C")
        return "HelmetLv1";
    if (str == "PickUp_BP_Helmet_Lv2_B_C")
        return "HelmetLv2";
    if (str == "PickUp_BP_Helmet_Lv3_B_C")
        return "HelmetLv3";
    if (str == "BP_DJ_Large_E_Pickup_C")
        return "AREx";
    if (str == "BP_DJ_Large_EQ_Pickup_C")
        return "ARExQk";
    if (str == "BP_QK_Large_Compensator_Pickup_C")
        return "LargeCompensator";
    if (str == "BP_QK_Large_Suppressor_Pickup_C")
        return "LargeSuppressor";
    if (str == "BP_QK_Sniper_Compensator_Pickup_C")
        return "SniperCompensator";
    if (str == "BP_QK_Sniper_Suppressor_Pickup_C")
        return "SniperSuppressor";
    if (str == "BP_WB_ThumbGrip_Pickup_C")
        return "ThumbGrip";
    if (str == "BP_WB_Angled_Pickup_C")
        return "Angled";
    if (str == "BP_WB_LightGrip_Pickup_C")
        return "LightGrip";
    if (str == "BP_WB_HalfGrip_Pickup_C")
        return "HalfGrip";
    if (str == "BP_WB_Vertical_Pickup_C")
        return "Vertical";
    if (str == "BP_MZJ_HD_Pickup_C")
        return "Reddot";
    if (str == "BP_MZJ_QX_Pickup_C")
        return "Holo";
    if (str == "BP_MZJ_2X_Pickup_C")
        return "Scope2X";
    if (str == "BP_MZJ_3X_Pickup_C")
        return "Scope3X";
    if (str == "BP_MZJ_4X_Pickup_C")
        return "Scope4X";
    if (str == "BP_MZJ_6X_Pickup_C")
        return "Scope6X";
    if (str == "BP_MZJ_8X_Pickup_C")
        return "Scope8X";
    if (str == "BP_QT_A_Pickup_C")
        return "M416Stock";
    if (str == "BP_Ammo_762mm_Pickup_C")
        return "7.62mm";
    if (str == "BP_Ammo_45ACP_Pickup_C")
        return "45ACP";
    if (str == "BP_Ammo_556mm_Pickup_C")
        return "5.56mm";
    if (str == "BP_Ammo_9mm_Pickup_C")
        return "9mm";
    if (str == "BP_Ammo_300Magnum_Pickup_C")
        return "300Magnum";
    if (str == "BP_Ammo_12Guage_Pickup_C")
        return "12Guage";
    if (str == "BP_Ammo_Flare_Pickup_C")
        return "FlareGun_Aimmo";
    if (str == "BP_AirDropBox_New_C")
        return "AirDrop";
    if (str == "BP_FootBallAirDropBox_C")
        return "AirDrop";
    if (str == "PickUpListWrapperActor")
        return "DeathBox";
    if (str == "BP_Grenade_Shoulei_Weapon_Wrapper_C")
        return "Grenade";
    if (str == "BP_Grenade_Smoke_Weapon_Wrapper_C")
        return "Smoke";
    if (str == "BP_Grenade_Burn_Weapon_Wrapper_C")
        return "Molotof";
    if (str == "BP_Grenade_Apple_Weapon_Wrapper_C")
        return "Apple";
    if (str == "GasCan_Destructible_Pickup_C")
        return "Gascan";
    if (str == "GoldenTokenWrapper_C")
        return "TokenShop";
    if (str == "SecurityCardWrapper_C")
        return "SecurityCard";
    return "";
}
std::string GetVehicleType(std::string type)
{
    if (type == ("BP_VH_Buggy_2_C") || type == ("BP_VH_Buggy_C") || type == ("BP_VH_Buggy_3_C") || type == ("BP_VH_Buggy_4_C") || type == ("BP_VH_Buggy_5_C") || type == ("BP_VH_Buggy_6_C") || type == ("BP_VH_Buggy_SI_C") // cheer park
        || type == ("ArmedBuggyFloat_low_C"))
        return "Buggy";
    else if (type == ("VH_Dacia_2_C") || type == ("VH_Dacia_C") || type == ("VH_Dacia_3_C") || type == ("VH_Dacia_4_C") || type == ("VH_Dacia_SI_C") || type == ("VH_Dacia_C") // cheer park - training grounds
             || type == ("ArmedDacia_low_C"))
        return "Dacia";
    else if (type == ("VH_UAZ02_C") || type == ("VH_UAZ01_C") || type == ("VH_UAZ03_C") || type == ("ArmedUAZ_C"))
        return "UAZ";
    else if (type == ("Mirado_open_2_C") || type == ("Mirado_open_1_C") || type == ("Mirado_open_3_C") || type == ("Mirado_open_4_C") || type == ("Mirado_open_Golden_C") || type == ("Mirado_close_1_C") || type == ("Mirado_close_2_C") || type == ("Mirado_close_3_C") || type == ("VH_UAZ01_SI_C") || type == ("VH_UAZ01_C") // cheer park - training grounds
             || type == ("Mirado_open_1_SI_C")                                                                                                                                                                                                                                                                                   // cheer park
             || type == ("Mirado_close_4_C"))
        return "Mirado";
    else if (type == ("VH_Scooter2_C") || type == ("VH_Scooter1_C")

             || type == ("VH_Scooter3_C"))
        return "Scooter";
    else if (type == ("VH_MotorcycleCart_C") || type == ("VH_Scooter_C") || type == ("VH_MotorcycleCart_SI_C") // training grounds
             || type == ("VH_Motorcycle_SI_C") || type == ("VH_MotorcycleCart_1_C"))
        return "Motorcycle Cart";
    else if (type == ("Rony_2_C") || type == ("Rony_1_C") || type == ("Rony_3_C"))
        return "Rony";
    else if (type == ("LadaNiva_02_C") || type == ("LadaNiva_01_C"))
        return "Lada Niva";
    else if (type == ("PickUp_02_C") || type == ("PickUp_01_C") || type == ("PickUp_03_C") || type == ("PickUp_04_C") || type == ("PickUp_05_C") || type == ("PickUp_06_C") || type == ("PickUp_07_C") || type == ("PickUp_08_C") || type == ("PickUp_09_C") || type == ("PickUp_10_C") || type == ("ArmedPickupFloat_low_C"))
        return "Pickup";
    else if (type == ("VH_MiniBus_02_C") || type == ("VH_MiniBus_01_C") || type == ("VH_MiniBus_03_C"))
        return "Mini Bus";
    else if (type == ("VH_Motorcycle_1_C") || type == ("VH_Motorcycle_C"))
        return "Motorcycle";
    else if (type == ("VH_Motorglider_SI_C") || type == ("VH_Motorglider_C"))
        return "Motor Glider";
    else if (type == ("BP_VH_Bigfoot_C"))
        return "Big Foot";
    else if (type == ("VH_CoupeRB_1_SI_C") || type == ("VH_CoupeRB_1_C"))
        return "Coupe RB";
    else if (type == ("VH_Snowbile_C"))
        return "Snowmobile";
    else if (type == ("BP_VH_Tuk_1_SI_C") || type == ("BP_VH_Tuk_1_C"))
        return "Tuk";
    else if (type == ("VH_BRDM_C"))
        return "BRDM";
    else if (type == ("VH_Snowbike_C") || type == ("VH_Snowmobile_C") || type == ("VH_Snowbike_C"))
        return "Snowbike";
    else if (type == ("AquaRail_1_C"))
        return "Aqua Rail";
    else if (type == ("water_Plane"))
        return "Water Plane";
    else if (type == ("VH_UTV_C"))
        return "UTV";
    else if (type == ("VH_PG117_SI_C") || type == ("VH_PG117_C"))
        return "PG117";
    else if (type == ("UH60_Vehicle_C"))
        return "UH-60";
    else if (type == ("AH6_Vehicle_C"))
        return "AH-6";
    else if (type == ("BP_Bike_WithRack_SI_C") || type == ("BP_Bike_Pickable_C"))
        return "Bicycle";
    else if (type.find("BP_RaptorVehicle_C") != std::string::npos)
        return "Raptor";
    else if (type.find("Pterosaur") != std::string::npos)
        return "Pterosaur";
    else if (type.find("TyrannosaurusRex") != std::string::npos)
        return "TyrannosaurusRex";
    else if (type.find("BP_TRexVehicle_C") != std::string::npos)
        return "TyrannosaurusRex";
    else if (type == "Rony_01_C")
        return "Rony";
    else if (type == "BP_CoupeRB_Base_C")
        return "CoupeRB";
    else if (type == "VH_ATV1_C")
        return "ATV1";
    else if (type == "VH_Scooter_C")
        return "Scooter";
    else
        return "";
    return "";
}
std::string IsAlerts(std::string str)
{
    if (str == "BP_Projectile_FragGrenade_C")
        return "Grenade";
    if (str == "BP_Projectile_BurnGrenade_C")
        return "Molotuf";
    if (str == "AirAttackBomb_C")
        return "Redzone";
    if (str == "AirAttackBomb_Livik_C")
        return "Redzone";
    return "";
}
std::string execute(std::string cmd)
{
    std::string file_name = "C:\\adb_log_server.txt";
    std::system((cmd + " > " + file_name).c_str());
    std::ifstream file(file_name);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
int GetValidBombPosition(DWORD AddvBomb)
{
    if (Data::BombPositionList.size() > 0)
    {
        for (int i = 0; i < Data::BombPositionList.size(); i++)
        {
            if (AddvBomb == Data::BombPositionList.at(i).Addv)
                return i;
        }
    }
    return -1;
}
std::string ScopeFOV(int fov)
{
    if (fov == 70)
        return "";
    else if (fov == 55)
        return "1x";
    else if (fov == 44)
        return "2x";
    else if (fov == 26)
        return "3x";
    else if (fov == 20)
        return "4x";
    else if (fov == 13)
        return "6x";
    else if (fov == 11)
        return "8x";
    else
        return "";
    return "";
}
string cmd(string textt)
{
    std::future<BOOL> fut = std::async(Adb_Cmd, textt);
    fut.wait();
}
DWORD CreatedTime(DWORD AddvBomb)
{
    for (int i = 0; i <= Data::GrenadeTimeList.size(); i++)
    {
        if (Data::GrenadeTimeList[i].Addv == AddvBomb)
            return Data::GrenadeTimeList[i].CreatedTime;
    }
    return GetTickCount();
}
FRotator1 ToRotator(VECTOR3 local, VECTOR3 target)
{
    VECTOR3 rotation;
    rotation.X = local.X - target.X;
    rotation.Y = local.Y - target.Y;
    rotation.Z = local.Z - target.Z;
    FRotator1 newViewAngle;
    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);
    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Roll = (float)0.f;
    if (rotation.X >= 0.f)
        newViewAngle.Yaw += 180.0f;
    return newViewAngle;
}
void ClampAngles(FRotator1 &angles)
{
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}
DWORD FindReplace(DWORD Addv, vector<TempEsp> AddvList)
{
    for (int i = 0; i < AddvList.size(); i++)
    {
        if (Addv == AddvList[i].Addv)
            return Addv;
    }
    return -1;
}
DWORD GetAddres(BYTE BypaRep[], SIZE_T size)
{
    std::vector<DWORD_PTR> FoundBase;
    Utility::MemSearch(BypaRep, size, 0x10000000, 0xD0000000, false, 0, FoundBase);
    return FoundBase[0];
}
DWORD GetMagicBullet()
{
    DWORD MagicHeader;
    BYTE tersafehead[] = {0xB8, 0x41, 0x00, 0x00, 0xC8, 0x41, 0x00, 0x00, 0xF4, 0x41};
    MagicHeader = GetAddres(tersafehead, sizeof(tersafehead));
    return MagicHeader;
}
VOID PatchAddress(DWORD_PTR addr, BYTE write[], SIZE_T size)
{
    unsigned long OldProtect;
    unsigned long OldProtect2;
    VirtualProtectEx(Utility::GameHandle, (BYTE *)addr, size, PAGE_EXECUTE_READWRITE, &OldProtect);
    WriteProcessMemory(Utility::GameHandle, (BYTE *)addr, write, size, NULL);
    VirtualProtectEx(Utility::GameHandle, (BYTE *)addr, size, OldProtect, &OldProtect2);
}
VOID ScanAndReplace(BYTE ScanByte[], BYTE Replace[])
{
    std::vector<DWORD_PTR> AddvList;
    Utility::MemSearch(ScanByte, sizeof(ScanByte), 0x56E30000, 0xB63BC000, false, 0, AddvList);
    if (AddvList.size() > 0)
    {
        DWORD_PTR Addv = AddvList[0];
        PatchAddress(Addv, Replace, sizeof(Replace));
        AddvList.clear();
    }
}
VOID TopGodView()
{
    DWORD root = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::RootComponent) + 0x158;
    FLOAT ground;
    ReadProcessMemory(Utility::GameHandle, (LPCVOID)root, &ground, sizeof(ground), nullptr);
    while (g_Running)
    {
        Utility::WriteMemoryEx<FLOAT>(root, ground + 281);
        if (!GetAsyncKeyState(Setting::Memory::GodviewTopKey))
        {
            break;
        }
        Sleep(1);
    }
}
std::string DecColor::Red = "16744576";
std::string DecColor::Green = "32768";
std::string DecColor::Aqua = "65535";
std::string DecColor::White = "16777215";
LinearColor RandomColor()
{

    static float x = 0, y = 0;
    static float r = 0, g = 0, b = 0;
    if (y >= 0.0f && y < 255.0f)
    {
        r = 255.0f;
        g = 0.0f;
        b = x;
    }
    else if (y >= 255.0f && y < 510.0f)
    {
        r = 255.0f - x;
        g = 0.0f;
        b = 255.0f;
    }
    else if (y >= 510.0f && y < 765.0f)
    {
        r = 0.0f;
        g = x;
        b = 255.0f;
    }
    else if (y >= 765.0f && y < 1020.0f)
    {
        r = 0.0f;
        g = 255.0f;
        b = 255.0f - x;
    }
    else if (y >= 1020.0f && y < 1275.0f)
    {
        r = x;
        g = 255.0f;
        b = 0.0f;
    }
    else if (y >= 1275.0f && y < 1530.0f)
    {
        r = 255.0f;
        g = 255.0f - x;
        b = 0.0f;
    }
    x += 1;
    if (x >= 255.0f)
        x = 0.0f;
    y += 1;
    if (y > 1530.0f)
        y = 0.0f;
    float maxComponent = 255.0f * 255.0f;
    r = r / maxComponent * 255.0f;
    g = g / maxComponent * 255.0f;
    b = b / maxComponent * 255.0f;
    return {(float)r, (float)g, (float)b, 255};
}
LinearColor RandomColor2()
{

    static float x = 0, y = 0;
    static float r = 0, g = 0, b = 0;
    if (y >= 0.0f && y < 255.0f)
    {
        r = 255.0f;
        g = 0.0f;
        b = x;
    }
    else if (y >= 255.0f && y < 510.0f)
    {
        r = 255.0f - x;
        g = 0.0f;
        b = 255.0f;
    }
    else if (y >= 510.0f && y < 765.0f)
    {
        r = 0.0f;
        g = x;
        b = 255.0f;
    }
    else if (y >= 765.0f && y < 1020.0f)
    {
        r = 0.0f;
        g = 255.0f;
        b = 255.0f - x;
    }
    else if (y >= 1020.0f && y < 1275.0f)
    {
        r = x;
        g = 255.0f;
        b = 0.0f;
    }
    else if (y >= 1275.0f && y < 1530.0f)
    {
        r = 255.0f;
        g = 255.0f - x;
        b = 0.0f;
    }
    x += 2;
    if (x >= 255.0f)
        x = 0.0f;
    y += 2;
    if (y > 1530.0f)
        y = 0.0f;
    float maxComponent = 255.0f * 255.0f;
    r = r / maxComponent * 255.0f;
    g = g / maxComponent * 255.0f;
    b = b / maxComponent * 255.0f;
    return {(float)r, (float)g, (float)b, 255};
}
LinearColor ToLinerColor(float Input[4])
{
    LinearColor Ouput;
    Ouput.R = Input[0];
    Ouput.G = Input[1];
    Ouput.B = Input[2];
    Ouput.A = Input[3];
    return Ouput;
}

VOID SendJump()
{
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = Setting::Memory::JumpKey;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT)); // Send K Press
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT)); // Send K Release
}
VOID SendProne()
{
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = Setting::Memory::ProneKey;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT)); // Send L Press
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT)); // Send L Release
}
VOID Data::MemoryAttach()
{
  
}
VOID UpdateOnlyAActor()
{
    DWORD EntityList = 0;
    DWORD EntityCount = 0;
    DWORD EntityAddv = 0;
    DWORD EntityStruct = 0;
    DWORD RootComponent = 0;
    DWORD MeshAddv = 0;
    DWORD BodyAddv = 0;
    DWORD BoneAddv = 0;
    std::string EntityType;
    std::string PlayerUID;
    std::vector<AActor> AActorList;
    while(g_Running)
    {
        if (Data::NetDriver > 0)
        {
            if (Cheat::ModuleLoaded == true)
            {
                if (Data::AActorList.size() > 0)
                {
                    if (Data::pGameData.bDraw == false)
                        continue;
                    for (int i = 0; i < Data::AActorList.size(); i++)
                    {
                        if (Data::AActorList[i].IsUpdatedBone == true)
                            continue;
                        docxuong docxuongg;
                        docxuongg = DocXuongList[GetDocXuongValue(Data::AActorList[i].Address)];
                        Data::AActorList[i].HeadPos = docxuongg.HeadPos;
                        Data::AActorList[i].ChestPos = docxuongg.ChestPos;
                        Data::AActorList[i].PelvisPos = docxuongg.PelvisPos;
                        Data::AActorList[i].lSholderPos = docxuongg.lSholderPos;
                        Data::AActorList[i].rSholderPos = docxuongg.rSholderPos;
                        Data::AActorList[i].lElbowPos = docxuongg.lElbowPos;
                        Data::AActorList[i].rElbowPos = docxuongg.rElbowPos;
                        Data::AActorList[i].lWristPos = docxuongg.lWristPos;
                        Data::AActorList[i].rWristPos = docxuongg.rWristPos;
                        Data::AActorList[i].lThighPos = docxuongg.lThighPos;
                        Data::AActorList[i].rThighPos = docxuongg.rThighPos;
                        Data::AActorList[i].lKneePos = docxuongg.lKneePos;
                        Data::AActorList[i].rKneePos = docxuongg.rKneePos;
                        Data::AActorList[i].lAnklePos = docxuongg.lAnklePos;
                        Data::AActorList[i].rAnklePos = docxuongg.rAnklePos;
                        Data::AActorList[i].IsUpdatedBone = true;
                    }
                }
            }
        }
        Sleep(1);
    }
}
void RunWait(string Path)
{
    SHELLEXECUTEINFO info1 = {0};
    info1.cbSize = sizeof(SHELLEXECUTEINFO);
    info1.fMask = SEE_MASK_NOCLOSEPROCESS;
    info1.hwnd = NULL;
    info1.lpVerb = NULL;
    info1.lpFile = Path.c_str();
    info1.lpParameters = "";
    info1.nShow = SW_HIDE;
    info1.hInstApp = NULL;
    ShellExecuteEx(&info1);
    WaitForSingleObject(info1.hProcess, INFINITE);
}
VOID Data::UpdateCache()
{
    std::string EntityType;
    DWORD EntityList = 0;
    INT EntityCount = 0;
    DWORD EntityAddv = 0;
    DWORD EntityStruct = 0;
    DWORD AActorCountVisible = 0;
    DWORD VisibleCheckPointer = 0;
    DWORD PersistentLevel = 0;
    DWORD ServerConnection = 0;
    DWORD PlayerController;
    BOOL IsCleared{false};
    uWorld = Utility::ReadMemoryEx<DWORD>(Data::LibUE4Addv + Offset::GWorld) + 0x3C;
    gNames = Utility::ReadMemoryEx<DWORD>(Data::LibUE4Addv + Offset::GName) + 0x88;
    auto LastedTime = high_resolution_clock::now();

    auto HackBase = 0;
    while(g_Running)
    {
        if (Data::LibUE4Addv)
        {
            Utility::WriteMemoryEx<int>(Data::LibUE4Addv + 0x59A0C48, 506149161);
            Utility::WriteMemoryEx<int>(Data::LibUE4Addv + 0x9DA452C, 506149161);

        }
        uWorld = Utility::ReadMemoryEx<DWORD>(Data::LibUE4Addv + Offset::GWorld) + 0x3C;
        gNames = Utility::ReadMemoryEx<DWORD>(Data::LibUE4Addv + Offset::GName) + 0x88;
        UINT uWorlds = Utility::ReadMemoryEx<DWORD>(uWorld);
        Data::NetDriver = Utility::ReadMemoryEx<DWORD>(uWorlds + Offset::NetDriver);
        if (Data::NetDriver > 0)
        {
            
            if (IsCleared == true)
            {
                IsCleared = false;
            }
            PersistentLevel = Utility::ReadMemoryEx<DWORD>(uWorlds + Offset::PersistentLevel);
            ServerConnection = Utility::ReadMemoryEx<DWORD>(Data::NetDriver + Offset::ServerConnection);
            PlayerController = Utility::ReadMemoryEx<DWORD>(ServerConnection + Offset::PlayerController);
            Data::PlayerKey = Utility::ReadMemoryEx<DWORD>(PlayerController + Offset::PlayerKeyController);
            auto GameStateBase = Utility::ReadMemoryEx<DWORD>(uWorlds + Offset::GameState);
            Data::GameID = Utility::ReadMemoryEx<uint32_t>(GameStateBase + Offset::GameID);
            if (Setting::SpecialFeatures::GameInformation)
            {
                Data::AlivePlayer = Utility::ReadMemoryEx<INT>(GameStateBase + Offset::AlivePlayerNum);
                Data::AliveTeam = Utility::ReadMemoryEx<INT>(GameStateBase + Offset::AliveTeamNum);
                Data::PlayerNum = Utility::ReadMemoryEx<INT>(GameStateBase + Offset::PlayerNum);
                Data::ElapsedTime = Utility::ReadMemoryEx<INT>(GameStateBase + Offset::ElapsedTime);
                Data::CirclePain = Utility::ReadMemoryEx<float>(GameStateBase + Offset::CirclePain);
                Data::CurCircleWave = Utility::ReadMemoryEx<int>(GameStateBase + Offset::CurCircleWave);
            }

            auto actorsPointerAddress = Decryption::DecryptActorsArray(PersistentLevel, 0x70, 0x334);
            EntityList = Utility::ReadMemoryEx<uint32_t>(actorsPointerAddress);
            EntityCount = Utility::ReadMemoryEx<INT>(actorsPointerAddress + 0x4);

            for (int i = 0; i < EntityCount; i++)
            {
                EntityAddv = Utility::ReadMemoryEx<DWORD>(EntityList + i * 4);
                EntityStruct = Utility::ReadMemoryEx<DWORD>(EntityAddv + 0x10);
                EntityType = GetEntityType3(gNames, EntityStruct);
                if (EntityAddv == (DWORD) nullptr || EntityAddv == -1 || EntityAddv == NULL)
                    continue;
                if (FindReplace(EntityAddv, TempCacheActorAdrress) != -1)
                    continue;
                if (Setting::SpecialFeatures::DevMode)
                {
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
                if (IsPlayer(EntityType))
                {
                    if (Utility::ReadMemoryEx<DWORD>(EntityAddv + Offset::PlayerKeyAActor) == Data::PlayerKey)
                    {
                        Data::LocalPlayer = EntityAddv;
                    }
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
                if (IsVehicle(EntityType))
                {
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
                if (IsItem(EntityType) != "")
                {
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
                if (IsAlerts(EntityType) != "")
                {
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
                if (IsNextZone(EntityType))
                {
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
                if (Setting::SpecialFeatures::DangerMode && IsBullet(EntityType))
                {
                    TempCacheActorAdrress.push_back({EntityAddv, EntityType});
                    continue;
                }
            }
            TempActorAdrress = TempCacheActorAdrress;
            TempCacheActorAdrress.clear();
        }
        else
        {
            if (IsCleared == false)
            {
                Data::AActorList.clear();
                Data::VehicleList.clear();
                Data::ItemList.clear();
                Data::AlertsList.clear();
                Data::GrenadeTimeList.clear();
                Data::DevModeList.clear();
                TempActorAdrress.clear();
                TempCacheActorAdrress.clear();
                LastPlayerPosition.clear();
                Data::AlivePlayer = 0;
                Data::AliveTeam = 0;
                Data::PlayerNum = 0;
                Data::ElapsedTime = 0;
                Data::CirclePain = 0.f;
                Data::CurCircleWave = 0;
                IsCleared = true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Setting::UpdateTick));
    }
}
VOID UpdateLocal()
{
    while(g_Running)
    {
        if (Data::NetDriver > 0)
        {
            Data::LocalTravelled += (Data::LocalTravelSpeed / 10);
        }
        else if (Data::LocalTravelled > 0)
        {
            Data::LocalTravelled = 0;
        }
        Sleep(50);
    }
}
VOID UpdateData()
{
    while(g_Running)
    {
        if (Cheat::ModuleLoaded == true)
        {
            Data::pGameData = Utility::ReadMemoryEx<GameData>(BaseModule);
           
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
VOID AntiAim()
{
    bool IsDown = false;
    bool IsLast = false;
    CameraCacheEntry LastCameraCached;
    FRotator LastRorator;
    auto CurrentYaw = 0.0f;
    while(g_Running)
    {
        if (Data::IsFreeUser == -1 || Data::IsFreeUser == 0)
        {
            if (Setting::Memory::CameraSpin)
            {
                if (Data::LocalPlayer)
                {
                    DWORD PlayerCameraManager = Utility::ReadMemoryEx<DWORD>(Data::STPlayerController + Offset::PlayerCameraManager);
                    if (GetAsyncKeyState(Setting::Memory::CameraSpinKey) & 0x8000)
                    {
                        if (0 <= CurrentYaw < 180)
                        {
                            CurrentYaw += Setting::Memory::CameraSpinYaw;
                        }
                        if (CurrentYaw >= 180)
                        {
                            if (CurrentYaw == 180)
                                CurrentYaw = -180.f;
                            CurrentYaw += Setting::Memory::CameraSpinYaw;
                        }
                        if (IsLast == false)
                        {
                            LastCameraCached = Utility::ReadMemoryEx<CameraCacheEntry>(PlayerCameraManager + Offset::CameraCache);
                            LastRorator = Utility::ReadMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation);
                            IsLast = true;
                        }
                        auto CurrentAngle = Utility::ReadMemoryEx<FMinimalViewInfo>(PlayerCameraManager + Offset::CameraCache + 0x10);
                        FRotator aimRotation;
                        aimRotation.Pitch = Setting::Memory::CameraSpinPitch;
                        aimRotation.Roll = 1;
                        aimRotation.Yaw = CurrentYaw;
                        Utility::WriteMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation, aimRotation);
                    }
                    else if (IsLast == true)
                    {
                        Utility::WriteMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation, LastRorator);
                        IsLast = false;
                    }
                }
            }
        }
        Sleep(20);
    }
}
struct ObjectName
{
    char Data[64];
};
string GetEntityName(LONG GNames, LONG Id)
{
    DWORD GName = Utility::ReadMemoryEx<DWORD>(GNames);
    DWORD NamePtr = Utility::ReadMemoryEx<DWORD>(GName + int(Id / 16384) * 0x4);
    DWORD Name = Utility::ReadMemoryEx<DWORD>(NamePtr + int(Id % 16384) * 0x4);
    ObjectName pBuffer = Utility::ReadMemoryEx<ObjectName>(Name + 0x8);
    return string(pBuffer.Data);
}
std::wstring GetPlayerNameUTF(DWORD BaseAddress)
{
    std::wstring Wide = read_unicode(BaseAddress, 32);
    return Wide;
}
VOID Data::UpdateEntity()
{
    std::string EntityType;
    std::string PlayerUID;
    std::vector<AActor> AActorList;
    std::vector<AActor> ABotList;
    std::vector<Vehicle> VehicleList;
    std::vector<Item> ItemList;
    std::vector<Alerts> AlertsList;
    std::vector<DevMode> DevModeList;
    std::vector<VECTOR3> BulletList;
    DWORD EntityList = 0;
    DWORD EntityCount = 0;
    DWORD EntityAddv = 0;
    DWORD EntityStruct = 0;
    DWORD RootComponent = 0;
    DWORD MeshAddv = 0;
    DWORD BodyAddv = 0;
    DWORD BoneAddv = 0;
    auto hThread_1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UpdateLocal, 0, 0, 0);
    Data::HandleList.push_back(hThread_1);

    while(g_Running)
    {
        if (Data::NetDriver > 0)
        {
            for (int i = 0; i < TempActorAdrress.size(); i++)
            {
                EntityAddv = TempActorAdrress[i].Addv;
                EntityType = TempActorAdrress[i].EntityType;
                if (IsNextZone(EntityType))
                {
                    Data::NextZonePosition = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(EntityAddv + Offset::RootComponent) + Offset::Position);
                }
                if (IsPlayer(EntityType))
                {
                    AActor AActor;
                    AActor.IsUpdatedBone = false;
                    if (Cheat::ModuleLoaded == false)
                        AActor.IsUpdatedBone = true;
                    NamedCache NamedCache;
                    AActor.Address = EntityAddv;
                    MeshAddv = Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::Mesh);
                    BodyAddv = MeshAddv + Offset::BodyAddv;
                    BoneAddv = Utility::ReadMemoryEx<DWORD>(MeshAddv + Offset::MinLOD) + 0x30;
                    AActor.TeamID = Utility::ReadMemoryEx<INT>(AActor.Address + Offset::TeamId);
                    std::string PlayerUID = StringToUtf8(GetPlayerName(Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::PlayerUID)));
                    AActor.PlayerUID = PlayerUID;
                    if (AActor.Address == Data::LocalPlayer)
                    {
                        if (IsInitUID == false)
                        {
                            Data::LocalUID = PlayerUID;
                            IsInitUID = true;
                        }
                        Data::STPlayerController = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::STPlayerController);
                        Data::CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CurrentReloadWeapon);
                        DWORD PlayerCameraManager = Utility::ReadMemoryEx<DWORD>(Data::STPlayerController + Offset::PlayerCameraManager);
                        Data::GlobalCameraCache = Utility::ReadMemoryEx<FMinimalViewInfo>(PlayerCameraManager + Offset::CameraCache + 0x10);

                        Data::LocalbIsWeaponFiring = Utility::ReadMemoryEx<BOOL>(AActor.Address + Offset::bIsWeaponFiring);
                        Data::LocalbDead = Utility::ReadMemoryEx<BOOL>(AActor.Address + Offset::bDead);
                        
                        if (Data::LocalbDead)
                        {
                            Data::LocalPosition = Data::GlobalCameraCache.Location;
                        }
                        else
                        {
                            Data::LocalPosition = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::RootComponent) + Offset::Position);
                        }
                        
                        Data::LocalHeadPos = Algorithm::GetBoneWorldPosition(Utility::ReadMemoryEx<FTTransform>(BodyAddv), BoneAddv + 5 * 48);
                        auto CurrentVehicle = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CurrentVehicle);
                        VECTOR3 VelocityLocal;

                        if (CurrentVehicle)
                            VelocityLocal = Utility::ReadMemoryEx<VECTOR3>(CurrentVehicle + Offset::ReplicatedMovement);
                        else
                            VelocityLocal = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CharacterMovement) + Offset::LastUpdateVelocity);
                        float velCalSize = sqrt(VelocityLocal.X * VelocityLocal.X + VelocityLocal.Y * VelocityLocal.Y + VelocityLocal.Z * VelocityLocal.Z);
                        auto km_h = (velCalSize * 3600) / (100000);
                        auto met_s = km_h / 3.6f;
                        Data::LocalTravelSpeed = (FLOAT)met_s;
                        Data::MyTeamID = AActor.TeamID;

                        // CameraComponent Ipad View (Safe Hack Style)
                        if (Setting::SpecialFeatures::IpadView && Data::LocalPlayer)
                        {
                            DWORD CameraComponentPtr = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CameraComponent);
                            if (CameraComponentPtr)
                                Utility::WriteMemoryEx<float>(CameraComponentPtr + Offset::FieldOfView, (float)Setting::SpecialFeatures::IpadViewValue);
                            DWORD CameraComponentVehicle = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CameraComponentVehicle);
                            if (CameraComponentVehicle)
                                Utility::WriteMemoryEx<float>(CameraComponentVehicle + Offset::FieldOfView, (float)Setting::SpecialFeatures::IpadViewValue);
                            Setting::Memory::IpadViewRunning = true;
                        }
                        else if (Setting::Memory::IpadViewRunning && Data::LocalPlayer)
                        {
                            DWORD CameraComponentPtr = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CameraComponent);
                            if (CameraComponentPtr)
                                Utility::WriteMemoryEx<float>(CameraComponentPtr + Offset::FieldOfView, 80.0f);
                            DWORD CameraComponentVehicle = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CameraComponentVehicle);
                            if (CameraComponentVehicle)
                                Utility::WriteMemoryEx<float>(CameraComponentVehicle + Offset::FieldOfView, 80.0f);
                            Setting::Memory::IpadViewRunning = false;
                        }

                        continue;
                    }
                    AActor.IsDead = Utility::ReadMemoryEx<BOOL>(AActor.Address + Offset::bDead);
                    if (AActor.IsDead)
                        continue;
                    if (AActor.TeamID == Data::MyTeamID)
                    {
                        // Spectate mode: allow teammates so ESP stays stable on their camera
                        bool bSpectatingNow = (Data::LocalbDead && Setting::QoL::bESPWhenSpectating);
                        if (!bSpectatingNow && Setting::Player::Miscellanouse::PlayerUID == false)
                            continue;
                    }
                    AActor.IsBot = Utility::ReadMemoryEx<bool>(AActor.Address + Offset::bEnsure);
                    AActor.IsAI = Utility::ReadMemoryEx<bool>(AActor.Address + Offset::bIsAI);

                    /*     int LenghtUID = AActor.PlayerUID.size();
                         if (LenghtUID <= 5 && AActor.IsBot == false)
                             AActor.IsBot = true;*/


                    AActor.bAutoDestroyWhenFinished = Utility::ReadMemoryEx<BOOL>(AActor.Address + Offset::bAutoDestroyWhenFinished);
                    auto CurrentVehicle = Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::CurrentVehicle);
                    if (CurrentVehicle)
                        AActor.Position = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::RootComponent) + Offset::Position);
                    else
                        AActor.Position = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::RootComponent) + Offset::RelativeLocation);

                    AActor.Health = Utility::ReadMemoryEx<FLOAT>(AActor.Address + Offset::Health);
                    AActor.HealthMax = Utility::ReadMemoryEx<FLOAT>(AActor.Address + Offset::HealthMax);
                    AActor.NearDeathBreath = Utility::ReadMemoryEx<FLOAT>(AActor.Address + Offset::NearDeathBreath);
                    if (AActor.NearDeathBreath == 0)
                        continue;
                    DWORD NearDeatchComponent = Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::NearDeatchComponent);
                    AActor.Nation = StringToUtf8(GetPlayerName(Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::Nation)));
                    DWORD player_name_ptr = Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::Name);
                    AActor.Name = GetPlayerNameUTF(player_name_ptr);
                    AActor.IsSpecialName = 0;

                    AActor.BreathMax = Utility::ReadMemoryEx<FLOAT>(NearDeatchComponent + Offset::BreathMax);
                    if (AActor.IsDead == false && AActor.Health <= 0 && AActor.NearDeathBreath <= 0)
                        continue;

                    FTTransform AActorBone = Utility::ReadMemoryEx<FTTransform>(BodyAddv);


                    AActor.HeadPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 5 * 48);      // Head
                    AActor.ChestPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 4 * 48);     // neck_01
                    AActor.PelvisPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 0 * 48);    // pelvis
                    AActor.lSholderPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 11 * 48); // upperarm_l
                    AActor.rSholderPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 32 * 48); // upperarm_r
                    AActor.lElbowPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 12 * 48);   // lowerarm_l
                    AActor.rElbowPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 33 * 48);   // lowerarm_r
                    AActor.lWristPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 30 * 48);   // lowerarm_twist_01_l
                    AActor.rWristPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 51 * 48);   // lowerarm_twist_01_r
                    AActor.lThighPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 52 * 48);   // thigh_l
                    AActor.rThighPos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 56 * 48);   // thigh_r
                    AActor.lKneePos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 53 * 48);    // calf_l
                    AActor.rKneePos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 57 * 48);    // calf_r
                    AActor.lAnklePos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 54 * 48);   // foot_l
                    AActor.rAnklePos = Algorithm::GetBoneWorldPosition(AActorBone, BoneAddv + 58 * 48);   // foot_r  


                    DWORD CharacterWeaponManager = Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::WeaponManagerComponent);
                    DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(CharacterWeaponManager + Offset::CurrentWeaponReplicated);
                    if (CurrentReloadWeapon != 0)
                    {
                        std::string wep = GetEntityName(gNames, Utility::ReadMemoryEx<UINT>(CurrentReloadWeapon + 16));
                        AActor.PlayerWeapon = wep;
                    }
                    /* Visible check: always read from game memory so skeleton/ESP can color by visibility */
                    {
                        uint32_t visibilityBits = Utility::ReadMemoryEx<uint32_t>(AActor.Address + 0x15BB);
                        AActor.IsVisible =
                            ((visibilityBits >> 0) & 1) ||
                            ((visibilityBits >> 1) & 1) ||
                            ((visibilityBits >> 2) & 1) ||
                            ((visibilityBits >> 3) & 1) ||
                            ((visibilityBits >> 4) & 1) ||
                            ((visibilityBits >> 5) & 1) ||
                            ((visibilityBits >> 6) & 1) ||
                            ((visibilityBits >> 7) & 1) ||
                            ((visibilityBits >> 8) & 1) ||
                            ((visibilityBits >> 9) & 1) ||
                            ((visibilityBits >> 10) & 1) ||
                            ((visibilityBits >> 11) & 1) ||
                            ((visibilityBits >> 12) & 1) ||
                            ((visibilityBits >> 13) & 1) ||
                            ((visibilityBits >> 14) & 1) ? 1 : 0;
                    }
                    if (CurrentReloadWeapon == 0)
                    {
                        AActor.PlayerWeapon = "Fist";
                    }
                    else
                    {
                        AActor.CurBullet = Utility::ReadMemoryEx<INT>(CurrentReloadWeapon + Offset::CurBulletNumInClip);
                        AActor.CurMaxBullet = Utility::ReadMemoryEx<INT>(CurrentReloadWeapon + Offset::CurMaxBulletNumInOneClip);
                    }
                    if (CurrentVehicle)
                    {
                        AActor.Velocity = Utility::ReadMemoryEx<VECTOR3>(CurrentVehicle + Offset::ReplicatedMovement);
                    }
                    else
                    {
                        AActor.Velocity = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::CharacterMovement) + Offset::LastUpdateVelocity);
                    }
                    DWORD PlayerState = Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::PlayerState);
                    AActor.Aimming = "";
                    FRotator1 RelativeRotation = Utility::ReadMemoryEx<FRotator1>(Utility::ReadMemoryEx<DWORD>(AActor.Address + Offset::RootComponent) + Offset::RelativeRotation);
                    ClampAngles(RelativeRotation);
                    VECTOR3 MyHead = Data::LocalHeadPos;
                    VECTOR3 EnemyHed = AActor.HeadPos;
                    auto Angle = ToRotator(EnemyHed, MyHead);
                    ClampAngles(Angle);
                    float num = abs(RelativeRotation.Pitch - Angle.Pitch);
                    float num2 = abs(RelativeRotation.Yaw - Angle.Yaw);
                    bool num3 = (num + num2) <= 6.f;
                    if (num3 == true)
                    {
                        AActor.Aimming = "Aiming";
                    }
                    AActor.CheaterType = NormalUser;
                    if (AActor.IsBot == false && Auth::bAdmin == true)
                    {
                        AActor.CheaterType = IsCheater(AActor.PlayerUID);
                    }
                    AActor.IsFriend = false;
                    if (AActor.IsBot == false)
                    {
                        AActor.IsFriend = IsOnTeammateA(AActor.PlayerUID);
                    }
                    AActor.IsDrawBone = pGameData.bDraw;
                    if (AActor.IsBot)
                    {
                        ABotList.emplace_back(AActor);
                    }
                    AActorList.push_back(AActor);
                }

                if (IsVehicle(EntityType))
                {
                    if (Setting::Memory::HighRiskFastCar)
                        Utility::WriteMemoryEx<float>(EntityAddv + Offset::ExtraBoostFactor, 999.f);
                    Vehicle Vehicle;
                    Vehicle.Addv = EntityAddv;
                    float lastForwardSpeed = Utility::ReadMemoryEx<float>(Vehicle.Addv + Offset::lastForwardSpeed);
                    if (lastForwardSpeed > 8 || lastForwardSpeed < -8) /* engine started check */
                        Vehicle.bIsEngineStarted = true;
                    else
                        Vehicle.bIsEngineStarted = false;
                    Vehicle.Name = GetVehicleType(EntityType);
                    DWORD RootComponent = Utility::ReadMemoryEx<DWORD>(Vehicle.Addv + Offset::RootComponent);
                    Vehicle.Position = Utility::ReadMemoryEx<VECTOR3>(RootComponent + Offset::Position);
                    Vehicle.ReplicatedMovement = Utility::ReadMemoryEx<RepMovement>(Vehicle.Addv + Offset::ReplicatedMovement);
                    Vehicle.Velocity = Vehicle.ReplicatedMovement.LinearVelocity;
                    float velSize = Vehicle.Velocity.X * Vehicle.Velocity.X + Vehicle.Velocity.Y * Vehicle.Velocity.Y + Vehicle.Velocity.Z * Vehicle.Velocity.Z;
                    Vehicle.driving = (velSize > 100.f * 100.f);
                    DWORD VehicleCommon = Utility::ReadMemoryEx<DWORD>(Vehicle.Addv + Offset::VehicleCommon);
                    if (VehicleCommon)
                    {
                        float hp = Utility::ReadMemoryEx<float>(VehicleCommon + Offset::vHP);
                        float hpMax = Utility::ReadMemoryEx<float>(VehicleCommon + Offset::vHPMax);
                        Vehicle.HP = (hpMax > 0.f) ? (INT)(hp * 100.f / hpMax) : 0;
                        float fuel = Utility::ReadMemoryEx<float>(VehicleCommon + Offset::vFuel);
                        float fuelMax = Utility::ReadMemoryEx<float>(VehicleCommon + Offset::vFuelMax);
                        Vehicle.Fuel = (fuelMax > 0.f) ? (INT)(fuel * 100.f / fuelMax) : 0;
                        float cc = (fuelMax > 0.f) ? (fuel * 100.f / fuelMax) : 0.f;
                        Vehicle.phantram = (FLOAT)(ceilf(cc * 100) / 100);
                        DWORD VehicleWheel = Utility::ReadMemoryEx<DWORD>(VehicleCommon + Offset::WheelsCurrentHP);
                        if (VehicleWheel)
                        {
                            Vehicle.Wheel.FirstWheel = Utility::ReadMemoryEx<float>(VehicleWheel + 0x0);
                            Vehicle.Wheel.SecondWheel = Utility::ReadMemoryEx<float>(VehicleWheel + 0x4);
                            Vehicle.Wheel.ThirdWheel = Utility::ReadMemoryEx<float>(VehicleWheel + 0x8);
                            Vehicle.Wheel.FourthWheel = Utility::ReadMemoryEx<float>(VehicleWheel + 0xC);
                        }
                        else
                            Vehicle.Wheel.FirstWheel = Vehicle.Wheel.SecondWheel = Vehicle.Wheel.ThirdWheel = Vehicle.Wheel.FourthWheel = 1.f;
                    }
                    else
                        Vehicle.HP = 0; Vehicle.Fuel = 0; Vehicle.phantram = 0.f;
                    VehicleList.push_back(Vehicle);
                }
                if (IsItem(EntityType) != "")
                {
                    Item Item;
                    Item.Addv = EntityAddv;
                    Item.Name = IsItem(EntityType);
                    Item.Position = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(Item.Addv + Offset::RootComponent) + Offset::Position);
                    if (Item.Name == "AirDrop" || Item.Name == "DeathBox")
                    {
                        Item.boxItem.clear();
                        INT boxItemsCount = Utility::ReadMemoryEx<INT>(Item.Addv + Offset::PickUpDataList + Offset::TypeSpecificID);
                        if (boxItemsCount > 25)
                            boxItemsCount = 25;
                        DWORD ItemsBase = Utility::ReadMemoryEx<DWORD>(Item.Addv + Offset::PickUpDataList);
                        DWORD itemAddv;
                        INT ammoCount;
                        std::string boxItem, itemName;
                        for (int count = 0; count < boxItemsCount; count++)
                        {
                            itemAddv = ItemsBase + count * 48;
                            boxItem = GetBoxItem(Utility::ReadMemoryEx<INT>(itemAddv + 4));
                            if (boxItem == "")
                                continue;
                            ammoCount = Utility::ReadMemoryEx<INT>(itemAddv + 24);
                            itemName = boxItem;
                            itemName.append(" x");
                            itemName.append(to_string(ammoCount));
                            Item.boxItem.push_back(itemName);
                        }
                    }
                    ItemList.push_back(Item);
                }
                if (IsAlerts(EntityType) != "")
                {
                    Alerts Alerts;
                    Alerts.Name = IsAlerts(EntityType);
                    Alerts.Addv = EntityAddv;
                    if (Alerts.Name == "Grenade" || Alerts.Name == "Molotuf")
                    {
                        if (!IsValidBomb(Alerts.Addv))
                        {
                            GrenadeTime GrenadeTime;
                            GrenadeTime.CreatedTime = GetTickCount64();
                            GrenadeTime.Addv = Alerts.Addv;
                            GrenadeTimeList.push_back(GrenadeTime);
                        }
                        Alerts.CreatedTime = CreatedTime(Alerts.Addv);
                        Alerts.TimeLeft = 0;
                        if (Alerts.Name == "Grenade")
                        {
                            DWORD now = GetTickCount();
                            DWORD elapsed = now - Alerts.CreatedTime;

                            const float MaxTime = 7000.0f; // ms
                            Alerts.TimeLeft = max(0.0f, (MaxTime - (float)elapsed) / 1000.0f);
                        }
                        if (Alerts.Name == "Molotuf")
                        {
                            if ((FLOAT)(GetTickCount() - Alerts.CreatedTime) >= 11500)
                                Alerts.TimeLeft = 0;
                            else
                                Alerts.TimeLeft = (11500 - (GetTickCount() - Alerts.CreatedTime)) / 1000;
                        }
                    }
                    Alerts.Position = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(Alerts.Addv + Offset::RootComponent) + Offset::Position);
                    if (Alerts.Name == "Grenade" && Alerts.TimeLeft >= 16)
                        continue;
                    AlertsList.push_back(Alerts);
                    if (Alerts.Name == "Grenade" || Alerts.Name == "Molotuf" || Alerts.Name == "Smoke")
                    {
                        int IsValidBombPosition = GetValidBombPosition(Alerts.Addv);
                        if (IsValidBombPosition == -1)
                        {
                            BombPosition BombPos;
                            BombPos.Addv = Alerts.Addv;
                            BombPos.PositionVec.push_back(Alerts.Position);
                            Data::BombPositionList.push_back(BombPos);
                        }
                        else if (IsValidBombPosition > -1)
                        {
                            Data::BombPositionList.at(IsValidBombPosition).PositionVec.push_back(Alerts.Position);
                        }
                    }
                }
                for (int i = 0; i < Data::BombPositionList.size(); i++)
                {
                    bool IsFound = false;
                    for (int j = 0; j < Data::AlertsList.size(); j++)
                    {
                        if (Data::BombPositionList[i].Addv == Data::AlertsList[j].Addv)
                            IsFound = true;
                    }
                    if (IsFound == false)
                    {
                        Data::BombPositionList.erase(Data::BombPositionList.begin() + i);
                    }
                }
                if (Data::AlertsList.size() <= 0 && GrenadeTimeList.size() > 0)
                    GrenadeTimeList.clear();
                if (Setting::SpecialFeatures::DevMode)
                {
                    UINT IsEntityItem = Utility::ReadMemoryEx<UINT>(EntityAddv + Offset::ItemDefineID + Offset::TypeSpecificID);
                    DevMode DevMode;
                    DevMode.EntityName = EntityType + " " + to_string(IsEntityItem);
                    DevMode.Position = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(EntityAddv + Offset::RootComponent) + Offset::Position);
                    DevModeList.push_back(DevMode);
                }
                if (Setting::SpecialFeatures::DangerMode && IsBullet(EntityType))
                {
                    VECTOR3 Position = Utility::ReadMemoryEx<VECTOR3>(Utility::ReadMemoryEx<DWORD>(EntityAddv + Offset::RootComponent) + Offset::Position);
                    BulletList.push_back(Position);
                }
            }
            /* Memory hacks + QoL: apply when LocalPlayer valid */
            if (Data::LocalPlayer)
            {
                DWORD CharMov = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CharacterMovement);
                if (CharMov && Setting::MemoryHacks::bHighJump)
                {
                    // Simple high jump: scale default JumpZVelocity; this is the most stable behaviour across states
                    Utility::WriteMemoryEx<float>(CharMov + Offset::JumpZVelocity, 420.f * Setting::MemoryHacks::fHighJumpMultiplier);
                }
                DWORD WpnMgr = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::WeaponManagerComponent);
                if (WpnMgr)
                {
                    DWORD CurWpn = Utility::ReadMemoryEx<DWORD>(WpnMgr + Offset::CurrentWeaponReplicated);
                    if (CurWpn)
                    {
                        DWORD ShootEntity = Utility::ReadMemoryEx<DWORD>(CurWpn + Offset::ShootWeaponEntityComp);
                        if (ShootEntity)
                        {
                            Utility::WriteMemoryEx<float>(ShootEntity + Offset::BulletTrackDistanceFix, 1000.0f);
                            float baseDmg = Utility::ReadMemoryEx<float>(ShootEntity + Offset::BaseImpactDamage);
                            if (baseDmg >= 0.f && baseDmg < 1.f)
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::BaseImpactDamage, 41.0f);
                            if (Setting::MemoryHacks::bRapidFire)
                            {
                                float scale = Setting::MemoryHacks::fRapidFireScale;
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::ShootInterval, 0.02f * scale);
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::ExtraShootInterval, 0.02f * scale);
                            }
                            if (Setting::MemoryHacks::bExtendedRange)
                            {
                                float mult = Setting::MemoryHacks::fRangeMultiplier;
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::TraceDistance, 50000.f * mult);
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::RangeModifier, mult);
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::BulletRange, 50000.f * mult);
                            }
                            if (Setting::MemoryHacks::bDamageScale)
                            {
                                float baseDmg = Utility::ReadMemoryEx<float>(ShootEntity + Offset::BaseImpactDamage);
                                if (baseDmg > 0.f && baseDmg < 1000.f)
                                    Utility::WriteMemoryEx<float>(ShootEntity + Offset::BaseImpactDamage, baseDmg * Setting::MemoryHacks::fDamageMultiplier);
                            }
                            if (Setting::MemoryHacks::bInstantMed)
                                Utility::WriteMemoryEx<float>(ShootEntity + Offset::ReloadRate, 5.f);
                        }
                        if (Setting::MemoryHacks::bNoWeaponSway)
                        {
                            DWORD EffectComp = Utility::ReadMemoryEx<DWORD>(CurWpn + Offset::ShootWeaponEffectComp);
                            if (EffectComp)
                            {
                                Utility::WriteMemoryEx<float>(EffectComp + Offset::CameraShakeInnerRadius, 0.f);
                                Utility::WriteMemoryEx<float>(EffectComp + Offset::CameraShakeOuterRadius, 0.f);
                            }
                        }
                    }
                }
            }
            if (Data::LocalPlayer)
            {
                DWORD CurVeh = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CurrentVehicle);
                if (CurVeh)
                {
                    if (Setting::MemoryHacks::bVehicleBoost)
                        Utility::WriteMemoryEx<float>(CurVeh + Offset::ExtraBoostFactor, Setting::MemoryHacks::fVehicleBoostMultiplier * 100.f);
                    if (Setting::MemoryHacks::bVehicleNoDamage)
                    {
                        DWORD VehicleCommon = Utility::ReadMemoryEx<DWORD>(CurVeh + Offset::VehicleCommon);
                        if (VehicleCommon)
                        {
                            float hpMax = Utility::ReadMemoryEx<float>(VehicleCommon + Offset::vHPMax);
                            if (hpMax > 0.f)
                                Utility::WriteMemoryEx<float>(VehicleCommon + Offset::vHP, hpMax);
                        }
                    }
                }
            }
            if (Data::LocalPlayer && Setting::MemoryHacks::bFastParachuteExtra)
            {
                DWORD ParachuteComp = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::ParachuteComponent);
                if (ParachuteComp)
                {
                    Utility::WriteMemoryEx<float>(ParachuteComp + Offset::Opened_MaxForwardSpeed, 5000.f);
                    Utility::WriteMemoryEx<float>(ParachuteComp + Offset::FreeFall_MaxFreeFallSpeed, 2000.f);
                }
            }
            Data::AActorList = (AActorList);
            Data::VehicleList = (VehicleList);
            Data::ItemList = (ItemList);
            Data::AlertsList = (AlertsList);
            Data::DevModeList = (DevModeList);
            Data::BulletList = (BulletList);
            AActorList.clear();
            VehicleList.clear();
            ItemList.clear();
            AlertsList.clear();
            DevModeList.clear();
            BulletList.clear();
        }
        else
        {
            Data::AActorList.clear();
            Data::VehicleList.clear();
            Data::ItemList.clear();
            Data::AlertsList.clear();
            Data::DevModeList.clear();
            Data::BulletList.clear();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

VOID Data::UpdateViewMatrix()
{
    while(g_Running)
    {
        Data::ViewMatrixBase = Utility::ReadMemoryEx<D3DMATRIX1>(Utility::ReadMemoryEx<DWORD>(Utility::ReadMemoryEx<DWORD>(Data::ViewMatrix) + 0x20) + 0x200);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

VOID Data::ApplyGraphicsPatch()
{
    /*
    if (Data::LibUE4Addv == 0) return;

    // [120 FPS] Offset: 0x32DD5B0, Hex: 78 00 A0 E3 1E FF 2F E1
    BYTE patch1[] = { 0x78, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1 };
    Utility::WriteProtectedRaw(Data::LibUE4Addv + 0x32DD5B0, patch1, sizeof(patch1));

    // [HDR UNLOCK] Offset: 0x32DD2AC, Hex: 05 00 A0 E3 1E FF 2F E1
    BYTE patch2[] = { 0x05, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1 };
    Utility::WriteProtectedRaw(Data::LibUE4Addv + 0x32DD2AC, patch2, sizeof(patch2));
    */
}

