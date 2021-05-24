#pragma once
#include "cocos2d.h"

class Arrow : public cocos2d::Sprite
{
public:
	static Arrow* createArrow(const char* filename);
	int length; // ³¤¶È
};