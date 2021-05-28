#pragma once
#ifndef _SETTINGS_SCENE_H_
#define _SETTINGS_SCENE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class SettingsScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	cocos2d::Sprite* background_1;
	cocos2d::Sprite* background_2;
	void update(float dt);
	void backToMenu(cocos2d::Ref* pSender);
	void onMusicSliderMoved(cocos2d::Ref* pSender, cocos2d::ui::Slider::EventType type);
	void onSoundSliderMoved(cocos2d::Ref* pSender, cocos2d::ui::Slider::EventType type);
	cocos2d::ui::Slider* musicSlider;
	cocos2d::ui::Slider* soundSlider;
	CREATE_FUNC(SettingsScene);
};
#endif