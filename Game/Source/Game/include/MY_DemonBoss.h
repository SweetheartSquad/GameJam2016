#pragma once

#include <Sprite.h>
#include <EventManager.h>

class MY_DemonBoss : public Sprite {
public:

	sweet::EventManager eventManager;

	explicit MY_DemonBoss(Shader * _shader);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};
