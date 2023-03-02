#include <Flame.h>


#include "EditorLayer.h"
#include "Editor/RawRenderTest/RawLayer.h"


namespace Flame {

	void MyAppInitialize(Application& app)
	{
		EditorLayer::EditorLogInit();
		app.Init();
		app.PushLayer(new EditorLayer());
		//app.PushLayer(new RawLayer());
	}

}