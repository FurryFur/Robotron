//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Controls the audio in the game
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include <fmod.hpp>

class Audio
{
public:
	Audio();
	~Audio();

	void playBgMusic();
	void playButtonClick();

private:
	bool InitFmod();
	const bool LoadAudio();

	FMOD::System* m_audioMgr;
	FMOD::Sound* m_bgMusic;
	FMOD::Sound* m_buttonClick;
	FMOD::Sound* m_hitSound;
	FMOD::Channel* m_channel;
};

