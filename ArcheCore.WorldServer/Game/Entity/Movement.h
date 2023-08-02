#pragma once

#include "common_archecore.h"
#include "common_entity.h"
#include "packet.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class WorldSession;

namespace e {

#pragma region Enums

	enum class EMoveType : uint8_t {
		DEFAULT = 0,
		UNIT = 1,
		VEHICLE = 2,
		SHIP = 3,
		SHIP_REQUEST = 4,
		TRANSFER = 5
	};

#pragma endregion

	/// <summary>
	/// Will hold all movement data from a net packet only on the stack, never meant
	/// to allocate on the heap long term. The relevant data will move into entity
	/// components under the frame this is used...
	/// </summary>
	struct MovementPacketData {
	public:
		MovementPacketData(EMoveType type) {
			Type = type;
		}

		void Read(packet& p);
		void Write(packet& p);

		void HandleUnitMovement(entt::registry& world, entt::entity movedEntity);

	private:
		void ReadUnitMovementData(packet& p);
		void WriteUnitMovementData(packet& p);

	public:
		EMoveType Type = EMoveType::DEFAULT;

		glm::uint32 Time = 0;
		glm::uint8 Flags = 0;
		glm::uint8 ActorFlags = 0;

		glm::uint8 GcFlags = 0;
		glm::uint32 GcId = 0;
		glm::uint16 GcPartId = 0;

		glm::uint32 ScType = 0;
		glm::uint8 Phase = 0;
		glm::int8 Stance = 0;
		glm::int8 Alertness = 0;
		glm::uint32 ClimbData = 0;

		glm::vec3 Pos = {};
		glm::vec3 Pos2 = {};

		//glm::quat Rot = glm::identity<glm::quat>();
		glm::i8vec3 RotSBytes = {};
		glm::i8vec3 RotSBytes2 = {};

		//glm::vec3 Vel = {};
		glm::i16vec3 VelShort = {};
		glm::uint16 FallVel = 0;

		glm::i8vec3 DeltaMov = {};
	};

#pragma region Components

	/// <summary>
	/// If an entity has a parent we put this component on the child, storing the parent entity ID.
	/// </summary>
	struct ParentExist {
		entt::entity ParentEntity;
	};

#pragma endregion

	class MovementSystem : public entt::dispatcher {
	public:
		MovementSystem(entt::registry& world_) : world(world_) {
			// Subscribe to events, if needed
		}
		void Update(float fixedDeltaTime);

	private:
		entt::registry& world;
	};
}