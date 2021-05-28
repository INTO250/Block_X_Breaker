#include "StageSelectScene.h"
#include "GameScene.h"
#include "AudioEngine.h"
USING_NS_CC;
extern int BGM;
extern float volumeSound;
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
    stage_1_menu->setPosition(visibleSize.width / 2 - 400, visibleSize.height / 2 + 400);
    this->addChild(stage_1_menu, -1);

    auto stage_2 = MenuItemImage::create("stage_2.png", "stage_2_selected.png", CC_CALLBACK_1(StageSelect::stage_2_select, this));
    auto stage_2_menu = Menu::create(stage_2, NULL);
    stage_2_menu->setPosition(visibleSize.width / 2 - 300, visibleSize.height / 2 - 200);
    this->addChild(stage_2_menu, -1);

    auto stage_3 = MenuItemImage::create("stage_3.png", "stage_3_selected.png", CC_CALLBACK_1(StageSelect::stage_3_select, this));
    auto stage_3_menu = Menu::create(stage_3, NULL);
    stage_3_menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(stage_3_menu, -1);

    auto stage_4 = MenuItemImage::create("stage_4.png", "stage_4_selected.png", CC_CALLBACK_1(StageSelect::stage_4_select, this));
    auto stage_4_menu = Menu::create(stage_4, NULL);
    stage_4_menu->setPosition(visibleSize.width / 2 + 300, visibleSize.height / 2 - 500);
    this->addChild(stage_4_menu, -1);

    auto stage_5 = MenuItemImage::create("stage_5.png", "stage_5_selected.png", CC_CALLBACK_1(StageSelect::stage_5_select, this));
    auto stage_5_menu = Menu::create(stage_5, NULL);
    stage_5_menu->setPosition(visibleSize.width / 2 + 400, visibleSize.height / 2 + 500);
    this->addChild(stage_5_menu, -1); 

    auto closeButton = MenuItemImage::create("close_button.png", "close_button_selected.png", CC_CALLBACK_1(StageSelect::backToMenu, this));
    auto closeButtonMenu = Menu::create(closeButton, NULL);
    closeButtonMenu->setAnchorPoint(Vec2(0, 1));
    closeButtonMenu->setPosition(Vec2(50, visibleSize.height - 50));

    AudioEngine::preload("sound_click.mp3");
    
    this->addChild(closeButtonMenu, -1);
   
    return true;
}
void StageSelect::stage_1_select(cocos2d::Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("stage_1_BGM.mp3", true, volumeBGM);
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(1)));
}
void StageSelect::stage_2_select(cocos2d::Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(2)));
}
void StageSelect::stage_3_select(cocos2d::Ref* pSender)
{
    
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(3)));
}
void StageSelect::stage_4_select(cocos2d::Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(4)));
}
void StageSelect::stage_5_select(cocos2d::Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(5)));
}
void StageSelect::backToMenu(cocos2d::Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("MainMenu.mp3", true, volumeBGM);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
}
