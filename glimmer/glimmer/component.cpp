#include "component.h"
#include "kernel/assert.h"

Component::Component(EntityHandle& h)
	: m_parent(h.GetEntity())
{
}