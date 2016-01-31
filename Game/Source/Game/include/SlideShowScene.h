#pragma once

#include <MY_Scene_Base.h>
#include <Slide.h>
#include <MeshEntity.h>
#include <sweet\UI.h>
#include <Timeout.h>
class SlideShowScene : public MY_Scene_Base{
private:
	Slide * currSlide;
	std::vector<Slide *> forwards;
	std::vector<Slide *> backwards;

	NodeUI * slide;
	NodeUI * slideOld;
	NodeUI * slideNew;

	bool isTransitioning;
public:
	SlideShowScene(Game * _game);
	~SlideShowScene();

	virtual void update(Step * _step) override;

	Timeout * transition;
	sweet::EventManager eventManager;

	void push(Slide * _slide);
	void next();
	void prev();
	void setNewSlide();
	void transitionNext();
};