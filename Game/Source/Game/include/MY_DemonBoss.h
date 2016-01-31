#pragma once

#include <Sprite.h>
#include <Timeout.h>
#include <MY_Spewer.h>
#include <NumberUtils.h>

class MY_DemonBoss : public Sprite {
private:
	float spawnSpewerTimer;
	float spawnSpewerTimerLength;

	bool disableSpewer(int _column);
public:	
	unsigned long int hits;
	bool dead;
	sweet::EventManager eventManager;

	explicit MY_DemonBoss(Shader * _shader);
	
	Timeout * spewerTimeout;
	Timeout * animationTimeout;
	std::vector<MY_Spewer *> spewers;
	std::vector<int> enabledSpewers;
	sweet::ShuffleVector<int> spewerIdx;

	void addSpewer(MY_Spewer * _spewer);

	void enableSpewers();
	void disableSpewers(int _column);
	

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};
