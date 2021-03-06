#pragma once

#include <Sprite.h>
#include <Timeout.h>
#include <Animation.h>
#include <NumberUtils.h>


class MY_Player;
class MY_Demon;

class MY_DemonSpirit : public Sprite{
private:
	
	bool canPlayGripit;
	Timeout gripitTimeout;

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
	MY_Player * player;
	Sprite * indicator;

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
	
	static sweet::ShuffleVector<unsigned long int> demonIds;
	static bool staticInitialized;
	static bool staticInit();

	bool canPlayGrowl;
	Timeout growlTimeout;

public:
	glm::vec3 scaleAnim;
	enum DemonState {
		kWALK,
		kIDLE,
		kDEAD,
		kSAVED,
		kSTUNNED
	} state;

	// reference to the spirit possessing this character
	MY_DemonSpirit * spirit;
	MY_DemonSpirit_False * spiritFake1;
	MY_DemonSpirit_False * spiritFake2;

	float speed;
	float damage;
	Transform * target;

	bool isDummy;

	Timeout * stateTimeout;

	sweet::EventManager eventManager;

	explicit MY_Demon(Shader * _shader, Transform * _target);

	void render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	void update(Step * _step) override;
	void unload() override;
	void load() override;

	void kill(bool _saved);
};