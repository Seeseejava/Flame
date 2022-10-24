#pragma once

#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Flame {

	class RendererAPI
	{
	public:
		// 渲染API的类型, 这一块应该由RendererAPI负责, 而不是Renderer负责
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		//与平台无关的渲染的API
		virtual ~RendererAPI() = default;
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}