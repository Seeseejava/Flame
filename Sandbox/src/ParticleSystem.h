#pragma once

#include <Flame.h>

// ��������ϵͳ�ͷ�����ʱ��ͳһ���Ӳ���, �����У�
// ��ʼ��С, ���մ�С, lifeTime, �ٶ�, λ��, ��ʼ��ɫ, ������ɫ��
struct ParticleProperties
{
	glm::vec2 Position;
	// �������Ӹ�����ͬ, �����VelocityVariation���������ٶȱ仯��, ����
	// [Velocity - VelocityVariation * 0.5f, Velocity + VelocityVariation * 0.5f]����������velocity
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
};

// ����ÿ��Particle�Ĳ����᲻ͬ, ������Ҫ�������һ����
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


// Player��������һ��ParticleSystem����
class ParticleSystem
{
public:
	ParticleSystem();

	// �ͷ�����, ����ס�ո��ʱ, ÿ֡������ô˺���, ���ǵĲ�����particlePropsͳһָ��
	// ���Ǿ��󲿷ֲ��������Randomϵͳ, ��ԭ��particleProps���Ļ�����΢��
	void Emit(const ParticleProperties& particleProps);

	void OnUpdate(Flame::Timestep ts, float playerSpeed);
	void OnRender();
private:

	std::vector<Particle> m_ParticlePool;// ���ڴ���Ĺ��캯����resize��1000, ��Pool��sizeΪ1000
	uint32_t m_PoolIndex = 999;
};