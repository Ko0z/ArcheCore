#include "Object.h"
#include "Managers/WorldManager.h"

namespace e {
	//uint32 Guid::IdCounter = 0;


	ObjectId::~ObjectId()
	{
		WorldManager::Instance().OnObjectDeleted(Id);
	}

}
