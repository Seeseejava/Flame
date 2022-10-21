#include <Flame.h>
#include <Flame/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Flame {

	class FlameEditor : public Application
	{
	public:
		FlameEditor()
			: Application("Flame Editor")
		{
			PushLayer(new EditorLayer());
		}

		~FlameEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new FlameEditor();
	}

}