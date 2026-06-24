import re
import sys

new_offsets_text = """
const DWORD CurrentWeapon = 0x20CC;
const DWORD STExtraBaseCharacter = 0x1F50;
const DWORD GameState = 0x2F8;
const DWORD bNoAliveHumanPlayer = 0x8C0;
const DWORD AlivePlayerNum = 0x934;
const DWORD ElapsedTime = 0x3F0;
const DWORD NoneAIGameTime = 0x93C;
const DWORD PlayerNum = 0x654;
const DWORD RealPlayerNum = 0x654;
const DWORD AliveTeamNum = 0x938;
const DWORD PlayerNumPerTeam = 0xC2C;
const DWORD GameModeID = 0xD38;
const DWORD CurCircleWave = 0x8BC;
const DWORD GameReplayType = 0x7D8;
const DWORD PersistentLevel = 0x20;
const DWORD NetDriver = 0x24;
const DWORD ServerConnection = 0x64;
const DWORD PlayerController = 0x20;
const DWORD AcknowledgedPawn = 0x400;
const DWORD RootComponent = 0x1B0;
const DWORD CurrentStates = 0xC48;
const DWORD LastUpdateVelocity = 0x2B0;
const DWORD TeamId = 0x748;
const DWORD IsBot = 0x7ED;
const DWORD Name = 0x720;
const DWORD Nation = 0x72C;
const DWORD IsDead = 0xB00;
const DWORD PlayerUID = 0x73C;
const DWORD Health = 0xAE8;
const DWORD HealthMax = 0xAEC;
const DWORD NearDeathBreath = 0x1570;
const DWORD NearDeatchComponent = 0x1560;
const DWORD BreathMax = 0x164;
const DWORD ControlRotation = 0x3C8;
const DWORD ComponentVelocity = 0x250;
const DWORD CurrentVehicle = 0xB24;
const DWORD ReplicatedMovement = 0xD8;
const DWORD RelativeLocation = 0x178;
const DWORD RelativeRotation = 0x184;
const DWORD Mesh = 0x3F0;
const DWORD BodyAddv = 0x1A0;
const DWORD MinLOD = 0x7F4;
const DWORD CurrentWeaponReplicated = 0x4BC;
const DWORD WeaponManagerComponent = 0x1D80;
const DWORD BulletTrackDistance = 0x774;
const DWORD CurrentReloadWeapon = 0x25D4;
const DWORD ShootWeaponComponent = 0xCA0;
const DWORD ShootMode = 0xD95;
const DWORD OwnerShootWeapon = 0x23C;
const DWORD CurBulletNumInClip = 0x0;
const DWORD CurMaxBulletNumInOneClip = 0xD08;
const DWORD ShootWeaponEntity = 0xF8C;
const DWORD ShootWeaponEffectComp = 0xF90;
const DWORD CameraShakeInnerRadius = 0x218;
const DWORD CameraShakeOuterRadius = 0x1B0;
const DWORD CameraShakFalloff = 0x220;
const DWORD CharacterOverrideAttrs = 0xED0;
const DWORD BulletFireSpeed = 0x484;
const DWORD BulletMomentum = 0x5C0;
const DWORD BulletRange = 0x5D0;
const DWORD BaseImpactDamage = 0x5B4;
const DWORD VehicleDamageScale = 0x5BC;
const DWORD LaunchGravityScale = 0x488;
const DWORD ShootInterval = 0x4B4;
const DWORD bHasSingleFireMode = 0x500;
const DWORD bHasAutoFireMode = 0x501;
const DWORD bHasBurstFireMode = 0x502;
const DWORD BurstShootInterval = 0x530;
const DWORD ReloadRate = 0x7BC;
const DWORD AccessoriesVRecoilFactor = 0x9C4;
const DWORD AccessoriesHRecoilFactor = 0x9CC;
const DWORD AccessoriesRecoveryFactor = 0x9C8;
const DWORD ShotGunCenterPerc = 0xA20;
const DWORD ShotGunVerticalSpread = 0xA24;
const DWORD ShotGunHorizontalSpread = 0xA28;
const DWORD GameDeviationFactor = 0xA18;
const DWORD GameDeviationAccuracy = 0xA1C;
const DWORD AccessoriesDeviationFactor = 0x9DC;
const DWORD VehicleWeaponDeviationAngle = 0xA38;
const DWORD RecoilKickADS = 0xAD8;
const DWORD HP = 0x27C;
const DWORD HPMax = 0x278;
const DWORD Fuel = 0x32C;
const DWORD FuelMax = 0x328;
const DWORD VehicleMovement = 0x182C;
const DWORD VehicleCommon = 0x95C;
const DWORD lastForwardSpeed = 0x9F0;
const DWORD PickUpDataList = 0x788;
const DWORD STPlayerController = 0x3B28;
const DWORD PlayerCameraManager = 0x410;
const DWORD CameraCache = 0x420;
const DWORD ScopeFov = 0x1650;
const DWORD ScopeCameraComp = 0x1694;
const DWORD IsFPPGameMode = 0x7E8;
const DWORD SwitchWeaponSpeedScale = 0x2200;
const DWORD CharacterParachuteComponent = 0x120C;
const DWORD CurrentFallSpeed = 0x1A8;
const DWORD PoseState = 0x1228;
const DWORD Position = 0x1B8;
const DWORD CharacterMovement = 0x3F4;
const DWORD SpringArmComp = 0x167C;
const DWORD MeshContainer = 0x1508;
const DWORD STPlayerController1 = 0x3B28;
const DWORD HitPerform = 0x4B0;
const DWORD bIsEngineStarted = 0x92C;
const DWORD ExtraBoostFactor = 0x199C;
const DWORD Status = 0x3C0;
const DWORD RelativeScale3D = 0x190;
const DWORD ShootWeaponEntityComp = 0xF8C;
const DWORD STCharacterMovement = 0x1758;
const DWORD bIsFPPOnVehicle = 0x11B0;
const DWORD IsNetFPP = 0x1970;
const DWORD IsGameModeFpp = 0x7C8;
const DWORD GameModeState = 0x8D8;
const DWORD IsCanSwitchFPP = 0x7E9;
const DWORD bIsGunADS = 0xCF4;
const DWORD bIsAirOpen = 0x5E0;
const DWORD Kills = 0x5A0;
const DWORD bIsWeaponFiring = 0x1218;
const DWORD AvatarComponent2 = 0x23E0;
const DWORD SynData = 0x5F8;
const DWORD NetAvatarData = 0x300;
const DWORD ServerZoneId = 0x768;
"""

