#include "MenuScene.h"
#include "MatchingScene.h"
#include "OnlineScene.h"
#include "AudioEngine.h"
#include "SettingsScene.h"

USING_NS_CC;
extern int BGM;
extern float volumeSound;
extern std::string IPAddr;
SOCKET client;
#define BG_HEIGHT 1404
cocos2d::Scene* MatchingScene::createScene()
{
    return MatchingScene::create();
}
bool MatchingScene::init()
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

    auto closeButton = MenuItemImage::create("close_button.png", "close_button_selected.png", CC_CALLBACK_1(MatchingScene::backToMenu, this));
    auto closeButtonMenu = Menu::create(closeButton, NULL);
    closeButtonMenu->setAnchorPoint(Vec2(0, 1));
    closeButtonMenu->setPosition(Vec2(50, visibleSize.height - 50));
    this->addChild(closeButtonMenu);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;//对这个类进行初始化
    serveraddr.sin_port = htons(12345);
    
    serveraddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    unsigned long u1;
    ioctlsocket(client, FIONBIO, (unsigned long*)&u1);
    matchLabel = Label::createWithTTF("Connecting", "fonts/Marker Felt.ttf", 48);
    matchLabel->setPosition(Vec2(324, 702));
    this->addChild(matchLabel);
    if (!connect(client, (sockaddr*)&serveraddr, sizeof(serveraddr)))
    {
        matchLabel->setString("Error!");
    }
   
    
   

    startButton = ui::Button::create("button_green.png", "button_yellow.png", "");
    startButton->addTouchEventListener(CC_CALLBACK_2(MatchingScene::gameStart, this));
    startButton->setPosition(Vec2(324, 324));
    startButton->setEnabled(false);
    this->addChild(startButton);

    this->scheduleUpdate();
    return true;
}
void MatchingScene::backToMenu(cocos2d::Ref* pSender)
{

    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    send(client, "quit", sizeof("quit"), 0);
    WSACleanup();
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("MainMenu.mp3", true, volumeBGM);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
}
void MatchingScene::update(float dt)
{
    char dataRecv[1024];
    recv(client, dataRecv, 1023, 0);
    std::string str = dataRecv;
    
    if (str == "quit")
    {
        matchLabel->setString("Your opponent has left.");
    }
    else if (str == "connected")
    {
        matchLabel->setString("Matching...");
    }
    else if (str == "matched")
    {
        matchLabel->setString("Matched! Push the button to start.");
        startButton->setEnabled(true);
    }
    else if (str == "start")
    {
        auto sound = AudioEngine::play2d("sound_matched.mp3", false, volumeSound);
       
        Director::getInstance()->replaceScene(TransitionFade::create(2.0f, OnlineScene::createScene()));
    }
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
void MatchingScene::gameStart(cocos2d::Ref* pSender, cocos2d::ui::Button::TouchEventType type)
{
    switch (type)
    {
        case ui::Button::TouchEventType::BEGAN:
            auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
            send(client, "ready", sizeof("ready"), 0);
            startButton->setEnabled(false);
            matchLabel->setString("Waiting for your opponent...");
            break;
           
    }
}