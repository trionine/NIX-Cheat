#include "config.h"
#include "../src/setting/Setting.h"
#include <Windows.h>
#define _STRINGIFY(str) #str
#define STRINGIFY(str) _STRINGIFY(str)
#define WRITE_CONFIG_SETTINGS(val) WRITE_CONFIG(j, "Settings", STRINGIFY(val), Setting::val)
#define WRITE_CONFIG_SETTINGS_ELE(val, ele) WRITE_CONFIG(j, "Settings", STRINGIFY(ele::val), Setting::ele::val)
#define WRITE_CONFIG_KEYS(val) WRITE_CONFIG(j, "Keys", STRINGIFY(val), Setting::val)
#define READ_CONFIG_SETTINGS(val) Setting::val = SAFE_READ_CONFIG(j, "Settings", STRINGIFY(val), Setting::val)
#define READ_CONFIG_SETTINGS_ELE(val, ele) Setting::ele::val = SAFE_READ_CONFIG(j, "Settings", STRINGIFY(ele::val), Setting::ele::val)
#define READ_CONFIG_SETTINGS_BUF(val)                                        \
    {                                                                        \
        auto arr = SAFE_READ_CONFIG(j, "Settings", STRINGIFY(val), nlohmann::json::array()); \
        for (int i = 0; i < sizeof(Setting::val) / sizeof(Setting::val[0]); i++)              \
            if (i < arr.size()) Setting::val[i] = arr[i];                    \
    }
#define READ_CONFIG_SETTINGS_BUF_ELE(val, ele)                                         \
    {                                                                                  \
        auto arr = SAFE_READ_CONFIG(j, "Settings", STRINGIFY(ele::val), nlohmann::json::array()); \
        for (int i = 0; i < sizeof(Setting::ele::val) / sizeof(Setting::ele::val[0]); i++) \
            if (i < arr.size()) Setting::ele::val[i] = arr[i];                         \
    }
#define READ_CONFIG_SETTINGS_STR(val)                                 \
    {                                                                 \
        std::string str = SAFE_READ_CONFIG(j, "Settings", STRINGIFY(val), ""); \
        snprintf(Settings::val, sizeof(Settings::val), str.c_str( )); \
    }
#define READ_CONFIG_KEYS(val) Setting::val = SAFE_READ_CONFIG(j, "Keys", STRINGIFY(val), Setting::val)

#define CHECK_CFG_FILE(var) (var.length( ) > 0 && var.find('.') == var.npos && var.find('\\') == var.npos && var.find('/') == var.npos)
void DecryptFileSpec(int key, const std::string& fileName, std::string output) {
    char c;
    std::ifstream fin;
    std::ofstream fout;
    fin.open(fileName.c_str(), std::ios::binary);
    fout.open(output.c_str(), std::ios::binary);
    while (!fin.eof()) {
        fin >> std::noskipws >> c;
        int temp = (c - key);
        fout << (char)temp;
    }
    fin.close();
    fout.close();
}
void EncryptFileSpec(int key, const std::string& fileName, std::string output) {
    char c;
    std::ifstream fin;
    std::ofstream fout;
    fin.open(fileName.c_str(), std::ios::binary);
    fout.open(output.c_str(), std::ios::binary);
    while (!fin.eof()) {
        fin >> std::noskipws >> c;
        int temp = (c + key);
        fout << (char)temp;
    }
    fin.close();
    fout.close();
}
bool config::LoadConfig(std::string ConfigName) {
    try {
        std::string ConfigAbsPath = ConfigName;

        std::ifstream cfg_file(ConfigAbsPath);
        if (!cfg_file.is_open())
            return false;

        std::stringstream ss;
        ss << cfg_file.rdbuf();
        cfg_file.close();

        nlohmann::json j = nlohmann::json::parse(ss.str(), nullptr, false); // false => no exceptions
        if (j.is_discarded())
            return false;

        if (!j.contains("ACMH_CFG_rdbo"))
            return false;

        config::LoadConfig(j);
        return true;
    }
    catch (...) {
        return false;
    }
}


