#include "MathV.h"
#define IM_PI 3.14159265358979323846f
#define RAD2DEG(x) ((float)(x) * (float)(180.f / IM_PI))
#define DEG2RAD(x) ((float)(x) * (float)(IM_PI / 180.f))
namespace MathV {
    void VectorAnglesRadar(FVector& forward, FVector& angles) {
        if (forward.X == 0.f && forward.Y == 0.f) {
            angles.X = forward.Z > 0.f ? -90.f : 90.f;
            angles.Y = 0.f;
        } else {
            angles.X = RAD2DEG(atan2(-forward.Z, forward.Size( )));
            angles.Y = RAD2DEG(atan2(forward.Y, forward.X));
        }
        angles.Z = 0.f;
    }
    void RotateTriangle(std::array<FVector, 3>& points, float rotation) {
        const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
        for (auto& point : points) {
            point = point - points_center;

            const auto temp_x = point.X;
            const auto temp_y = point.Y;

            const auto theta = DEG2RAD(rotation);
            const auto c = cosf(theta);
            const auto s = sinf(theta);

            point.X = temp_x * c - temp_y * s;
            point.Y = temp_x * s + temp_y * c;

            point = point + points_center;
        }
    }
    FVector2D WorldToRadar(FRotator Rotation, FVector CameraLocation, FVector Origin, FVector2D RadarPosition, FVector2D RadarSize) {
        FVector2D DotPos;
        FVector2D Direction;

        // Get Origin Of Entity
        FVector EntityPosition = Origin;

        // Get Origin Of LocalPlayer
        FVector LocalPosition = CameraLocation;

        // Calculate Direction
        Direction.X = EntityPosition.Y - LocalPosition.Y;
        Direction.Y = EntityPosition.X - LocalPosition.X;

        // Get Rotation
        float LocalAngles = Rotation.Yaw;

        float Radian = DEG2RAD(LocalAngles);

        // Calculate Raw DotPos
        DotPos.X = (Direction.X * (float)cos(Radian) - Direction.Y * (float)sin(Radian)) / 150.0f;
        DotPos.Y = (Direction.Y * (float)cos(Radian) + Direction.X * (float)sin(Radian)) / 150.0f;

        // Add RadarPos To Calculated DotPos
        DotPos.X = DotPos.X + RadarPosition.X + RadarSize.X / 2.f;
        DotPos.Y = -DotPos.Y + RadarPosition.Y + RadarSize.Y / 2.f;

        // Clamp Dots To RadarSize ( Where 18 = Width/Height of the Dot)
        if (DotPos.X < RadarPosition.X)
            DotPos.X = RadarPosition.X;

        if (DotPos.X > RadarPosition.X + RadarSize.X - 18)
            DotPos.X = RadarPosition.X + RadarSize.X - 18;

        if (DotPos.Y < RadarPosition.Y)
            DotPos.Y = RadarPosition.Y;

        if (DotPos.Y > RadarPosition.Y + RadarSize.Y - 18)
            DotPos.Y = RadarPosition.Y + RadarSize.Y - 18;

        return DotPos;
    }
};
