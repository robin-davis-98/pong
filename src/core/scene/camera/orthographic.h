#ifndef PET_ORTHOGRAPHIC_H
#define PET_ORTHOGRAPHIC_H

#include "petrichor_math.h"

struct CameraOrthographic
{
    Vec2 position;
    float rotation;
    float zoom;
    float aspect_ratio;

    Mat4 projection;
};

void camera_UpdateOrtho(CameraOrthographic* camera);

#endif