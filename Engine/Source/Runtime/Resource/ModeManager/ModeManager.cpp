#include "flamepch.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace Flame
{
	bool ModeManager::bEditState = true;
	int ModeManager::b3DMode = 1;
	SceneMode ModeManager::m_SceneMode = SceneMode::None;
	EditMode ModeManager::m_EditMode = EditMode::Select;
}