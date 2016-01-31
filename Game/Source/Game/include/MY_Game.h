#pragma once

#include <Game.h>

#define NUM_BGM_TRACKS 4

class MY_Game : public Game{
public:
	MY_Game();
	~MY_Game();

	void addSplashes() override;

	OpenAL_Sound * bgm;
	unsigned long int currentBgm;

	void playBGM();

	void update(Step * _step) override;
};