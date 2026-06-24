#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <chrono>
#include "def.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <future>
#include <WinBase.h>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <sstream>
#include "../offset.h"
#include "../Utility/Utility.h"
#include "../Algorithm/Algorithm.h"
#include "../setting/Setting.h"
#include "../../mathnvec/MathV.h"
using namespace std;
using namespace std::chrono;
using std::cout; using std::cin;
using std::endl; using std::string;
using std::filesystem::current_path;
enum _Cheating_Class {
    CheaterUser,
    CheaterTeammate,
    NormalUser
};
enum BoneId {
    head = 6,
    neck_01 = 5,
    neck_02 = 4,
    body_03 = 3,
    body_02 = 2,
    body_01 = 1,
    body_00 = 0,
    l_hand_01 = 10,
    l_hand_02 = 11,
    l_hand_03 = 12,
    l_hand_04 = 13,
    r_hand_01 = 31,
    r_hand_02 = 32,
    r_hand_03 = 33,
    r_hand_04 = 34,
    l_leg_01 = 52,
    l_leg_02 = 53,
    l_leg_03 = 54,
    l_leg_04 = 55,
    r_leg_01 = 56,
    r_leg_02 = 57,
    r_leg_03 = 58,
    r_leg_04 = 59,
};
struct FRotator1 {
    float Pitch;
    float Yaw;
    float Roll;
};
struct FMinimalViewInfo {
    FVector Location;
    FVector LocationLocalSpace;
    FRotator Rotation;
    float FOV;
};
struct CameraCacheEntry {
    float TimeStamp;
    char chunks[0xC];
    FMinimalViewInfo POV;
};
struct ActorsEncryption {
    uint32_t Enc_1, Enc_2;
    uint32_t Enc_3, Enc_4;
};

