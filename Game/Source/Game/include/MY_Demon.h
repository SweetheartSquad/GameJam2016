#pragma once

#include <Sprite.h>
#include <Timeout.h>

enum DemonState {
	kWALKING,
	kIDLE
};

enum DemonSpiritState {
	kIN,
	kSTUNNED,
	kOUT
};



class MY_Demon;

class MY_DemonSpirit : public Sprite{
private:

	glm::vec3 speed;
	// when ripped, the spirit is stunned and stops targeting its body
	// after a timer runs out, it starts going back
	Timeout * stunTimer;
public:
	DemonSpiritState state;
	// reference to the character this spirit is possessing
	MY_Demon * possessed;
	MY_DemonSpirit(Shader * _shader, MY_Demon * _possessed);

	virtual void update(Step * _step) override;
	
	// rip the spirit from its body
	void ripIt();
	// grip the spirit away from its body
	void gripIt();
	// put the spirit back in its body
	void getBackInThere();
};

class MY_Demon : public Sprite {
public:
	// reference to the spirit possessing this character
	MY_DemonSpirit * spirit;

	float speed;
	DemonState currentState;
	Transform * target;

	Timeout * stateTimeout;

	sweet::EventManager eventManager;

	explicit MY_Demon(Shader * _shader, Transform * _target);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;
};