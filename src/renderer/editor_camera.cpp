#include "editor_camera.h"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMANTAL
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
namespace Donut
{
	EditorCamera::EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip)
		:fov_(fov), aspect_ratio_(aspect_ratio), near_clip_(near_clip), far_clip_(far_clip),
		Camera(glm::perspective(glm::radians(fov),aspect_ratio_, near_clip_, far_clip_))
	{
		updateView();
	}


	glm::vec3 EditorCamera::getUpDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getRightDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getForwardDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCamera::getOrientation() const
	{
		// 使用欧拉角创建四元数
		return glm::quat(glm::vec3(-pitch_, yaw_, 0.0f));
	}

	void EditorCamera::updateProjection()
	{
		aspect_ratio_ = viewport_width_ / viewport_height_;
		projection_ = glm::perspective(glm::radians(fov_), aspect_ratio_, near_clip_, far_clip_);
	}

	void EditorCamera::updateView()
	{
		// 2D模式下，禁用X Y轴的旋转
		//yaw_ = 0.0f;
		//pitch_ = 0.0f;


		position_ = calculatePosition();

		glm::quat orientation = getOrientation();
		view_matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::toMat4(orientation);
		view_matrix_ = glm::inverse(view_matrix_);
	}


	void EditorCamera::mousePan(const glm::vec2& delta)
	{
		//auto [x_speed, y_speed] = panSpeed();
		//focal_point_ += -getRightDirection() * delta.x * x_speed * distance_;
		//focal_point_ += getUpDirection() * delta.y * y_speed * distance_;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta)
	{
		float yaw_sign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		yaw_ += yaw_sign * delta.x * rotationSpeed();
		pitch_ += delta.y * rotationSpeed();
	}

	void EditorCamera::mouseZoom(float delta)
	{
		distance_ -= delta * zoomSpeed();
		if (distance_ < 1.0f)
		{
			focal_point_ += getForwardDirection();
			distance_ = 1.0f;
		}
	}

	glm::vec3 EditorCamera::calculatePosition() const
	{
		return focal_point_ - getForwardDirection() * distance_;
	}



	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::zoomSpeed() const
	{
		float distance = distance_ * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}
}