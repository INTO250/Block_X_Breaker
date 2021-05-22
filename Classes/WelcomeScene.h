#pragma once
#ifndef  _WELCOME_SCENE_H_
#define  _WELCOME_SCENE_H_
#include "cocos2d.h"

class Welcome : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void  myUpdate(float dt);
    CREATE_FUNC(Welcome);
};
#endif