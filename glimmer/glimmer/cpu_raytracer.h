#pragma once

#include "core/system.h"
#include "traceboi.h"
#include <memory>
#include <atomic>

namespace Render
{
	class Texture;
	class TextureSource;
	class Camera;
}

namespace DebugGui
{
	class DebugGuiSystem;
}

namespace SDE
{
	class JobSystem;
	class ScriptSystem;
}

// Owns an opengl texture + handles rendering a scene on multiple cores
class CpuRaytracer
{
public:
	struct Parameters
	{
		int m_jobCount = 64;
		int m_maxRecursion = 8;
		SDE::JobSystem* m_jobSystem = nullptr;
		ImageParameters m_image;
	};
	enum Status : int
	{
		Ready = 0,
		InProgress = 1,
		Complete = 2,
		Paused = 3
	};

	CpuRaytracer(const Parameters& params);
	~CpuRaytracer();

	bool TryDrawScene(Scene& scene, Render::Camera& camera);	// Returns true if we can kick off a render
	void Tick();						// Must be called every frame on render thread to handle job completion
	Render::Texture* GetTexture();		// Can return null if no image drawn yet

	inline double GetLastDrawTime()		{ return m_lastTraceTime.load(); }
	inline Status GetStatus()			{ return static_cast<Status>(m_traceStatus.load()); }

private:
	void SubmitRenderJobs(Scene& s, Render::Camera& camera);
	void CreateOrUpdateTexture(const std::vector<Render::TextureSource>& ts);
	void UpdateTextureFromResult();

	Parameters m_parameters;
	std::unique_ptr<Render::Texture> m_texture;

	std::vector<uint32_t> m_rawOutput;			// Raw output from trace

	std::atomic<int> m_jobsInProgress;			// how many jobs in flight, the last one sets status to Complete
	std::atomic<int> m_traceStatus;				// overal status

	std::atomic<double> m_traceStartTime;		// when did the current trace start
	std::atomic<double> m_lastTraceTime;		// how long did the last trace take
};

class CpuRaytracerSystem : public Core::ISystem
{
public:
	CpuRaytracerSystem() = default;
	virtual ~CpuRaytracerSystem() = default;

	virtual bool PreInit(Core::ISystemEnumerator& systemEnumerator);
	virtual bool PostInit();
	virtual bool Tick();
	virtual void Shutdown();

private:
	void CreateScene();

	void UpdateControls();
	void UpdateSceneControls();

	Scene m_scene;
	Render::Camera m_camera;
	std::unique_ptr<CpuRaytracer> m_cpuTracer;

	bool m_isPaused = false;
	DebugGui::DebugGuiSystem* m_debugGui = nullptr;
	SDE::ScriptSystem* m_scriptSystem = nullptr;
};