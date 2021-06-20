#include "OnlineScene.h"
#include "StageSelectScene.h"
#include "AudioEngine.h"
#include <cmath>

extern float volumeSound;
extern int BGM;
extern std::string IPAddr;
extern SOCKET client;
USING_NS_CC;

Scene* OnlineScene::createScene()
{
    OnlineScene* rua = new OnlineScene();
    if (rua->initWithPhysics())
    {
        rua->autorelease();
        return rua;
    }
    return NULL;
}
bool OnlineScene::initWithPhysics()
{
    if (!Scene::initWithPhysics())
    {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    gameStart = false;
    srand((unsigned)time(NULL));
     
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;//对这个类进行初始化
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());

    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("battle.mp3", true, volumeBGM);

    while (1)
    {
        char dataRecv[1024];
        if (recv(client, dataRecv, 1023, 0) != -1)
        {
            stage = atoi(dataRecv);
            break;
        }
    }
   

    Vec2 gravity = Vec2(0.0f, 0.0f);
    getPhysicsWorld()->setGravity(gravity);
    this->getPhysicsWorld()->setAutoStep(true);

   

    auto edge = PhysicsBody::createEdgeBox(Size(visibleSize.width, 1280), PhysicsMaterial(10000.0f, 1.0f, 0.0f));
    edge->setTag(0);
    edge->setCategoryBitmask(1);
    edge->setCollisionBitmask(1);
    edge->setContactTestBitmask(0);
    auto edgeNode = Node::create();
    edgeNode->setPosition(visibleSize.width / 2,  0);
    edgeNode->setPhysicsBody(edge);
    this->addChild(edgeNode);

    board = Board::createBoard("board.png");
    board->setPosition(Vec2(visibleSize.width / 2, 160));
    board->life = 3;
    this->addChild(board);
    auto boardBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(10000.f, 1.0f, 0.0f));
    boardBody->setCategoryBitmask(1);
    boardBody->setCollisionBitmask(1);
    boardBody->setContactTestBitmask(0);
    boardBody->setRotationEnable(true);
    boardBody->setGravityEnable(false);
    boardBody->setDynamic(false);
    boardBody->setTag(0);
    board->setPhysicsBody(boardBody);

    //蓄力槽
    powerpng = Arrow::createArrow("power.png");
    powerpng->setPosition(Vec2(visibleSize.width / 3, 260));
    this->addChild(powerpng);

    //蓄力槽arrow
    powerArrow = Arrow::createArrow("power_arrow.png");
    powerArrow->setPosition(Vec2(visibleSize.width / 3, 160));
    this->addChild(powerArrow);

    //箭头指引发射方向
    arrow = Arrow::createArrow("arrow.png");
    arrow->setPosition(Vec2(visibleSize.width / 2,260));
    this->addChild(arrow);

    Balls[0] = Ball::createBall("ball.png");
    Balls[1] = Ball::createBall("ball.png");
    Balls[2] = Ball::createBall("ball.png");
    Balls[0]->setPosition(Vec2(visibleSize.width / 2, Balls[0]->getContentSize().height / 2 + board->getContentSize().height / 2 + 160));
    Balls[0]->existence = true;
    Balls[1]->existence = false;
    Balls[2]->existence = false;
    this->addChild(Balls[0]);

    auto ballBody = PhysicsBody::createCircle(Balls[0]->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 0.0f));
    ballBody->setCategoryBitmask(1);
    ballBody->setCollisionBitmask(1);
    ballBody->setContactTestBitmask(1);
    ballBody->setRotationEnable(false);
    ballBody->setGravityEnable(true);
    ballBody->setDynamic(false);
    ballBody->setTag(0);
    Balls[0]->setPhysicsBody(ballBody);

    auto scoreBoard = Sprite::create("score_board_online.png");
    scoreBoard->setPosition(Vec2(324, 702));
    this->addChild(scoreBoard);

    opponentBoard = Sprite::create("board.png");
    opponentBoard->setPosition(Vec2(visibleSize.width / 2, 924));
    this->addChild(opponentBoard);
    opponentBall = Sprite::create("ball.png");
    opponentBall->setPosition(Vec2(visibleSize.width / 2, 924 + opponentBoard->getContentSize().height / 2 + opponentBall->getContentSize().height / 2));
    this->addChild(opponentBall);

    blocks_create(stage);

    life = Label::createWithTTF("3", "fonts/Marker Felt.ttf", 48);
    life->setString(trans(board->life));
    life->setPosition(Vec2(100, 671));
    this->addChild(life);

    opponentLife = Label::createWithTTF("3", "fonts/Marker Felt.ttf", 48);
    opponentLife->setPosition(Vec2(548, 733));
    this->addChild(opponentLife);

    auto heart = Sprite::create("heart.png");
    heart->setPosition(Vec2(40, 671));
    this->addChild(heart);

    auto opponentHeart = Sprite::create("heart.png");
    opponentHeart->setPosition(Vec2(608, 733));
    this->addChild(opponentHeart);

    skillsBar = ui::LoadingBar::create("skillsbar.png");
    skillsBar->setPosition(Vec2(384, 671));
    skillsBar->setPercent(0);
    this->addChild(skillsBar);

    opponentBar = ui::LoadingBar::create("skillsbar.png");
    opponentBar->setPosition(Vec2(264, 733));
    opponentBar->setPercent(0);
    this->addChild(opponentBar);




    char stageBackgroundString[30];
    sprintf(stageBackgroundString, "stage_%d_background.png", stage);
    auto background = Sprite::create(stageBackgroundString);
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(background, -10);
    this->scheduleUpdate();
    this->schedule((CC_SCHEDULE_SELECTOR(OnlineScene::dataRS)), 0.0333f);
    return true;
}

