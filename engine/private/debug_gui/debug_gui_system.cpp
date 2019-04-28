/*
SDLEngine
Matt Hoyle
*/
#include "debug_gui_system.h"
#include "debug_gui_render.h"
#include "imgui_sdl_gl3_render.h"
#include "graph_data_buffer.h"
#include "render/mesh.h"
#include "render/shader_binary.h"
#include "render/shader_program.h"
#include "render/material.h"
#include "render/texture_source.h"
#include "render/texture.h"
#include "render/window.h"
#include "render/device.h"
#include "render/mesh_instance_render_pass.h"
#include "input/input_system.h"
#include "core/system_enumerator.h"
#include "sde/render_system.h"
#include "sde/event_system.h"
#include "kernel/log.h"
#include <imgui\imgui.h>

namespace DebugGui
{
	DebugGuiSystem::DebugGuiSystem()
		: m_inputSystem(nullptr)
		, m_renderSystem(nullptr)
	{
	}

	DebugGuiSystem::~DebugGuiSystem()
	{
	}

	bool DebugGuiSystem::PreInit(Core::ISystemEnumerator& systemEnumerator)
	{
		m_renderSystem = (SDE::RenderSystem*)systemEnumerator.GetSystem("Render");
		m_inputSystem = (Input::InputSystem*)systemEnumerator.GetSystem("Input");
		auto EventSystem = (SDE::EventSystem*)systemEnumerator.GetSystem("Events");
		EventSystem->RegisterEventHandler([this](void* e)
		{
			this->m_imguiPass->HandleEvent(e);
		});

		return true;
	}

	bool DebugGuiSystem::Initialise()
	{
		return true;
	}

	bool DebugGuiSystem::PostInit()
	{
		ImGui::SdeImguiInit();
		m_imguiPass = std::make_unique<ImguiSdlGL3RenderPass>(m_renderSystem->GetWindow(), m_renderSystem->GetDevice());
		m_renderSystem->AddPass(*m_imguiPass);

		float viewportWidth = (float)m_renderSystem->GetViewportWidth();
		float viewportHeight = (float)m_renderSystem->GetViewportHeight();

		return true;
	}

	void DebugGuiSystem::BeginWindow(bool& windowOpen, const char* windowName, glm::vec2 size)
	{
		ImGui::Begin(windowName, &windowOpen, 0);
		if (size.x > 0 && size.y > 0)
		{
			ImGui::SetWindowSize(ImVec2(size.x, size.y));
		}
	}

	void DebugGuiSystem::EndWindow()
	{
		ImGui::End();
	}

	bool DebugGuiSystem::DragFloat(const char* label, float& f, float step, float min, float max)
	{
		return ImGui::DragFloat(label, &f, step, min, max);
	}

	bool DebugGuiSystem::DragVector(const char* label, glm::vec3& v, float step, float min, float max)
	{
		return ImGui::DragFloat3(label, glm::value_ptr(v), step, min, max);
	}

	bool DebugGuiSystem::ColourEdit(const char* label, glm::vec4& c, bool showAlpha)
	{
		return ImGui::ColorEdit4(label, glm::value_ptr(c), showAlpha);
	}

	bool DebugGuiSystem::DragVector(const char* label, glm::vec4& v, float step, float min, float max)
	{
		return ImGui::DragFloat4(label, glm::value_ptr(v), step, min, max);
	}

	bool DebugGuiSystem::Button(const char* txt)
	{
		return ImGui::Button(txt);
	}

	void DebugGuiSystem::Text(const char* txt)
	{
		ImGui::Text(txt);
	}

	void DebugGuiSystem::Separator()
	{
		ImGui::Separator();
	}

	void DebugGuiSystem::GraphLines(const char* label, glm::vec2 size, GraphDataBuffer& buffer)
	{
		ImVec2 graphSize(size.x, size.y);
		ImGui::PlotLines("", buffer.GetValues(), buffer.ValueCount(), 0, label, FLT_MAX, FLT_MAX, graphSize);
	}

	bool DebugGuiSystem::Checkbox(const char* text, bool* val)
	{
		return ImGui::Checkbox(text, val);
	}

	void DebugGuiSystem::GraphHistogram(const char* label, glm::vec2 size, GraphDataBuffer& buffer)
	{
		ImVec2 graphSize(size.x, size.y);
		ImGui::PlotHistogram("", buffer.GetValues(), buffer.ValueCount(), 0, label, FLT_MAX, FLT_MAX, graphSize);
	}

	void DebugGuiSystem::Image(Render::Texture& src, glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1)
	{
		size_t texHandle = static_cast<size_t>(src.GetHandle());
		ImGui::Image(reinterpret_cast<ImTextureID>(texHandle), { size.x,size.y }, { uv0.x, uv0.y }, { uv1.x, uv1.y });
	}

	bool DebugGuiSystem::Tick()
	{
		// Start next frame
		m_imguiPass->NewFrame();
		ImGui::NewFrame();

		return true;
	}

	void DebugGuiSystem::Shutdown()
	{
		m_imguiPass = nullptr;
		ImGui::SdeImguiShutdown();
	}
}