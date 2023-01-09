#include "ParticleSystem.h"

#include "Random.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

ParticleSystem::ParticleSystem(uint32_t maxParticles)
	:m_PoolIndex(maxParticles - 1)
{
	m_ParticlePool.resize(maxParticles);
}

void ParticleSystem::Emit(const ParticleProperties& particleProps)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];// m_PoolIndex��ʼΪ999, Ҳ���Ǵ����Pool������濪ʼ����
	particle.Active = true;
	particle.Position = particleProps.Position;// λ����ͳһ��
	particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();// �������䷽���������

	// Velocity
	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);// x��y������ٶȶ�������仯��
	particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

	// Color
	particle.ColorBegin = particleProps.ColorBegin;
	particle.ColorEnd = particleProps.ColorEnd;

	// Size
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
	particle.SizeEnd = particleProps.SizeEnd;

	// LifeTime���ǵ����ĸ�ֵ
	particle.LifeTime = particleProps.LifeTime;
	particle.LifeRemaining = particleProps.LifeTime;

	// m_PoolIndex--, ֱ����Ϊ0, ���ֻ���999
	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

// Update������, ������е����ӵ�״̬, ��������ڵ���, ����ΪinActive
// �������LifeTime��position��rotation
void ParticleSystem::OnUpdate(Flame::Timestep ts)
{
	for (auto& particle : m_ParticlePool)
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= ts;
		particle.Position += particle.Velocity * (float)ts;
		particle.Rotation += 0.01f * ts;
	}
}

