#pragma once
#ifndef  _BLOCK_H_
#define  _BLOCK_H_
#include "cocos2d.h"

class Block : public cocos2d::Sprite
{
public:
	static Block* createBlock(const char* filename);
	Sprite* block_check;
	bool exsistence; //�Ƿ����
	int type; //��״
	int life; //�;�
	int bouns; //��������
};
#endif