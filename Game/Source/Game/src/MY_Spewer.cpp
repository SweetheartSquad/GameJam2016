#include <MY_Spewer.h>
#include <MeshInterface.h>
#include <MY_ResourceManager.h>
#include <Easing.h>

#define SPEWER_SIZE 5

MY_Spewer::MY_Spewer(Shader * _shader, glm::vec3 _startPos, glm::vec3 _targetPos) :
	Sprite(_shader),
	startPos(_startPos),
	deltaX(_targetPos.x - _startPos.x),
	deltaY(_targetPos.y - _startPos.y),
	speed(20.f),
	moveTimer(0),
	moveTimerDuration(std::abs(deltaX)/speed),
	spewTimer(0),
	spewTimerDuration(2.f),
	spew(new Sprite(_shader)),
	isComplete(false)
{
	deltaY -= SPEWER_SIZE * 0.5;
	meshTransform->scale(SPEWER_SIZE, SPEWER_SIZE, 1.f);

	mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture);
	
	spew->meshTransform->scale(glm::vec3(SPEWER_SIZE, deltaY, 1.f));
	// align spew sprite's bottom with the origin
	for(int i = 0; i < spew->mesh->vertices.size(); ++i){
		spew->mesh->vertices.at(i).y += 0.5;
	}
	spew->setVisible(false);
	childTransform->addChild(spew);
}

MY_Spewer::~MY_Spewer(){
}

void MY_Spewer::update(Step * _step){
	Sprite::update(_step);
	eventManager.update(_step);

	if(!isComplete){
		if(moveTimer <= moveTimerDuration){
			float x = deltaX * moveTimer/moveTimerDuration + startPos.x;
			float y = Easing::easeOutCubic(moveTimer, startPos.y, deltaY, moveTimerDuration);
			childTransform->translate(x, y, 0.f, false);

			moveTimer += _step->getDeltaTime();
		}else{
			spew->setVisible(true);
		}

		if(spew->isVisible()){
			// Spew!!!!!
			if(spewTimer <= spewTimerDuration){
				spew->firstParent()->translate(0, -spewTimer/spewTimerDuration * deltaY, 0, false);
				spewTimer += _step->getDeltaTime();
			}else{
				isComplete = true;
				eventManager.triggerEvent("spewComplete");
			}
		}
	}
}