struct Encryption_Chunk {
    uint32_t val_1, val_2;
    uint32_t val_3, val_4;
};
extern struct SpecialName {
    std::string PlayerUID;
    std::wstring SpecialNameOut;
};
extern struct VisibleCheckBone {
    int Head;
    int Neck;
    int Pelvis;
    int lSholder;
    int rSholder;
    int lElbow;
    int rElbow;
    int lWrist;
    int rWrist;
    int lThigh;
    int rThigh;
    int lKnee;
    int rKnee;
    int lAnkle;
    int rAnkle;
};
extern struct LinearColor {
    float R;
    float G;
    float B;
    float A;
};
struct AActor
{
    BOOL IsBot;
    BOOL IsAI;
    BOOL IsDead;
    BOOL bAutoDestroyWhenFinished;
    BOOL IsHiddenFOW;
    BOOL IsFriend;
    BOOL IsUpdatedBone;
    BOOL IsDrawBone;
    INT IsVisible;
    INT CheaterType;
    INT IsSpecialName;
    INT CurBullet;
    INT CurMaxBullet;
    DWORD Address;
    DWORD TeamID;
    FLOAT Health;
    FLOAT HealthMax;
    FLOAT BreathMax;
    FLOAT NearDeathBreath;
    FLOAT BeRevivedNum;
    VECTOR3 BoxSphereBounds;
    VECTOR3  Position;
    VECTOR3 HeadPos;
    VECTOR3 ChestPos;
    VECTOR3 PelvisPos;
    VECTOR3 lSholderPos;
    VECTOR3 rSholderPos;
    VECTOR3 lElbowPos;
    VECTOR3 rElbowPos;
    VECTOR3 lWristPos;
    VECTOR3 rWristPos;
    VECTOR3 lThighPos;
    VECTOR3 rThighPos;
    VECTOR3 lKneePos;
    VECTOR3 rKneePos;
    VECTOR3 lAnklePos;
    VECTOR3 rAnklePos;
    VECTOR3 lFootPos;
    VECTOR3 rFootPos;
    VECTOR3 RootPos;
    VECTOR3 Velocity;
    VisibleCheckBone VisCheckBone;
    std::wstring Name;
    std::string PlayerUID;
    std::string PlayerWeapon;
    std::string Aimming;
    std::string Nation;
};
struct RepMovement
{
    VECTOR3 LinearVelocity;
    VECTOR3 AngularVelocity;
    VECTOR3 Location;
    FRotator1 Rotation;
};
struct Vehicle
{
    DWORD Addv;
    BOOL bIsEngineStarted;
    std::string Name;
    VECTOR3 Position;
    VECTOR3 Velocity;
    INT HP;
    FLOAT phantram;
    INT Fuel;
    bool driving;
    RepMovement ReplicatedMovement;
    struct { float FirstWheel, SecondWheel, ThirdWheel, FourthWheel; } Wheel;
};
extern struct TempEsp {
    DWORD Addv;
    std::string EntityType;
};
extern struct GrenadeTime {
    DWORD Addv;
    DWORD CreatedTime;
};
extern struct Alerts {
    std::string Name;
    DWORD Addv;
    DWORD CreatedTime;
    FLOAT TimeLeft;
    VECTOR3 Position;
};
extern struct Item {
    DWORD Addv;
    std::string Name;
    VECTOR3 Position;
    std::vector<std::string> boxItem;
};
extern struct ARNearest
{
    INT Distance;
    VECTOR3 Position;
};
extern struct DevMode
{
    string EntityName;
    VECTOR3 Position;
};
extern struct NamedCache
{
    BOOL IsBot;
    DWORD Address;
    DWORD TeamID;
    FLOAT HealthMax;
    std::wstring Name;
    std::string PlayerUID;
    std::string Nation;
};
extern struct ScopeWeapon 
{
    DWORD Addv;
    INT OriginalFOV;
};
extern struct CachedPosition
{
    DWORD Addv;
    VECTOR3 Position;
};
extern struct _Mod_Skin_Class {
    std::string Name;
    std::string Pattern;
    std::string Replace;
    std::string Status;
    bool bEdit;
};
extern struct _Mod_Skin_Effect_Class {
public:
    std::string Name;
    std::string Replace;
    std::string Status;
    bool bEdit;
};
extern struct BombPosition {
    DWORD Addv;
    std::vector<VECTOR3> PositionVec;
};
extern struct docxuong {
    uint32_t address;
    VECTOR3 HeadPos;
    VECTOR3 ChestPos;
    VECTOR3 PelvisPos;
    VECTOR3 lSholderPos;
    VECTOR3 rSholderPos;
    VECTOR3 lElbowPos;
    VECTOR3 rElbowPos;
    VECTOR3 lWristPos;
    VECTOR3 rWristPos;
    VECTOR3 lThighPos;
    VECTOR3 rThighPos;
    VECTOR3 lKneePos;
    VECTOR3 rKneePos;
    VECTOR3 lAnklePos;
    VECTOR3 rAnklePos;
};
extern struct checknup {
    uint32_t address;
    int visible;
};

struct TArrayEx {
    uintptr_t base;
    int count, max;
};

class CLibCom {
public:
    uintptr_t validate;
    uintptr_t localplayer;
    uintptr_t localcontroller;
    TArrayEx actors;
    TArrayEx visibles;
    uintptr_t HackSetting;
    uintptr_t world;
    uintptr_t gname;
};

