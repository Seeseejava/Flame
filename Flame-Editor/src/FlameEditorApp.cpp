#include <Flame.h>
#include <Flame/Core/EntryPoint.h>

#include "EditorLayer.h"
#include "EditorLayer3D.h"

namespace Flame {

	class FlameEditor : public Application
	{
	public:
		FlameEditor(const ApplicationProps& props)
			: Application(props)
		{
			PushLayer(new EditorLayer3D());
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