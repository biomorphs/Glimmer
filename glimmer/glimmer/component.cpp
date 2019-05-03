#include "component.h"
#include "kernel/assert.h"

SDE_SERIALISE_BEGIN(Component)
SDE_SERIALISE_PROPERTY("Type", GetTypeString());
SDE_SERIALISE_END()

Component::Component(EntityHandle& h)
	: m_parent(h.GetEntity())
{
}