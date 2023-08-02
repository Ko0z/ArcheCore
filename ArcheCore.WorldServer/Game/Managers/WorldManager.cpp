#include "WorldManager.h"
#include "Entity/Transform.h"

using namespace e;

void WorldManager::Initialize()
{
	Worlds[WORLD_DEFAULT_ID] = entt::registry();
}

/// <summary>
/// Update all world instances
/// </summary>
void WorldManager::Update(float fixedDeltaTime)
{
	
}

inline entt::registry& WorldManager::GetWorld(uint32 id)
{
	if (Worlds.contains(id)) {
		return Worlds[id];
	}
	spdlog::error("Tried to get world that doesn't exist!");

	entt::registry emptyWorld = entt::registry();
	return emptyWorld;
}

void WorldManager::SpawnCharacter(entt::entity characterEntity, uint32 worldId)
{
	if (worldId == WORLD_DEFAULT_ID) {
		entt::registry& world = Worlds[worldId];

		//WorldSpawnCharacter& spawnInfo = world.get<WorldSpawnCharacter>(characterEntity);
		//Transform& transform = world.emplace<Transform>(characterEntity);
		//transform.Position = glm::vec3(spawnInfo.X, spawnInfo.Y, spawnInfo.Z);

		Velocity& velocity = world.emplace<Velocity>(characterEntity);
		
		WorldComponent& worldComp = world.get<WorldComponent>(characterEntity);
		worldComp.Id = worldId;

		//world.erase<WorldSpawnCharacter>(characterEntity);

		

		// Initially set Character Spatial information
	}
	
}

entt::entity WorldManager::CreateObjectEntity(uint32 worldId)
{
	if (!Worlds.contains(worldId)) {
		return entt::null;
	}
	entt::registry& world = Worlds[worldId];
	entt::entity entity = world.create();

	// Set the actual entity ID to something we want
	e::ObjectId& newObject = world.emplace<e::ObjectId>(entity, NextObjectId++);
	e::WorldComponent& wc = world.emplace<e::WorldComponent>(entity, worldId);

	spdlog::info("[WorldManager] - Created object in World: {} with Id: {}", wc.Id, newObject.Id);
	Entities.insert({ newObject.Id, entity });

	return entity;
}

void WorldManager::OnObjectDeleted(uint32 objId)
{
	// TODO: maybe store the deleted object ID's to some vector and reuse their ID's

	// TODO: check what region the object was in and send RemoveObjectPacket to nearby players
	if (Entities.contains(objId)) {
		Entities.erase(objId);
	}
}

entt::entity WorldManager::GetEntityByObjId(uint32 objId)
{
	if (Entities.contains(objId)) {
		return Entities[objId];
	}
	return entt::null;
}


std::vector<std::shared_ptr<WorldSession>> WorldManager::GetNearbyConnections(uint32 objId, uint32 excludeId)
{
	std::vector<std::shared_ptr<WorldSession>> connections;
	entt::registry& world = GetDefaultWorld();

	// Temp, just grab all connections for now
	
	for (auto& [key, conn] : Connections) {
		if (key != excludeId) {
			connections.push_back(conn);
		}
			
	}
	
	return connections;

	// Get the region and sector that our asking entity is in

	// Loop through all the entities in the same region and add them to the list

	// Send back the list of all nearby connections

}

void WorldManager::AddConnection(uint32 id, std::shared_ptr<WorldSession> connection)
{
	if (!Connections.contains(id)) {
		Connections.insert({ id, connection });
	}
}

void WorldManager::NotifyConnectionLost(uint32 id)
{
	if (Connections.contains(id)) {

		// TODO, check if the connection has a entity still in the world,
		// if it has, put a component on it that will remove it from the world
		// after some time.
		Connections.erase(id);
	}
}

std::shared_ptr<WorldSession> WorldManager::GetConnection(uint32 id)
{
	if (Connections.contains(id)) {
		return Connections[id];
	}
	return nullptr;
}

entt::registry& WorldManager::CreateWorld(uint32 id)
{
	if (Worlds.contains(id)) {
		spdlog::error("World already exist with this ID!");
		return Worlds[id];
	}

	Worlds[id] = entt::registry();
	return Worlds[id];
}

void WorldManager::UpdateWorld(uint32 id, float fixedDeltaTime)
{

}