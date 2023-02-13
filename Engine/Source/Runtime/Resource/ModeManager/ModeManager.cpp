#include "flamepch.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace Flame
{
	bool ModeManager::bEditState = true;
	int ModeManager::b3DMode = 1;
	bool ModeManager::bHdrUse = false;
	SceneMode ModeManager::m_SceneMode = SceneMode::SkyBox;
	EditMode ModeManager::m_EditMode = EditMode::Select;
}