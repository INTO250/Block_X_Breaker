#include "GameScene.h"
#include "StageSelectScene.h"
#include "AudioEngine.h"
extern float volumeSound;
extern int BGM;
USING_NS_CC;

Scene* GameScene::createScene(int stage)
{
    GameScene* rua = new GameScene();
    if (rua->initWithPhysics(stage))
    {
        rua->autorelease();
        return rua;
    }
    return NULL;
}
bool GameScene::initWithPhysics(int stage)
{
    if (!Scene::initWithPhysics())
    {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    gameStart = false;
    srand((unsigned)time(NULL));

    Vec2 gravity = Vec2(0.0f, 10.0f);
    getPhysicsWorld()->setGravity(gravity);
    this->getPhysicsWorld()->setAutoStep(true);

    auto edge = PhysicsBody::createEdgeBox(Size(visibleSize.width, visibleSize.height - 216), PhysicsMaterial(1.0f, 1.0f, 0.0f));
    edge->setTag(0);
    edge->setCategoryBitmask(1);
    edge->setCollisionBitmask(1);
    edge->setContactTestBitmask(0);
    auto edgeNode = Node::create();
    edgeNode->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 108);
    edgeNode->setPhysicsBody(edge);
    this->addChild(edgeNode);

    auto scoreBoard = Sprite::create("score_board.png");
    scoreBoard->setAnchorPoint(Vec2(0, 1));
    scoreBoard->setPosition(Vec2(0, 1404));
    this->addChild(scoreBoard);

    auto pauseButton = MenuItemImage::create("pause_button.png", "pause_button_selected.png", CC_CALLBACK_1(GameScene::onButtonPressed, this));
    auto pauseButtonMenu = Menu::create(pauseButton, NULL);
    pauseButton->setAnchorPoint(Vec2(1, 1));
    pauseButtonMenu->setAnchorPoint(Vec2(1, 1));
    pauseButtonMenu->setPosition(Vec2(648, 1404));
    this->addChild(pauseButtonMenu);

    Score = Label::createWithTTF("TextAlas", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
    Score->setColor(Color3B::WHITE); //设置颜色
    Score->setPosition(Vec2(100, 1274));
    Score->setString(GameScene::trans(score));
    this->addChild(Score);

    auto scoreLabel = Label::createWithTTF("Score", "fonts/Marker Felt.ttf", 50);
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->setPosition(Vec2(100, 1354));
    this->addChild(scoreLabel);
    
    //板
    board = Board::createBoard("board.png");
    board->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5));
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
    powerpng->setPosition(Vec2(visibleSize.width / 3, visibleSize.height / 5+100));
    this->addChild(powerpng);

    //蓄力槽arrow
    powerArrow = Arrow::createArrow("power_arrow.png");
    powerArrow->setPosition(Vec2(visibleSize.width / 3, visibleSize.height / 5));
    this->addChild(powerArrow);

    //箭头指引发射方向
    arrow = Arrow::createArrow("arrow.png");
    arrow->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + 100));
    this->addChild(arrow);
    /*auto arrowBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(0.f, 0.0f, 0.0f));
    arrowBody->setCategoryBitmask(0);
    arrowBody->setCollisionBitmask(1);
    arrowBody->setContactTestBitmask(1);
    arrowBody->setRotationEnable(true);
    arrowBody->setGravityEnable(false);
    arrowBody->setDynamic(false);
    arrowBody->setTag(0);
    arrow->setPhysicsBody(arrowBody);
    */

    //球
    Balls.push_back(Ball::createBall("ball.png"));
    Balls.back()->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + Balls.back()->getContentSize().height / 2 + board->getContentSize().height / 2));
    Balls.back()->existence = true;
    Balls.back()->type = 0;
    this->addChild(Balls.back());

    auto ballBody = PhysicsBody::createCircle(Balls.back()->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 0.0f));
    ballBody->setCategoryBitmask(1);
    ballBody->setCollisionBitmask(1);
    ballBody->setContactTestBitmask(1);
    ballBody->setRotationEnable(false);
    ballBody->setGravityEnable(true);
    ballBody->setTag(0);
    Balls.back()->setPhysicsBody(ballBody);

    joint = PhysicsJointPin::construct(Balls.back()->getPhysicsBody(), board->getPhysicsBody(), Vec2(Balls.back()->getAnchorPoint().x, Balls.back()->getAnchorPoint().y - Balls.back()->getContentSize().height / 2), board->getAnchorPoint());
    getPhysicsWorld()->addJoint(joint);

    blocks_create(stage);

    char stageBackgroundString[30];
    sprintf(stageBackgroundString, "stage_%d_background.png", stage);
    auto background = Sprite::create(stageBackgroundString);
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(background, -10);

   

    
    

    this->scheduleUpdate();

    return true;
}

