#pragma once
#include <iostream>
#include <Windows.h>
#include "../src/setting/Setting.h"
#include "../src/Utility/Utility.h"
#include "../src/Algorithm/Algorithm.h"
#include "../src/data/Data.h"
#include "../src/hooks/hooks.hpp"
#include "../Teammate/Teammate.h"
using namespace std;
namespace AimForm {
    extern VOID Macro();
    extern VOID Aim();
    extern VOID RestoreHook();
    extern VOID AutoTap();
    extern INT BestTargetID;
    extern AActor BestAActor;
    extern VECTOR2 ScreenBullet;
}
