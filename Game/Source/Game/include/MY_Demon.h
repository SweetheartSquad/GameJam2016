#pragma once

#include <Sprite.h>
#include <Timeout.h>

enum DemonState {
	kWALKING,
	kIDLE
};

class MY_Demon : public Sprite {
public:	

	float speed;
	DemonState currentState;
	Transform * target;

	Timeout * stateTimeout;

	explicit MY_Demon(Shader * _shader, Transform * _target);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};