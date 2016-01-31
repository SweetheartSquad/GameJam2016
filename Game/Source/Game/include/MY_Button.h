#pragma once

#include <NodeUI.h>
class Shader;

class MY_Button : public NodeUI{
private:
	Texture * texNormal;
	Texture * texOver;
	Texture * texDown;
public:

	MY_Button(BulletWorld * _world, std::string _name);
	
	void update(Step * _step) override;
};