#include <Flame.h>
#include <Runtime/Core/EntryPoint.h>

#include "EditorLayer.h"


namespace Flame {

	class FlameEditor : public Application
	{
	public:
		FlameEditor(const ApplicationProps& props)
			: Application(props)
		{
			PushLayer(new EditorLayer());
		}

		~FlameEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new FlameEditor({ "Flame Editor", 1600, 900 });
	}

}