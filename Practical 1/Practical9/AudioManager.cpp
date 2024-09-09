#include "AudioManager.h"

void AudioManager::InitializeAudio()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
}

void AudioManager::PlaySound1()
{
	result = system->playSound(sound1, 0, true, &channel);
	channel->setVolume(2);
	channel->setPaused(false);
}

void AudioManager::PlaySoundTrack()
{
	result = system->playSound(sound2, 0, true, &channel);
	channel->setPan(0);
	channel->setVolume(0.7);
	channel->setPaused(false);
}

void AudioManager::LoadSounds()
{
	result = system->createSound("boing.wav", FMOD_DEFAULT, 0, &sound1);
	result - sound1->setMode(FMOD_LOOP_OFF);

	result = system->createStream("chillz.wav", FMOD_DEFAULT, 0, &sound2);
	result - sound2->setMode(FMOD_LOOP_OFF);
}

void AudioManager::UpdateSound()
{
	result = system->update();
}

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
}
