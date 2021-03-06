#include <MY_Spewer.h>
#include <MeshInterface.h>
#include <MY_ResourceManager.h>
#include <Easing.h>

#define SPEWER_SIZE 5
#define SPEW_SIZE 4

MY_Spewer::MY_Spewer(Shader * _shader, glm::vec3 _startPos, glm::vec3 _targetPos, int _column, float _columnWidth) :
	Sprite(_shader),
	column(_column),
	columnWidth(_columnWidth),
	startPos(_startPos),
	deltaX(_targetPos.x - _startPos.x),
	deltaY(_targetPos.y - _startPos.y),
	speed(25.f),
	moveTimer(0),
	moveTimerDuration(std::abs(deltaX)/speed),
	slideX(-(columnWidth - SPEWER_SIZE * 0.5)),
	spewTimer(0),
	spewTimerDuration(2.f),
	vOffset(0),
	spew(new Sprite(_shader)),
	isComplete(false)
{
	deltaY -= SPEWER_SIZE * 0.5;
	deltaX += columnWidth - SPEWER_SIZE * 0.5;

	meshTransform->scale(SPEWER_SIZE, SPEWER_SIZE, 1.f);

	mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("SPEWER")->texture);
	
	spew->meshTransform->scale(glm::vec3(SPEW_SIZE, 1.f, 1.f));
	spew->mesh->pushTexture2D(MY_ResourceManager::globalAssets->getTexture("SPEW")->texture);
	
	childTransform->addChild(spew);

	disable();
}

MY_Spewer::~MY_Spewer(){
}

void MY_Spewer::start(){
	isComplete = false;
	vOffset = 0;
	moveTimer = 0;
	spewTimer = 0;

	for(int i = 0; i < spew->mesh->vertices.size(); ++i){
		spew->mesh->vertices.at(i).y = 0;
	}
	spew->mesh->dirty = true;

	spew->setVisible(false);

	meshTransform->scale(glm::vec3(SPEWER_SIZE, SPEWER_SIZE, 1.f), false);
	spew->meshTransform->scale(glm::vec3(SPEW_SIZE, 1.f, 1.f), false);

	enable();
}

void MY_Spewer::update(Step * _step){
	Sprite::update(_step);
	eventManager.update(_step);

	if(isEnabled){
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
					float progress = spewTimer/spewTimerDuration;

					// Slide & Shrink
					float x = startPos.x + deltaX + slideX * progress;
					childTransform->translate(x, startPos.y + deltaY, 0, false);

					meshTransform->scale(SPEWER_SIZE * (1 - progress), false);
					spew->meshTransform->scale(glm::vec3(SPEW_SIZE * (1 - progress), 1, 1), false);

					if(spewTimer <= spewTimerDuration * 0.5){
						// Falling
						float y = -spewTimer/(spewTimerDuration * 0.5) * deltaY;

						spew->mesh->vertices.at(2).y = y;
						spew->mesh->vertices.at(3).y = y;
					}else{
						// Collapsing
						float y = -(spewTimer - spewTimerDuration * 0.5)/(spewTimerDuration * 0.5) * deltaY;

						spew->mesh->vertices.at(0).y = y;
						spew->mesh->vertices.at(1).y = y;
					}
					vOffset += 0.05;
					spewTimer += _step->getDeltaTime();

					// Additional offset
					for(auto &v : spew->mesh->vertices){
						v.v =  v.y/SPEW_SIZE + vOffset;
					}

					spew->mesh->dirty = true;
				}else{
					isComplete = true;
					disable();
					sweet::Event * e = new sweet::Event("spewComplete");
					e->setIntData("column", column);
					std::stringstream s;
					s << "COMPLETE: " << column;
					Log::info(s.str());
					eventManager.triggerEvent(e);
				}
			}
		}
	}
}

void MY_Spewer::enable(){
	setVisible(true);
	isEnabled = true;
}

void MY_Spewer::disable(){
	setVisible(false);
	isEnabled = false;
}