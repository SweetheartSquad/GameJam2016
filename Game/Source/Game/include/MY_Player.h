#pragma once

#include <Box2DSprite.h>
#include <Joystick.h>
#include <EventManager.h>
#include <Animation.h>
#include <Timeout.h>
#include <OpenALSound.h>

#define MAX_LIVES 8

class MY_DemonSpirit;

class MY_Player : public Sprite {
public:
	enum PlayerState {
		kIDLE,
		kWALK,
		kSIP,
		kRIP_AND_GRIP,
		kHURT
	} state;
	Timeout * stateChangeTimer;

	bool isDead;

	float speed;
	float bounds;
	static int lives;

	Animation<float> * idleScaleAnim;
	glm::vec3 scaleAnim;

	MY_DemonSpirit * spirit;

	bool quipPlayed;

	bool invincible;
	float invincibilityTimer; // Counts DOWN
	float invincibilityTimerLength;

	Joystick * joystick;
	float mouseWalk;

	sweet::EventManager eventManager;

	explicit MY_Player(Shader * _shader);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;

	void spawnDemon();

	Timeout * voiceTimer;

	// prevents player input for _seconds
	void delayChange(float _seconds, PlayerState _state);

private :
	OpenAL_Sound * footsetpSound;
	float stepTimer;
	bool highStep;

	// state set when the stateChange timer runs out
	PlayerState delayedState;

	//Invicibility timers
	float flashTimer;

	OpenAL_Sound * voice;
};

