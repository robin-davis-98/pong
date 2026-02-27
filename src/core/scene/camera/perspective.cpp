#include "core/scene/camera/perspective.h"

#include "petrichor_math.h"

void camera_UpdatePerspective(CameraPerspective* camera)
{
    camera->projection_matrix = mat4_perspective(
        camera->fov,
        camera->aspect_ratio,
        camera->near_clip,
        camera->far_clip
        );

    Vec3 target = {0, 0, 0};
    Vec3 up = {0, 0, 1};
    camera->view_matrix = mat4_lookat(camera->position, target, up);
}
