#pragma once

#include <Flame.h>

// 代表粒子系统释放粒子时的统一粒子参数, 参数有：
// 初始大小, 最终大小, lifeTime, 速度, 位置, 起始颜色, 最终颜色等
struct ParticleProperties
{
	glm::vec2 Position;
	// 由于粒子各不相同, 这里的VelocityVariation代表最大的速度变化量, 会在
	// [Velocity - VelocityVariation * 0.5f, Velocity + VelocityVariation * 0.5f]区间产生随机velocity
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
};

// 由于每个Particle的参数会不同, 所以需要单独设计一个类
struct Particle
{
	glm::vec2 Position;
	glm::vec2 Velocity;
	glm::vec4 ColorBegin, ColorEnd;
	float Rotation = 0.0f;
	float SizeBegin, SizeEnd;

	float LifeTime = 1.0f;
	float LifeRemaining = 0.0f;

	bool Active = false;
};


// Player对象里会存一个ParticleSystem对象
class ParticleSystem
{
public:
	ParticleSystem(uint32_t maxParticles = 10000);

	// 释放粒子, 当按住左时, 每帧都会调用此函数, 它们的参数由particleProps统一指定
	// 但是绝大部分参数会基于Random系统, 在原本particleProps给的基础上微变
	void Emit(const ParticleProperties& particleProps);

	void OnUpdate(Flame::Timestep ts);
	void OnRender(Flame::OrthographicCamera& camera);
private:

	std::vector<Particle> m_ParticlePool;// 会在此类的构造函数里resize到1000, 即Pool的size为1000
	uint32_t m_PoolIndex;
};