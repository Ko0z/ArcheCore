#include "Movement.h"
#include "packet.h"
#include "Entity/Transform.h"

namespace e {

	void MovementPacketData::Read(packet& p)
	{
		p >> Time >> Flags;

		if ((Flags & 0x10) == 0x10) {
			p >> ScType >> Phase;
		}

		switch (Type)
		{
			case e::EMoveType::DEFAULT:
				break;
			case e::EMoveType::UNIT:
				ReadUnitMovementData(p);
				break;
			case e::EMoveType::VEHICLE:
				break;
			case e::EMoveType::SHIP:
				break;
			case e::EMoveType::SHIP_REQUEST:
				break;
			case e::EMoveType::TRANSFER:
				break;
		}
	}

	void MovementPacketData::Write(packet& p)
	{
		p << Time << Flags;

		if ((Flags & 0x10) == 0x10)
			p << ScType << Phase;

		switch (Type)
		{
			case e::EMoveType::DEFAULT:
				break;
			case e::EMoveType::UNIT:
				WriteUnitMovementData(p);
				break;
			case e::EMoveType::VEHICLE:
				break;
			case e::EMoveType::SHIP:
				break;
			case e::EMoveType::SHIP_REQUEST:
				break;
			case e::EMoveType::TRANSFER:
				break;
		}

	}

	void MovementPacketData::ReadUnitMovementData(packet& p)
	{
		p.ReadPosition(Pos);

		//spdlog::info("Converted Pos: x:{}, y:{}, z:{}", Pos.x, Pos.y, Pos.z);
		//spdlog::info("Flags: {:08b}", Flags);

		p >> VelShort.x >> VelShort.y >> VelShort.z
			>> RotSBytes.x >> RotSBytes.y >> RotSBytes.z
			>> DeltaMov.x >> DeltaMov.y >> DeltaMov.z
			>> Stance >> Alertness >> ActorFlags;

		// Is falling?
		if ((ActorFlags & 0x80) == 0x80)
			p >> FallVel;
		
		if ((ActorFlags & 0x20) == 0x20) {
			p >> GcFlags >> GcPartId;
			p.ReadPosition(Pos2);
			p >> RotSBytes2.x >> RotSBytes2.y >> RotSBytes2.z;
		}

		if ((ActorFlags & 0x60) != 0)
			p >> GcId;
		
		if ((ActorFlags & 0x40) == 0x40)
			p >> ClimbData;
	}

	void MovementPacketData::WriteUnitMovementData(packet& p)
	{
		p.WritePosition(Pos);
		p << VelShort.x << VelShort.y << VelShort.z;
		p << RotSBytes.x << RotSBytes.y << RotSBytes.z;
		p << DeltaMov.x << DeltaMov.y << DeltaMov.z;
		p << Stance << Alertness << ActorFlags;

		if ((ActorFlags & 0x80) == 0x80)
			p << FallVel;

		if ((ActorFlags & 0x20) == 0x20) {
			p << GcFlags << GcPartId;
			p.WritePosition(Pos2);
			p << RotSBytes2.x << RotSBytes2.y << RotSBytes2.z;
		}

		if ((ActorFlags & 0x60) != 0)
			p << GcId;

		if ((ActorFlags & 0x40) == 0x40)
			p << ClimbData;
	}

	void MovementPacketData::HandleUnitMovement(entt::registry& world, entt::entity movedEntity)
	{
		if (movedEntity == entt::null) {
			spdlog::info("[Movement] Entity null in HandleUnitMovement!");
			return;
		}

		Transform* transform = world.try_get<Transform>(movedEntity);
		Velocity* velocity = world.try_get<Velocity>(movedEntity);

		if (transform == nullptr || velocity == nullptr) {
			spdlog::info("[Movement] Transform or Velocity component is missing on entity!");
			return;
		}

		// Check if the entity is a Mount
			// Do Mount stuff
			// return
		
		// Check if the entity is a player who is riding a mount
			// Do player riding mount stuff
			// return
		
		// Else do ordinary stuff...

		// Check if this entity has a parent and GcId != 1

		// Remove effects that should get removed when units moves

		// Actually update the Unit Transform with the new Position and Rotation
		transform->SetPosition(Pos);
		transform->SetRotationWithSBytes(RotSBytes);
		// Send the information to everyone around.
	}

#pragma region Systems

	

	//void UpdateTransform(entt::registry& world, float deltaTime) {
	//	auto query = world.view<Transform, Velocity>();
	//
	//	for (auto entity : query) {
	//		auto& transform = query.get<Transform>(entity);
	//		auto& velocity = query.get<Velocity>(entity);
	//
	//		transform.Position += velocity.Velocity * deltaTime;
	//		transform.Rotation += glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime;
	//	}
	//}

	/// <summary>
	/// After we have updated all transforms no matter child or parent, we apply the parent entity move changes to their children
	/// in the correct order so that the "youngest" child get all parent movement updates..
	/// </summary>
	/// <param name="world"></param>
	/*
	void UpdateChildTransforms(entt::registry& world) {
		auto query = world.view<Transform, ParentExist>();
		std::vector<entt::entity> childEntities;

		for (auto entity : query) {
			childEntities.push_back(entity);
		}

		std::sort(childEntities.begin(), childEntities.end(), [&world](entt::entity a, entt::entity b) {
			int depthA = 0;
			int depthB = 0;

			// Calculate depth of A
			auto parentA = world.try_get<ParentExist>(a);
			while (parentA) {
				depthA++;
				parentA = world.try_get<ParentExist>(parentA->ParentEntity);
			}

			// Calculate depth of A
			auto parentB = world.try_get<ParentExist>(b);
			while (parentB) {
				depthB++;
				parentB = world.try_get<ParentExist>(parentB->ParentEntity);
			}

			return depthA > depthB;
			});

		for (auto entity : childEntities) {
			auto& transform = query.get<Transform>(entity);
			auto& parent = query.get<ParentExist>(entity);

			if (world.valid(parent.ParentEntity)) {
				auto& parentTransform = world.get<Transform>(parent.ParentEntity);

				transform.Position = parentTransform.Position + transform.Position;
				transform.Rotation = parentTransform.Rotation + transform.Rotation;
				transform.Scale = parentTransform.Scale * transform.Scale;
			}

		}
	}
	*/

	void MovementSystem::Update(float fixedDeltaTime)
	{

	}

#pragma endregion

	

}
