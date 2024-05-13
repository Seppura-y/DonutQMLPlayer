#include "donut_scene_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Donut
{
    DonutSceneCamera::DonutSceneCamera(float fov, float aspect_ratio, float near_clip, float far_clip)
    {

    }

    void DonutSceneCamera::setOrthographic(float size, float near_clip, float far_clip)
    {
        projection_type_ = ProjectionType::Orthographic;

        orthographic_size_ = size;
        orthographic_near_ = near_clip;
        orthographic_far_ = far_clip;

        recalculateProjection();
    }

    void DonutSceneCamera::setPerspective(float fov, float near_clip, float far_clip)
    {
        projection_type_ = ProjectionType::Perspective;

        perspective_fov_ = glm::radians(fov);
        perspective_near_ = near_clip;
        perspective_far_ = far_clip;
    }

    void DonutSceneCamera::setViewportSize(uint32_t width, uint32_t height)
    {
        //DN_CORE_ASSERT(width > 0 && height > 0, "invalid width or height values");
        aspect_ratio_ = (float)width / (float)height;
        recalculateProjection();
    }

    void DonutSceneCamera::recalculateProjection()
    {
        if (projection_type_ == ProjectionType::Orthographic)
        {
            float ortho_left = -orthographic_size_ * aspect_ratio_ * 0.5f * zoom_level_;
            float ortho_right = orthographic_size_ * aspect_ratio_ * 0.5f * zoom_level_;
            float ortho_bottom = -orthographic_size_ * 0.5f * zoom_level_;
            float ortho_top = orthographic_size_ * 0.5f * zoom_level_;

            projection_ = glm::ortho(
                ortho_left, ortho_right,
                ortho_bottom, ortho_top,
                orthographic_near_, orthographic_far_);
        }
        else if (projection_type_ == ProjectionType::Perspective)
        {
            projection_ = glm::perspective(glm::radians(perspective_fov_), aspect_ratio_, perspective_near_, perspective_far_);
        }
    }
}