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
    auto stage_1 = MenuItemImage::create("stage_1.png", "stage_1_selected.png", CC_CALLBACK_1(StageSelect::stage_1_select, this));
    auto stage_1_menu = Menu::create(stage_1, NULL);
    stage_1_menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);

    auto closeButton = MenuItemImage::create("close_button.png", "close_button_selected.png", CC_CALLBACK_1(StageSelect::backToMenu, this));
    auto closeButtonMenu = Menu::create(closeButton, NULL);
    closeButtonMenu->setAnchorPoint(Vec2(0, 1));
    closeButtonMenu->setPosition(Vec2(50, visibleSize.height - 50));

   
    this->addChild(stage_1_menu, -1);
    this->addChild(closeButtonMenu, -1);
   
    return true;
}
void StageSelect::stage_1_select(cocos2d::Ref* pSender)
{
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(1)));
}
void StageSelect::backToMenu(cocos2d::Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
}