void GameScene::blocks_create(int stage)
{
    char stageString[20];
    if (!stage)
    {
        auto rand = RandomHelper();
        stage = rand.random_int(1, 1);
    }
    sprintf(stageString, "stage_%d.tmx", stage);
    //导入瓦片地图
    map = TMXTiledMap::create(stageString);
    map->setAnchorPoint(Vec2(0.5, 0.5));
    map->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    addChild(map);

    TMXLayer* blocklayer = map->getLayer("normalBlock");//获取需要添加PhysicsBody的瓦片所在的图层
    auto mapSize = map->getMapSize();
    for (float i = 0; i < mapSize.width; i++)
    {
        for (float j = 0; j < mapSize.height; j++)
        {
           
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

    //获取需要打不动的砖块
    TMXLayer* toughblocklayer = map->getLayer("toughBlock");
    for (float i = 0; i < mapSize.width - 1; i++)
    {
        for (float j = 0; j < mapSize.height - 1; j++)
        {
            if (toughblocklayer->getTileAt(Vec2(i, j)))//并不是所有位置都有瓦片，如果没有瓦片就是空
            {
                
                auto toughblockBody = PhysicsBody::createBox(toughblocklayer->getTileAt(Vec2(i, j))->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));//设置PhysicsBody组件               
                toughblockBody->setDynamic(false);
                toughblockBody->setCategoryBitmask(1);
                toughblockBody->setCollisionBitmask(1);
                toughblockBody->setContactTestBitmask(0);
                toughblockBody->setTag(0);//忽视
                toughblocklayer->getTileAt(Vec2(i, j))->setPhysicsBody(toughblockBody);//给瓦片添加上PhysicsBody组件
            }
        }
    }

    //获取bonus
    TMXLayer* bonuslayer = map->getLayer("bonusBlock");
    for (float i = 0; i < mapSize.width; i++)
    {
        for (float j = 0; j < mapSize.height; j++)
        {
            if (bonuslayer->getTileAt(Vec2(i, j)))//并不是所有位置都有瓦片，如果没有瓦片就是空
            {
                Block rua;
                rua.exsistence = true;
                rua.life = 1;
                rua.bonus = 1;
                rua.sprite = bonuslayer->getTileAt(Vec2(i, j));
                rua.position = Vec2(i, j);
                auto bonusBody = PhysicsBody::createBox(bonuslayer->getTileAt(Vec2(i, j))->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));//设置PhysicsBody组件               
                bonusBody->setDynamic(false);
                bonusBody->setCategoryBitmask(1);
                bonusBody->setCollisionBitmask(1);
                bonusBody->setContactTestBitmask(1);
                bonusBody->setTag(block_order);
                bonusBody->setGravityEnable(false);
                rua.sprite->setPhysicsBody(bonusBody);//给瓦片添加上PhysicsBody组件
                block_order++;
                Blocks.push_back(rua);
            }
        }
    }

    
            /*小球加倍
            
            */

            /*缩放
            ActionInterval * scaleto = ScaleTo ::create(2, 0.5);
            board->runAction(scaleto);
            break;
            */ 
}

