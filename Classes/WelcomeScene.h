#pragma once

#include "cocos2d.h"

class Welcome : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void  myUpdate(float dt);
    CREATE_FUNC(Welcome);
};