#pragma once

#include <Sprite.h>
#include <Timeout.h>

class MY_DemonBoss : public Sprite {
private:
	float spawnSpewerTimer;
	float spawnSpewerTimerLength;
public:	
	explicit MY_DemonBoss(Shader * _shader);

	sweet::EventManager eventManager;

	Timeout * spewerTimeout;

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};