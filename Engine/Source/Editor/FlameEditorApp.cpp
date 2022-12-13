#include <Flame.h>


#include "EditorLayer.h"


namespace Flame {

	void MyAppInitialize(Application& app)
	{
		app.Init();
		app.PushLayer(new EditorLayer());
	}

}