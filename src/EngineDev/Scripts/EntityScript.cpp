
#include "pch.h"
#include "EntityScript.h"
#include "Entities/Entity.h"

EntityScript::EntityScript(Entity& attachedEntity) :
	m_AttachedEntity(&attachedEntity) {
	m_AttachedEntity->m_AttachedScript = this;
};