void OnlineScene::blocks_create(int stage)
{
    char stageString[20];
    if (!stage)
    {
        auto rand = RandomHelper();
        stage = rand.random_int(1, 1);
    }
    sprintf(stageString, "stage_%d_online.tmx", stage);
    //导入瓦片地图
    map = TMXTiledMap::create(stageString);
    map->setAnchorPoint(Vec2(0.5, 0.5));
    map->setPosition(Vec2(visibleSize.width / 2, 320));
    addChild(map);
    mapOpponent = TMXTiledMap::create(stageString);
    mapOpponent->setAnchorPoint(Vec2(0.5, 1));
    mapOpponent->setPosition(Vec2(visibleSize.width / 2, visibleSize.height));
    addChild(mapOpponent);
    TMXLayer* blocklayer = map->getLayer("normalBlock");//获取需要添加PhysicsBody的瓦片所在的图层
    auto mapSize = map->getMapSize();
    for (float i = 0; i < mapSize.width; i++)
    {
        for (float j = 0; j < mapSize.height; j++)
        {
            blockInfo += "*";
            if (blocklayer->getTileAt(Vec2(i, j)))//并不是所有位置都有瓦片，如果没有瓦片就是空
            {
                
                Block rua;
                rua.sprite = blocklayer->getTileAt(Vec2(i, j));
                rua.exsistence = true;
                rua.life = 1;
                rua.bonus = 0;
                rua.position = Vec2(i, j);
                
                auto blockBody = PhysicsBody::createBox(rua.sprite->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));//设置PhysicsBody组件               
                blockBody->setDynamic(false);
                blockBody->setCategoryBitmask(1);
                blockBody->setCollisionBitmask(1);
                blockBody->setContactTestBitmask(1);
                blockBody->setTag(block_order);
                rua.sprite->setPhysicsBody(blockBody);//给瓦片添加上PhysicsBody组件
                block_order++;//start at 0
                Blocks.push_back(rua);
            }
        }
    }


    
    
    
}


    
    


