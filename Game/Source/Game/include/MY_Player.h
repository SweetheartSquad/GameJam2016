#pragma once

#include <Box2DSprite.h>

class MY_Player : public Sprite {
public:

	explicit MY_Player(Shader * _shader);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};

