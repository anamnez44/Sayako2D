#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
View view;
View GetView(float x, float y)
{
	float tempx=0, tempy;
	float dx =0.3, dy;
	float sx,sy;
	
	tempx = x;
	tempy = y;
	
	if (tempx < 320)tempx = 320;
	if (tempy >1450)tempy = 1450;
	
	view.setCenter(tempx, tempy);
	return view;
}