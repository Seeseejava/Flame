#include "flamepch.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace Flame
{
	bool ModeManager::bEditState = true;
	int ModeManager::b3DMode = 1;
	EditMode ModeManager::mEditMode = EditMode::Select;
}