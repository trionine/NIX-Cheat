#pragma once
#include <iostream>
#include <Windows.h>
#include "../dependencies/imgui/imgui.h"
#define M_PI 3.14159265358979323846f
struct VECTOR2 {
    FLOAT X;
    FLOAT Y;
};
struct VECTOR3 {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
};
struct VECTOR4 {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    FLOAT W;
};
struct D3DMATRIX1 {
    FLOAT _11, _12, _13, _14;
    FLOAT _21, _22, _23, _24;
    FLOAT _31, _32, _33, _34;
    FLOAT _41, _42, _43, _44;
};
struct FTTransform {
    VECTOR4 Rotation;
    VECTOR3 Translation;
    FLOAT _Padding;
    VECTOR3 Scale3D;
};
namespace Algorithm {
    VECTOR3 WorldToRadar(float Yaw, VECTOR3 Origin, VECTOR3 LocalOrigin, float PosX, float PosY, VECTOR3 Size, bool& outbuff);
    BOOL WorldToScreen(VECTOR3 Position, VECTOR3& Screen, D3DMATRIX1 ViewMatrix);
    BOOL WorldToScreenPlayer(VECTOR3 Position, VECTOR3& Screen, D3DMATRIX1 ViewMatrix);
    BOOL WorldToScreenBone(D3DMATRIX1 ViewMatrix, VECTOR3 Position, VECTOR2& Screen);
    D3DMATRIX1 ToMatrixWithScale(VECTOR3 Translation, VECTOR3 Scale, VECTOR4 Rot);
    D3DMATRIX1 MatrixMultiplication(D3DMATRIX1 pM1, D3DMATRIX1 pM2);
    VECTOR3 GetBoneWorldPosition(FTTransform Actor, DWORD BoneAddv);
    VECTOR3 getBoneWithRotation(uintptr_t entity, int id, DWORD OffsetMesh, DWORD Rootcomponent);
    void Draw3Dcircle(D3DMATRIX1 matrix, VECTOR3 LPosition, VECTOR3 position, float points, float radius, float thickness, ImColor Color);
} // namespace Algorithm
