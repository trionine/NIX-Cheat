#include "Vector.h"
#include <array>
namespace MathV {
    void VectorAnglesRadar(FVector& forward, FVector& angles);
    void RotateTriangle(std::array<FVector, 3>& points, float rotation);
    FVector2D WorldToRadar(FRotator Rotation, FVector CameraLocation, FVector Origin, FVector2D RadarPosition, FVector2D RadarSize);
} // namespace Utils
