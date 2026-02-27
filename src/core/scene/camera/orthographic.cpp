#include "core/scene/camera/orthographic.h"

void camera_UpdateOrtho(CameraOrthographic* camera)
{
    float half_height = 1.0f / camera->zoom;
    float half_width = half_height * camera->aspect_ratio;

    float left = camera->position.x - half_width;
    float right = camera->position.x + half_width;
    float bottom = camera->position.y - half_height;
    float top = camera->position.y + half_height;

    camera->projection = mat4_orthographic(
        left,
        right,
        bottom,
        top,
        -1.0f,
        1.0f
    );
}
