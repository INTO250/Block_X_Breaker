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
using namespace std;
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int stage);
    virtual bool initWithPhysics(int stage);
    static cocos2d::PhysicsWorld* world;
    std::vector<Block*> Blocks;
    std::vector<Ball*> Balls;
    Board* board;
    Arrow* arrow;
    void blocks_create(int stage);
    void onEnter();
    void update(float dt);
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
   
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
    std::map<cocos2d::EventKeyboard::KeyCode, bool> keys;
    cocos2d::PhysicsJointPin* joint;
    bool gameStart;
    cocos2d::Size visibleSize;
    void GameScene::BackToStage(cocos2d::Ref* pSender);
    int power=0;
    int shootvec = 0;//初始发射方向
    int remainBlocks = 0;
    string GameScene::trans(long long int value);
};
#endif