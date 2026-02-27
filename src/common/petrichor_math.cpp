#include "petrichor_math.h"

#include <cmath>
#include <petrichor_math.h>
#include <string.h>

static Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return { a.x - b.x,a.y - b.y,a.z - b.z };
}

static float vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
    };
}

static Vec3 vec3_normalize(Vec3 v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0.0f) return { 0, 0, 0};
    return { v.x / len, v.y / len, v.z / len };
}

Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far) {
    Mat4 res = {0};
    res.m[0]  = 2.0f / (right - left);
    res.m[5]  = 2.0f / (top - bottom);
    res.m[10] = -2.0f / (far - near);
    res.m[12] = -(right + left) / (right - left);
    res.m[13] = -(top + bottom) / (top - bottom);
    res.m[14] = -(far + near) / (far - near);
    res.m[15] = 1.0f;
    return res;
}

Mat4 mat4_perspective(float fov_deg, float aspect, float near, float far) {
    Mat4 res = {0};
    float fov_rad = fov_deg * (3.1415926535f / 180.0f);
    float tanHalfFovy = tanf(fov_rad / 2.0f);

    res.m[0]  = 1.0f / (aspect * tanHalfFovy);
    res.m[5]  = 1.0f / (tanHalfFovy);
    res.m[10] = -(far + near) / (far - near);
    res.m[11] = -1.0f;
    res.m[14] = -(2.0f * far * near) / (far - near);
    return res;
}

Mat4 mat4_lookat(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = vec3_normalize(vec3_sub(target, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);

    Mat4 res = {0};
    res.m[0] = s.x;
    res.m[4] = s.y;
    res.m[8] = s.z;

    res.m[1] = u.x;
    res.m[5] = u.y;
    res.m[9] = u.z;

    res.m[2] = -f.x;
    res.m[6] = -f.y;
    res.m[10] = -f.z;

    res.m[12] = -vec3_dot(s, eye);
    res.m[13] = -vec3_dot(u, eye);
    res.m[14] =  vec3_dot(f, eye);
    res.m[15] = 1.0f;

    return res;
}

Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 res = {0};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            for (int k = 0; k < 4; ++k) {
                res.m[col * 4 + row] += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
        }
    }
    return res;
}