void OnlineScene::onEnter()
{
    Scene::onEnter();
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(OnlineScene::onContactBegin, this);  //开始碰撞监听


    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(OnlineScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(OnlineScene::onKeyReleased, this);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(OnlineScene::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(OnlineScene::onTouchEnded, this);


    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    dispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool OnlineScene::onContactBegin(const PhysicsContact& contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    if (bodyA->getTag() == 0 && bodyB->getTag() >= 1)//b is a block
    {
        Blocks[bodyB->getTag() - 1].life -= 1;
        if (Blocks[bodyB->getTag() - 1].life <= 0)
        {
            auto sound = AudioEngine::play2d("sound_crash.mp3", false, volumeSound);
            Blocks[bodyB->getTag() - 1].exsistence = false;
            
            blockInfo[Blocks[bodyB->getTag() - 1].position.y * 20 + Blocks[bodyB->getTag() - 1].position.x] = '!';
            auto effect = ParticleFire::create();
            effect->setDuration(0.5);
            effect->setPosition(map->getLayer("normalBlock")->getTileAt(Blocks[bodyB->getTag() - 1].position)->getPosition());
            this->addChild(effect);
            map->getLayer("normalBlock")->removeTileAt(Blocks[bodyB->getTag() - 1].position);
            score++;

            check_win();
        }
    }
    else if (bodyB->getTag() == 0 && bodyA->getTag() >= 1)
    {

        Blocks[bodyA->getTag() - 1].life -= 1;
        if (Blocks[bodyA->getTag() - 1].life <= 0)
        {
            auto sound = AudioEngine::play2d("sound_crash.mp3", false, volumeSound);
            Blocks[bodyA->getTag() - 1].exsistence = false;
            auto effect = ParticleFire::create();
            effect->setDuration(0.5);
            effect->setPosition(map->getLayer("normalBlock")->getTileAt(Blocks[bodyA->getTag() - 1].position)->getPosition());
            this->addChild(effect);
            map->getLayer("normalBlock")->removeTileAt(Blocks[bodyA->getTag() - 1].position);
            blockInfo[Blocks[bodyA->getTag() - 1].position.y * 20 + Blocks[bodyA->getTag() - 1].position.x] = '!';
            
            score++;

            check_win();
        }
    }
    skillsBar->setPercent((score % 10) * 10);

    if (score && score % 10 == 0)
    {
        switch (1 + rand() % 3)
        {
            case 1:
                oppoTransform();
                break;
            case 2:
                oppoRotation();
                break;
            case 3:
                oppoSpeedUp();
                break;   
        }

    }
    return true;
}
void OnlineScene::dataRS(float dt)
{
    std::string dataSend = blockInfo + "# ";
    dataSend += trans(board->getPositionX());
    dataSend += " ";
    dataSend += trans(Balls[0]->getPositionX());
    dataSend += " ";
    dataSend += trans(Balls[0]->getPositionY());
    dataSend += " ";
    dataSend += trans(board->life);
    dataSend += " ";
    dataSend += trans(skillsBar->getPercent());
    dataSend += " ";
    dataSend += trans(oppoSmall);
    dataSend += " ";
    dataSend += trans(oppoRotating);
    dataSend += " ";
    dataSend += trans(oppoBoost);
    dataSend += " ";
    send(client, dataSend.c_str(), dataSend.size(), 0);


    char dataRecv[1024];
    int ret = recv(client, dataRecv, 1023, 0);
    if (ret != -1 && dataRecv[0])
    {
        char blockRecv[402] = { 0 }, posX[10] = { 0 }, posY[10] = { 0 }, posBoard[10] = { 0 }, oppoLife[10] = { 0 };
        int oppoBar, smal, rota, boost;
        int retu = sscanf(dataRecv, "%s%s%s%s%s%d%d%d%d", blockRecv, posBoard, posX, posY, oppoLife, &oppoBar, &smal, &rota, &boost);

        if (retu == 9 && blockRecv[400] == '#')
        {
            for (int i = 0; i < 400; i++)
            {
                if (blockRecv[i] == '!' && mapOpponent->getLayer("normalBlock")->getTileAt(Vec2(i % 20, i / 20)))
                {
                    auto effect = ParticleFire::create();
                    effect->setDuration(0.5);
                    effect->setPosition(Vec2(mapOpponent->getLayer("normalBlock")->getTileAt(Vec2(i % 20, i / 20))->getPosition().x, mapOpponent->getLayer("normalBlock")->getTileAt(Vec2(i % 20, i / 20))->getPosition().y + 764));
                    this->addChild(effect);
                    mapOpponent->getLayer("normalBlock")->removeTileAt(Vec2(i % 20, i / 20));
                    auto sound = AudioEngine::play2d("sound_crash.mp3", false, volumeSound);
                }
            }


            opponentBoard->setPosition(Vec2(atoi(posBoard), 924));

            opponentBall->setPosition(Vec2(atoi(posX), atoi(posY) + 764));

            opponentLife->setString(trans(atoi(oppoLife)));

            opponentBar->setPercent(oppoBar);


            if (smal)
            {
                transform();

            }

            if (rota)
            {
                rotation();
            }

            if (boost)
            {
                speedUp();
            }
            
            
        }
        else if (blockRecv[0] == 'w')
        {
            gameLose();
        }
        else if (blockRecv[0] == 'l')
        {
            gameWin();
        }
    }
}
void OnlineScene::speedUp()
{
    if (!isBoost)
    {
        auto sound = AudioEngine::play2d("sound_bonus.mp3", false, volumeSound);
        auto sign = Sprite::create("bonus_sign_3.png");
        auto fadeIn = FadeIn::create(0.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto de = DelayTime::create(0.5f);
        sign->setPosition(Vec2(324, 320));
        sign->runAction(Sequence::create(fadeIn, de, fadeOut, NULL));
        this->addChild(sign);
        auto delay = DelayTime::create(5.0f);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(OnlineScene::isBoostChange, this));
        CallFunc* func2 = func->clone();
        board->runAction(Sequence::create(func,  delay, func2,  NULL));
    }
}
void OnlineScene::isBoostChange()
{
    if (isBoost)
    {
        isBoost = false;
        speed = 1.0f;
    }
    else
    {
        isBoost = true;
        speed = 0.1f;
    }
}
void OnlineScene::oppoSpeedUp()
{
    if (!oppoBoost)
    {
        auto sound = AudioEngine::play2d("sound_bonus.mp3", false, volumeSound);
        auto sign = Sprite::create("bonus_sign_3.png");
        auto fadeIn = FadeIn::create(0.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto de = DelayTime::create(0.5f);
        sign->setPosition(Vec2(324, 1084));
        sign->runAction(Sequence::create(fadeIn, de, fadeOut, NULL));
        this->addChild(sign);
        auto delay = DelayTime::create(5.0f);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(OnlineScene::oppoBoostChange, this));
        CallFunc* func2 = func->clone();
        opponentBoard->runAction(Sequence::create(func, delay, func2, NULL));
    }
}
void OnlineScene::oppoBoostChange()
{
    if (oppoBoost)
    {
        oppoBoost = 0;
      
    }
    else
    {
        oppoBoost = 1;
    }
}
void OnlineScene::transform()
{
    if (!isSmall)
    {
        auto sound = AudioEngine::play2d("sound_bonus.mp3", false, volumeSound);
        auto sign = Sprite::create("bonus_sign_1.png");
        auto fadeIn = FadeIn::create(0.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto de = DelayTime::create(0.5f);
        sign->setPosition(Vec2(324, 320));
        sign->runAction(Sequence::create(fadeIn, de, fadeOut, NULL));
        this->addChild(sign);
        ActionInterval* scaleto = ScaleTo::create(0.5f, 0.5f);
        auto delay = DelayTime::create(5.0f);
        ActionInterval* scaleback = ScaleTo::create(0.5f, 1.0f);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(OnlineScene::isSmallChange, this));
        CallFunc* func2 = func->clone();
        auto sequence = Sequence::create(func, scaleto, delay, scaleback, func2, NULL);
        board->runAction(sequence);
    }
}
void OnlineScene::rotation()
{
    if (!isRotating)
    {
        auto sound = AudioEngine::play2d("sound_bonus.mp3", false, volumeSound);
        auto sign = Sprite::create("bonus_sign_2.png");
        auto fadeIn = FadeIn::create(0.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto de = DelayTime::create(0.5f);
        sign->setPosition(Vec2(324, 320));
        sign->runAction(Sequence::create(fadeIn, de, fadeOut, NULL));
        this->addChild(sign);
        ActionInterval* actionTo = RotateBy::create(5, 180);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(OnlineScene::isRotatingChange, this));
        CallFunc* func2 = func->clone();
        board->runAction(Sequence::create(func, actionTo, func2, NULL));
    }
}
void OnlineScene::isRotatingChange()
{
    if (isRotating)
        isRotating = false;
    else
        isRotating = true;
}
void OnlineScene::oppoTransform()
{
    if (!oppoSmall)
    {
        auto sound = AudioEngine::play2d("sound_bonus.mp3", false, volumeSound);
        auto sign = Sprite::create("bonus_sign_1.png");
        auto fadeIn = FadeIn::create(0.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto de = DelayTime::create(0.5f);
        sign->setPosition(Vec2(324, 1084));
        sign->runAction(Sequence::create(fadeIn, de, fadeOut, NULL));
        this->addChild(sign);
        ActionInterval* scaleto = ScaleTo::create(0.5f, 0.5f);
        auto delay = DelayTime::create(5.0f);
        ActionInterval* scaleback = ScaleTo::create(0.5f, 1.0f);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(OnlineScene::oppoSmallChange, this));
        CallFunc* func2 = func->clone();
        auto sequence = Sequence::create(func, scaleto, delay, scaleback, func2, NULL);
        opponentBoard->runAction(sequence);
    }
}
void OnlineScene::oppoRotatingChange()
{
    if (oppoRotating)
        oppoRotating = 0;
    else
        oppoRotating = 1;
}
void OnlineScene::oppoRotation()
{
    if (!oppoRotating)
    {
        
        auto sound = AudioEngine::play2d("sound_bonus.mp3", false, volumeSound);
        auto sign = Sprite::create("bonus_sign_2.png");
        auto fadeIn = FadeIn::create(0.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto de = DelayTime::create(0.5f);
        sign->setPosition(Vec2(324, 1084));
        sign->runAction(Sequence::create(fadeIn, de, fadeOut, NULL));
        this->addChild(sign);
        ActionInterval* actionTo = RotateBy::create(5, 180);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(OnlineScene::oppoRotatingChange, this));
        CallFunc* func2 = func->clone();
        opponentBoard->runAction(Sequence::create(func, actionTo, func2, NULL));
    }
}
void OnlineScene::oppoSmallChange()
{
    if (oppoSmall)
        oppoSmall = 0;
    else
        oppoSmall = 1;
}
void OnlineScene::gameLose()
{
    
        
    
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    
    this->unscheduleAllCallbacks();
    this->schedule(CC_SCHEDULE_SELECTOR(OnlineScene::loseSend), 0.1f);
    auto label_false = Label::createWithTTF("You Lose!", "fonts/Marker Felt.ttf", 148);
    label_false->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(label_false);
    auto label_back = Label::createWithTTF("Back To Map", "fonts/Marker Felt.ttf", 48);
    auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(OnlineScene::onButtonPressed, this));
    auto backbuttom = Menu::create(item_back, NULL);
    backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
    this->addChild(backbuttom);
    
}
void OnlineScene::gameWin()
{
    
    
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

    this->unscheduleAllCallbacks();
    this->schedule((CC_SCHEDULE_SELECTOR(OnlineScene::winSend)), 0.1f);
    auto label_success = Label::createWithTTF("Congratulations!", "fonts/Marker Felt.ttf", 48);
    label_success->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(label_success);
    auto label_back = Label::createWithTTF("Back To Map", "fonts/Marker Felt.ttf", 48);
    auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(OnlineScene::onButtonPressed, this));
    auto backbuttom = Menu::create(item_back, NULL);
    backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
    this->addChild(backbuttom);
}
void OnlineScene::winSend(float dt)
{
    send(client, "win", sizeof("win"), 0);
}
void OnlineScene::loseSend(float dt)
{
    send(client, "lose", sizeof("lose"), 0);
}
void OnlineScene::update(float dt)
{
    getPhysicsWorld()->setSpeed(speed * MIN(1.0f + 0.02 * score, 3.0f));
    if (gameStart)
    {
        bool signal = true;

        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW] || touches["left"])
        {
            if (board->getPosition().x - board->getContentSize().width / 2 > 0)
            {
                board->setPositionX(board->getPosition().x - 20);
            }
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] || touches["right"])
        {
            if (board->getPosition().x + board->getContentSize().width / 2 < 648)
            {
                board->setPositionX(board->getPosition().x + 20);
            }
        }
        for (int i = 0; i < 1; i++)

        {
            if (Balls[i]->existence)
            {
                if (Balls[i]->getPosition().y < board->getPositionY() - 50)
                {
                    this->removeChild(Balls[i]);
                    Balls[i]->existence = false;


                }
            }
        }
        for (int i = 0; i < 1; i++)
        {
            if (Balls[i]->existence)
            {
                signal = false;
                break;
            }
        }
        if (signal)
        {
            board->life--;
            life->setString(trans(board->life));
            
            if (board->life)
            {
                Balls[0] = Ball::createBall("ball.png");
                Balls[0]->setPosition(Vec2(board->getPosition().x, board->getPosition().y + Balls[0]->getContentSize().height / 2 + board->getContentSize().height / 2));
                Balls[0]->existence = true;
                auto ballBody = PhysicsBody::createCircle(Balls[0]->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 0.0f));
                ballBody->setCategoryBitmask(1);
                ballBody->setCollisionBitmask(1);
                ballBody->setContactTestBitmask(1);
                ballBody->setRotationEnable(false);
                ballBody->setGravityEnable(true);
                ballBody->setDynamic(false);
                ballBody->setTag(0);
                Balls[0]->setPhysicsBody(ballBody);
                this->addChild(Balls[0]);
                powerpng = Arrow::createArrow("power.png");
                powerArrow = Arrow::createArrow("power_arrow.png");
                arrow = Arrow::createArrow("arrow.png");
                powerpng->setPosition(Vec2(board->getPosition().x - 70, 260));
                powerArrow->setPosition(Vec2(board->getPosition().x - 70, 160));
                arrow->setPosition(Vec2(board->getPosition().x, 260));
                this->addChild(powerpng);
                this->addChild(arrow);
                this->addChild(powerArrow);
                gameStart = false;
            }
            else
            {
                gameLose();

                gameStart = false;
                gameEnd = true;

            }
        }
        for (int i = 0; i < Bonus.size(); i++)
        {
            Bonus[i]->setPosition(Vec2(Bonus[i]->getPosition().x, Bonus[i]->getPosition().y - 1));
            if (board->getBoundingBox().containsPoint(Bonus[i]->getPosition()))
            {

                switch (Bonus[i]->getTag())
                {
                    case 1:
                        transform();
                        break;
                }
                this->removeChild(Bonus[i]);
                Bonus.erase(Bonus.begin() + i, Bonus.begin() + i + 1);
                continue;
            }
        }
    }
    else if (!gameStart && !gameEnd)
    {
        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW] || touches["left"])
        {
            if (shootvec >= -12)
            {
                ActionInterval* actionBy = RotateBy::create(0, -5.f);
                arrow->runAction(Sequence::create(actionBy, NULL));
                shootvec--;
            }
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] || touches["right"])
        {
            if (shootvec <= 12)
            {
                ActionInterval* actionBy = RotateBy::create(0, 5.f);
                arrow->runAction(Sequence::create(actionBy, NULL));
                shootvec++;
            }
        }

        if (keys[EventKeyboard::KeyCode::KEY_SPACE] || touches["middle"])
        {
            if (power <= 50)
            {
                power++;
                powerArrow->setPositionY(powerArrow->getPosition().y + 3);
            }
        }
    }




}
std::string OnlineScene::trans(long long int value)
{
    char buff[16];
    snprintf(buff, sizeof(buff), "%lld", value);
    return buff;
}

void OnlineScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = true;

}
void OnlineScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = false;
    switch (code)
    {
        case EventKeyboard::KeyCode::KEY_SPACE:
            if (!gameStart)
            {

                Balls[0]->getPhysicsBody()->setDynamic(true);
                gameStart = true;

                Balls[0]->getPhysicsBody()->setVelocity(Vec2((float)2.0f * (power + 50) * sin(5 * shootvec * 3.14 / 180), (power + 50) * 2.0f * cos(5 * shootvec * 3.14 / 180)));
                removeChild(arrow, true);
                removeChild(powerArrow, true);
                removeChild(powerpng, true);
                power = 0;
                shootvec = 0;
                break;
            }
    }
}
void OnlineScene::check_win()
{
    bool win = 1;
    for (auto it = Blocks.begin(); it != Blocks.end(); it++)
    {
        if ((*it).exsistence)
        {
            win = 0;
            break;
        }
    }
    if (win)//胜利的标志
    {

        gameWin();
    }
}
void OnlineScene::bonus_create(int type, Vec2 position)
{
    char bonusString[20];
    sprintf(bonusString, "bonus_%d.png", type);
    auto bonus = Sprite::create(bonusString);
    bonus->setPosition(position);
    bonus->setTag(type);
    bonus->setAnchorPoint(Vec2(0.5f, 0));
    this->addChild(bonus);
    Bonus.push_back(bonus);

}
void OnlineScene::isSmallChange()
{
    if (isSmall)
        isSmall = false;
    else
        isSmall = true;
}
void OnlineScene::onButtonPressed(Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("MainMenu.mp3", true, volumeBGM);
#ifdef WIN32
    WSACleanup();
#endif
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
}
bool OnlineScene::onTouchBegan(Touch* touch, Event* event)
{
    if (touch->getLocation().y < 1000)
    {
        if (touch->getLocation().x < 216)
            touches["left"] = true;
        else if (touch->getLocation().x > 432)
            touches["right"] = true;
        else
            touches["middle"] = true;
        return true;
    }
    else
        return false;
}
void OnlineScene::onTouchEnded(Touch* touch, Event* event)
{
    if (touch->getLocation().x < 216)
        touches["left"] = false;
    else if (touch->getLocation().x > 432)
        touches["right"] = false;
    else
    {
        touches["middle"] = false;
        if (!gameStart)
        {

            Balls[0]->getPhysicsBody()->setDynamic(true);
            gameStart = true;

            Balls[0]->getPhysicsBody()->setVelocity(Vec2(2.0f * (power + 50) * (float)sin(5 * (long long)shootvec * 3.14 / 180), (power + 50) * 2.0f * (float)cos(5 * (long long)shootvec * 3.14 / 180)));
            removeChild(arrow, true);
            removeChild(powerArrow, true);
            removeChild(powerpng, true);
            power = 0;
            shootvec = 0;
        }
    }
}
