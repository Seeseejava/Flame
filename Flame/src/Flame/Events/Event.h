#pragma once

#include "Flame/Core/Core.h"
#include "flamepch.h"

namespace Flame {

	//����ʹ�õ�ʽBlock�¼����Ժ��������Ϊbuffer�¼�

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
	//C++ԭ����enumֵ�ᱻ��ʽת��(implicitly convert)Ϊint����
	//C++���enum������һ��ȫ�ֵķ�Χ��������ȫ�ֱ���������û��ǰ׺��
	//����������������ͻ��������
	//C++��enum���޷��涨�ö���λ�����ݽṹȥ�洢�����洢�����Ϳ�����
	//char��short��int�����ͣ�ѡ��һ�����õ����ݽṹ����

	//���ڸ��������Event���Ͷ���һ���ģ��������ǿ�����һ��static����ȥ�洢�����;͹���
	//#����Ϊ�����˫���ţ������ַ�������
	//##����ֱ�ӽ����ַ���ƴ��
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)  virtual int GetCategoryFlags() const override { return category; }

	class FLAME_API Event
	{
		friend class EventDispatcher;
	public:
		bool m_Handled = false;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }//����C#�������ӡһЩ��Ϣ

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		

	};

	class EventDispatcher  //�е��ѿ���
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;//����ֵ��bool��������T&
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);//û����
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
	// use for log library
}