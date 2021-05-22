#pragma once
#ifndef  _BLOCK_H_
#define  _BLOCK_H_
#include "cocos2d.h"

class Block : public cocos2d::Sprite
{
public:
	static Block* createBlock(const char* filename);
	bool exsistence; //是否存在
	int type; //形状
	int life; //耐久
	int bouns; //道具种类
};
#endif