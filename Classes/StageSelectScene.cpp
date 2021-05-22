#include "StageSelectScene.h"
#include "GameScene.h"
#include "AudioEngine.h"
USING_NS_CC;

Scene* StageSelect::createScene()
{
	return StageSelect::create();
}
bool StageSelect::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto worldMap = Sprite::create("WorldMap.png");
    worldMap->setAnchorPoint(Vec2(0, 0));
    worldMap->setPosition(Vec2(0, 0));
    this->addChild(worldMap, -5);
    auto stage_1 = MenuItemImage::create("stage_1.png", "stage_1_selected.png", CC_CALLBACK_1(StageSelect::stage_select, this));
    auto stages = Menu::create(stage_1, NULL);
    stages->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(stages, -1);
    return true;
}
void StageSelect::stage_select(cocos2d::Ref* pSender)
{
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(0)));
}