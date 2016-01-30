#pragma once

#include <MeshEntity.h>
class Entity;
class Shader;

class Room : public MeshEntity {
public:
	float width, height, depth;

	Transform * foreground;
	Transform * gameground;
	Transform * background;

	bool unlocked;
	float roomWidth;
	float doorPos;

	std::vector<Entity *> demons;

	Room(Shader * _shader);
	~Room();

	void unlock();
	
	void placeFG();
	void placeBG();
	void placeGG();
};