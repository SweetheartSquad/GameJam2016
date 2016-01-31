#pragma once

#include <Sprite.h>
#include <Timeout.h>

class MY_DemonBoss : public Sprite {
private:
	float spawnSpewerTimer;
	float spawnSpewerTimerLength;
public:	
	sweet::EventManager eventManager;

	explicit MY_DemonBoss(Shader * _shader);

	Timeout * spewerTimeout;

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};
