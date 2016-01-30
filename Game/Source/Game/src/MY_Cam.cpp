#pragma once

#include <MY_Cam.h>

MY_Cam::MY_Cam(){

}

void MY_Cam::update(Step * _step){
	PerspectiveCamera::update(_step);
}