#pragma once

#include "common_archecore.h"
#include "common_entity.h"

namespace e {
	/// Entity components and systems about player entities

	struct PlayerSession 
	{
		PlayerSession(uint32 connectionId, bool inWorld = false) {
			ConnectionId = connectionId;
			InWorld = inWorld;
		}

		//entt::entity CharacterEntity = entt::entt_traits
		uint32 ConnectionId;
		bool InWorld;
	};
}
