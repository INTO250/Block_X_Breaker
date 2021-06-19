#include "MenuScene.h"
#include "GameScene.h"
#include "AudioEngine.h"
#include "StageSelectScene.h"
#include "SettingsScene.h"
#include "MatchingScene.h"
USING_NS_CC;
#define BG_HEIGHT 1404
extern int BGM;
extern float volumeSound;
#define WINDOWS
Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}
bool MenuScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    


    background_1 = Sprite::create("background.jpg");
    background_2 = Sprite::create("background.jpg"); //用两张背景图来实现背景滚动的效果
    background_1->setAnchorPoint(Vec2(0, 0));
    background_2->setAnchorPoint(Vec2(0, 0));
    background_1->setPosition(Vec2(0, 0));
    background_2->setPosition(Vec2(0, BG_HEIGHT));
    this->addChild(background_1, -5);
    this->addChild(background_2, -5);

    auto item_1 = MenuItemImage::create("story_mode.png", "story_mode_selected.png", CC_CALLBACK_1(MenuScene::storyModeCallback, this));
    auto item_2 = MenuItemImage::create("arcade_mode.png", "arcade_mode_selected.png", CC_CALLBACK_1(MenuScene::storyModeCallback, this));
    auto item_3 = MenuItemImage::create("multiplayer_mode.png", "multiplayer_mode_selected.png", CC_CALLBACK_1(MenuScene::multiplayerModeCallback, this));
    auto item_4 = MenuItemImage::create("settings.png", "settings_selected.png", CC_CALLBACK_1(MenuScene::settingsCallback, this));
    auto MainMenu = Menu::create(item_1, item_2, item_3, item_4, NULL);
    MainMenu->alignItemsVerticallyWithPadding(20);
    MainMenu->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 3 + origin.y));
    this->addChild(MainMenu, 0);
    auto logo = Sprite::create("logo_title.png");
    if (logo == nullptr)
    {
        problemLoading("logo");
    }
    else
    {
        // position the sprite on the center of the screen
       logo->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 4 * 3 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(logo, 0);
    }
    scheduleUpdate(); //每帧执行update()
    return true;
}
void MenuScene::menuCloseCallback(cocos2d::Ref* pSender)
{
    Director::getInstance()->end();
}
void MenuScene::arcadeModeCallback(cocos2d::Ref* pSender)
{
    Director::getInstance()->replaceScene(GameScene::createScene(0));
}
void MenuScene::storyModeCallback(cocos2d::Ref* pSender)
{
    
    auto sound = AudioEngine::play2d("sound_click.mp3.mp3", false, volumeSound);
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("stage_select_BGM.mp3", true, volumeBGM);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, StageSelect::createScene()));
}
void MenuScene::multiplayerModeCallback(cocos2d:: Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
#ifdef WINDOWS
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("matching_BGM.mp3", true, volumeBGM);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MatchingScene::createScene()));
#endif
}
void MenuScene::settingsCallback(cocos2d::Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, SettingsScene::createScene()));
}
void MenuScene::update(float dt)
{
    background_1->setPositionY(background_1->getPositionY() - 1);
    background_2->setPositionY(background_2->getPositionY() - 1);
    if (background_1->getPositionY() < -BG_HEIGHT)
    {
        background_1->setPositionY(BG_HEIGHT);
    }
    if (background_2->getPositionY() < -BG_HEIGHT)
    {
        background_2->setPositionY(BG_HEIGHT);
    }
}
