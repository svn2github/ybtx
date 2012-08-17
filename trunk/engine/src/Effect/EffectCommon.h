#pragma once

#include "EffectBase.h"

#define			MAX_PARTICLE		10000
#define			MAX_TRAIL			10000

struct Particle : public CEffectMallocObject
{
	float				m_LeftLife;
	float				m_LifeSpan;

	float				m_AngularVelocity;
	float				m_CurAngular;
	CVector3f	m_CurSpd;
	CVector3f	m_CurPos;
	Particle*		m_Next;
};

struct Trail : public CEffectMallocObject
{
	float			m_LifeTime;

	CVector3f		m_Position1;
	CVector3f		m_Position2;
	Trail*			m_Next;
};
