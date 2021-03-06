/*
SDLEngine
Matt Hoyle
*/
#pragma once

#include "core/system.h"
#include <vector>
#include <functional>

namespace SDE
{
	// This class handles polling system events and controls the lifetime of the engine
	class EventSystem : public Core::ISystem
	{
	public:
		EventSystem();
		virtual ~EventSystem();
		bool Tick();

		using EventHandler = std::function<void(void*)>;		// void* = SDL_Event*
		void RegisterEventHandler(EventHandler);
	private:
		std::vector<EventHandler> m_handlers;
	};
}