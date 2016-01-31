#pragma once

#include <Sprite.h>
#include <EventManager.h>

class MY_Spewer : public Sprite  {
public:
	int column;
	float columnWidth;

	// trajectory
	glm::vec3 startPos;
	float deltaX;
	float deltaY;
	float speed;
	
	float moveTimer;
	float moveTimerDuration;

	// spew
	float slideX;
	float spewTimer;
	float spewTimerDuration;

	float vOffset;

	bool isComplete;
	bool isEnabled;

	Sprite * spew;

	sweet::EventManager eventManager;

	MY_Spewer(Shader * _shader, glm::vec3 _startPos, glm::vec3 _targetPos, int _column, float _columnWidth);
	~MY_Spewer();

	void start();
	virtual void update(Step * _step) override;

	void enable();
	void disable();
};