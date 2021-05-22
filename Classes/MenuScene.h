#pragma once
#ifndef  _MENU_SCENE_H_
#define  _MENU_SCENE_H_
#include "cocos2d.h"

class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    cocos2d::Sprite* background_1;
    cocos2d::Sprite* background_2;
    virtual bool init();
    void menuCloseCallback(cocos2d::Ref* pSender);
    void arcadeModeCallback(cocos2d::Ref* pSender);
    void storyModeCallback(cocos2d::Ref* pSender);
    void update(float dt);
    CREATE_FUNC(MenuScene);
};
#endif