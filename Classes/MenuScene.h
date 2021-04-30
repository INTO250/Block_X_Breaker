#pragma once

#include "cocos2d.h"

class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    char* filename;
    void menuCloseCallback(cocos2d::Ref* pSender);
    CREATE_FUNC(MenuScene);
};