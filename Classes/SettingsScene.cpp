#include "MenuScene.h"
#include "SettingsScene.h"

#include "AudioEngine.h"
USING_NS_CC;
extern int BGM;
extern float volumeSound;
std::string IPAddr = "100.66.206.163";
#define BG_HEIGHT 1404
cocos2d::Scene* SettingsScene::createScene()
{
	return SettingsScene::create();
}
bool SettingsScene::init()
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

    auto closeButton = MenuItemImage::create("close_button.png", "close_button_selected.png", CC_CALLBACK_1(SettingsScene::backToMenu, this));
    auto closeButtonMenu = Menu::create(closeButton, NULL);
    closeButtonMenu->setAnchorPoint(Vec2(0, 1));
    closeButtonMenu->setPosition(Vec2(50, visibleSize.height - 50));
    this->addChild(closeButtonMenu); 
  
    musicSlider = ui::Slider::create();
    musicSlider->loadBarTexture("slider_back.png");
    musicSlider->loadSlidBallTextures("slider_node.png", "slider_node.png", "slider_node.png");
    musicSlider->loadProgressBarTexture("slider_front.png");
    musicSlider->setPercent(AudioEngine::getVolume(BGM) * 100);
    musicSlider->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 50));
    musicSlider->addEventListener(CC_CALLBACK_2(SettingsScene::onMusicSliderMoved, this));
    this->addChild(musicSlider);
    
    auto musicLabel = Label::createWithTTF("Music","fonts/Marker Felt.ttf", 48);
    musicLabel->setTextColor(Color4B::WHITE);
    musicLabel->setPosition(Vec2(visibleSize.width / 2 , visibleSize.height / 2 + 150));
    this->addChild(musicLabel);

    soundSlider = ui::Slider::create();
    soundSlider->loadBarTexture("slider_back.png");
    soundSlider->loadSlidBallTextures("slider_node.png", "slider_node.png", "slider_node.png");
    soundSlider->loadProgressBarTexture("slider_front.png");
    soundSlider->setPercent(volumeSound * 100);
    soundSlider->setPosition(Vec2(visibleSize.width / 2 , visibleSize.height / 2 - 150));
    soundSlider->addEventListener(CC_CALLBACK_2(SettingsScene::onSoundSliderMoved, this));
    this->addChild(soundSlider);
   
    auto soundLabel = Label::createWithTTF("Sound", "fonts/Marker Felt.ttf", 48);
    soundLabel->setTextColor(Color4B::WHITE);
    soundLabel->setPosition(Vec2(visibleSize.width / 2 , visibleSize.height / 2 - 50));
    this->addChild(soundLabel);

    IP = ui::TextField::create();
    IP->setMaxLength(30);
    IP->setColor(Color3B::WHITE);
    IP->setFontSize(48);
    IP->setFontName("fonts/Marker Felt.ttf");
    IP->setPlaceHolder(IPAddr);
    IP->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 450));
    this->addChild(IP);

    auto IPLabel = Label::createWithTTF("IP Address", "fonts/Marker Felt.ttf", 48);
    IPLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 375));
    IPLabel->setColor(Color3B::WHITE);
    this->addChild(IPLabel);

    this->scheduleUpdate();
    return true;
}
void SettingsScene::backToMenu(cocos2d::Ref* pSender)
{
    IPAddr = IP->getString();
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
}
void SettingsScene::update(float dt)
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
void SettingsScene::onMusicSliderMoved(Ref* pSender, ui::Slider::EventType type)
{
    switch (type)
    {
        case ui::Slider::EventType::ON_PERCENTAGE_CHANGED:
            AudioEngine::setVolume(BGM, musicSlider->getPercent() / 100.0f);
            break;
    }
}
void SettingsScene::onSoundSliderMoved(Ref* pSender, ui::Slider::EventType type)
{
    switch (type)
    {
        case ui::Slider::EventType::ON_PERCENTAGE_CHANGED:
            volumeSound = soundSlider->getPercent() / 100.0f;
            break;
    }
}