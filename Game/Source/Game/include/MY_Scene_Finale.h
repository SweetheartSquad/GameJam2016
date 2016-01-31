#pragma once

#include <MY_Scene_MenuBase.h>

#define NUM_REQUIRED_PRESSES 10

class MY_Scene_Finale : public MY_Scene_MenuBase{
public:
	MY_Scene_Finale(Game * _game);
	~MY_Scene_Finale();
	
	// number of correctly pressed buttons since start of QTE
	unsigned long int correct;
	// alternates between true and false on each correct button press
	bool nextIsRight;

	Timeout * timer;
	// ripit, gripit, or sipit (in that order)
	std::string curTarget;

	sweet::EventManager * eventManager;

	virtual void update(Step * _step) override;


	void correctPress();
	void incorrectPress();

	void passTarget();
};