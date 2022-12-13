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
		// ���������һ��ִ�е�GetInstance����ʱ��ִ��instance����ĳ�ʼ��
		// C++11֮�󣬱�static���εı������Ա�֤���̰߳�ȫ��
		static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value) // ���Թ����򷵻�true
		{
			static T instance;
			return instance;
		}
		virtual ~PublicSingleton() noexcept = default;                          // =noexcept(true) ���׳��쳣
		PublicSingleton(const PublicSingleton&) = delete;
		PublicSingleton& operator=(const PublicSingleton&) = delete;
	};
}