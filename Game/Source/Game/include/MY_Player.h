#pragma once

#include <Box2DSprite.h>
#include <Joystick.h>
#include <EventManager.h>
#include <Animation.h>
#include <Timeout.h>

#define MAX_LIVES 5

class MY_Player : public Sprite {
public:
	bool paused;
	Timeout * pauseTimer;

	bool isDead;

	float speed;
	float bounds;
	static int lives;

	Animation<float> * idleScaleAnim;
	glm::vec3 scaleAnim;

	bool invincible;
	float invincibilityTimer; // Counts DOWN
	float invincibilityTimerLength;

	Joystick * joystick;

	sweet::EventManager eventManager;

	explicit MY_Player(Shader * _shader);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;

	Timeout * voiceTimer;

	// prevents player input for _seconds
	void pause(float _seconds);
};

