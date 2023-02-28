#include "flamepch.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace Flame
{
	bool ModeManager::bEditState = true;
	int ModeManager::b3DMode = 1;
	bool ModeManager::bHdrUse = false;
	SceneMode ModeManager::m_SceneMode = SceneMode::None;
	EditMode ModeManager::m_EditMode = EditMode::Select;
	bool ModeManager::bShowPhysicsColliders = false;
	PhysicsDebugDrawModeFlag ModeManager::m_PhysicsDebugDrawModeFlag = PhysicsDebugDrawModeFlag::FLAME_DrawWireframe;
}