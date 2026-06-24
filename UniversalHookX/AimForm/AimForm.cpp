#include "AimForm.h"
INT AimForm::BestTargetID = 0;
AActor AimForm::BestAActor;
VECTOR2 AimForm::ScreenBullet;
BYTE MagicBulletShell[60] = {
0xC7, 0x45, 0x78, 0x00, 0x00, 0x00, 0x00,                        // mov [ebp+78], 00000000
0x83, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x01,                        // cmp dword ptr [00000000], 01
0x74, 0x0E,                                                      // je +0E
0x60,                                                            // pushad
0x8B, 0x5D, 0x50,                                                // mov ebx, [ebp+50]
0x8B, 0x53, 0x18,                                                // mov edx, [ebx+18]
0x8A, 0x02,                                                      // mov al, [edx]
0x3C, 0xF0,                                                      // cmp al, F0  (First Cmp)
0x74, 0x02,                                                      // je +02
0x61,                                                            // popad
0xC3,                                                            // ret
0x8A, 0x42, 0x01,                                                // mov al, [edx+01]
0x3C, 0x3F,                                                      // cmp al, 3F  (Sec Cmp)
0x75, 0x14,                                                      // jne +14
0xC7, 0x83, 0xD0, 0x02, 0x00, 0x00, 0x45, 0x4D, 0x49, 0x52,     // mov [ebx+2D0], 'EMIR'
0xC7, 0x83, 0xD4, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // mov [ebx+2D4], 00000000
0xEB, 0xE1                                                       // jmp back
};
const DWORD ByteChecks = 0x56;  // VERIFY PATTERN 4.4: (offset+0xC) == 0x56
BYTE BackupSeach[] = { 0xE8, '?', '?', '?', '?', 0x90, 0x90 }; // Need update
BYTE MagicBulletSearch1[] = { 0x66, 0x0F, 0x7E, '?', 0xD0, 0x02, 0x00, 0x00, 0xC7, 0x45, 0x78, 0x68 }; // Updated pattern
BYTE MagicBulletSearch2[] = { 0x66, 0x0F, 0x7E, '?', 0xD0, 0x02, 0x00, 0x00, 0xC7, 0x45, 0x78, 0x68 }; // Updated pattern (same as search1)
//static BYTE ByteMagicSearch[] = { 0x66, 0x0F, 0x7E, 0xB9, 0xD0, 0x02, 0x00, 0x00, 0xC7, 0x45, 0x78, 0x68 }; // Magic Shell Code 4.4 Done
BYTE RealByteCode[7];
DWORD MagicBulletAddress;
DWORD MagicBulletHook;
BOOL IsMagicInitialized{ FALSE };
FRotator MagicCoordinate;
vector<DWORD_PTR> MagicBulletList = { };
BOOL IsEqual(BYTE Array1[], BYTE Array2[]) {
    for (int i = 0; i < 6; i++) {
        if (Array1[i] != Array2[i]) {
            return false;
        }
    }
    return true;
}
bool Hooked(int Address) {
    unsigned char Tmp[7];
    ReadProcessMemory(Utility::GameHandle, (LPCVOID)(Address + 0x8), &Tmp, sizeof(Tmp), 0);
    if (Tmp[0] == 0xE8 && Tmp[5] == 0x90 && Tmp[6] == 0x90) {
        return true;
    }
    return false;
}
BOOL InsideFov(FLOAT OverlayScreenWidth, FLOAT OverlayScreenHeight, VECTOR2 PlayerW2SBone, FLOAT FovRadius) {
    VECTOR2 Cenpoint;
    Cenpoint.X = PlayerW2SBone.X - (OverlayScreenWidth / 2);
    Cenpoint.Y = PlayerW2SBone.Y - (OverlayScreenHeight / 2);
    if (Cenpoint.X * Cenpoint.X + Cenpoint.Y * Cenpoint.Y <= FovRadius * FovRadius) {
        return true;
    }
    return false;
}
FRotator CalcAngleR(FVector LocalHeadPosition, FVector AimPosition) {
    FVector vecDelta = FVector((LocalHeadPosition.X - AimPosition.X), (LocalHeadPosition.Y - AimPosition.Y), (LocalHeadPosition.Z - AimPosition.Z));
    float hyp = (float)sqrt(vecDelta.X * vecDelta.X + vecDelta.Y * vecDelta.Y);

    FRotator ViewAngles = FRotator();
    ViewAngles.Pitch = -(float)atan(vecDelta.Z / hyp) * (float)(180.0f / 3.14159265358979323846f);
    ViewAngles.Yaw = (float)atan(vecDelta.Y / vecDelta.X) * (float)(180.0f / 3.14159265358979323846f);
    ViewAngles.Roll = (float)0;

    if (vecDelta.X >= 0.0f)
        ViewAngles.Yaw += 180.0f;

    return ViewAngles;
}
FRotator ClampAnglesR(FRotator r) {

    if (r.Yaw > 180.f)
        r.Yaw -= 360.f;
    else if (r.Yaw < -180.f)
        r.Yaw += 360.f;

    if (r.Pitch > 180.f)
        r.Pitch -= 360.f;
    else if (r.Pitch < -180.f)
        r.Pitch += 360.f;

    if (r.Pitch < -74.0f)
        r.Pitch = -74.0f;
    else if (r.Pitch > 74.0f)
        r.Pitch = 74.0f;

    r.Roll = 0.f;

    return r;
}
VOID GetMagicAddress()
{
    std::vector<DWORD_PTR> FoundedBase;
AGAIN:
    Utility::MemSearch(MagicBulletSearch1, sizeof(MagicBulletSearch1), 0x0D000000, 0x10000000, 0, 0, FoundedBase);
    if (FoundedBase.size() > 0)
    {
        for (int i = 0; i < FoundedBase.size(); i++)
        {
            if (Utility::ReadMemoryEx<BYTE>(FoundedBase[i] + 0xC) == ByteChecks)//Update
            {
                MagicBulletList.push_back(FoundedBase[i]);
                MagicBulletAddress = FoundedBase[i];
            }
        }
    }
    else { goto AGAIN; }
    FoundedBase.clear();
}
VOID InitializeMagic()
{
    INT Addv = MagicBulletAddress;
    if (Addv > 0)
    {
        MagicBulletHook = (INT)VirtualAllocEx(Utility::GameHandle, 0, 500, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!MagicBulletHook) return;
        WriteProcessMemory(Utility::GameHandle, (LPVOID)MagicBulletHook, &MagicBulletShell, sizeof(MagicBulletShell), 0);
        Utility::WriteMemoryEx<INT>(MagicBulletHook + 0x9, MagicBulletHook + 0x3C);//0x5E
        ReadProcessMemory(Utility::GameHandle, (LPCVOID)(Addv + 0x8), &RealByteCode, sizeof(RealByteCode), 0);
        WriteProcessMemory(Utility::GameHandle, (LPVOID)(MagicBulletHook), &RealByteCode, sizeof(RealByteCode), 0);
    }
}
VOID HookCall(INT Address, INT HookAdress) {
    BYTE Shell[7] = { 0xE8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90 };
    *(INT*)(Shell + 1) = (INT)(HookAdress - Address - 5);
    WriteProcessMemory(Utility::GameHandle, (LPVOID)Address, &Shell, sizeof(Shell), 0);
}
VOID HookMagic()
{
    for (DWORD_PTR& Addv : MagicBulletList)
    {
        BYTE Tmp[7];
        ReadProcessMemory(Utility::GameHandle, (LPCVOID)((int)Addv + 0x8), &Tmp, sizeof(Tmp), 0);
        if (IsEqual(Tmp, RealByteCode))
        {
            HookCall((INT)(Addv + 0x8), (INT)MagicBulletHook);
        }
    }
}
VOID AimForm::RestoreHook() {
    for (DWORD_PTR& Addv : MagicBulletList) {
        if (Hooked(Addv)) {
            WriteProcessMemory(Utility::GameHandle, (LPVOID)(Addv + 0x8), &RealByteCode, sizeof(RealByteCode), 0);
        }
    }
}
//VOID CheckMagic(BOOL BanDuoc) {
//    if (BanDuoc)
//        Utility::WriteMemoryEx<INT>(MagicBulletHook + 0x5e, 1);
//    else
//        Utility::WriteMemoryEx<INT>(MagicBulletHook + 0x5e, 0);
//}
FLOAT Magnit(VECTOR3 Vec) {
    return sqrtf(Vec.X * Vec.X + Vec.Y * Vec.Y + Vec.Z * Vec.Z);
}
VECTOR3 Subtract(VECTOR3 Src, VECTOR3 Dst) {
    VECTOR3 Result;
    Result.X = Src.X - Dst.X;
    Result.Y = Src.Y - Dst.Y;
    Result.Z = Src.Z - Dst.Z;
    return Result;
}
FLOAT GetDistance(VECTOR3 Src, VECTOR3 Dst) {
    VECTOR3 Result = Subtract(Src, Dst);
    return Magnit(Result);
}
FRotator ToRotator(VECTOR3 local, VECTOR3 target) {
    VECTOR3 rotation;
    rotation.X = local.X - target.X;
    rotation.Y = local.Y - target.Y;
    rotation.Z = local.Z - target.Z;
    FRotator newViewAngle;
    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);
    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (float)(180.f / 3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (float)(180.f / 3.14159265358979323846);
    newViewAngle.Roll = (float)0.f;
    if (rotation.X >= 0.f)
        newViewAngle.Yaw += 180.0f;
    return newViewAngle;
}
VECTOR3 BestGanTam(AActor AActor) {
    VECTOR3 EmptyVec = { 0, 0, 0 };
    std::vector<std::pair<VECTOR3, VECTOR2>> bonePairs = {
        { AActor.HeadPos, {} },
        { AActor.ChestPos, {} },
        { AActor.PelvisPos, {} }
    };

    float minDist = FLT_MAX;
    VECTOR3 bestTarget = EmptyVec;
    VECTOR2 screenCenter = { ScreenInfo::Width / 2.f, ScreenInfo::Height / 2.f };

    for (auto& [world, screen] : bonePairs) {
        if (world.X == AActor.HeadPos.X && world.Y == AActor.HeadPos.Y)
            world.Z += 5.f;
        if (Algorithm::WorldToScreenBone(Data::ViewMatrixBase, world, screen)) {
            float dist = std::hypot(screen.X - screenCenter.X, screen.Y - screenCenter.Y);
            if (dist < minDist) {
                minDist = dist;
                bestTarget = world;
            }
        }
    }
    return bestTarget;
}
VECTOR3 BestAimTarget(const AActor& actor)
{
    VECTOR3 EmptyVec;
    EmptyVec.X = 0;
    EmptyVec.Y = 0;
    EmptyVec.Z = 0;
    VECTOR3 ReturnNearest;
    VECTOR3 HeadPos;
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
    VECTOR2 Velocity;
    float FloatBorder = 1.f;
    goto again;
again: {
    HeadPos = actor.HeadPos;
    HeadPos.Z += 7;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, HeadPos, Head))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.ChestPos, Chest))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.PelvisPos, Pelvis))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.lSholderPos, lSholder))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.rSholderPos, rSholder))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.lElbowPos, lElbow))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.rElbowPos, rElbow))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.lWristPos, lWrist))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.rWristPos, rWrist))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.lThighPos, lThigh))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.rThighPos, rThigh))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.lKneePos, lKnee))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.rKneePos, rKnee))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.lAnklePos, lAnkle))
        return EmptyVec;
    if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, actor.rAnklePos, rAnkle))
        return EmptyVec;
    if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, Head, FloatBorder) && Setting::TargetAim == 3)
        return actor.HeadPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, Chest, FloatBorder))
        return actor.ChestPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, Pelvis, FloatBorder))
        return actor.PelvisPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, lSholder, FloatBorder))
        return actor.lSholderPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, rSholder, FloatBorder))
        return actor.rSholderPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, lElbow, FloatBorder))
        return actor.lElbowPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, rElbow, FloatBorder))
        return actor.rElbowPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, lWrist, FloatBorder))
        return actor.lWristPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, rWrist, FloatBorder))
        return actor.rWristPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, lThigh, FloatBorder))
        return actor.lThighPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, rThigh, FloatBorder))
        return actor.rThighPos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, lKnee, FloatBorder))
        return actor.lKneePos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, rKnee, FloatBorder))
        return actor.rKneePos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, lAnkle, FloatBorder))
        return actor.lAnklePos;
    else if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, rAnkle, FloatBorder))
        return actor.rAnklePos;
    else {
        FloatBorder += 1;
        goto again;
    }
    }
}
DWORD LockedTargetID = -1;
VOID CalculateMagicCoordinate(const VECTOR3& MyPosition, const VECTOR3& TargetPos)
{
    VECTOR3 Dif;
    Dif.X = TargetPos.X - MyPosition.X;
    Dif.Y = TargetPos.Y - MyPosition.Y;
    Dif.Z = TargetPos.Z - MyPosition.Z;

    float Hyp = sqrt(Dif.X * Dif.X + Dif.Y * Dif.Y);

    MagicCoordinate.Pitch = atan2(Dif.Z, Hyp) * 180.0f / 3.1415926535897f;
    MagicCoordinate.Yaw = atan2(Dif.Y, Dif.X) * 180.0f / 3.1415926535897f;
    MagicCoordinate.Roll = 0.0f;
}
VECTOR3 BestTarget() {
    AActor ToGet;
    AActor EmptyAActor;
    EmptyAActor.PlayerUID = "Empty";
    FLOAT TargetDistance = 9999.0f;
    FLOAT ClosestDistance = 99999.0f;
    FLOAT CrossDist = 0;
    FLOAT Health = 0;
    BOOL IsBot{ false };
    VECTOR3 PosShoot = { 0, 0, 0 };
    VECTOR3 OriginalPos;
    VECTOR3 Velocity;
    VECTOR3 ClosestTargetPos = { 0, 0, 0 };
    AActor ClosestAActor = EmptyAActor;
    VECTOR3 EmptyVec3 = { 0, 0, 0 };
    VECTOR3 Dif;
    VECTOR2 Screen;
    VECTOR2 EmptyVec2 = { 0, 0 };

    DWORD MeshAddv = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Mesh);
    DWORD BodyAddv = MeshAddv + Offset::BodyAddv;
    DWORD BoneAddv = Utility::ReadMemoryEx<DWORD>(MeshAddv + Offset::MinLOD) + 48;

    bool IsScoped = Utility::ReadMemoryEx<bool>(Data::LocalPlayer + Offset::bIsGunADS);
    float AimDistLimit = Setting::AimLimitDistanceValue;
    int AimFovLimit = Setting::FovAim;

    INT TargetID = 0;

    for (AActor& AActor : Data::AActorList)
    {
        TargetID += 1;
        int iDistance = sqrt(pow(Data::LocalPosition.X - AActor.Position.X, 2) +
            pow(Data::LocalPosition.Y - AActor.Position.Y, 2) +
            pow(Data::LocalPosition.Z - AActor.Position.Z, 2)) / 100;

        // Skip Bots logic
        if (Setting::SpecialFeatures::V3SkipBot && AActor.IsBot)
            continue;

        // Skip Knocked logic (with key override support)
        if (Setting::SpecialFeatures::V3SkipKnock && AActor.Health <= 0.0f) {
            if (Setting::KeyShootKnock == 0 || !(GetAsyncKeyState(Setting::KeyShootKnock) & 0x8000))
                continue;
        }

        if (Setting::BulletCheckVisible && AActor.IsVisible == 0 && Cheat::ModuleLoaded)
            continue;

        if (AActor.IsFriend)
            continue;

        if (Setting::AimLimitDistance && iDistance > AimDistLimit)
            continue;

        if (iDistance < ClosestDistance)
        {
            ClosestDistance = iDistance;
            ClosestTargetPos = AActor.HeadPos;
            ClosestAActor = AActor;
        }

        if (Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.HeadPos, Screen))
        {
            CrossDist = sqrt(pow(Screen.X - ScreenInfo::Width / 2, 2) + pow(Screen.Y - ScreenInfo::Height / 2, 2));
            if (CrossDist < TargetDistance) {
                switch (Setting::TargetAim) {
                case 0: PosShoot = AActor.HeadPos; PosShoot.Z += 7; break;
                case 1: PosShoot = AActor.ChestPos; break;
                case 2: PosShoot = AActor.PelvisPos; break;
                case 3:
                case 4: PosShoot = BestAimTarget(AActor); break;
                }

                if (Setting::TypeAim == 0)
                    PosShoot.Z += Setting::BulletTrack::ZPosition;

                TargetDistance = CrossDist;
                Velocity = AActor.Velocity;
                Health = AActor.Health;
                IsBot = AActor.IsBot;
                OriginalPos = PosShoot;
                AimForm::BestTargetID = TargetID;
                AimForm::BestAActor = AActor;
            }
        }
    }

    if (PosShoot.X == 0 && PosShoot.Y == 0 && PosShoot.Z == 0 && Setting::FovAim <= -2)
    {
        if (ClosestTargetPos.X != 0 || ClosestTargetPos.Y != 0 || ClosestTargetPos.Z != 0)
        {
            PosShoot = ClosestTargetPos;
            AimForm::BestAActor = ClosestAActor;
            AimForm::BestTargetID = -1;
        }
        else
        {
            AimForm::ScreenBullet = EmptyVec2;
            AimForm::BestTargetID = -1;
            AimForm::BestAActor = EmptyAActor;
            return EmptyVec3;
        }
    }

    bool IsOnScreen = Algorithm::WorldToScreenBone(Data::ViewMatrixBase, PosShoot, Screen);
    AimForm::ScreenBullet = Screen;

    if ((AimFovLimit == 0 && !IsOnScreen) ||
        (AimFovLimit > 0 && !InsideFov(ScreenInfo::Width, ScreenInfo::Height, Screen, AimFovLimit))) {
        AimForm::ScreenBullet = EmptyVec2;
        AimForm::BestTargetID = -1;
        AimForm::BestAActor = EmptyAActor;
        return EmptyVec3;
    }

    VECTOR3 MyPosition = Algorithm::GetBoneWorldPosition(Utility::ReadMemoryEx<FTTransform>(BodyAddv), BoneAddv + 5 * 48);
    FLOAT Distance = GetDistance(MyPosition, PosShoot);
    FLOAT DistanceDiv = GetDistance(MyPosition, PosShoot) / 100.f;

    FLOAT zAssist = 1.23f;
    if (Distance < 5000.f) zAssist = 1.8f;
    else if (Distance < 10000.f) zAssist = 1.72f;
    else if (Distance < 15000.f) zAssist = 1.23f;
    else if (Distance < 20000.f) zAssist = 1.24f;
    else if (Distance < 25000.f) zAssist = 1.25f;
    else if (Distance < 30000.f) zAssist = 1.26f;
    else if (Distance < 35000.f) zAssist = 1.27f;
    else if (Distance < 40000.f) zAssist = 1.28f;
    else if (Distance < 45000.f) zAssist = 1.29f;
    else if (Distance < 50000.f) zAssist = 1.30f;

    zAssist *= Setting::PredictAim;

    DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::WeaponManagerComponent) + Offset::CurrentWeaponReplicated);
    DWORD ShootWeaponEntity = Utility::ReadMemoryEx<DWORD>(CurrentReloadWeapon + Offset::ShootWeaponEntityComp);
    FLOAT BulletSpeed = Utility::ReadMemoryEx<FLOAT>(ShootWeaponEntity + Offset::BulletFireSpeed);
    FLOAT BulletTravelTime = DistanceDiv / 750.0f;

    if (Setting::Predict)
    {
        PosShoot.X += Velocity.X * BulletTravelTime;
        PosShoot.Y += Velocity.Y * BulletTravelTime;
        PosShoot.Z += Velocity.Z * BulletTravelTime * zAssist + 50.5f * 5.72f * BulletTravelTime * BulletTravelTime;
    }

    if (Setting::TypeAim == 0)
    {
        CalculateMagicCoordinate(MyPosition, PosShoot);
    }

    return PosShoot;
}


