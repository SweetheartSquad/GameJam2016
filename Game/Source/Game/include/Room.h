#pragma once

#include <MeshEntity.h>
#include <NumberUtils.h>

class Entity;
class Shader;
class Texture;

class Room : public MeshEntity {
private:
	static bool staticInit();
	static bool staticInitialized;
protected:
	static sweet::ShuffleVector<unsigned long int> roomTexIdx;
	static sweet::ShuffleVector<unsigned long int> furnitureSetIdx;
public:

	Transform * foreground;
	Transform * gameground;
	Transform * background;

	bool unlocked;
	float roomWidth;
	float doorPos;

	Room(Shader * _shader);
	~Room();

	void unlock();
	
	void placeFG();
	void placeBG();
	void placeGG();

	virtual Texture * getRoomTex();
	virtual MeshEntity * getFurnitureSet();
};