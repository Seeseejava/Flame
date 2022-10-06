#pragma once

#include "Flame/Renderer/Shader.h"
//#include <glad/glad.h>���ﲻ�����������Ϊsandboxʹ�������ͷ�ļ����ᱨû��glad�Ĵ���
#include <glm/glm.hpp>

// TODO:REMOVED! 
typedef unsigned int GLenum;

namespace Flame {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexsrc, const std::string& fragmentsrc);
		OpenGLShader(const std::string& filepath);
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
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);//�浽map����
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
	};
}