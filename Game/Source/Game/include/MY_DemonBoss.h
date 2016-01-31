#pragma once

#include <Sprite.h>

class MY_DemonBoss : public Sprite {
public:	
	explicit MY_DemonBoss(Shader * _shader);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};