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
		// 仅当程序第一次执行到GetInstance函数时，执行instance对象的初始化
		// C++11之后，被static修饰的变量可以保证是线程安全的
		static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value) // 可以构造则返回true
		{
			static T instance;
			return instance;
		}
		virtual ~PublicSingleton() noexcept = default;                          // =noexcept(true) 不抛出异常
		PublicSingleton(const PublicSingleton&) = delete;
		PublicSingleton& operator=(const PublicSingleton&) = delete;
	};
}