#pragma once

#include "OrthographicCamera.h"

namespace Flame {

	class Renderer2D
	{
	public:
		// 这里设置为static的好处是什么？
		/*里面全部都是静态函数，之所以不做成成员函数是因为没有必要，毕竟成员函数本质上也是静态函数，
		无非静态函数的第一个参数变成了this指针而已*/
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Primitives
		//vec2和vec3的position是为了加入depth信息
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	};

}