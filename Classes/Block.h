#pragma once
#ifndef  _BLOCK_H_
#define  _BLOCK_H_
#include "cocos2d.h"

class Block
{
public:
	cocos2d::Sprite* sprite;
	bool exsistence; //�Ƿ����
	int life; //�;�
	int bonus; //��������
	cocos2d::Vec2 position;
};
#endif