float Get3DDistance(const VECTOR3& a, const VECTOR3& b) {
    return sqrtf(powf(a.X - b.X, 2) + powf(a.Y - b.Y, 2) + powf(a.Z - b.Z, 2));
}
float GetZAssistFactor(float distance) {
    if (distance < 5000.f) return 1.8f;
    if (distance < 10000.f) return 1.72f;
    if (distance < 15000.f) return 1.23f;
    if (distance < 20000.f) return 1.24f;
    if (distance < 25000.f) return 1.25f;
    if (distance < 30000.f) return 1.26f;
    if (distance < 35000.f) return 1.27f;
    if (distance < 40000.f) return 1.28f;
    if (distance < 45000.f) return 1.29f;
    return 1.30f;
}
VECTOR3 PredictPosition(const VECTOR3& PosShoot, const VECTOR3& Velocity, float BulletTravelTime, float zAssist, bool gravityEnabled = true) {
    VECTOR3 result = PosShoot;
    result.X += Velocity.X * BulletTravelTime;
    result.Y += Velocity.Y * BulletTravelTime;

    if (gravityEnabled) {
        const float gravity = 5.72f;
        result.Z += Velocity.Z * BulletTravelTime - 0.5f * gravity * BulletTravelTime * BulletTravelTime * zAssist;
    }
    else {
        result.Z += Velocity.Z * BulletTravelTime;
    }

    return result;
}
VECTOR3 BestAimbotTarget() {
    AActor EmptyAActor;
    EmptyAActor.PlayerUID = "Empty";
    VECTOR3 EmptyVec3 = { 0, 0, 0 };
    VECTOR2 EmptyVec2 = { 0, 0 };

    float TargetDistance = 9999.f;
    VECTOR3 PosShoot = EmptyVec3;
    VECTOR3 OriginalPos;
    VECTOR2 Screen;
    float CrossDist = 0;

    int TargetID = 0;

    DWORD MeshAddv = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Mesh);
    DWORD BodyAddv = MeshAddv + Offset::BodyAddv;
    DWORD BoneAddv = Utility::ReadMemoryEx<DWORD>(MeshAddv + Offset::MinLOD) + 48;

    bool IsScoped = Utility::ReadMemoryEx<bool>(Data::LocalPlayer + Offset::bIsGunADS);
    float AimDistLimit = Setting::AimLimitDistanceValue;
    int AimFovLimit = Setting::FovAim;

    for (AActor& AActor : Data::AActorList) {
        TargetID++;

        float iDistance = Get3DDistance(Data::LocalPosition, AActor.Position) / 100.f;

        // Skip Bots logic
        if (Setting::SpecialFeatures::V3SkipBot && AActor.IsBot)
            continue;

        // Skip Knocked logic (with key override support)
        if (Setting::SpecialFeatures::V3SkipKnock && AActor.Health <= 0.0f) {
            if (Setting::KeyShootKnock == 0 || !(GetAsyncKeyState(Setting::KeyShootKnock) & 0x8000))
                continue;
        }

        if (Setting::BulletCheckVisible && AActor.IsVisible == 0 && Cheat::ModuleLoaded)
            continue;

        if (AActor.IsFriend)
            continue;

        if (Setting::AimLimitDistance && iDistance > AimDistLimit)
            continue;

        if (!Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.HeadPos, Screen))
            continue;

        CrossDist = std::hypot(Screen.X - ScreenInfo::Width / 2.f, Screen.Y - ScreenInfo::Height / 2.f);

        if (CrossDist < TargetDistance) {
            switch (Setting::TargetAim) {
            case 0: PosShoot = AActor.HeadPos; PosShoot.Z += 7; break;
            case 1: PosShoot = AActor.ChestPos; break;
            case 2: PosShoot = AActor.PelvisPos; break;
            case 3:
            case 4: PosShoot = BestGanTam(AActor); break;
            }

            if (Setting::TypeAim == 0)
                PosShoot.Z += Setting::BulletTrack::ZPosition;

            if (IsScoped) {
                float distance3D = Get3DDistance(Data::LocalPosition, PosShoot);
                float expectedDistance = AimDistLimit;
                float diff = distance3D - expectedDistance;
                float factor = (Setting::TargetAim == 3 || Setting::TargetAim == 4) ? 0.005f : 0.011f;

                if (distance3D > expectedDistance)
                    PosShoot.Z -= diff * factor;
            }

            TargetDistance = CrossDist;
            AimForm::BestTargetID = TargetID;
            AimForm::BestAActor = AActor;
            OriginalPos = PosShoot;
        }
    }

    if (PosShoot.X == 0 && PosShoot.Y == 0 && PosShoot.Z == 0)
    {
        AimForm::ScreenBullet = EmptyVec2;
        AimForm::BestTargetID = -1;
        AimForm::BestAActor = EmptyAActor;
        return EmptyVec3;
    }

    bool IsOnScreen = Algorithm::WorldToScreenBone(Data::ViewMatrixBase, PosShoot, Screen);
    AimForm::ScreenBullet = Screen;

    if ((AimFovLimit == 0 && !IsOnScreen) ||
        (AimFovLimit > 0 && !InsideFov(ScreenInfo::Width, ScreenInfo::Height, Screen, AimFovLimit))) {
        AimForm::ScreenBullet = EmptyVec2;
        AimForm::BestTargetID = -1;
        AimForm::BestAActor = EmptyAActor;
        return EmptyVec3;
    }

    VECTOR3 MyPosition = Algorithm::GetBoneWorldPosition(Utility::ReadMemoryEx<FTTransform>(BodyAddv), BoneAddv + 5 * 48);
    float Distance = Get3DDistance(MyPosition, PosShoot);

    float zAssist = GetZAssistFactor(Distance) * Setting::PredictAim;
    DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::WeaponManagerComponent) + Offset::CurrentWeaponReplicated);
    DWORD ShootWeaponEntity = Utility::ReadMemoryEx<DWORD>(CurrentReloadWeapon + Offset::ShootWeaponEntityComp);
    float BulletSpeed = Utility::ReadMemoryEx<float>(ShootWeaponEntity + Offset::BulletFireSpeed);
    float BulletTravelTime = Distance / BulletSpeed;

    AActor BestTarget = AimForm::BestAActor;
    VECTOR3 Velocity = BestTarget.Velocity;

    if (Setting::Predict)
        PosShoot = PredictPosition(PosShoot, Velocity, BulletTravelTime, zAssist, true);
    else
        PosShoot = PredictPosition(PosShoot, Velocity, BulletTravelTime, zAssist, false);

    return PosShoot;
}
bool IsOnTeammate(std::string PlayerID)
{
    if (Teammate::TeammateList.size() > 0)
    {
        for (int i = 0; i < Teammate::TeammateList.size(); i++) {
            if (Teammate::TeammateList[i] == PlayerID)
                return true;
        }
    }
    return false;
}
VOID AddFriendEnemy() {
    while (g_Running)
    {
        if (Data::AActorList.size() > 0)
        {
            if (GetAsyncKeyState(VK_F10) & 1) {
                FLOAT TargetDistance = 9999.0f;
                FLOAT CrossDist = 0;
                VECTOR2 Screen;
                std::string PlayerUIDTarget = "0";
                for (AActor& AActor : Data::AActorList) {
                    if (AActor.IsBot)
                        continue;
                    if (IsOnTeammate(AActor.PlayerUID))
                        continue;
                    if ((Algorithm::WorldToScreenBone(Data::ViewMatrixBase, AActor.HeadPos, Screen))) {
                        CrossDist = sqrt(pow(Screen.X - ScreenInfo::Width / 2, 2) + pow(Screen.Y - ScreenInfo::Height / 2, 2));
                        if (CrossDist < TargetDistance) {
                            TargetDistance = CrossDist;
                            PlayerUIDTarget = AActor.PlayerUID;
                        }
                    }
                }
                if (PlayerUIDTarget != "0") {
                    bool IsAdded = false;
                    for (int i = 0; i < Teammate::TeammateList.size(); i++) {
                        if (IsAdded == true)
                            continue;
                        if (Teammate::TeammateList[i] == "0" && IsAdded == false) {
                            Teammate::TeammateList[i] = PlayerUIDTarget;
                            IsAdded = true;
                        }
                    }
                }
            }
        }
        Sleep(10);
    }
}
VOID FixGayFake()
{
    DWORD CharacterWeaponManagerComponent = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::WeaponManagerComponent);//CharacterWeaponManagerComponent* WeaponManagerComponent;
    if (CharacterWeaponManagerComponent != 0) {
        DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(CharacterWeaponManagerComponent + Offset::CurrentWeaponReplicated);//STExtraWeapon* CurrentReloadWeapon;
        if (CurrentReloadWeapon != 0)
        {
            DWORD ShootWeaponEntity = Utility::ReadMemoryEx<DWORD>(CurrentReloadWeapon + Offset::ShootWeaponEntityComp);//ShootWeaponEntity* ShootWeaponEntityComp;//[Offset:
            if (ShootWeaponEntity != 0) { Utility::WriteMemoryEx<float>(ShootWeaponEntity + Offset::BulletTrackDistanceFix, 1000.0f); }
        }
    }
}
VOID BulletTrackMemoryCheck()
{

    DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CurrentWeapon);
    if (Data::STPlayerController && CurrentReloadWeapon)
    {
        DWORD PlayerCameraManager = Utility::ReadMemoryEx<DWORD>(Data::STPlayerController + Offset::PlayerCameraManager);
        if (PlayerCameraManager) {
            CameraCacheEntry CameraCached = Utility::ReadMemoryEx<CameraCacheEntry>(PlayerCameraManager + Offset::CameraCache);
            FVector CurrentViewAngle = CameraCached.POV.Location;
            bool bIsWeaponFiring = Utility::ReadMemoryEx<bool>(Data::LocalPlayer + Offset::bIsWeaponFiring);
            VECTOR3 HeadPos = BestTarget();
            if (HeadPos.X != 0 && HeadPos.Y != 0 && HeadPos.Z != 0 && AimForm::BestTargetID != -1)
            {
                VECTOR3 vec3CurrentViewAngle;
                vec3CurrentViewAngle.X = CurrentViewAngle.X;
                vec3CurrentViewAngle.Y = CurrentViewAngle.Y;
                vec3CurrentViewAngle.Z = CurrentViewAngle.Z;
                FRotator aimRotation = ToRotator(vec3CurrentViewAngle, HeadPos);
                CameraCached.POV.Rotation = aimRotation;
                if (Setting::BulletTrack::CameraCache) {
                    FixGayFake();
                    Utility::WriteMemoryEx<CameraCacheEntry>(PlayerCameraManager + Offset::CameraCache, CameraCached);
                }
                if (Setting::BulletTrack::OpenAim) {
                    FixGayFake();
                    Utility::WriteMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation, aimRotation);
                }
            }
        }
    }
}
VOID MouseMove(FLOAT x, FLOAT y) {
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dx = (LONG)x;
    Input.mi.dy = (LONG)y;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &Input, sizeof(INPUT));
}
VOID AutoFixMagic() {
    while (g_Running) {
        if (Data::NetDriver > 0) {
            if (Setting::TypeAim == 0) {
                if (IsMagicInitialized) {
                    AimForm::RestoreHook();
                    MagicBulletList.clear();
                    VirtualFreeEx(Utility::GameHandle, (LPVOID)MagicBulletHook, 0, MEM_RELEASE);
                    MagicBulletAddress = 0;
                    MagicBulletHook = 0;
                    ZeroMemory(RealByteCode, sizeof(RealByteCode));
                    IsMagicInitialized = false;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}
VOID LeftClick(void) {
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    ::SendInput(1, &Input, sizeof(INPUT));
    ::ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    ::SendInput(1, &Input, sizeof(INPUT));
}
inline bool IsAutoFiring = false;
VOID AutoFire()
{
    while (g_Running)
    {
        if (Data::NetDriver > 0 && Data::AActorList.size() > 0)
        {
            if (Setting::SpecialFeatures::AutoFire && Setting::BulletCheckVisible && (Data::IsFreeUser == -1 || Data::IsFreeUser == 0))
            {
                DWORD WeaponManagerComponent = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::WeaponManagerComponent);
                if (WeaponManagerComponent > 0)
                {
                    DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(WeaponManagerComponent + Offset::CurrentWeaponReplicated);
                    if (CurrentReloadWeapon > 0)
                    {
                        for (AActor& Actor : Data::AActorList)
                        {
                            if ((Actor.IsVisible == 1 || Actor.IsVisible == 2) && Setting::ShowMenu == false) {
                                if (Setting::FovAim > 0 && InsideFov(ScreenInfo::Width, ScreenInfo::Height, AimForm::ScreenBullet, Setting::FovAim))
                                {
                                    IsAutoFiring = true;
                                    LeftClick();
                                }
                                else if (Setting::FovAim <= 0)
                                {
                                    IsAutoFiring = true;
                                    LeftClick();
                                }
                            }
                        }
                    }

                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        IsAutoFiring = false;
    }
}
VOID AimForm::AutoTap()
{
    while (g_Running)
    {
        if (Data::NetDriver > 0)
        {
            if (Setting::Macro)
            {
                DWORD CurrentReloadWeapon = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CurrentReloadWeapon);
                if (Setting::AutoTap && Setting::ShowMenu == false && CurrentReloadWeapon > 0 && Data::LocalbIsWeaponFiring == true)
                {
                    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
                    {
                        SendMessage(ScreenInfo::hWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
                        SendMessage(ScreenInfo::hWindow, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Setting::AutoTapDelay));
    }
}
VOID AimForm::Macro() {
    auto hThread_1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AutoFire, 0, 0, 0);
    auto hThread_2 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AimForm::AutoTap, 0, 0, 0);
    Data::HandleList.push_back(hThread_1);
    Data::HandleList.push_back(hThread_2);
    int dwSpeedTimeMacro = 0;
    POINT dWPointerUpdate;
    while (g_Running) {
        if (Data::NetDriver > 0) {
            if (true) {
#pragma region Macro
                if (Setting::Macro) {
                    CURSORINFO ci = { sizeof(CURSORINFO) };
                    if (GetCursorInfo(&ci)) {
                        if (ci.flags == 0) // mouse hidden
                        {
                            bool IsScope = Utility::ReadMemoryEx<bool>(Data::LocalPlayer + Offset::bIsGunADS);
                            if (GetCursorPos(&dWPointerUpdate)) {
                                if (Setting::ShowMenu == false && Setting::MacroWithAim == false && GetAsyncKeyState(0x1) & 0x8000) {
                                    if (dwSpeedTimeMacro <= 1)
                                        dwSpeedTimeMacro += 1;
                                    if (dwSpeedTimeMacro >= 2 && Data::LocalbIsWeaponFiring && IsScope) {
                                        GetCursorPos(&dWPointerUpdate);
                                        MouseMove(0, Setting::MacroAim);
                                    }
                                }
                                else if (Setting::ShowMenu == false && Setting::MacroWithAim == true && (GetAsyncKeyState(0x1) & 0x8000) && (GetAsyncKeyState(Setting::KeyAim) & 0x8000) && AimForm::BestTargetID != -1) {
                                    if (dwSpeedTimeMacro <= 1)
                                        dwSpeedTimeMacro += 1;
                                    if (dwSpeedTimeMacro >= 2 && Data::LocalbIsWeaponFiring && IsScope) {
                                        GetCursorPos(&dWPointerUpdate);
                                        MouseMove(0, Setting::MacroAim);
                                    }
                                }
                                else {
                                    dwSpeedTimeMacro = 0;
                                }
                            }
                        }
                    }
                }
#pragma endregion
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
VOID MouseMoveSmooth(FLOAT x, FLOAT y)
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = (LONG)roundf(x);
    input.mi.dy = (LONG)roundf(y);
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(INPUT));
}
VOID RunAimbot()
{
    VECTOR3 Target = BestAimbotTarget();

    if (Target.X == 0.f && Target.Y == 0.f && Target.Z == 0.f)
        return;

    if (!(GetAsyncKeyState(Setting::KeyAim) & 0x8000))
        return;

    DWORD WeaponMgr = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::WeaponManagerComponent);
    DWORD Current = Utility::ReadMemoryEx<DWORD>(WeaponMgr + Offset::CurrentWeaponReplicated);

    if (!Current) return;

    VECTOR3 ScreenAim = {};

    if (!Algorithm::WorldToScreen(Target, ScreenAim, Data::ViewMatrixBase))
        return;

    float centerX = ScreenInfo::Width / 2.0f;
    float centerY = ScreenInfo::Height / 2.0f;

    float deltaX = ScreenAim.X - centerX;
    float deltaY = ScreenAim.Y - centerY;

    if (Setting::FovAim > 0 &&
        !InsideFov(ScreenInfo::Width, ScreenInfo::Height, { ScreenAim.X, ScreenAim.Y }, Setting::FovAim))
        return;

    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci) || ci.flags != 0)
        return;

    float Smooth = max(1.0f, Setting::SmoothAim);
    bool IsScoped = Utility::ReadMemoryEx<bool>(Data::LocalPlayer + Offset::bIsGunADS);
    float TargetDistance3D = Get3DDistance(Data::LocalPosition, Target) / 100.f;
    if (!IsScoped && TargetDistance3D <= 40.0f)
    {
        Smooth = max(1.0f, Setting::SmoothAim * 0.75f);
    }
    else if (IsScoped && TargetDistance3D <= 40.0f)
    {
        Smooth = max(1.0f, Setting::SmoothAim * 0.6f);
    }

    deltaX /= Smooth;
    deltaY /= Smooth;
    mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(deltaX), static_cast<DWORD>(deltaY), NULL, NULL);
    MouseMoveSmooth(deltaX, deltaY);
}
VOID AimForm::Aim() {
    VECTOR2 Aim;
    VECTOR2 Head;
    VECTOR3 Target;
    VECTOR3 ScreenAim;
    BOOL OriginalGetFovLimit;
    FLOAT OriginalGetFovLimitValue[6];
    auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AutoFixMagic, 0, 0, 0);
    Data::HandleList.push_back(hThread);
    while (g_Running) {
        if (Data::NetDriver > 0 && Data::LocalPlayer != 0) {
            if (Setting::AimAlternative) {
#pragma region BulletTrack
                if (Setting::TypeAim == 0) {
                    if (IsMagicInitialized == false && Setting::SpecialFeatures::V3Selector != 3) {
                        GetMagicAddress();
                        InitializeMagic();
                        HookMagic();
                        IsMagicInitialized = true;
                    }
                    Target = BestTarget();
                    if (Setting::BulletTrack::BypassFovLitmit == true)
                    {
                        DWORD PartHitComponent = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::PartHitComponent);
                        if (PartHitComponent)
                        {
                            DWORD ConfigCollisionDistSqAngles = Utility::ReadMemoryEx<DWORD>(PartHitComponent + Offset::ConfigCollisionDistSqAngles);
                            if (ConfigCollisionDistSqAngles)
                            {
                                if (OriginalGetFovLimit == false)
                                {
                                    OriginalGetFovLimitValue[0] = Utility::ReadMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x4);
                                    OriginalGetFovLimitValue[1] = Utility::ReadMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0xC);
                                    OriginalGetFovLimitValue[2] = Utility::ReadMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x14);
                                    OriginalGetFovLimitValue[3] = Utility::ReadMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x1C);
                                    OriginalGetFovLimitValue[4] = Utility::ReadMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x24);
                                    OriginalGetFovLimitValue[5] = Utility::ReadMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x2C);
                                    OriginalGetFovLimit = true;
                                }
                                Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x4, 180.0f);
                                Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0xC, 180.0f);
                                Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x14, 180.0f);
                                Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x1C, 180.0f);
                                Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x24, 180.0f);
                                Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x2C, 180.0f);
                            }
                        }
                    }
                    if (Data::AActorList.size() > 0)
                    {
                        if (Setting::FovAim > 0)
                        {
                            if ((GetAsyncKeyState(Setting::KeyAim) & 0x8000 || IsAutoFiring)) {
                                if (InsideFov(ScreenInfo::Width, ScreenInfo::Height, AimForm::ScreenBullet, Setting::FovAim) && Setting::SpecialFeatures::V3Selector != 3)
                                {
                                    if (MagicCoordinate.Pitch != 0 && MagicCoordinate.Yaw != 0)
                                    {
                                        BulletTrackMemoryCheck();
                                        HookMagic();
                                        Utility::WriteMemoryEx<FLOAT>(MagicBulletHook + 44, MagicCoordinate.Pitch);
                                        Utility::WriteMemoryEx<FLOAT>(MagicBulletHook + 54, MagicCoordinate.Yaw);
                                    }
                                }
                                else
                                {
                                    RestoreHook();
                                }
                            }
                        }
                        else if (Setting::FovAim <= 0)
                        {
                            if ((GetAsyncKeyState(Setting::KeyAim) & 0x8000 || IsAutoFiring) && Setting::FovAim <= 0 && Setting::SpecialFeatures::V3Selector != 3) {
                                if (MagicCoordinate.Pitch != 0 && MagicCoordinate.Yaw != 0)
                                {
                                    BulletTrackMemoryCheck();
                                    HookMagic();
                                    Utility::WriteMemoryEx<FLOAT>(MagicBulletHook + 44, MagicCoordinate.Pitch);
                                    Utility::WriteMemoryEx<FLOAT>(MagicBulletHook + 54, MagicCoordinate.Yaw);
                                }
                            }
                            else
                            {
                                RestoreHook();
                            }
                        }
                        if (GetAsyncKeyState(0x50) & 1) {
                            if (IsMagicInitialized == true) {
                                //DX11::InsertNotification({ ImGuiToastType_Info, 2500, (Setting::LanguageVNM == 1) ? u8"Đang làm mới theo dõi đạn" : "Refreshing Bullet Tracking" });
                                RestoreHook();
                                MagicBulletList.clear();
                                VirtualFreeEx(Utility::GameHandle, (LPVOID)MagicBulletHook, 0, MEM_RELEASE);
                                MagicBulletAddress = 0;
                                MagicBulletHook = 0;
                                ZeroMemory(RealByteCode, sizeof(RealByteCode));
                                IsMagicInitialized = false;
                            }
                        }
                    }
                    else { RestoreHook(); }
                }
