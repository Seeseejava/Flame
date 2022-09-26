#pragma once

#include <string>
#include <glm/glm.hpp>
namespace Flame {

	class Shader
	{
	public:
		Shader(const std::string& vertexsrc, const std::string& fragmentsrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		uint32_t m_RendererID;
	};
}