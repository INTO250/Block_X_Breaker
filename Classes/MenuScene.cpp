#include "MenuScene.h"
#include "GameScene.h"
#include "AudioEngine.h"
#include "StageSelectScene.h"
USING_NS_CC;
#define BG_HEIGHT 1440
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
    auto label_1 = Label::createWithTTF("Story Mode", "fonts/Marker Felt.ttf", 48);
    auto label_2 = Label::createWithTTF("Arcade Mode", "fonts/Marker Felt.ttf", 48);
    auto label_3 = Label::createWithTTF("Multiplayer Mode", "fonts/Marker Felt.ttf", 48);
    auto label_4 = Label::createWithTTF("Ranking", "fonts/Marker Felt.ttf", 48);
    auto label_5 = Label::createWithTTF("Settings", "fonts/Marker Felt.ttf", 48);
    auto label_6 = Label::createWithTTF("Exit", "fonts/Marker Felt.ttf", 48);  //不支持中文，以后再改
    auto item_1 = MenuItemLabel::create(label_1, CC_CALLBACK_1(MenuScene::storyModeCallback, this));
    auto item_2 = MenuItemLabel::create(label_2, CC_CALLBACK_1(MenuScene::arcadeModeCallback, this));
    auto item_3 = MenuItemLabel::create(label_3, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_4 = MenuItemLabel::create(label_4, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_5 = MenuItemLabel::create(label_5, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_6 = MenuItemLabel::create(label_6, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto MainMenu = Menu::create(item_1, item_2, item_3, item_4, item_5, item_6, NULL);
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
       logo->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 3 * 2 + origin.y));

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
    Director::getInstance()->replaceScene(GameScene::create());
}
void MenuScene::storyModeCallback(cocos2d::Ref* pSender)
{
    AudioEngine::stopAll();
    auto stageSelectBGM = AudioEngine::play2d("stage_select_BGM.mp3");
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, StageSelect::create()));
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
