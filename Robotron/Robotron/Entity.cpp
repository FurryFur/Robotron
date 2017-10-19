#include "Entity.h"

bool Entity::operator==(const Entity& rhs) const
{
	return this == &rhs;
}

bool Entity::hasAllComponents(size_t _componentMask) const
{
	return (this->componentMask & _componentMask) == _componentMask;
}

bool Entity::hasAnyComponent(size_t _componentMask) const
{
	return (this->componentMask & _componentMask) > 0;
}
