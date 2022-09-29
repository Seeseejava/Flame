#pragma once

#include <string>

namespace Flame {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string& vertexsrc, const std::string& fragmentsrc);

	};
}