#pragma endregion BulletTrack
#pragma region Aimbot
                if (Setting::TypeAim == 1) {
                    RunAimbot();
                }
                if (Setting::TypeAim == 2) {
                    Target = BestTarget();
                    if (GetAsyncKeyState(Setting::KeyAim) & 0x8000) {
                        if (Target.X > 0 && Target.Y > 0 && Target.Z > 0 && AimForm::BestTargetID != -1) {
                            DWORD PlayerCameraManager = Utility::ReadMemoryEx<DWORD>(Data::STPlayerController + Offset::PlayerCameraManager);
                            if (PlayerCameraManager)
                            {
                                CameraCacheEntry CameraCached = Utility::ReadMemoryEx<CameraCacheEntry>(PlayerCameraManager + Offset::CameraCache);
                                FVector CurrentViewAngle = CameraCached.POV.Location;
                                VECTOR3 vec3CurrentViewAngle;
                                vec3CurrentViewAngle.X = CurrentViewAngle.X;
                                vec3CurrentViewAngle.Y = CurrentViewAngle.Y;
                                vec3CurrentViewAngle.Z = CurrentViewAngle.Z;
                                FRotator aimRotation = ToRotator(vec3CurrentViewAngle, Target);
                                Utility::WriteMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation, aimRotation);
                            }
                        }
                    }
                }
                if (Setting::TypeAim == 3) {
                    Target = BestTarget();
                    if (GetAsyncKeyState(Setting::KeyAim) & 0x8000) {
                        if (Target.X > 0 && Target.Y > 0 && Target.Z > 0 && AimForm::BestTargetID != -1) {
                            DWORD CurrentWeaponReplicated = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::CurrentWeaponReplicated);
                            if (CurrentWeaponReplicated) {
                                FRotator ControlRotation;
                                ControlRotation = Utility::ReadMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation);
                                auto r = CalcAngleR(Data::GlobalCameraCache.Location, FVector{ Target.X, Target.Y, Target.Z });
                                r.Pitch -= ControlRotation.Pitch;
                                r.Yaw -= ControlRotation.Yaw;
                                r = ClampAnglesR(r);
                                ControlRotation.Pitch += r.Pitch * 0.0050 * Setting::SmoothAim;
                                ControlRotation.Yaw += r.Yaw * 0.0050 * Setting::SmoothAim;
                                Utility::WriteMemoryEx<FRotator>(Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::Controller) + Offset::ControlRotation, ControlRotation);
                            }
                        }
                    }
                }
