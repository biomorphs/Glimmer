#include "sde/asset_system.h"
#include "sde/render_system.h"
#include "sde/job_system.h"
#include "debug_gui/debug_gui_system.h"
#include "engine/engine_startup.h"
#include "input/input_system.h"
#include "core/system_registrar.h"
#include "render.h"

class SystemRegistration : public Engine::IAppSystemRegistrar
{
public:
	void RegisterSystems(Core::ISystemRegistrar& systemManager)
	{
		systemManager.RegisterSystem("Jobs", new SDE::JobSystem());
		systemManager.RegisterSystem("Input", new Input::InputSystem());

		systemManager.RegisterSystem("MyRender", new MyRender(1280,720));
		
		systemManager.RegisterSystem("DebugGui", new DebugGui::DebugGuiSystem());
		systemManager.RegisterSystem("Render", new SDE::RenderSystem());
	}
};

int main()
{
	SystemRegistration sysRegistration;
	return Engine::Run(sysRegistration, 0, nullptr);
}
