#pragma once

namespace Flame {

	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time)
		{
		}
		
		//
		operator float() const { return m_Time; }//类型转换函数

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const{ return m_Time * 1000.f; }
			
	private:
		float m_Time;
	};

}