#include "WelcomeScene.h"
#include "MenuScene.h"
#include "AudioEngine.h"
#include "SettingsScene.h"
USING_NS_CC;
int BGM;
float volumeSound = 1.0f;
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
    AudioEngine::preload("sound_openning.mp3");
    AudioEngine::preload("sound_click.mp3");
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto logo = Sprite::create("logo.jpg");
    if (logo == nullptr)
    {
        problemLoading("logo_1.png");
    }
    else
    {
        logo->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y)); //屏幕中央
        this->addChild(logo, 0);
    }
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Welcome::myUpdate), 4.0f); //计时，只执行一次myUpdate()

    auto sound = AudioEngine::play2d("sound_openning.mp3", false, 1.0f);
    return true;
}
void Welcome::myUpdate(float dt)
{
    BGM = AudioEngine::play2d("MainMenu.mp3", true, 1.0); 
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
}