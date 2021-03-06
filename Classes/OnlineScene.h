#pragma once
#ifndef  _ONLINE_SCENE_H_
#define  _ONLINE_SCENE_H_
#include<iostream>
#include<string>
#include <vector>
#include "cocos2d.h"
#include "Ball.h"
#include "Board.h"
#include "Arrow.h"
#include "Block.h"
#include "ui/CocosGUI.h"
#ifdef WIN32
#include <WINSOCK2.H>
#pragma comment(lib,"ws2_32.lib")
typedef int				socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
typedef int				SOCKET;
#endif

class OnlineScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool initWithPhysics();
    static cocos2d::PhysicsWorld* world;
    std::vector<Block> Blocks;
    Ball* Balls[3];
    Board* board;
    Arrow* arrow;
    Arrow* powerArrow;
    Arrow* powerpng;
    cocos2d::Sprite* opponentBoard;
    cocos2d::Sprite* opponentBall;
    std::string blockInfo = "";

    void blocks_create(int stage);
    void onEnter();
    void update(float dt);
    bool onContactBegin(const cocos2d::PhysicsContact& contact);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
    
    void transform();
    std::map<cocos2d::EventKeyboard::KeyCode, bool> keys;
    std::map<std::string, bool> touches;

    bool gameStart;
    bool gameEnd = false;
    cocos2d::Size visibleSize;
    // void BackToStage(cocos2d::Ref* pSender);
    int power = 0;
    int shootvec = 0;//??ʼ???䷽??

    std::string trans(long long int value);
    int block_order = 1;

    void bonus_create(int type, cocos2d::Vec2);
    void check_win();
    int score = 0;
    cocos2d::Label* Score;
    cocos2d::Label* life;
    cocos2d::Label* opponentLife;
    cocos2d::ui::LoadingBar* skillsBar;
    cocos2d::ui::LoadingBar* opponentBar;
    cocos2d::TMXTiledMap* map;
    cocos2d::TMXTiledMap* mapOpponent;

    std::vector<cocos2d::Sprite*> Bonus;
    bool isSmall = false;
    bool isRotating = false;
    bool isBoost = false;
    void isSmallChange();
    void isRotatingChange();
    void isBoostChange();
    void oppoTransform();
    void oppoSmallChange();
    void oppoRotatingChange();
    void oppoBoostChange();
    void rotation();
    void oppoRotation();
    void speedUp();
    void oppoSpeedUp();


    void onButtonPressed(cocos2d::Ref* pSender);
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void gameWin();
    void gameLose();
    void winSend(float dt);
    void loseSend(float dt);


    int oppoSmall = 0;
    int oppoRotating = 0;
    int oppoBoost = 0;
    int stage = 0;
    float speed = 1.0f;
    
    void dataRS(float dt);


};
#endif