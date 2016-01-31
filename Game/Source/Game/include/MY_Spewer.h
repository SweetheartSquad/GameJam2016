#pragma once

#include <Sprite.h>
#include <EventManager.h>

class MY_Spewer : public Sprite  {
public:
	glm::vec3 startPos;

	float deltaX;
	float deltaY;

	float speed;
	
	float moveTimer;
	float moveTimerDuration;

	float spewTimer;
	float spewTimerDuration;

	float vOffset;

	bool isComplete;

	Sprite * spew;

	sweet::EventManager eventManager;

	MY_Spewer(Shader * _shader, glm::vec3 _startPos, glm::vec3 _targetPos);
	~MY_Spewer();

	virtual void update(Step * _step) override;
};