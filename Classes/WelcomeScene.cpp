#include "WelcomeScene.h"
#include "MenuScene.h"
USING_NS_CC;

Scene*  Welcome::createScene()
{
	return Welcome::create();
}
static void problemLoading(const char* filename) 
{
    printf("这里应该是: %s\n", filename);
}
bool Welcome::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto logo = Sprite::create("logo_1.png");
    if (logo == nullptr)
    {
        problemLoading("logo_1.png");
    }
    else
    {
        logo->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y)); //屏幕中央
        this->addChild(logo, 0);
    }
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Welcome::myUpdate), 4.0f); //计时
    return true;
}
void Welcome::myUpdate(float dt)
{
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
    //Director::getInstance()->end();
}