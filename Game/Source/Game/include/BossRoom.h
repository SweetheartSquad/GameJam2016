#pragma once

#include <Room.h>

class Entity;
class Shader;
class Texture;

class BossRoom : public Room{
private:
public:
	BossRoom(Shader * _shader);
	~BossRoom();
	
	void placeFG();
	void placeBG();
	void placeGG();

	virtual Texture * getRoomTex() override;
	virtual MeshEntity * getFurnitureSet() override;
};