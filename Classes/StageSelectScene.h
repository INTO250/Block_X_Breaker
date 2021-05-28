#pragma once
#ifndef  _STAGE_SELECT_H_
#define  _STAGE_SELECT_H_
#include "cocos2d.h"
#include "MenuScene.h"

class StageSelect : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void stage_1_select(cocos2d::Ref* pSender);
    void stage_2_select(cocos2d::Ref* pSender);
    void stage_3_select(cocos2d::Ref* pSender);
    void stage_4_select(cocos2d::Ref* pSender);
    void stage_5_select(cocos2d::Ref* pSender);
    void backToMenu(cocos2d::Ref* pSender);
    CREATE_FUNC(StageSelect);
};
#endif