void GameScene::onEnter()
{
    Scene::onEnter();
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);  //开始碰撞监听


    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);


    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    dispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool GameScene::onContactBegin(const PhysicsContact& contact)
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    if (bodyA->getTag() == 0 && bodyB->getTag() >= 1)//b is a block
    {
        Blocks[bodyB->getTag() - 1].life -= 1;
        if (Blocks[bodyB->getTag() - 1].life <= 0)
        {
            
            Blocks[bodyB->getTag() - 1].exsistence = false;
            if (Blocks[bodyB->getTag() - 1].bonus)
            {
                
                bonus_create(Blocks[bodyB->getTag() - 1].bonus, map->getLayer("bonusBlock")->getTileAt(Blocks[bodyB->getTag() - 1].position)->getPosition());
                map->getLayer("bonusBlock")->removeTileAt(Blocks[bodyB->getTag() - 1].position);
            }
            else
            {
                map->getLayer("normalBlock")->removeTileAt(Blocks[bodyB->getTag() - 1].position);
            }
            score++;
            Score->setString(GameScene::trans(score));
            check_win();
        }
    }
    else if (bodyB->getTag() == 0 && bodyA->getTag() >= 1)
    {
        
        Blocks[bodyA->getTag() - 1].life -= 1;
        if (Blocks[bodyA->getTag() - 1].life <= 0)
        {
           
            Blocks[bodyA->getTag() - 1].exsistence = false;
            if (Blocks[bodyA->getTag() - 1].bonus)
            {
                bonus_create(Blocks[bodyA->getTag() - 1].bonus, map->getLayer("bonusBlock")->getTileAt(Blocks[bodyA->getTag() - 1].position)->getPosition());
                map->getLayer("bonusBlock")->removeTileAt(Blocks[bodyA->getTag() - 1].position);
            }
            else
            {
                
                map->getLayer("normalBlock")->removeTileAt(Blocks[bodyA->getTag() - 1].position);
            }
            score++;
            Score->setString(GameScene::trans(score));
            check_win();
        }
    }
   /* else if (bodyB->getTag() == 0 && bodyA->getTag() < 0&& bodyA->getTag() !=-1000)
    {
        
        bodyA->setGravityEnable(true);
        bodyA->setVelocity(Vec2(0,100));
        GameScene::transform();//用type判断
    }
    else if (bodyA->getTag() == 0 && bodyB->getTag() < 0 && bodyB->getTag() != -1000)
    {
        
        bodyB->setGravityEnable(true);
        bodyB->setVelocity(Vec2(0, -100)); 
    }
    else if (bodyA->getTag() == 0 && bodyA->getTag() < 0 && bodyA->getTag() != -1000)
    {
        removeChild(bonusBlocks[bodyA->getTag() * (-1) - 1].sprite);
        bonusBlocks[bodyA->getTag() * (-1) - 1].exsistence = false;
        bodyA->removeFromWorld();
        bonusBlocks[bodyA->getTag() * (-1) - 1].sprite->removeFromParent();
        GameScene::transform();//用type判断
    }
    else if (bodyA->getTag() == 0 && bodyB->getTag() < 0 && bodyB->getTag() != -1000)
    {
        removeChild(Blocks[bodyB->getTag() * (-1) - 1].sprite);
        bonusBlocks[bodyB->getTag() * (-1) - 1].exsistence = false;
        bodyB->removeFromWorld();
        bonusBlocks[bodyB->getTag() * (-1) - 1].sprite->removeFromParent();
        GameScene::transform();//用type判断
    } */
    return true;  //返回true，才干够继续监听其他碰撞
}

void GameScene::boardrotation()
{
    ActionInterval* actionTo = RotateTo::create(50, 180);
    board->runAction(Sequence::create(actionTo, NULL));
}

void GameScene::transform()
{
    if (!isSmall)
    {
        
        ActionInterval* scaleto = ScaleTo::create(0.5f, 0.5f);
        auto delay = DelayTime::create(5.0f);
        ActionInterval* scaleback = ScaleTo::create(0.5f, 1.0f);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameScene::isSmallChange, this));
        CallFunc* func2 = func->clone();
        auto sequence = Sequence::create(func, scaleto, delay, scaleback, func2, NULL);
        board->runAction(sequence);
    }
}

