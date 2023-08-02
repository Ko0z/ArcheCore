#pragma once

#include <entt/entity/registry.hpp>
#include "Enums.h"
#include "Object.h"
#include "Inventory.h"
#include "Items.h"
#include "Skill.h"
#include "Player.h"
#include "Unit.h"

// ------- My naive plan

/// 1. Packets received

/// 2. Process packets, multi-threaded, check packet flag if it can be multithreaded, leave if not

/// 3. Process packets, single-threaded that need main thread...

/// 4a. If packet dont need data from entities, ( such as a DB query.. just get the data and send it back )

/// 4b. Otherwise apply other changes in form of component addition / deletion on entities - ( Do Damage to x entity etc. )

/// 5. Run all entity systems and watch everything take care of itself ????

// ------------------------>

namespace e 
{
	// Interface like base system
	class system_base {
	public:
		virtual void start(entt::registry& registry) {}
		virtual void run(entt::registry& registry) = 0;	// must be impl
		virtual void stop(entt::registry& registry) {}
	};
	
}

