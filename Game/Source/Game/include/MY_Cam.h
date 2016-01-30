#pragma once

#include <PerspectiveCamera.h>

class MY_Cam : public PerspectiveCamera{
public:
	MY_Cam();

	virtual void update(Step * _step);
};