void GameScene::doubleball()
{
    for (int i = 0; i < 1; i++)
    {
        Balls.push_back(Ball::createBall("ball.png"));
        Balls.back()->setPosition(Vec2(Balls[0]->getPositionX(), Balls[0]->getPositionY()+1000));
        Balls.back()->existence = true;
        Balls.back()->type = 0;
        this->addChild(Balls.back());

        auto ballBody = PhysicsBody::createCircle(Balls.back()->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 1.0f));
        ballBody->setCategoryBitmask(1);
        ballBody->setCollisionBitmask(1);
        ballBody->setContactTestBitmask(1);
        ballBody->setRotationEnable(false);
        ballBody->setGravityEnable(true);
        ballBody->setTag(0);
        Balls.back()->setPhysicsBody(ballBody);
    }
}
void GameScene::update(float dt)
{
    if (gameStart)
    { 
        bool signal_of_end = false;

        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW] || touches["left"])
        {
            if (board->getPosition().x > 0)
            {
                board->setPositionX(board->getPosition().x - 20);
            }
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] || touches["right"])
        {
            if (board->getPosition().x < 1000)
            {
                board->setPositionX(board->getPosition().x + 20);
            }
        }
        for (auto i = Balls.begin(); i != Balls.end(); i++)
            if ((*i)->getPositionY() < board->getPositionY() - 50)
            {
                    auto label_false = Label::createWithTTF("You Loser!", "fonts/Marker Felt.ttf", 148);
                    label_false->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
                    this->addChild(label_false);
                    auto label_back = Label::createWithTTF("Back To Your Map", "fonts/Marker Felt.ttf", 48);
                    auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::onButtonPressed, this));
                    auto backbuttom = Menu::create(item_back, NULL);
                    backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
                    this->addChild(backbuttom);
                    Balls.back()->existence = false;
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
    else if (!gameStart)
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
std::string GameScene::trans(long long int value)
{
    char buff[16];
    snprintf(buff, sizeof(buff), "%lld", value);
    return buff;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = true;

}
void GameScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = false;
    switch (code)
    {
        case EventKeyboard::KeyCode::KEY_SPACE:
            if (!gameStart)
            {
                Balls.back()->getPhysicsBody()->setRotationEnable(true);
                gameStart = true;
                getPhysicsWorld()->removeJoint(joint);
                Balls.back()->getPhysicsBody()->setVelocity(Vec2((float)10.f * power * sin(5 * shootvec * 3.14 / 180), (float)power * 10.f * cos(5 * shootvec * 3.14 / 180)));
                removeChild(arrow, true);
                removeChild(powerArrow, true);
                removeChild(powerpng, true);
                break;
            }
    }
}
void GameScene::check_win()
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

        auto label_success = Label::createWithTTF("Congratulations!", "fonts/Marker Felt.ttf", 148);
        label_success->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        this->addChild(label_success);
        auto label_back = Label::createWithTTF("Back To Your Map", "fonts/Marker Felt.ttf", 48);
        auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::onButtonPressed, this));
        auto backbuttom = Menu::create(item_back, NULL);
        backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
        this->addChild(backbuttom);
    }
}
void GameScene::bonus_create(int type, Vec2 position)
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
void GameScene::isSmallChange()
{
    if (isSmall)
        isSmall = false;
    else
        isSmall = true;
}
void GameScene::onButtonPressed(Ref* pSender)
{
    auto sound = AudioEngine::play2d("sound_click.mp3", false, volumeSound);
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d("stage_select_BGM.mp3", true, volumeBGM);
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, StageSelect::createScene()));
}
bool GameScene::onTouchBegan(Touch* touch, Event* event)
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
void GameScene::onTouchEnded(Touch* touch, Event* event)
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
            Balls.back()->getPhysicsBody()->setRotationEnable(true);
            gameStart = true;
            getPhysicsWorld()->removeJoint(joint);
            Balls.back()->getPhysicsBody()->setVelocity(Vec2(10.f * power * (float)sin(5 * (long long)shootvec * 3.14 / 180), power * 10.f * (float)cos(5 * (long long)shootvec * 3.14 / 180)));
            removeChild(arrow, true);
            removeChild(powerArrow, true);
            removeChild(powerpng, true);
        }
    }
}

