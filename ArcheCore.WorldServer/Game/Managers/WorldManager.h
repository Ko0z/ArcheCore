#pragma once
#include "common_archecore.h"
#include "Entity/common_entity.h"
#include "Entity/Movement.h"

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <unordered_map>

class WorldSession;

class WorldManager {
public:
	static constexpr int32 CELL_SIZE = 1024;
	static constexpr int32 REGION_SIZE = 64;
	static constexpr int32 SECTORS_PER_CELL = CELL_SIZE / REGION_SIZE;
	static constexpr int32 CELL_HMAP_RESOLUTION = CELL_SIZE / 2;
	/// <summary>
	/// Cell sector size, used for polling objects in your proximity <para/>
	/// recommended 3 and max 5, anything higher is overkill <para/>
	/// as you can't target things beyond that distance in the client.
	/// </summary>
	static constexpr int8 REGION_NEIGHBORHOOD_SIZE = 2;	// Maybe this can be variable that we can manipulate if server struggles with bandwidth

	static constexpr uint32 WORLD_DEFAULT_ID = 0;

private:
	WorldManager() {}

public:
	static WorldManager& Instance() {
		static WorldManager _Instance;
		return _Instance;
	}

	void Initialize();

	/// <summary>
	/// Update all worlds with a fixed time step delta time value
	/// </summary>
	/// <param name="fixedDeltaTime"></param>
	void Update(float fixedDeltaTime);

	inline entt::registry& GetDefaultWorld() { return Worlds[WORLD_DEFAULT_ID]; }
	inline entt::registry& GetWorld(uint32 id);
	void SpawnCharacter(entt::entity characterEntity, uint32 worldId);
	entt::entity CreateObjectEntity(uint32 worldId);
	void OnObjectDeleted(uint32 objId);

	/// <summary>
	/// Since object ID is unique even among worlds / instances, we can grab the entity
	/// and figure out stuff about it then, what world, type, etc...
	/// </summary>
	/// <param name="objId"></param>
	/// <returns></returns>
	entt::entity GetEntityByObjId(uint32 objId);
	std::vector<std::shared_ptr<WorldSession>> GetNearbyConnections(uint32 objId, uint32 excludeId = 0);
	void AddConnection(uint32 id, std::shared_ptr<WorldSession> connection);
	void NotifyConnectionLost(uint32 id);
	std::shared_ptr<WorldSession> GetConnection(uint32 id);

private:
	entt::registry& CreateWorld(uint32 id);
	void UpdateWorld(uint32 id, float fixedDeltaTime);

private:
	uint32 NextObjectId = 0x0000100;
	uint32 NextItemId = 0x00FF0000;

	
	std::unordered_map<uint32, entt::entity> Entities;	// ObjId to Entity
	std::unordered_map<uint32, entt::registry> Worlds;	// WorldId to World

	//std::unordered_map<uint32, uint32> EntityToWorldLookUp;

	std::unordered_map<uint32, std::shared_ptr<WorldSession>> Connections;	// ConnectionId to WorldSession
	// We want an easy way to get a WorldSession from an Id
	// And also we want to be able to get Id from WorldSession
};