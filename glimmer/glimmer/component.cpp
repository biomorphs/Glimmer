#include "component.h"

Component::Component(EntityHandle& h)
	: m_parent(h.GetEntity())
{
}