#pragma endregion Aimbot
            }
            if ((Setting::AimAlternative == FALSE || (Setting::AimAlternative == TRUE && Setting::TypeAim != 0) || (Setting::TypeAim == 0 && Setting::BulletTrack::BypassFovLitmit == false)) && OriginalGetFovLimit == true)
            {
                DWORD PartHitComponent = Utility::ReadMemoryEx<DWORD>(Data::LocalPlayer + Offset::PartHitComponent);
                if (PartHitComponent) {
                    DWORD ConfigCollisionDistSqAngles = Utility::ReadMemoryEx<DWORD>(PartHitComponent + Offset::ConfigCollisionDistSqAngles);
                    if (ConfigCollisionDistSqAngles) {
                        Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x4, OriginalGetFovLimitValue[0]);
                        Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0xC, OriginalGetFovLimitValue[1]);
                        Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x14, OriginalGetFovLimitValue[2]);
                        Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x1C, OriginalGetFovLimitValue[3]);
                        Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x24, OriginalGetFovLimitValue[4]);
                        Utility::WriteMemoryEx<FLOAT>(ConfigCollisionDistSqAngles + 0x2C, OriginalGetFovLimitValue[5]);
                        OriginalGetFovLimit = false;
                    }
                }
            }
            if (Setting::AimAlternative == FALSE || (Setting::AimAlternative == TRUE && Setting::TypeAim != 0)) {
#pragma region DetachTrack
                if (IsMagicInitialized) {
                    //ImGui::InsertNotification({ ImGuiToastType_Info, 2500, (Setting::LanguageVNM == 1) ? u8"Đang xóa danh sách ma thuật" : "Clearing magic list" });
                    RestoreHook();
                    MagicBulletList.clear();
                    VirtualFreeEx(Utility::GameHandle, (LPVOID)MagicBulletHook, 0, MEM_RELEASE);
                    MagicBulletAddress = 0;
                    MagicBulletHook = 0;
                    ZeroMemory(RealByteCode, sizeof(RealByteCode));
                    IsMagicInitialized = false;
                }
#pragma endregion DetachTrack
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
