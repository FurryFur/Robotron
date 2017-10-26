//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School.
//
// File Name	: Clock.h
// Description	: Header file for the clock
// Author		: Jack Mair
// Mail			: jack.mair@mediadesign.school.nz
//

#pragma once

#if !defined(__CLOCK_H__)
#define __CLOCK_H__

// Library Includes
#include <GLFW\glfw3.h>

// Local Includes

// Types

// Constants

// Prototypes
class Clock
{
	// Member Functions
public:
	Clock();
	~Clock();

	bool Initialise();

	void Process();

	float GetDeltaTick();

	double GetCurTime();

protected:

private:
	Clock(const Clock& _kr);
	Clock& operator= (const Clock& _kr);

	// Member Variables
public:

protected:
	double m_timeElapsed;
	float m_deltaTime;
	double m_lastTime;
	double m_currentTime;
	double m_secondsPerCount;
	int m_iFrameCount = 0;

private:

};


#endif    // __CLOCK_H__