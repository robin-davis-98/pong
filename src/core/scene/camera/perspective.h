#ifndef PET_PERSPECTIVE_H
#define PET_PERSPECTIVE_H

#include "petrichor_math.h"

struct CameraPerspective
{
    Vec3 position;
    Vec3 rotation;
    float fov;
    float aspect_ratio;
    float near_clip;
    float far_clip;

    Mat4 projection_matrix;
    Mat4 view_matrix;
};

void camera_UpdatePerspective(CameraPerspective* camera);

#endif