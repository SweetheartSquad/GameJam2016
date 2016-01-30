#pragma once

#include <MY_Cam.h>
#include <NumberUtils.h>

MY_Cam::MY_Cam() :
	shake(0)
{
	shakeTimer = new Timeout(0.25f, [&](sweet::Event * _event){
		shake = 0;
	});
	shakeTimer->eventManager->addEventListener("progress", [&](sweet::Event * _event){
		shake = 1.f - _event->getFloatData("progress");
	});
	childTransform->addChild(shakeTimer, false);
}

void MY_Cam::update(Step * _step){
	lookAtOffset.x = sin(_step->time + sweet::NumberUtils::randomFloat(-0.25f, 0.25f)) * 0.01f;
	lookAtOffset.y = cos(_step->time + sweet::NumberUtils::randomFloat(-0.25f, 0.25f)) * 0.01f;

	lookAtOffset += (sweet::NumberUtils::randomVec3(glm::vec3(-shake), glm::vec3(shake)) - lookAtOffset) * 0.03f;

	PerspectiveCamera::update(_step);
}