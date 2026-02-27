#ifndef PET_MATH_H
#define PET_MATH_H

struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };

struct Mat4
{
    float m[16];
};

Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far);
Mat4 mat4_perspective(float fov, float aspect, float near, float far);
Mat4 mat4_lookat(Vec3 eye, Vec3 target, Vec3 up);
Mat4 mat4_mul(Mat4 a, Mat4 b);

#endif