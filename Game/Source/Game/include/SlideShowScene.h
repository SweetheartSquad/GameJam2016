#pragma once

#include <MY_Scene_Base.h>

namespace sweet{
	class EventManager;
};
class Timeout;
class NodeUI;
class Slide;

class SlideShowScene : public MY_Scene_Base{
private:
	Slide * currSlide;
	std::vector<Slide *> forwards;
	std::vector<Slide *> backwards;

	NodeUI * slide;
	NodeUI * slideOld;
	NodeUI * slideNew;

	bool isTransitioning;
	bool isTransitioningForwards;

	void next();
	void prev();
	void setNewSlide(bool _isForwards);
public:
	SlideShowScene(Game * _game);
	~SlideShowScene();

	virtual void update(Step * _step) override;

	Timeout * transition;
	sweet::EventManager * eventManager;

	void push(Slide * _slide);
	
	void changeSlide(bool _isForwards);
};