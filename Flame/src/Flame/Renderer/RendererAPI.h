#pragma once

#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Flame {

	class RendererAPI
	{
	public:
		// ��ȾAPI������, ��һ��Ӧ����RendererAPI����, ������Renderer����
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		//��ƽ̨�޹ص���Ⱦ��API
		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}