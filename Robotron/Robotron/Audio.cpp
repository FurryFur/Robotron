#include "Audio.h"



Audio::Audio()
{
	InitFmod();
	LoadAudio();
}


Audio::~Audio()
{
}

bool Audio::InitFmod() {
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_audioMgr);
	if (result != FMOD_OK) return false;
	result = m_audioMgr->init(50, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) return false;
	return true;
}

const bool Audio::LoadAudio() {
	FMOD_RESULT result;
	result = m_audioMgr->createSound("Assets/Audio/clang.mp3", FMOD_DEFAULT, 0, &m_hitSound);
	result = m_audioMgr->createSound("Assets/Audio/button_click.wav", FMOD_DEFAULT, 0, &m_buttonClick);
	result = m_audioMgr->createSound("Assets/Audio/birbs.wav", FMOD_DEFAULT, 0, &m_bgMusic);
	m_bgMusic->setMode(FMOD_LOOP_NORMAL);
	m_buttonClick->setMode(FMOD_LOOP_OFF);
	return true;
}

void  Audio::playBgMusic()
{
	// Play the background music
	m_audioMgr->playSound(m_bgMusic, 0, false, &m_channel);
}

void Audio::playButtonClick()
{
	// Play the click music
	m_audioMgr->playSound(m_buttonClick, 0, false, &m_channel);
}

