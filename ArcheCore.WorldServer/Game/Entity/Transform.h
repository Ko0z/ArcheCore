#pragma once

#include "common_archecore.h"
#include "common_entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

struct Transform
{
	glm::vec3 GetEulerRotation() const {
		return glm::eulerAngles(Rotation);
	}

	glm::vec3 GetEulerRotationDegrees() const {
		return glm::degrees(glm::eulerAngles(Rotation));;
	}

	glm::i8vec3 GetSByteRotation() {
		using namespace archecore::utils;

		glm::i8vec3 sByteRotation;
		glm::vec3 eulerRotation = GetEulerRotationDegrees();

		sByteRotation.x = (int8)(eulerRotation.x / PI_X2 / ToSByteDivider);
		sByteRotation.y = (int8)(eulerRotation.y / PI_X2 / ToSByteDivider);
		sByteRotation.z = (int8)(eulerRotation.z / PI_X2 / ToSByteDivider);

		return sByteRotation;
	}

	void SetPosition(glm::vec3 position) {
		Position = position;
	}

	void SetRotation(glm::vec3 rotation) {
		Rotation = glm::quat(rotation);
	}

	void SetRotationWithSBytes(glm::i8vec3 rotation) {
		glm::vec3 eulerRotation;
		eulerRotation.x = archecore::utils::ConvertSByteDirectionToDegree(rotation.x);
		eulerRotation.y = archecore::utils::ConvertSByteDirectionToDegree(rotation.y);
		eulerRotation.z = archecore::utils::ConvertSByteDirectionToDegree(rotation.z);

		Rotation = glm::quat(eulerRotation);
	}

	/// Temp, for spawn
	uint32_t ZoneId = 0;

	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat Rotation = glm::identity<glm::quat>();
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct Velocity
{
	glm::vec3 Linear = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Angular = glm::vec3(0.0f, 0.0f, 0.0f);
};