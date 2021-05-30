#pragma once
#include "cocos2d.h"

class Board : public cocos2d::Sprite
{
public:
	static Board* createBoard(const char* filename);
	int length; // ³¤¶È
	int life;
};