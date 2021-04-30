#include "MenuScene.h"

USING_NS_CC;

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
    auto label_1 = Label::createWithTTF("Stroy Mode", "fonts/Marker Felt.ttf", 48);
    auto label_2 = Label::createWithTTF("Arcade Mode", "fonts/Marker Felt.ttf", 48);
    auto label_3 = Label::createWithTTF("Multiplayer Mode", "fonts/Marker Felt.ttf", 48);
    auto label_4 = Label::createWithTTF("Ranking", "fonts/Marker Felt.ttf", 48);
    auto label_5 = Label::createWithTTF("Settings", "fonts/Marker Felt.ttf", 48);
    auto item_1 = MenuItemLabel::create(label_1, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_2 = MenuItemLabel::create(label_2, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_3 = MenuItemLabel::create(label_3, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_4 = MenuItemLabel::create(label_4, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto item_5 = MenuItemLabel::create(label_5, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    auto MainMenu = Menu::create(item_1, item_2, item_3, item_4, item_5, NULL);
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
    return true;
}
void MenuScene::menuCloseCallback(cocos2d::Ref* pSender)
{
    Director::getInstance()->end();
}
