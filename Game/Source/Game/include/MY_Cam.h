#pragma once

#include <PerspectiveCamera.h>
#include <Timeout.h>

class MY_Cam : public PerspectiveCamera{
public:
	float shake;
	Timeout * shakeTimer;

	MY_Cam();

	virtual void update(Step * _step);
};