#pragma once
#ifndef  _STAGE_SELECT_H_
#define  _STAGE_SELECT_H_
#include "cocos2d.h"

class StageSelect : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void stage_select(Ref* pSender);
    CREATE_FUNC(StageSelect);
};
#endif