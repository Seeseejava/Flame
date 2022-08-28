#include <Flame.h>
#include <flamepch.h>


class ExampleLayer : public Flame::Layer
{
public:
	ExampleLayer()
		:Layer("Example") {}

	void OnUpdate() override
	{
		FLAME_INFO("ExampleLayer::Update");
	}

	void OnEvent(Flame::Event& event) override
	{
		FLAME_TRACE("{0}", event);
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