extern struct GameData {
    int validate_value = 0xDEADC0DE;
    int players_count = 0;
    uintptr_t entity_base = 0;
    int entity_count = 0;
    DWORD actors = { };
    bool bDraw = false;
    uintptr_t localplayer = 0;
};
extern struct SkeletonRead {
    int validate_value = 0xDEADC0DE;
    DWORD skeletonlist = { };
};
extern struct ModMSG {
    int validate_value = 0xDEADC0DE;
    int clothes_mod = 0;
    int wep_mod = 0;
};
extern struct VehicleMod {
public:
    int validate_value = 0xDEADC0DE;
    bool IsActive = false;
    int Buggy = 0;
    int Coupe = 0;
    int Dacia = 0;
    int UAZ = 0;
    int Mirado = 0;
    int MotorCart = 0;
    int MotorCycle = 0;
    int Rony = 0;
};
extern struct DeadBoxMod {
public:
    int validate_value = 0xDEADC0DE;
    int weapon_deadbox_mod = 0;
};
namespace DecColor {
    extern std::string Red;
    extern std::string Green;
    extern std::string Aqua;
    extern std::string White;
}; // namespace DecColor
namespace VisibleCheck {
    const DWORD MoveAntiCheatComponent = 0xfe0;
    const DWORD Head = 0xc4;     // MoveAntiCheatComponent -> bUseMoveAntiCheatCheck
    const DWORD Neck = 0x36c;      // MoveAntiCheatComponent -> IsOpenRTPunish
    const DWORD Pelvis = 0xe4;   // MoveAntiCheatComponent -> bIsForceAdjustZWhenExceed
    const DWORD lSholder = 0x1270; // LocalPlayer -> AntiCheatRandValue0 
    const DWORD rSholder = 0x1b8;  // RootComponent -> AntiCheatRandValue1
    const DWORD lElbow = 0x1c0;    // RootComponent -> AntiCheatRandValue2
    const DWORD rElbow = 0x1c8;    // RootComponent -> AntiCheatRandValue3
    const DWORD lWrist = 0x1d0;    // RootComponent -> AntiCheatRandValue4
    const DWORD rWrist = 0x1d8;    // RootComponent -> AntiCheatRandValue5
    const DWORD lThigh = 0x1e0;    // RootComponent -> AntiCheatRandValue6
    const DWORD rThigh = 0x1c4;    // RootComponent -> bUseAttachParentBound
    const DWORD lKnee = 0x1c4;     // RootComponent -> bBoundsChangeTriggersStreamingDataRebuild
    const DWORD rKnee = 0x1dc;     // RootComponent -> bAbsoluteTranslation
    const DWORD lAnkle = 0x1cc;    // RootComponent -> bShouldUpdateOverLaps
    const DWORD rAnkle = 0x1ac;    // RootComponent -> bAbsoluteLocation
}
namespace OpenedProcessHandles {
    extern HANDLE UpdateCache;
    extern HANDLE UpdateEntity;
    extern HANDLE UpdateViewMatrix;
    extern HANDLE MemoryAttach;
    extern HANDLE Aim;
    extern HANDLE Macro;
}
namespace Data {
    inline DWORD LibGCloud = 0;