replacements = {}
for line in new_offsets_text.strip().split('\n'):
    line = line.strip()
    if not line: continue
    # match `const DWORD Name = 0x123;`
    match = re.match(r'const\s+DWORD\s+(\w+)\s*=\s*(0x[0-9A-Fa-f]+);', line)
    if match:
        name = match.group(1)
        val = match.group(2)
        replacements[name] = val.lower()

with open('e:/LB TOOLS/SRC/4.4 Stealth - NIX HACK/UniversalHookX/src/offset.h', 'r', encoding='utf-8') as f:
    content = f.read()

# For each line in content, check if it defines one of the DWORDS
lines = content.split('\n')
for i, line in enumerate(lines):
    match = re.match(r'^(\s*const\s+DWORD\s+)(\w+)(\s*=\s*)(0x[0-9a-fA-F]+)(.*)', line)
    if match:
        prefix = match.group(1)
        name = match.group(2)
        eq_space = match.group(3)
        old_val = match.group(4)
        suffix = match.group(5)
        
        if name in replacements:
            new_val = replacements[name]
            # preserve original case or just format as 0xabc
            lines[i] = f"{prefix}{name}{eq_space}{new_val}{suffix}"

with open('e:/LB TOOLS/SRC/4.4 Stealth - NIX HACK/UniversalHookX/src/offset.h', 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))

print("Successfully updated offsets.")
