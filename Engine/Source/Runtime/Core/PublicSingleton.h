#pragma once

#include <type_traits>

namespace Flame
{

	template<typename T>
	class PublicSingleton
	{
	protected:
		PublicSingleton() = default;

	public:
		static T& getInstance() noexcept(std::is_nothrow_constructible<T>::value) // 可以构造则返回true
		{
			static T instance;
			return instance;
		}
		virtual ~PublicSingleton() noexcept = default;                          // =noexcept(true) 不抛出异常
		PublicSingleton(const PublicSingleton&) = delete;
		PublicSingleton& operator=(const PublicSingleton&) = delete;
	};
}