bool config::SaveConfig(std::string ConfigName) {
    try {
        std::string ConfigAbsPath = ConfigName;

        nlohmann::json j;
        config::SaveConfig(j);

        std::ofstream cfg_file(ConfigAbsPath);
        if (!cfg_file.is_open()) {
            return false;
        }
        std::stringstream ss;
        ss << j.dump();
        cfg_file.write(ss.str().c_str(), ss.str().length());
        cfg_file.close();
    }

    catch (...) {
        return false;
    }

    return true;
}
void config::SaveConfig(nlohmann::json& ConfigOut) {
    nlohmann::json& j = ConfigOut;
    // Normal
    WRITE_CONFIG_SETTINGS(ShowSide);
    WRITE_CONFIG_SETTINGS(InfoOverlay);
    WRITE_CONFIG_SETTINGS(InfoOverlayVer);
    WRITE_CONFIG_SETTINGS(Watermark);
    WRITE_CONFIG_SETTINGS(BackgroundTransparency);
    WRITE_CONFIG_SETTINGS(MenuTransparency);
    WRITE_CONFIG_SETTINGS(MenuFontSize);
    WRITE_CONFIG_SETTINGS(ImportantFontSize);
    WRITE_CONFIG_SETTINGS(AntiScreenCapture);
    WRITE_CONFIG_SETTINGS(DiscordRPC);
    WRITE_CONFIG_SETTINGS(AimAlternative);
    WRITE_CONFIG_SETTINGS(AimAssist);
    WRITE_CONFIG_SETTINGS(AssistSpeed);
    WRITE_CONFIG_SETTINGS(BypassAssistDistance);
    WRITE_CONFIG_SETTINGS(AssistFOV);
    WRITE_CONFIG_SETTINGS(Predict);
    WRITE_CONFIG_SETTINGS(AimLimitDistance);
    WRITE_CONFIG_SETTINGS(Fov);
    WRITE_CONFIG_SETTINGS(Macro);
    WRITE_CONFIG_SETTINGS(MacroWithAim);
    WRITE_CONFIG_SETTINGS(ShootKnock);
    WRITE_CONFIG_SETTINGS(SkipAI);
    WRITE_CONFIG_SETTINGS(AimLine);
    WRITE_CONFIG_SETTINGS(AimLineColTeamID);
    WRITE_CONFIG_SETTINGS(AimLineChange);
    WRITE_CONFIG_SETTINGS(LockEnemyPressAim);
    WRITE_CONFIG_SETTINGS(LanguageVNM);
    WRITE_CONFIG_SETTINGS(HideHack);
    WRITE_CONFIG_SETTINGS(BorderString);
    WRITE_CONFIG_SETTINGS(AutoTap);
    WRITE_CONFIG_SETTINGS(BulletCheckVisible);
    WRITE_CONFIG_SETTINGS(LibmoduleEsp);
    WRITE_CONFIG_SETTINGS(PredictAim);
    WRITE_CONFIG_SETTINGS(AimLimitDistanceValue);
    WRITE_CONFIG_SETTINGS(AimLimitDistanceValueScope);
    WRITE_CONFIG_SETTINGS(Fov_Color);
    WRITE_CONFIG_SETTINGS(AimLine_Color);
    WRITE_CONFIG_SETTINGS(AimLineChangeCol_Color);
    WRITE_CONFIG_SETTINGS(TypeAim);
    WRITE_CONFIG_SETTINGS(TargetAim);
    WRITE_CONFIG_SETTINGS(KeyAim);
    WRITE_CONFIG_SETTINGS(FovAim);
    WRITE_CONFIG_SETTINGS(FovAimScope);
    WRITE_CONFIG_SETTINGS(SmoothAim);
    WRITE_CONFIG_SETTINGS(MacroAim);
    WRITE_CONFIG_SETTINGS(KeyShootKnock);
    WRITE_CONFIG_SETTINGS(AutoTapDelay);
    WRITE_CONFIG_SETTINGS(UpdateTick);
    WRITE_CONFIG_SETTINGS(MenuTick);
    WRITE_CONFIG_SETTINGS(UpdateModuleTick);
    WRITE_CONFIG_SETTINGS(AimLine_ThickNess);
    // Player
    WRITE_CONFIG_SETTINGS_ELE(BoxType, Player);
    WRITE_CONFIG_SETTINGS_ELE(BoxTypeColor, Player);
    WRITE_CONFIG_SETTINGS_ELE(SnapLinesType, Player);
    WRITE_CONFIG_SETTINGS_ELE(SkeletonType, Player);
    WRITE_CONFIG_SETTINGS_ELE(RadarType, Player);
    WRITE_CONFIG_SETTINGS_ELE(Details, Player);
    WRITE_CONFIG_SETTINGS_ELE(Box, Player);
    WRITE_CONFIG_SETTINGS_ELE(SnapLines, Player);
    WRITE_CONFIG_SETTINGS_ELE(Skeleton, Player);
    WRITE_CONFIG_SETTINGS_ELE(Name, Player);
    WRITE_CONFIG_SETTINGS_ELE(Distance, Player);
    WRITE_CONFIG_SETTINGS_ELE(Weapon, Player);
    WRITE_CONFIG_SETTINGS_ELE(WeaponAsIcon, Player);
    WRITE_CONFIG_SETTINGS_ELE(WeaponColorDefault, Player);
    WRITE_CONFIG_SETTINGS_ELE(AimStatus, Player);
    WRITE_CONFIG_SETTINGS_ELE(ZonePrediction, Player);
    WRITE_CONFIG_SETTINGS_ELE(Radar, Player);
    WRITE_CONFIG_SETTINGS_ELE(VisibleCheck, Player);
    WRITE_CONFIG_SETTINGS_ELE(EachBone, Player);
    WRITE_CONFIG_SETTINGS_ELE(Box_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(SnapLines_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(Skeleton_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(Distance_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(Name_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(Weapon_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(AimStatus_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(ZonePrediction_ThickNess, Player);
    WRITE_CONFIG_SETTINGS_ELE(Radar_Distance, Player);
    WRITE_CONFIG_SETTINGS_ELE(Box_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(SnapLines_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(Skeleton_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(Distance_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(Name_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(NameBorder_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(Weapon_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(AimStatus_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(ZonePrediction_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(VisibleCheck_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(VisibleCheckPart2_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(VisibleCheckPart3_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(VisibleCheckPart4_Color, Player);
    WRITE_CONFIG_SETTINGS_ELE(HealthID, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(PlayerUID, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(Health, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(Nation, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(TeamIDCol, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(PlayerUID_ThickNess, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(Health_ThickNess, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(PlayerUID_Color, Player::Miscellanouse);
    WRITE_CONFIG_SETTINGS_ELE(Health_Color, Player::Miscellanouse);
    // PickUP
    WRITE_CONFIG_SETTINGS_ELE(SkipItemsKey, PickupItems::Visual);
    WRITE_CONFIG_SETTINGS_ELE(DetailsItems, PickupItems::Visual);
    WRITE_CONFIG_SETTINGS_ELE(DistanceItems, PickupItems::Visual);
    WRITE_CONFIG_SETTINGS_ELE(SkipItems, PickupItems::Visual);
    WRITE_CONFIG_SETTINGS_ELE(IsShowItems, PickupItems::Visual);
    WRITE_CONFIG_SETTINGS_ELE(ShowItemInDeathBox, PickupItems::Visual);
    WRITE_CONFIG_SETTINGS_ELE(M16A4, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(SCARL, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M416, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(FAMAS, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(QBZ, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(G63C, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(AUG, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(AKM, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M762, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(HoneyBadger, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(GROZA, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(MK14, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M16A4_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(SCARL_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M416_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(FAMAS_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(QBZ_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(G63C_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(AUG_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(AKM_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M762_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(HoneyBadger_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(GROZA_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(MK14_ThickNess, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M16A4_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(SCARL_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M416_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(FAMAS_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(QBZ_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(G63C_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(AUG_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(AKM_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(M762_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(HoneyBadger_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(GROZA_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(MK14_Color, PickupItems::Rifles);
    WRITE_CONFIG_SETTINGS_ELE(THOMPSON, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(UMP45, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(P90, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(UZI, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(BIZON, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(MP5K, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(VECTOR, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(THOMPSON_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(UMP45_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(P90_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(UZI_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(BIZON_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(MP5K_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(VECTOR_ThickNess, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(THOMPSON_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(UMP45_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(P90_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(UZI_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(BIZON_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(MP5K_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(VECTOR_Color, PickupItems::SMG);
    WRITE_CONFIG_SETTINGS_ELE(WIN94, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(VSS, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MINI14, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(QBU, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MK12, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MK47, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(SLR, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(SKS, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(KAR98, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MOSIN, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(BOW, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(AWM, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(AMR, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(WIN94_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(VSS_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MINI14_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(QBU_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MK12_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MK47_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(SLR_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(SKS_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(KAR98_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MOSIN_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(BOW_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(AWM_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(AMR_ThickNess, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(WIN94_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(VSS_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MINI14_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(QBU_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MK12_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MK47_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(SLR_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(SKS_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(KAR98_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(MOSIN_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(BOW_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(AWM_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(AMR_Color, PickupItems::Sniper);
    WRITE_CONFIG_SETTINGS_ELE(M249, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(MG3, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(DP28, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(M249_ThickNess, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(MG3_ThickNess, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(DP28_ThickNess, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(M249_Color, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(MG3_Color, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(DP28_Color, PickupItems::AutoMachine);
    WRITE_CONFIG_SETTINGS_ELE(S686, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S1897, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(M1014, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(Neostead2000, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S12K, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(DBS, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S686_ThickNess, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S1897_ThickNess, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(M1014_ThickNess, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(Neostead2000_ThickNess, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S12K_ThickNess, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(DBS_ThickNess, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S686_Color, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S1897_Color, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(M1014_Color, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(Neostead2000_Color, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(S12K_Color, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(DBS_Color, PickupItems::ShotGun);
    WRITE_CONFIG_SETTINGS_ELE(P1911, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(R45, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(DesertEagle, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P92, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P18C, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Vz61, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(R1895, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Flaregun, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P1911_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(R45_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(DesertEagle_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P92_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P18C_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Vz61_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(R1895_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Flaregun_ThickNess, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P1911_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(R45_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(DesertEagle_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P92_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(P18C_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Vz61_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(R1895_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Flaregun_Color, PickupItems::Pistols);
    WRITE_CONFIG_SETTINGS_ELE(Adrenaline, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(EnergyDrink, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Painkiller, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(FirstaidKit, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Medkit, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Adrenaline_ThickNess, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(EnergyDrink_ThickNess, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Painkiller_ThickNess, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(FirstaidKit_ThickNess, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Medkit_ThickNess, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Adrenaline_Color, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(EnergyDrink_Color, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Painkiller_Color, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(FirstaidKit_Color, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Medkit_Color, PickupItems::NeccessaryHealth);
    WRITE_CONFIG_SETTINGS_ELE(Pan, PickupItems::Melee);
    WRITE_CONFIG_SETTINGS_ELE(Pan_ThickNess, PickupItems::Melee);
    WRITE_CONFIG_SETTINGS_ELE(Pan_Color, PickupItems::Melee);
    WRITE_CONFIG_SETTINGS_ELE(BagLv1, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv2, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv3, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv1_ThickNess, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv2_ThickNess, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv3_ThickNess, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv1_Color, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv2_Color, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(BagLv3_Color, PickupItems::Bag);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv1, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv2, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv3, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv1_ThickNess, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv2_ThickNess, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv3_ThickNess, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv1_Color, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv2_Color, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(ArmorLv3_Color, PickupItems::Armor);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv1, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv2, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv3, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv1_ThickNess, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv2_ThickNess, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv3_ThickNess, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv1_Color, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv2_Color, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(HelmetLv3_Color, PickupItems::Helmet);
    WRITE_CONFIG_SETTINGS_ELE(AREx, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(ARExQk, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LargeCompensator, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LargeSuppressor, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(SniperCompensator, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(SniperSuppressor, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(ThumbGrip, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Angled, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LightGrip, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(HalfGrip, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Vertical, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Reddot, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Holo, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope2X, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope3X, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope4X, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope6X, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope8X, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(M416Stock, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(AREx_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(ARExQk_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LargeCompensator_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LargeSuppressor_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(SniperCompensator_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(SniperSuppressor_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(ThumbGrip_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Angled_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LightGrip_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(HalfGrip_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Vertical_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Reddot_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Holo_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope2X_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope3X_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope4X_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope6X_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope8X_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(M416Stock_ThickNess, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(AREx_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(ARExQk_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LargeCompensator_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LargeSuppressor_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(SniperCompensator_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(SniperSuppressor_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(ThumbGrip_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Angled_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(LightGrip_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(HalfGrip_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Vertical_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Reddot_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Holo_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope2X_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope3X_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope4X_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope6X_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Scope8X_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(M416Stock_Color, PickupItems::Accessory);
    WRITE_CONFIG_SETTINGS_ELE(Ammo762, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo45ACP, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo556, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo9, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo300Magnum, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo12Guage, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(AmmoFlareGun, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo762_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo45ACP_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo556_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo9_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo300Magnum_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo12Guage_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(AmmoFlareGun_ThickNess, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo762_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo45ACP_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo556_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo9_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo300Magnum_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(Ammo12Guage_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(AmmoFlareGun_Color, PickupItems::Ammo);
    WRITE_CONFIG_SETTINGS_ELE(AirDrop, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(DeathBox, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Grenade, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Smoke, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Molotof, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Apple, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Gascan, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(TokenShop, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(SecurityCard, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(AirDrop_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(DeathBox_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Grenade_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Smoke_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Molotof_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Apple_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Gascan_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(TokenShop_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(SecurityCard_ThickNess, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(AirDrop_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(DeathBox_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Grenade_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Smoke_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Molotof_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Apple_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Gascan_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(TokenShop_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(SecurityCard_Color, PickupItems::Others);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_Details, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_HP, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_Fuel, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_Distance, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(VehicleChange, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(VehicleSkip, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(IsShowVehicle, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(VehicleSkipkey, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_ThickNess, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_Color, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(VehicleChange_Color, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_Box, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_DirectionLine, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(VehicleLineDirectionSize, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_Warning, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_WheelDamage, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_VehType, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(Vehicle_MaxDistance, Vehicle);
    WRITE_CONFIG_SETTINGS_ELE(OpenAllAlert, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(GrenadeAlert, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(MolotufAlert, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(RedzoneAlert, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(GrenadeAlert_Trajectory, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(GrenadeAlert_ThickNess, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(MolotufAlert_ThickNess, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(RedzoneAlert_ThickNess, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(GrenadeAlert_Color, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(MolotufAlert_Color, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(RedzoneAlert_Color, Alerts);
    WRITE_CONFIG_SETTINGS_ELE(GrenadeAlert_Trajectory_Color, Alerts);
    // Memory
    WRITE_CONFIG_SETTINGS_ELE(bScaleDance, Memory);
    WRITE_CONFIG_SETTINGS_ELE(SHBV, Memory);
    WRITE_CONFIG_SETTINGS_ELE(MagicBullet, Memory);
    WRITE_CONFIG_SETTINGS_ELE(AutoMagic, Memory);
    WRITE_CONFIG_SETTINGS_ELE(Extended, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bShowDame, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bFastCar, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bFlyCar, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FlyCarValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FlyCarKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bNoRecoil, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bHighJump, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bHighJumpKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bHighJumpValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(SGV_Value, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bFly, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bFlyKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bFlyValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bInstantHit, Memory);
    WRITE_CONFIG_SETTINGS_ELE(InstantHitValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(TypeMouse, Memory);
    WRITE_CONFIG_SETTINGS_ELE(TypeMethod, Memory);
    WRITE_CONFIG_SETTINGS_ELE(SpeedShoot, Memory);
    WRITE_CONFIG_SETTINGS_ELE(BaseFire, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CalcFireSpeed, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CalculateBaseFire, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bAntiMemory, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bModAttachment, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bModEffect, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bGodView, Memory);

    WRITE_CONFIG_SETTINGS_ELE(bSilentGodView, Memory);
    WRITE_CONFIG_SETTINGS_ELE(xEffect, Memory);
    WRITE_CONFIG_SETTINGS_ELE(xEffectHead_ColorRandom, Memory);
    WRITE_CONFIG_SETTINGS_ELE(xEffectBody_ColorRandom, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CrosshairColor, Memory);
    WRITE_CONFIG_SETTINGS_ELE(JumpProne, Memory);
    WRITE_CONFIG_SETTINGS_ELE(LessRecoil, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CameraSpin, Memory);
    WRITE_CONFIG_SETTINGS_ELE(AutoReport, Memory);

    WRITE_CONFIG_SETTINGS_ELE(bBigWeapon, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bBigVehicle, Memory);
    WRITE_CONFIG_SETTINGS_ELE(BigVehicleValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(BigPlayer, Memory);
    WRITE_CONFIG_SETTINGS_ELE(BigPlayerValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bWallShot, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bTeleport, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bTeleportKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bTeleportToCar, Memory);
    WRITE_CONFIG_SETTINGS_ELE(SpinBot, Memory);
    WRITE_CONFIG_SETTINGS_ELE(MagicValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(GodUpValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastCarForwardKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastCarBackwardKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(AntiMemoryValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(GodviewTopKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(GodviewBotKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(GodviewFrontKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(GodviewLeftKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(GodviewRightKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(xEffectScale, Memory);
    WRITE_CONFIG_SETTINGS_ELE(JumpProneKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(JumpKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(ProneKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(LessRecoilType, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CameraSpinYaw, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CameraSpinPitch, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CameraSpinKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(CountReport, Memory);
    WRITE_CONFIG_SETTINGS_ELE(BigWeaponValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(SpinBotSpeed, Memory);
    WRITE_CONFIG_SETTINGS_ELE(SpinBotKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(xEffectHead_Color, Memory);
    WRITE_CONFIG_SETTINGS_ELE(xEffectBody_Color, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRun, Memory);
    WRITE_CONFIG_SETTINGS_ELE(HighRiskFastCar, Memory);
    WRITE_CONFIG_SETTINGS_ELE(UseInventorySkinMap, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV1, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV2, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV2Key, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV2Mode, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV2Value, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV3, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV3Mode, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV3Key, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV3Value, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV4, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV4Key, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV4Mode, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV4Value, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV5, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV5Key, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV5Mode, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV5Move, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastRunV5World, Memory);
    WRITE_CONFIG_SETTINGS_ELE(PositionControl, Memory);
    WRITE_CONFIG_SETTINGS_ELE(PositionControlValue, Memory);
    WRITE_CONFIG_SETTINGS_ELE(PositionControlKey, Memory);
    WRITE_CONFIG_SETTINGS_ELE(PositionControlMode, Memory);
    WRITE_CONFIG_SETTINGS_ELE(PositionTick, Memory);
    WRITE_CONFIG_SETTINGS_ELE(StopLocal, Memory);
    WRITE_CONFIG_SETTINGS_ELE(bAnimation, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastParachute, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastParachuteKeybUP, Memory);
    WRITE_CONFIG_SETTINGS_ELE(FastParachuteKeybDOWN, Memory);
    // Memory::ModSkin
// Non-Weapon Settings












    // Additional Settings removed to keep them permanently enabled

    // Assault Rifles (AR)













    // Sniper Rifles (SR)







    // Designated Marksman Rifles (DMR)







    // Submachine Guns (SMG)








    // Shotguns






    // Light Machine Guns (LMG)



    // Pistols






    // Melee



    // Other


    // SpecialFeatures
    WRITE_CONFIG_SETTINGS_ELE(V3Fov, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(V3SkipKnock, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(V3SkipBot, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(AimbotInternal, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(HeadPosZi, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(UCCWFov, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(FixFlash, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(DangerMode, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(CacheVer, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(HideName, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(MacroSpeed, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(GameInformation, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(ArNearest, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(AutoMagicCar, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(AutoTCP, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(AutoUDP, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(bAllTCP, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(bAllUDP, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(ParachuteAssist, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(MessageKill, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(DeadBox, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(SlowAnimation, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(BirdMode, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(FogOfWar, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(ESPD, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(V3Selector, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(DevMode, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(AutoFire, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(WWCDPost, SpecialFeatures);

    WRITE_CONFIG_SETTINGS_ELE(FieldOfView, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(ThirdPersonFOV, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(ScopeFOV, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(IpadView, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(IpadViewValue, SpecialFeatures);

    WRITE_CONFIG_SETTINGS_ELE(UltraHD, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(RedBarLength, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(RedBarHeight, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(RedBarTextOffset, SpecialFeatures);
    WRITE_CONFIG_SETTINGS_ELE(bSpeedHack, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fSpeedMultiplier, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bNoRecoil, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bNoSpread, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bInstantHit, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bFastSwitch, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bInfiniteAmmo, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bVehicleBoost, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fVehicleBoostMultiplier, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bHighJump, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fHighJumpMultiplier, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bNoFallDamage, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bFOVUnlock, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fFOVMin, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fFOVMax, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bNoGrass, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bRadar, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bESPThroughWalls, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bVehicleFly, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bTeleport, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(iTeleportKey, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bRapidFire, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fRapidFireScale, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bNoWeaponSway, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bExtendedRange, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fRangeMultiplier, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bDamageScale, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(fDamageMultiplier, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bFastParachuteExtra, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bInstantMed, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bVehicleNoDamage, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bNoFlash, MemoryHacks);
    WRITE_CONFIG_SETTINGS_ELE(bSnaplines, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(bDistanceColor, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(fDistanceNear, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(fDistanceFar, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(bHealthBarStyle, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(fSkeletonThickness, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(bFOVFilterESP, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(bThreatHighlight, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(ThreatHighlight_Color, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(bRecentlyShotHighlight, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(RecentlyShotHighlight_Color, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(fRecentlyShotDurationSec, ESPExtras);
    WRITE_CONFIG_SETTINGS_ELE(bHumanizedAim, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(fAimSmooth, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(iAimBone, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(bStickyAim, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(fStickyThreshold, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(bAutoSwitchTarget, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(bPrediction, AimbotExtras);
    WRITE_CONFIG_SETTINGS_ELE(bKeyHints, QoL);

    WRITE_CONFIG_SETTINGS_ELE(bMatchStats, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bSafeZoneTimer, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bCustomCrosshair, QoL);
    WRITE_CONFIG_SETTINGS_ELE(fCrosshairSize, QoL);
    WRITE_CONFIG_SETTINGS_ELE(iTheme, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bNotificationHistory, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bPingOverlay, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bSessionTimer, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bCompass, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bWaypoint, QoL);
    WRITE_CONFIG_SETTINGS_ELE(fWaypointX, QoL);
    WRITE_CONFIG_SETTINGS_ELE(fWaypointY, QoL);
    WRITE_CONFIG_SETTINGS_ELE(fWaypointZ, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bEnemyLastSeen, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bShowSpectatorList, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bESPWhenSpectating, QoL);
    WRITE_CONFIG_SETTINGS_ELE(bSearchMenu, MenuExtras);
    WRITE_CONFIG_SETTINGS_ELE(bSaveMenuPosition, MenuExtras);
    WRITE_CONFIG_SETTINGS_ELE(iConfigPreset, MenuExtras);
    WRITE_CONFIG_SETTINGS_ELE(bKeybindList, MenuExtras);
    {
        nlohmann::json arr = nlohmann::json::array({Setting::QoL::fCrosshairColor[0], Setting::QoL::fCrosshairColor[1], Setting::QoL::fCrosshairColor[2], Setting::QoL::fCrosshairColor[3]});
        j["ACMH_CFG_rdbo"]["Settings"]["QoL::fCrosshairColor"] = arr;
    }
    // BulletTrack
    WRITE_CONFIG_SETTINGS_ELE(AutoFix, BulletTrack);
    WRITE_CONFIG_SETTINGS_ELE(CameraCache, BulletTrack);
    WRITE_CONFIG_SETTINGS_ELE(OpenAim, BulletTrack);
    WRITE_CONFIG_SETTINGS_ELE(BypassFovLitmit, BulletTrack);
    WRITE_CONFIG_SETTINGS_ELE(ZPosition, BulletTrack);
    // Animation
    WRITE_CONFIG_SETTINGS_ELE(bRainbowBorder, Animation);
    WRITE_CONFIG_SETTINGS_ELE(bRGBTabs, Animation);
    WRITE_CONFIG_SETTINGS_ELE(bPulseText, Animation);
    WRITE_CONFIG_SETTINGS_ELE(bMovingParticles, Animation);
    WRITE_CONFIG_SETTINGS_ELE(iParticleMode, Animation);
    WRITE_CONFIG_SETTINGS_ELE(fAnimationSpeed, Animation);
}
void config::LoadConfig(nlohmann::json& ConfigOut) {
    nlohmann::json& j = ConfigOut;
    // Normal
    READ_CONFIG_SETTINGS(ShowSide);
    READ_CONFIG_SETTINGS(InfoOverlay);
    READ_CONFIG_SETTINGS(InfoOverlayVer);
    READ_CONFIG_SETTINGS(Watermark);
    READ_CONFIG_SETTINGS(BackgroundTransparency);
    READ_CONFIG_SETTINGS(MenuTransparency);
    READ_CONFIG_SETTINGS(MenuFontSize);
    READ_CONFIG_SETTINGS(ImportantFontSize);
    READ_CONFIG_SETTINGS(AntiScreenCapture);
    READ_CONFIG_SETTINGS(DiscordRPC);
    READ_CONFIG_SETTINGS(AimAlternative);
    READ_CONFIG_SETTINGS(AimAssist);
    READ_CONFIG_SETTINGS(AssistSpeed);
    READ_CONFIG_SETTINGS(BypassAssistDistance);
    READ_CONFIG_SETTINGS(AssistFOV);
    READ_CONFIG_SETTINGS(Predict);
    READ_CONFIG_SETTINGS(AimLimitDistance);
    READ_CONFIG_SETTINGS(Fov);
    READ_CONFIG_SETTINGS(Macro);
    READ_CONFIG_SETTINGS(MacroWithAim);
    READ_CONFIG_SETTINGS(ShootKnock);
    READ_CONFIG_SETTINGS(SkipAI);
    READ_CONFIG_SETTINGS(AimLine);
    READ_CONFIG_SETTINGS(AimLineColTeamID);
    READ_CONFIG_SETTINGS(AimLineChange);
    READ_CONFIG_SETTINGS(LockEnemyPressAim);
    READ_CONFIG_SETTINGS(LanguageVNM);
    READ_CONFIG_SETTINGS(HideHack);
    READ_CONFIG_SETTINGS(BorderString);
    READ_CONFIG_SETTINGS(AutoTap);
    READ_CONFIG_SETTINGS(BulletCheckVisible);
    READ_CONFIG_SETTINGS(LibmoduleEsp);
    READ_CONFIG_SETTINGS(PredictAim);
    READ_CONFIG_SETTINGS(AimLimitDistanceValue);
    READ_CONFIG_SETTINGS(AimLimitDistanceValueScope);
    READ_CONFIG_SETTINGS_BUF(Fov_Color);
    READ_CONFIG_SETTINGS_BUF(AimLine_Color);
    READ_CONFIG_SETTINGS_BUF(AimLineChangeCol_Color);
    READ_CONFIG_SETTINGS(TypeAim);
    READ_CONFIG_SETTINGS(TargetAim);
    READ_CONFIG_SETTINGS(KeyAim);
    READ_CONFIG_SETTINGS(FovAim);
    READ_CONFIG_SETTINGS(FovAimScope);
    READ_CONFIG_SETTINGS(SmoothAim);
    READ_CONFIG_SETTINGS(MacroAim);
    READ_CONFIG_SETTINGS(KeyShootKnock);
    READ_CONFIG_SETTINGS(AutoTapDelay);
    READ_CONFIG_SETTINGS(UpdateTick);
    READ_CONFIG_SETTINGS(MenuTick);
    READ_CONFIG_SETTINGS(UpdateModuleTick);
    READ_CONFIG_SETTINGS(AimLine_ThickNess);
    // Player
    READ_CONFIG_SETTINGS_ELE(BoxType, Player);
    READ_CONFIG_SETTINGS_ELE(BoxTypeColor, Player);
    READ_CONFIG_SETTINGS_ELE(SnapLinesType, Player);
    READ_CONFIG_SETTINGS_ELE(SkeletonType, Player);
    READ_CONFIG_SETTINGS_ELE(RadarType, Player);
    READ_CONFIG_SETTINGS_ELE(Details, Player);
    READ_CONFIG_SETTINGS_ELE(Box, Player);
    READ_CONFIG_SETTINGS_ELE(SnapLines, Player);
    READ_CONFIG_SETTINGS_ELE(Skeleton, Player);
    READ_CONFIG_SETTINGS_ELE(Name, Player);
    READ_CONFIG_SETTINGS_ELE(Distance, Player);
    READ_CONFIG_SETTINGS_ELE(Weapon, Player);
    READ_CONFIG_SETTINGS_ELE(WeaponAsIcon, Player);
    READ_CONFIG_SETTINGS_ELE(WeaponColorDefault, Player);
    READ_CONFIG_SETTINGS_ELE(AimStatus, Player);
    READ_CONFIG_SETTINGS_ELE(ZonePrediction, Player);
    READ_CONFIG_SETTINGS_ELE(Radar, Player);
    READ_CONFIG_SETTINGS_ELE(VisibleCheck, Player);
    READ_CONFIG_SETTINGS_ELE(EachBone, Player);
    READ_CONFIG_SETTINGS_ELE(Box_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(SnapLines_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(Skeleton_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(Distance_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(Name_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(Weapon_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(AimStatus_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(ZonePrediction_ThickNess, Player);
    READ_CONFIG_SETTINGS_ELE(Radar_Distance, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(Box_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(SnapLines_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(Skeleton_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(Distance_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(Name_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(NameBorder_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(Weapon_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(AimStatus_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(ZonePrediction_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(VisibleCheck_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(VisibleCheckPart2_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(VisibleCheckPart3_Color, Player);
    READ_CONFIG_SETTINGS_BUF_ELE(VisibleCheckPart4_Color, Player);
    READ_CONFIG_SETTINGS_ELE(HealthID, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_ELE(PlayerUID, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_ELE(Health, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_ELE(Nation, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_ELE(TeamIDCol, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_ELE(PlayerUID_ThickNess, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_ELE(Health_ThickNess, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_BUF_ELE(PlayerUID_Color, Player::Miscellanouse);
    READ_CONFIG_SETTINGS_BUF_ELE(Health_Color, Player::Miscellanouse);
    // PickUP
    READ_CONFIG_SETTINGS_ELE(SkipItemsKey, PickupItems::Visual);
    READ_CONFIG_SETTINGS_ELE(DetailsItems, PickupItems::Visual);
    READ_CONFIG_SETTINGS_ELE(DistanceItems, PickupItems::Visual);
    READ_CONFIG_SETTINGS_ELE(SkipItems, PickupItems::Visual);
    READ_CONFIG_SETTINGS_ELE(IsShowItems, PickupItems::Visual);
    READ_CONFIG_SETTINGS_ELE(ShowItemInDeathBox, PickupItems::Visual);
    READ_CONFIG_SETTINGS_ELE(M16A4, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(SCARL, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(M416, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(FAMAS, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(QBZ, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(G63C, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(AUG, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(AKM, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(M762, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(HoneyBadger, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(GROZA, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(MK14, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(M16A4_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(SCARL_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(M416_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(FAMAS_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(QBZ_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(G63C_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(AUG_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(AKM_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(M762_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(HoneyBadger_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(GROZA_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(MK14_ThickNess, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(M16A4_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(SCARL_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(M416_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(FAMAS_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(QBZ_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(G63C_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(AUG_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(AKM_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(M762_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(HoneyBadger_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(GROZA_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_BUF_ELE(MK14_Color, PickupItems::Rifles);
    READ_CONFIG_SETTINGS_ELE(THOMPSON, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(UMP45, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(P90, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(UZI, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(BIZON, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(MP5K, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(VECTOR, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(THOMPSON_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(UMP45_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(P90_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(UZI_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(BIZON_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(MP5K_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(VECTOR_ThickNess, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(THOMPSON_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(UMP45_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(P90_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(UZI_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(BIZON_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(MP5K_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_BUF_ELE(VECTOR_Color, PickupItems::SMG);
    READ_CONFIG_SETTINGS_ELE(WIN94, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(VSS, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MINI14, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(QBU, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MK12, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MK47, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(SLR, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(SKS, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(KAR98, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MOSIN, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(BOW, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(AWM, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(AMR, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(WIN94_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(VSS_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MINI14_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(QBU_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MK12_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MK47_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(SLR_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(SKS_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(KAR98_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(MOSIN_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(BOW_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(AWM_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(AMR_ThickNess, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(WIN94_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(VSS_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(MINI14_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(QBU_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(MK12_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(MK47_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(SLR_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(SKS_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(KAR98_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(MOSIN_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(BOW_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(AWM_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_BUF_ELE(AMR_Color, PickupItems::Sniper);
    READ_CONFIG_SETTINGS_ELE(M249, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_ELE(MG3, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_ELE(DP28, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_ELE(M249_ThickNess, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_ELE(MG3_ThickNess, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_ELE(DP28_ThickNess, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_BUF_ELE(M249_Color, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_BUF_ELE(MG3_Color, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_BUF_ELE(DP28_Color, PickupItems::AutoMachine);
    READ_CONFIG_SETTINGS_ELE(S686, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(S1897, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(M1014, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(Neostead2000, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(S12K, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(DBS, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(S686_ThickNess, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(S1897_ThickNess, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(M1014_ThickNess, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(Neostead2000_ThickNess, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(S12K_ThickNess, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(DBS_ThickNess, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_BUF_ELE(S686_Color, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_BUF_ELE(S1897_Color, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_BUF_ELE(M1014_Color, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_BUF_ELE(Neostead2000_Color, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_BUF_ELE(S12K_Color, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_BUF_ELE(DBS_Color, PickupItems::ShotGun);
    READ_CONFIG_SETTINGS_ELE(P1911, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(R45, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(DesertEagle, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(P92, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(P18C, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(Vz61, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(R1895, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(Flaregun, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(P1911_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(R45_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(DesertEagle_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(P92_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(P18C_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(Vz61_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(R1895_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(Flaregun_ThickNess, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(P1911_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(R45_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(DesertEagle_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(P92_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(P18C_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(Vz61_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(R1895_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_BUF_ELE(Flaregun_Color, PickupItems::Pistols);
    READ_CONFIG_SETTINGS_ELE(Adrenaline, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(EnergyDrink, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(Painkiller, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(FirstaidKit, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(Medkit, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(Adrenaline_ThickNess, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(EnergyDrink_ThickNess, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(Painkiller_ThickNess, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(FirstaidKit_ThickNess, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(Medkit_ThickNess, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_BUF_ELE(Adrenaline_Color, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_BUF_ELE(EnergyDrink_Color, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_BUF_ELE(Painkiller_Color, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_BUF_ELE(FirstaidKit_Color, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_BUF_ELE(Medkit_Color, PickupItems::NeccessaryHealth);
    READ_CONFIG_SETTINGS_ELE(Pan, PickupItems::Melee);
    READ_CONFIG_SETTINGS_ELE(Pan_ThickNess, PickupItems::Melee);
    READ_CONFIG_SETTINGS_BUF_ELE(Pan_Color, PickupItems::Melee);
    READ_CONFIG_SETTINGS_ELE(BagLv1, PickupItems::Bag);
    READ_CONFIG_SETTINGS_ELE(BagLv2, PickupItems::Bag);
    READ_CONFIG_SETTINGS_ELE(BagLv3, PickupItems::Bag);
    READ_CONFIG_SETTINGS_ELE(BagLv1_ThickNess, PickupItems::Bag);
    READ_CONFIG_SETTINGS_ELE(BagLv2_ThickNess, PickupItems::Bag);
    READ_CONFIG_SETTINGS_ELE(BagLv3_ThickNess, PickupItems::Bag);
    READ_CONFIG_SETTINGS_BUF_ELE(BagLv1_Color, PickupItems::Bag);
    READ_CONFIG_SETTINGS_BUF_ELE(BagLv2_Color, PickupItems::Bag);
    READ_CONFIG_SETTINGS_BUF_ELE(BagLv3_Color, PickupItems::Bag);
    READ_CONFIG_SETTINGS_ELE(ArmorLv1, PickupItems::Armor);
    READ_CONFIG_SETTINGS_ELE(ArmorLv2, PickupItems::Armor);
    READ_CONFIG_SETTINGS_ELE(ArmorLv3, PickupItems::Armor);
    READ_CONFIG_SETTINGS_ELE(ArmorLv1_ThickNess, PickupItems::Armor);
    READ_CONFIG_SETTINGS_ELE(ArmorLv2_ThickNess, PickupItems::Armor);
    READ_CONFIG_SETTINGS_ELE(ArmorLv3_ThickNess, PickupItems::Armor);
    READ_CONFIG_SETTINGS_BUF_ELE(ArmorLv1_Color, PickupItems::Armor);
    READ_CONFIG_SETTINGS_BUF_ELE(ArmorLv2_Color, PickupItems::Armor);
    READ_CONFIG_SETTINGS_BUF_ELE(ArmorLv3_Color, PickupItems::Armor);
    READ_CONFIG_SETTINGS_ELE(HelmetLv1, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_ELE(HelmetLv2, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_ELE(HelmetLv3, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_ELE(HelmetLv1_ThickNess, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_ELE(HelmetLv2_ThickNess, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_ELE(HelmetLv3_ThickNess, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_BUF_ELE(HelmetLv1_Color, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_BUF_ELE(HelmetLv2_Color, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_BUF_ELE(HelmetLv3_Color, PickupItems::Helmet);
    READ_CONFIG_SETTINGS_ELE(AREx, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(ARExQk, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(LargeCompensator, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(LargeSuppressor, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(SniperCompensator, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(SniperSuppressor, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(ThumbGrip, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Angled, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(LightGrip, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(HalfGrip, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Vertical, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Reddot, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Holo, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope2X, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope3X, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope4X, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope6X, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope8X, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(M416Stock, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(AREx_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(ARExQk_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(LargeCompensator_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(LargeSuppressor_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(SniperCompensator_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(SniperSuppressor_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(ThumbGrip_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Angled_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(LightGrip_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(HalfGrip_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Vertical_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Reddot_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Holo_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope2X_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope3X_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope4X_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope6X_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Scope8X_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(M416Stock_ThickNess, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(AREx_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(ARExQk_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(LargeCompensator_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(LargeSuppressor_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(SniperCompensator_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(SniperSuppressor_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(ThumbGrip_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Angled_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(LightGrip_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(HalfGrip_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Vertical_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Reddot_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Holo_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Scope2X_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Scope3X_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Scope4X_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Scope6X_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(Scope8X_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_BUF_ELE(M416Stock_Color, PickupItems::Accessory);
    READ_CONFIG_SETTINGS_ELE(Ammo762, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo45ACP, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo556, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo9, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo300Magnum, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo12Guage, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(AmmoFlareGun, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo762_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo45ACP_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo556_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo9_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo300Magnum_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(Ammo12Guage_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(AmmoFlareGun_ThickNess, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(Ammo762_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(Ammo45ACP_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(Ammo556_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(Ammo9_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(Ammo300Magnum_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(Ammo12Guage_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_BUF_ELE(AmmoFlareGun_Color, PickupItems::Ammo);
    READ_CONFIG_SETTINGS_ELE(AirDrop, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(DeathBox, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Grenade, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Smoke, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Molotof, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Apple, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Gascan, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(TokenShop, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(SecurityCard, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(AirDrop_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(DeathBox_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Grenade_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Smoke_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Molotof_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Apple_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Gascan_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(TokenShop_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(SecurityCard_ThickNess, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(AirDrop_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(DeathBox_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(Grenade_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(Smoke_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(Molotof_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(Apple_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(Gascan_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(TokenShop_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_BUF_ELE(SecurityCard_Color, PickupItems::Others);
    READ_CONFIG_SETTINGS_ELE(Vehicle, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_Details, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_HP, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_Fuel, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_Distance, Vehicle);
    READ_CONFIG_SETTINGS_ELE(VehicleChange, Vehicle);
    READ_CONFIG_SETTINGS_ELE(VehicleSkip, Vehicle);
    READ_CONFIG_SETTINGS_ELE(IsShowVehicle, Vehicle);
    READ_CONFIG_SETTINGS_ELE(VehicleSkipkey, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_ThickNess, Vehicle);
    READ_CONFIG_SETTINGS_BUF_ELE(Vehicle_Color, Vehicle);
    READ_CONFIG_SETTINGS_BUF_ELE(VehicleChange_Color, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_Box, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_DirectionLine, Vehicle);
    READ_CONFIG_SETTINGS_ELE(VehicleLineDirectionSize, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_Warning, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_WheelDamage, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_VehType, Vehicle);
    READ_CONFIG_SETTINGS_ELE(Vehicle_MaxDistance, Vehicle);
    READ_CONFIG_SETTINGS_ELE(OpenAllAlert, Alerts);
    READ_CONFIG_SETTINGS_ELE(GrenadeAlert, Alerts);
    READ_CONFIG_SETTINGS_ELE(MolotufAlert, Alerts);
    READ_CONFIG_SETTINGS_ELE(RedzoneAlert, Alerts);
    READ_CONFIG_SETTINGS_ELE(GrenadeAlert_Trajectory, Alerts);
    READ_CONFIG_SETTINGS_ELE(GrenadeAlert_ThickNess, Alerts);
    READ_CONFIG_SETTINGS_ELE(MolotufAlert_ThickNess, Alerts);
    READ_CONFIG_SETTINGS_ELE(RedzoneAlert_ThickNess, Alerts);
    READ_CONFIG_SETTINGS_BUF_ELE(GrenadeAlert_Color, Alerts);
    READ_CONFIG_SETTINGS_BUF_ELE(MolotufAlert_Color, Alerts);
    READ_CONFIG_SETTINGS_BUF_ELE(RedzoneAlert_Color, Alerts);
    READ_CONFIG_SETTINGS_BUF_ELE(GrenadeAlert_Trajectory_Color, Alerts);
    // Memory
    READ_CONFIG_SETTINGS_ELE(bScaleDance, Memory);
    READ_CONFIG_SETTINGS_ELE(SHBV, Memory);
    READ_CONFIG_SETTINGS_ELE(MagicBullet, Memory);
    READ_CONFIG_SETTINGS_ELE(AutoMagic, Memory);
    READ_CONFIG_SETTINGS_ELE(Extended, Memory);
    READ_CONFIG_SETTINGS_ELE(bShowDame, Memory);
    READ_CONFIG_SETTINGS_ELE(bFastCar, Memory);
    READ_CONFIG_SETTINGS_ELE(bFlyCar, Memory);
    READ_CONFIG_SETTINGS_ELE(FlyCarValue, Memory);
    READ_CONFIG_SETTINGS_ELE(FlyCarKey, Memory);
    READ_CONFIG_SETTINGS_ELE(bNoRecoil, Memory);
    READ_CONFIG_SETTINGS_ELE(bHighJump, Memory);
    READ_CONFIG_SETTINGS_ELE(bHighJumpKey, Memory);
    READ_CONFIG_SETTINGS_ELE(bHighJumpValue, Memory);
    READ_CONFIG_SETTINGS_ELE(SGV_Value, Memory);
    READ_CONFIG_SETTINGS_ELE(bFly, Memory);
    READ_CONFIG_SETTINGS_ELE(bFlyKey, Memory);
    READ_CONFIG_SETTINGS_ELE(bFlyValue, Memory);
    READ_CONFIG_SETTINGS_ELE(bInstantHit, Memory);
    READ_CONFIG_SETTINGS_ELE(InstantHitValue, Memory);
    READ_CONFIG_SETTINGS_ELE(TypeMouse, Memory);
    READ_CONFIG_SETTINGS_ELE(TypeMethod, Memory);
    READ_CONFIG_SETTINGS_ELE(SpeedShoot, Memory);
    READ_CONFIG_SETTINGS_ELE(BaseFire, Memory);
    READ_CONFIG_SETTINGS_ELE(CalcFireSpeed, Memory);
    READ_CONFIG_SETTINGS_ELE(CalculateBaseFire, Memory);
    READ_CONFIG_SETTINGS_ELE(bAntiMemory, Memory);
    READ_CONFIG_SETTINGS_ELE(bModAttachment, Memory);
    READ_CONFIG_SETTINGS_ELE(bModEffect, Memory);
    READ_CONFIG_SETTINGS_ELE(bGodView, Memory);
    READ_CONFIG_SETTINGS_ELE(xEffect, Memory);
    READ_CONFIG_SETTINGS_ELE(xEffectHead_ColorRandom, Memory);
    READ_CONFIG_SETTINGS_ELE(xEffectBody_ColorRandom, Memory);
    READ_CONFIG_SETTINGS_ELE(CrosshairColor, Memory);
    READ_CONFIG_SETTINGS_ELE(JumpProne, Memory);
    READ_CONFIG_SETTINGS_ELE(LessRecoil, Memory);
    READ_CONFIG_SETTINGS_ELE(CameraSpin, Memory);
    READ_CONFIG_SETTINGS_ELE(AutoReport, Memory);

    READ_CONFIG_SETTINGS_ELE(bBigWeapon, Memory);
    READ_CONFIG_SETTINGS_ELE(bBigVehicle, Memory);
    READ_CONFIG_SETTINGS_ELE(BigVehicleValue, Memory);
    READ_CONFIG_SETTINGS_ELE(BigPlayer, Memory);
    READ_CONFIG_SETTINGS_ELE(BigPlayerValue, Memory);
    READ_CONFIG_SETTINGS_ELE(bWallShot, Memory);
    READ_CONFIG_SETTINGS_ELE(bTeleport, Memory);
    READ_CONFIG_SETTINGS_ELE(bTeleportKey, Memory);
    READ_CONFIG_SETTINGS_ELE(bTeleportToCar, Memory);
    READ_CONFIG_SETTINGS_ELE(SpinBot, Memory);
    READ_CONFIG_SETTINGS_ELE(MagicValue, Memory);
    READ_CONFIG_SETTINGS_ELE(GodUpValue, Memory);
    READ_CONFIG_SETTINGS_ELE(FastCarForwardKey, Memory);
    READ_CONFIG_SETTINGS_ELE(FastCarBackwardKey, Memory);
    READ_CONFIG_SETTINGS_ELE(AntiMemoryValue, Memory);
    READ_CONFIG_SETTINGS_ELE(GodviewTopKey, Memory);
    READ_CONFIG_SETTINGS_ELE(GodviewBotKey, Memory);
    READ_CONFIG_SETTINGS_ELE(GodviewFrontKey, Memory);
    READ_CONFIG_SETTINGS_ELE(GodviewLeftKey, Memory);
    READ_CONFIG_SETTINGS_ELE(GodviewRightKey, Memory);


    READ_CONFIG_SETTINGS_ELE(bSilentGodView, Memory);

    READ_CONFIG_SETTINGS_ELE(xEffectScale, Memory);
    READ_CONFIG_SETTINGS_ELE(JumpProneKey, Memory);
    READ_CONFIG_SETTINGS_ELE(JumpKey, Memory);
    READ_CONFIG_SETTINGS_ELE(ProneKey, Memory);
    READ_CONFIG_SETTINGS_ELE(LessRecoilType, Memory);
    READ_CONFIG_SETTINGS_ELE(CameraSpinYaw, Memory);
    READ_CONFIG_SETTINGS_ELE(CameraSpinPitch, Memory);
    READ_CONFIG_SETTINGS_ELE(CameraSpinKey, Memory);
    READ_CONFIG_SETTINGS_ELE(CountReport, Memory);
    READ_CONFIG_SETTINGS_ELE(BigWeaponValue, Memory);
    READ_CONFIG_SETTINGS_ELE(SpinBotSpeed, Memory);
    READ_CONFIG_SETTINGS_ELE(SpinBotKey, Memory);
    READ_CONFIG_SETTINGS_BUF_ELE(xEffectHead_Color, Memory);
    READ_CONFIG_SETTINGS_BUF_ELE(xEffectBody_Color, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRun, Memory);
    READ_CONFIG_SETTINGS_ELE(HighRiskFastCar, Memory);
    READ_CONFIG_SETTINGS_ELE(UseInventorySkinMap, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV1, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV2, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV2Key, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV2Mode, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV2Value, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV3, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV3Key, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV3Mode, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV3Value, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV4, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV4Key, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV4Mode, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV4Value, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV5, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV5Key, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV5Mode, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV5Move, Memory);
    READ_CONFIG_SETTINGS_ELE(FastRunV5World, Memory);
    READ_CONFIG_SETTINGS_ELE(PositionControl, Memory);
    READ_CONFIG_SETTINGS_ELE(PositionControlValue, Memory);
    READ_CONFIG_SETTINGS_ELE(PositionControlKey, Memory);
    READ_CONFIG_SETTINGS_ELE(PositionControlMode, Memory);
    READ_CONFIG_SETTINGS_ELE(PositionTick, Memory);
    READ_CONFIG_SETTINGS_ELE(StopLocal, Memory);
    READ_CONFIG_SETTINGS_ELE(bAnimation, Memory);
    READ_CONFIG_SETTINGS_ELE(FastParachute, Memory);
    READ_CONFIG_SETTINGS_ELE(FastParachuteKeybUP, Memory);
    READ_CONFIG_SETTINGS_ELE(FastParachuteKeybDOWN, Memory);
    // Memory::ModSkin
// Non-Weapon Settings












    // Additional Settings removed to keep them permanently enabled

    // Assault Rifles (AR)













    // Sniper Rifles (SR)







    // Designated Marksman Rifles (DMR)







    // Submachine Guns (SMG)








    // Shotguns






    // Light Machine Guns (LMG)



    // Pistols






    // Melee



    // Other


    //SpecialFeatures
    READ_CONFIG_SETTINGS_ELE(V3Fov, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(V3SkipKnock, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(V3SkipBot, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(AimbotInternal, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(HeadPosZi, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(UCCWFov, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(FixFlash, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(CacheVer, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(DangerMode, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(HideName, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(MacroSpeed, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(GameInformation, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(ArNearest, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(AutoMagicCar, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(AutoTCP, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(AutoUDP, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(bAllTCP, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(bAllUDP, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(ParachuteAssist, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(MessageKill, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(DeadBox, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(SlowAnimation, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(BirdMode, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(FogOfWar, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(ESPD, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(V3Selector, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(DevMode, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(AutoFire, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(WWCDPost, SpecialFeatures);

    READ_CONFIG_SETTINGS_ELE(FieldOfView, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(ThirdPersonFOV, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(ScopeFOV, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(IpadView, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(IpadViewValue, SpecialFeatures);

    READ_CONFIG_SETTINGS_ELE(UltraHD, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(RedBarLength, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(RedBarHeight, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(RedBarTextOffset, SpecialFeatures);
    READ_CONFIG_SETTINGS_ELE(bSpeedHack, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fSpeedMultiplier, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bNoRecoil, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bNoSpread, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bInstantHit, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bFastSwitch, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bInfiniteAmmo, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bVehicleBoost, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fVehicleBoostMultiplier, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bHighJump, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fHighJumpMultiplier, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bNoFallDamage, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bFOVUnlock, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fFOVMin, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fFOVMax, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bNoGrass, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bRadar, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bESPThroughWalls, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bVehicleFly, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bTeleport, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(iTeleportKey, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bRapidFire, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fRapidFireScale, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bNoWeaponSway, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bExtendedRange, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fRangeMultiplier, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bDamageScale, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(fDamageMultiplier, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bFastParachuteExtra, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bInstantMed, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bVehicleNoDamage, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bNoFlash, MemoryHacks);
    READ_CONFIG_SETTINGS_ELE(bSnaplines, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(bDistanceColor, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(fDistanceNear, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(fDistanceFar, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(bHealthBarStyle, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(fSkeletonThickness, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(bFOVFilterESP, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(bThreatHighlight, ESPExtras);
    READ_CONFIG_SETTINGS_BUF_ELE(ThreatHighlight_Color, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(bRecentlyShotHighlight, ESPExtras);
    READ_CONFIG_SETTINGS_BUF_ELE(RecentlyShotHighlight_Color, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(fRecentlyShotDurationSec, ESPExtras);
    READ_CONFIG_SETTINGS_ELE(bHumanizedAim, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(fAimSmooth, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(iAimBone, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(bStickyAim, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(fStickyThreshold, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(bAutoSwitchTarget, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(bPrediction, AimbotExtras);
    READ_CONFIG_SETTINGS_ELE(bKeyHints, QoL);

    READ_CONFIG_SETTINGS_ELE(bMatchStats, QoL);
    READ_CONFIG_SETTINGS_ELE(bSafeZoneTimer, QoL);
    READ_CONFIG_SETTINGS_ELE(bCustomCrosshair, QoL);
    READ_CONFIG_SETTINGS_ELE(fCrosshairSize, QoL);
    READ_CONFIG_SETTINGS_ELE(iTheme, QoL);
    READ_CONFIG_SETTINGS_ELE(bNotificationHistory, QoL);
    READ_CONFIG_SETTINGS_ELE(bPingOverlay, QoL);
    READ_CONFIG_SETTINGS_ELE(bSessionTimer, QoL);
    READ_CONFIG_SETTINGS_ELE(bCompass, QoL);
    READ_CONFIG_SETTINGS_ELE(bWaypoint, QoL);
    READ_CONFIG_SETTINGS_ELE(fWaypointX, QoL);
    READ_CONFIG_SETTINGS_ELE(fWaypointY, QoL);
    READ_CONFIG_SETTINGS_ELE(fWaypointZ, QoL);
    READ_CONFIG_SETTINGS_ELE(bEnemyLastSeen, QoL);
    READ_CONFIG_SETTINGS_ELE(bShowSpectatorList, QoL);
    READ_CONFIG_SETTINGS_ELE(bESPWhenSpectating, QoL);
    READ_CONFIG_SETTINGS_ELE(bSearchMenu, MenuExtras);
    READ_CONFIG_SETTINGS_ELE(bSaveMenuPosition, MenuExtras);
    READ_CONFIG_SETTINGS_ELE(iConfigPreset, MenuExtras);
    READ_CONFIG_SETTINGS_ELE(bKeybindList, MenuExtras);
    READ_CONFIG_SETTINGS_BUF_ELE(fCrosshairColor, QoL);
    // BulletTrack
    READ_CONFIG_SETTINGS_ELE(AutoFix, BulletTrack);
    READ_CONFIG_SETTINGS_ELE(CameraCache, BulletTrack);
    READ_CONFIG_SETTINGS_ELE(OpenAim, BulletTrack);
    READ_CONFIG_SETTINGS_ELE(BypassFovLitmit, BulletTrack);
    READ_CONFIG_SETTINGS_ELE(ZPosition, BulletTrack);
    // Animation
    READ_CONFIG_SETTINGS_ELE(bRainbowBorder, Animation);
    READ_CONFIG_SETTINGS_ELE(bRGBTabs, Animation);
    READ_CONFIG_SETTINGS_ELE(bPulseText, Animation);
    READ_CONFIG_SETTINGS_ELE(bMovingParticles, Animation);
    READ_CONFIG_SETTINGS_ELE(iParticleMode, Animation);
    READ_CONFIG_SETTINGS_ELE(fAnimationSpeed, Animation);
}


void SaveConfigEffect(const std::string& filename, const std::vector<_Mod_Skin_Effect_Class>& configList) {
    std::ofstream outputFile(filename);
    if (!outputFile) {
        return;
    }
    for (const auto& config : configList) {
        outputFile << config.Name << "," << config.Replace << ","
            << "Waiting" << std::endl;
    }
    outputFile.close();
}

std::vector<_Mod_Skin_Effect_Class> LoadConfigEffect(const std::string& filename) {
    std::vector<_Mod_Skin_Effect_Class> configList;
    std::ifstream inputFile(filename);
    if (!inputFile) {
        return configList;
    }
    std::string line;
    int lineCount = 0;
    while (std::getline(inputFile, line)) {
        lineCount++;
        std::istringstream iss(line);
        std::string Name, Replace, Status;
        if (std::getline(iss, Name, ',') &&
            std::getline(iss, Replace, ',') &&
            std::getline(iss, Status)) {
            if (Name.empty() || Replace.empty() || Status.empty()) {
            }
            else {
                _Mod_Skin_Effect_Class config;
                config.Name = Name;
                config.Replace = Replace;
                config.Status = Status;
                configList.push_back(config);
            }
        }
        else {
        }
    }
    inputFile.close();
    return configList;
}


void config::SaveEffect()
{
    SaveConfigEffect("C:\\Windows\\notein1", Data::bModEffectList);
    EncryptFileSpec('KA', "C:\\Windows\\notein1", "Custom_skin_effect.cfg");
    std::remove("C:\\Windows\\notein1");
}
void config::LoadEffect()
{
    DecryptFileSpec('KA', "Custom_skin_effect.cfg", "C:\\Windows\\notein1");
    Data::bModEffectList = LoadConfigEffect("C:\\Windows\\notein1");
    std::remove("C:\\Windows\\notein1");
}

