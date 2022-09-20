#pragma once

#include <string>
namespace Flame {

	class Shader
	{
	public:
		Shader(const std::string& vertexsrc, const std::string& fragmentsrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_RendererID;
	};
}