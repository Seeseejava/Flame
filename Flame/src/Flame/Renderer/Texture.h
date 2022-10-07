#pragma once

#include <string>

#include "Flame/Core/Core.h"

namespace Flame
{
	// Texture ���Է�Ϊ��������, ����CubeTexture, Texture2D
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	// ע��, ���������˸�Texture2D��, �̳���Texture��
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);
	};

	
}
