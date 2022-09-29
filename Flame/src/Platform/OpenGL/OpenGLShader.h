#pragma once

#include "Flame/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Flame {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexsrc, const std::string& fragmentsrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformFloat(const std::string& name, float values);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
		void UploadUniformInt(const std::string& name, int values);

	private:
		uint32_t m_RendererID;
	};
}