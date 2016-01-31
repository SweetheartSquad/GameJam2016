#pragma once

#include <Sprite.h>
#include <Timeout.h>
#include <Animation.h>


class MY_Player;
class MY_Demon;

class MY_DemonSpirit : public Sprite{
private:

	glm::vec3 speed;
	// when ripped, the spirit is stunned and stops targeting its body
	// after a timer runs out, it starts going back
	Timeout * stunTimer;

	Animation<float> * idleScaleAnim;
public:
	enum DemonSpiritState {
		kIN,
		kSTUNNED,
		kOUT,
		kDEAD
	} state;

	glm::vec3 scaleAnim;
	glm::vec3 origin;
	// reference to the character this spirit is possessing
	MY_Demon * possessed;
	MY_Demon * player;

	MY_DemonSpirit(Shader * _shader, MY_Demon * _possessed, unsigned long int _mode);
	MY_DemonSpirit(Shader * _shader, MY_Player * _possessed);

	virtual void update(Step * _step) override;
	
	// rip the spirit from its body
	virtual void ripIt();
	// grip the spirit away from its body
	void gripIt();
	// kill the demon i guess?
	void sipIt();
	// put the spirit back in its body
	void getBackInThere();


	// returns the sum of this spirit's translation vector and its body's translation vector
	glm::vec3 getGamePos();
};


class MY_DemonSpirit_False : public MY_DemonSpirit{
public:
	MY_DemonSpirit_False(Shader * _shader, MY_Demon * _possessed, unsigned long int _mode);

	virtual void ripIt() override;
};

class MY_Demon : public Sprite {
private:
	Animation<float> * idleScaleAnim;
public:
	glm::vec3 scaleAnim;
	enum DemonState {
		kWALK,
		kIDLE,
		kDEAD,
		kSTUNNED
	} state;

	// reference to the spirit possessing this character
	MY_DemonSpirit * spirit;
	MY_DemonSpirit_False * spiritFake1;
	MY_DemonSpirit_False * spiritFake2;

	float speed;
	float damage;
	Transform * target;

	Timeout * stateTimeout;

	sweet::EventManager eventManager;

	explicit MY_Demon(Shader * _shader, Transform * _target);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;

	void kill();
};