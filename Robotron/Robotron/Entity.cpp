#include "Entity.h"

bool Entity::operator==(const Entity& rhs) const
{
	return this == &rhs;
}

bool Entity::hasComponents(size_t componentMask) const
{
	return (m_componentMask & componentMask) == componentMask;
}

bool Entity::hasAnyComponent(size_t componentMask) const
{
	return (m_componentMask & componentMask) > 0;
}

bool Entity::hasComponents()
{
	return m_componentMask != COMPONENT_NONE;
}
