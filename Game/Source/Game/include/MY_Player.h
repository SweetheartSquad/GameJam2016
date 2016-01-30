#pragma once

#include <Box2DSprite.h>

class MY_Player : public Box2DSprite {
public:

	explicit MY_Player(Box2DWorld* _world);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};

