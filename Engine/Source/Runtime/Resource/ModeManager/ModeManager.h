#pragma once

#include "Runtime/Core/PublicSingleton.h"

namespace Flame
{
	// From UE4.27.2
	// TODO
	enum class EditMode
	{
		Select = 0,
		Landscape = 1,
		Foliage = 2,
		BrushEditing = 3,
		MeshPaint = 4
	};

	enum class SceneMode
	{
		None = 0,
		EnvironmentHdr = 1,
		SkyBox = 2,
	};

	enum class PhysicsDebugDrawModeFlag
	{
		FLAME_NoDebug = 0,
		FLAME_DrawWireframe = 1,
		FLAME_DrawAabb = 2,
		FLAME_DrawFeaturesText = 4,
		FLAME_DrawContactPoints = 8,
		FLAME_NoDeactivation = 16,
		FLAME_NoHelpText = 32,
		FLAME_DrawText = 64,
		FLAME_ProfileTimings = 128,
		FLAME_EnableSatComparison = 256,
		FLAME_DisableBulletLCP = 512,
		FLAME_EnableCCD = 1024,
		FLAME_DrawConstraints = (1 << 11),
		FLAME_DrawConstraintLimits = (1 << 12),
		FLAME_FastWireframe = (1 << 13),
		FLAME_DrawNormals = (1 << 14),
		FLAME_DrawFrames = (1 << 15),
		FLAME_MAX_DEBUG_DRAW_MODE
	};


	class ModeManager final : public PublicSingleton<ModeManager>
	{
	public:
		static bool IsEditState() { return ModeManager::GetInstance().bEditState; }
		static void ChangeState() { ModeManager::GetInstance().bEditState = !ModeManager::GetInstance().bEditState; }
	public:
		static int b3DMode;  // 0 for 2D, 1 for 3D
		static bool bHdrUse;
		static SceneMode m_SceneMode;
		static bool bShowPhysicsColliders;
		static PhysicsDebugDrawModeFlag m_PhysicsDebugDrawModeFlag;
	private:
		static bool bEditState;
		static EditMode m_EditMode;
	};
}