    extern INT AlivePlayer;
    extern INT AliveTeam;
    extern INT PlayerNum;
    extern INT ElapsedTime;
    extern float CirclePain;
    extern int CurCircleWave;
    extern INT IsFreeUser;//Admin = -1 , Vip = 0 , Free = 1
    extern INT EntityCount;
    extern INT GameID;
    extern INT LastGameID;
    extern INT TotalCheater;
    extern BOOL LocalbIsWeaponFiring;
    extern BOOL LocalbDead;
    extern BOOL IsMouseClick;
    extern BOOL ResetMenu;
    extern FLOAT LocalTravelled;
    extern FLOAT LocalTravelSpeed;
    extern DWORD ViewMatrix;
    extern DWORD LibUE4Addv;
    extern DWORD PlayerKey;
    extern DWORD LocalPlayer;
    extern DWORD NetDriver;
    extern DWORD STPlayerController;
    extern DWORD PlayerController;
    extern DWORD CurrentReloadWeapon;
    extern string LocalUID;
    extern string UserKey;
    extern string UserTime;
    extern string TotalUser;
    extern vector<AActor> AActorList;
    extern vector<Vehicle> VehicleList;
    extern vector<Item> ItemList;
    extern vector<Alerts> AlertsList;
    extern vector<GrenadeTime> GrenadeTimeList;
    extern vector<DevMode> DevModeList;
    extern vector<VECTOR3> BulletList;
    extern vector<NamedCache> NamedCacheList;
    extern vector<_Mod_Skin_Effect_Class> bModEffectList;
    extern vector<HANDLE> HandleList;
    extern vector<BombPosition> BombPositionList;
    extern vector<string> LocalChatList;
    extern vector<std::string> SpectatorNames;
    extern VECTOR3 NextZonePosition;
    extern VECTOR3 LocalPosition;
    extern VECTOR3 LocalHeadPos;
    extern D3DMATRIX1 ViewMatrixBase;
    extern DWORD LastHitActorAddress;
    extern double LastHitTime;
    extern std::map<DWORD, INT> ActorLastCurBullet;
    extern std::map<DWORD, double> ThreatUntilTime;
    extern FMinimalViewInfo GlobalCameraCache;
    extern GameData pGameData;
    extern SkeletonRead skeData;
    extern DWORD MyTeamID;
    extern VOID MemoryAttach( );
    extern VOID UpdateCache( );
    extern VOID UpdateEntity( );
    extern VOID UpdateViewMatrix( );
    extern VOID ApplyGraphicsPatch( );
}; // namespace Data                          // Size: 0x1040
class GameStateReadBase {
public:
    char pad_0000[840];      // 0x0000
    int16_t ElapsedTime;     // 0x0348
    char pad_034A[522];      // 0x034A
    uint16_t PlayerNum;      // 0x0554
    char pad_0556[670];      // 0x0556
    uint16_t AlivePlayerNum; // 0x07F4
    char pad_07F6[2];        // 0x07F6
    uint16_t AliveTeamNum;   // 0x07F8
    char pad_07FA[2];        // 0x07FA
    uint16_t NoneAIGameTime; // 0x07FC
    char pad_07FE[2110];     // 0x07FE
};                           // Size: 0x103C
class UWorldBase {
public:
    char pad_0000[32];        // 0x0000
    uint32_t PersistentLevel; // 0x0020
    uint32_t NetDriver;       // 0x0024
    char pad_0028[272];       // 0x0028
    uint32_t GameState;       // 0x0138
    char pad_013C[3844];      // 0x013C
};
extern struct PlayerStruct {
public:
    char pad_0000[336];           // 0x0000
    uint32_t RootComponent;       // 0x0150
    char pad_0154[456];           // 0x0154
    uint32_t PlayerState;         // 0x031C
    char pad_0320[864];           // 0x0320
    uint32_t Name;                // 0x0680
    char pad_0684[8];             // 0x0684
    uint32_t Nation;              // 0x068C
    char pad_0690[12];            // 0x0690
    uint32_t PlayerUID;           // 0x069C
    char pad_06A0[20];            // 0x06A0
    uint32_t TeamID;              // 0x06B4
    char pad_06B8[124];           // 0x06B8
    bool IsBot;                   // 0x0734
    char pad_0735[40];            // 0x0735
    bool bIsEngineStarted;        // 0x075D
    char pad_075E[636];           // 0x075E
    int16_t N00001B5D;            // 0x09DA
    float Health;                 // 0x09DC
    float HealthMax;              // 0x09E0
    char pad_09E4[15];            // 0x09E4
    bool N00001B93;               // 0x09F3
    bool bDead;                   // 0x09F4
    char pad_09F5[2173];          // 0x09F5
    int16_t N0000180A;            // 0x1272
    uint32_t NearDeatchComponent; // 0x1274
    char pad_1278[14];            // 0x1278
    uint16_t N0000180E;           // 0x1286
    float NearDeathBreath;        // 0x1288
    char pad_128C[2700];          // 0x128C
    uint32_t CurrentReloadWeapon; // 0x1D18
    char pad_1D1C[628];           // 0x1D1C
};                                //Size: 0x1F90


