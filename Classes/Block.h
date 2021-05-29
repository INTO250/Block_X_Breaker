#pragma once
#ifndef  _BLOCK_H_
#define  _BLOCK_H_
#include "cocos2d.h"

class Block
{
public:
	cocos2d::Sprite* sprite;
	bool exsistence; //是否存在
	int life; //耐久
	int bonus; //道具种类
	cocos2d::Vec2 position;
};
#endif