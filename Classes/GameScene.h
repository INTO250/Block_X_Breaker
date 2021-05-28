#pragma once
#ifndef  _GAME_SCENE_H_
#define  _GAME_SCENE_H_
#include<iostream>
#include<string>
#include "cocos2d.h"
#include "Ball.h"
#include "Board.h"
#include "Arrow.h"
#include "Block.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int stage);
    virtual bool initWithPhysics(int stage);
    static cocos2d::PhysicsWorld* world;
    std::vector<Block*> Blocks;
    Block Blockstore[1000];
    Block toughBlockstore[1000];
    Block bonus[1000];
    std::vector<Ball*> Balls;
    Board* board;
    Arrow* arrow;
    Arrow* powerArrow;
    Arrow* powerpng;
    void blocks_create(int stage);
    void onEnter();
    void update(float dt);
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
   
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
    void boardrotation();
    void doubleball();
    void transform();
    std::map<cocos2d::EventKeyboard::KeyCode, bool> keys;
    cocos2d::PhysicsJointPin* joint;
    bool gameStart;
    cocos2d::Size visibleSize;
    void BackToStage(cocos2d::Ref* pSender);
    int power=0;
    int shootvec = 0;//初始发射方向
    int remainBlocks = 0;
    std::string trans(long long int value);
    int block_order = 0;
    int toughblock_order = 0;
    int bonus_order = 0;
};
#endif