//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School.
//
// File Name	: clock.cpp
// Description	: The clock for keeping time
// Author		: Jack Mair
// Mail			: jack.mair@mediadesign.school.nz
//


// Library Includes

// Local Includes
#include "Clock.h"

// Static Variables

// Static Function Prototypes

// Implementation

Clock::Clock()
	: m_timeElapsed(0.0)
	, m_deltaTime(0.0)
	, m_lastTime(0.0)
	, m_currentTime(0.0)
{

}

Clock::~Clock()
{

}

bool
Clock::Initialise()
{
	m_currentTime = glfwGetTime();;
	m_lastTime = glfwGetTime();;
	return (true);
}

void
Clock::Process()
{
	// Get the time this frame.
	m_currentTime = glfwGetTime();;

	// Time difference between this frame and the previous frame
	m_deltaTime = static_cast<float>(m_currentTime - m_lastTime);

	// Prepare for the next frame
	m_lastTime = m_currentTime;

	// Force non-negative
	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}

	// Cap DeltaTime
	if (m_deltaTime > 1.0)
	{
		m_deltaTime = 1.0;
	}

	m_timeElapsed += m_deltaTime;
}

float Clock::GetDeltaTick()
{
	return m_deltaTime;
}

double Clock::GetCurTime()
{
	return m_currentTime;
}
