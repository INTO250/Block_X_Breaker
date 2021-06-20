#pragma once
#ifndef  _MATCHING_SCENE_H_
#define  _MATCHING_SCENE_H_
#include "cocos2d.h"
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
class MatchingScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    cocos2d::Sprite* background_1;
    cocos2d::Sprite* background_2;
    virtual bool init();
    void update(float dt);
    void backToMenu(cocos2d::Ref* pSender);
    CREATE_FUNC(MatchingScene);
    
    cocos2d::Label* matchLabel;
    cocos2d::ui::Button* startButton;
    void gameStart(cocos2d::Ref* pSender, cocos2d::ui::Button::TouchEventType type);
};
#endif