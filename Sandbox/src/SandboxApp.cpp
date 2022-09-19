#include <Flame.h>
#include <flamepch.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include "imgui/imgui.h"

class ExampleLayer : public Flame::Layer
{
public:
	ExampleLayer()
		:Layer("Example") 
	{

	}

	void OnUpdate() override
	{
		//FLAME_INFO("ExampleLayer::Update");

		if (Flame::Input::IsKeyPressed(FLAME_KEY_TAB))
			FLAME_TRACE("Tab key is pressed(poll)!");
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}
	void OnEvent(Flame::Event& event) override
	{
		if (event.GetEventType() == Flame::EventType::KeyPressed)
		{
			Flame::KeyPressedEvent& e = (Flame::KeyPressedEvent&)event;
			if (e.GetKeyCode() == FLAME_KEY_TAB)
				FLAME_TRACE("Tab key is pressed(event)!");
			FLAME_TRACE("{0}", char(e.GetKeyCode()));
		}
	}
};

class Sandbox : public Flame::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Flame::Application* Flame::CreateApplication()
{
	return new Sandbox();
}