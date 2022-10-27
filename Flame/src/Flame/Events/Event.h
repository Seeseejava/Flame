#pragma once

#include "flamepch.h"
#include "Flame/Core/Core.h"

namespace Flame {

	//现在使用的式Block事件，以后可能升级为buffer事件

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication  = BIT(0),
		EventCategoryInput        = BIT(1),
		EventCategoryKeyboard     = BIT(2),
		EventCategoryMouse        = BIT(3),
		EventCategoryMouseButton  = BIT(4),
	};
	//C++原本的enum值会被隐式转换(implicitly convert)为int类型
	//C++里的enum好像是一个全局的范围，类似于全局变量，而且没有前缀，
	//这很容易造成命名冲突和理解错误
	//C++的enum，无法规定用多少位的数据结构去存储它，存储的类型可能是
	//char、short、int等类型，选择一个够用的数据结构就行

	//由于该类的所有Event类型都是一样的，所以我们可以用一个static变量去存储该类型就够了
	//#：是为其加上双引号，当作字符串处理
	//##：是直接进行字符的拼接
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)  virtual int GetCategoryFlags() const override { return category; }

	class FLAME_API Event
	{
		friend class EventDispatcher;
	public:
		virtual ~Event() = default;
		bool m_Handled = false;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }//仿照C#，方便打印一些消息

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		

	};

	class EventDispatcher  //有点难看懂
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;// EventFn存储了一个输入为任意类型的引用，返回值为bool的函数指针
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		// Dispatch会直接执行响应事件对应的函数指针对应的函数
		// T指的是事件类型, 如果输入的类型没有GetStaticType会报错
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			// 只有Event类型跟模板T匹配时, 才响应事件
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);// 使用(T*)把m_Event转换成输入事件的指针类型 结构体赋值（要考虑空间分布）
				return true; // Temporary: 现在不会直接对应的Handler里都返回true
			}
			return false;
		}
	private:
		Event& m_Event; // 必须是引用，不可以是Event的实例，因为Event带有纯虚函数
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
	// use for log library
}