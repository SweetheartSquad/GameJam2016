#pragma once

#include <Box2DSprite.h>
#include <Joystick.h>
#include <EventManager.h>

class MY_Player : public Sprite {
public:
	bool isDead;

	float speed;
	float health;

	float invincibilityTimer; // Counts DOWN
	float invincibilityTimerLength;

	Joystick * joystick;

	sweet::EventManager eventManager;

	explicit MY_Player(Shader * _shader);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};
