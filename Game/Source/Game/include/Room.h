#pragma once

#include <MeshEntity.h>
#include <NumberUtils.h>

class Entity;
class Shader;
class Texture;

class Room : public MeshEntity {
private:
	static sweet::ShuffleVector<unsigned long int> roomTexIdx;
	static bool staticInit();
	static bool staticInitialized;
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

	Texture * getRoomTex();
};