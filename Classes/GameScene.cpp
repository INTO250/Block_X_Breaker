#include "GameScene.h"
#include "StageSelectScene.h"
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

    Vec2 gravity = Vec2(0.0f, 0.0f);
    getPhysicsWorld()->setGravity(gravity);
    
    this->getPhysicsWorld()->setAutoStep(true);

    auto edge = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(1.0f, 1.0f, 0.0f));
    edge->setTag(0);
    auto edgeNode = Node::create();
    edgeNode->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    edgeNode->setPhysicsBody(edge);
    this->addChild(edgeNode);

    board = Board::createBoard("board.jpg");
    board->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5));
    this->addChild(board);
    auto boardBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(10000.f, 1.0f, 2.0f));
    boardBody->setCategoryBitmask(0xFFFFFFFF);
    boardBody->setCollisionBitmask(0xFFFFFFFF);
    boardBody->setContactTestBitmask(0xFFFFFFFF);
    boardBody->setRotationEnable(true);
    boardBody->setGravityEnable(false);
    boardBody->setDynamic(false);
    boardBody->setTag(0);
    board->setPhysicsBody(boardBody);

    //箭头指引发射方向
    arrow = Arrow::createArrow("arrow.png");
    arrow->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5+100));
    this->addChild(arrow);
    auto arrowBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(0.f, 1.0f, 2.0f));
    arrowBody->setCategoryBitmask(0x7);
    arrowBody->setCollisionBitmask(0x7);
    arrowBody->setContactTestBitmask(0x7);
    arrowBody->setRotationEnable(true);
    arrowBody->setGravityEnable(false);
    arrowBody->setDynamic(false);
    arrowBody->setTag(0);
    arrow->setPhysicsBody(arrowBody);

    Balls.push_back(Ball::createBall("ball.png"));
    Balls.back()->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + Balls.back()->getContentSize().height / 2 + board->getContentSize().height / 2));
    Balls.back()->existence = true;
    Balls.back()->type = 0;
    this->addChild(Balls.back());

    auto ballBody = PhysicsBody::createCircle(Balls.back()->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 1.0f));
    ballBody->setCategoryBitmask(0xFFFFFFFF);
    ballBody->setCollisionBitmask(0xFFFFFFFF);
    ballBody->setContactTestBitmask(0xFFFFFFFF);
    ballBody->setRotationEnable(false);
    ballBody->setGravityEnable(true);
    ballBody->setTag(0);
    Balls.back()->setPhysicsBody(ballBody);

    
    
    joint = PhysicsJointPin::construct(Balls.back()->getPhysicsBody(), board->getPhysicsBody(), Vec2(Balls.back()->getAnchorPoint().x, Balls.back()->getAnchorPoint().y - Balls.back()->getContentSize().height / 2), board->getAnchorPoint());
    getPhysicsWorld()->addJoint(joint);

    blocks_create(stage);
    
    auto background = Sprite::create("background.png");
    this->addChild(background, -10);

    this->scheduleUpdate();

    return true;
}

void GameScene::blocks_create(int stage)
{
    if (!stage)
    {
        auto rand = RandomHelper();
        stage = rand.random_int(1, 1);
    }
    switch (stage)
    {
        case 1:
            for (int i = 1; i <= 3; i++)
            {
                auto rua = Block::createBlock("block.png");
                rua->setPosition(Vec2(visibleSize.width / 5 * (i + 1), visibleSize.height / 5 * 4));
                rua->exsistence = true;
                rua->life = 1;
                auto blockBody = PhysicsBody::createBox(rua->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));
                blockBody->setDynamic(false);
                blockBody->setCategoryBitmask(0xFFFFFFFF);
                blockBody->setCollisionBitmask(0xFFFFFFFF);
                blockBody->setContactTestBitmask(0xFFFFFFFF);
                blockBody->setTag(i);
                rua->setPhysicsBody(blockBody);
                Blocks.push_back(rua);
            }

            /*旋转效果
            ActionInterval* actionTo = RotateTo::create(100, 180);
            board->runAction(Sequence::create(actionTo, NULL));
            
            */
            
            /*小球加倍
            for (int i = 0; i <= 1; i++)
            {
                Balls.push_back(Ball::createBall("ball.png"));
                Balls.back()->setPosition(Vec2(Balls[i]->getPositionX(), Balls[i]->getPositionY()));
                Balls.back()->existence = true;
                Balls.back()->type = 0;
                this->addChild(Balls.back());

                auto ballBody = PhysicsBody::createCircle(Balls.back()->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 1.0f));
                ballBody->setCategoryBitmask(0xFFFFFFFF);
                ballBody->setCollisionBitmask(0xFFFFFFFF);
                ballBody->setContactTestBitmask(0xFFFFFFFF);
                ballBody->setRotationEnable(false);
                ballBody->setGravityEnable(true);
                ballBody->setTag(0);
                Balls.back()->setPhysicsBody(ballBody);
            }
            */

            /*缩放
            ActionInterval * scaleto = ScaleTo ::create(2, 0.5);
            board->runAction(scaleto);
            break;
            */
    }
    for (auto it = Blocks.begin(); it != Blocks.end(); it++)
        this->addChild(*it, 3); 
}
void GameScene::onEnter()
{
    Scene::onEnter();
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);  //开始碰撞监听
  

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    dispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}
bool GameScene::onContactBegin(const PhysicsContact& contact) 
{
    auto bodyA = contact.getShapeA()->getBody();
    auto bodyB = contact.getShapeB()->getBody();

    if (bodyA->getTag() == 0 && bodyB->getTag() >= 1)
    {
       Blocks[bodyB->getTag() - 1]->life -= 1;
        if (Blocks[bodyB->getTag() - 1]->life <= 0)
        {
           
            this->removeChild(Blocks[bodyB->getTag() - 1]);
            Blocks[bodyB->getTag() - 1]->exsistence = false;
        } 
    }
    else if (bodyB->getTag() == 0 && bodyA->getTag() >= 1)
    {
        Blocks[bodyA->getTag() - 1]->life -= 1;
        if (Blocks[bodyA->getTag() - 1]->life <= 0)
        {

            this->removeChild(Blocks[bodyA->getTag() - 1]);
            Blocks[bodyA->getTag() - 1]->exsistence = false;
        }
    }
    return true;  //返回true，才干够继续监听其他碰撞
}

void GameScene::update(float dt)
{
    if (gameStart)
    {
        bool signal_of_end = false;
        auto it = Blocks.begin();
        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW])
        {
            if (board->getPosition().x > 50)
            {
                board->setPositionX(board->getPosition().x - 10);
            }
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
        {
            if (board->getPosition().x < 950)
            {
                board->setPositionX(board->getPosition().x + 10);
            }
        }


        for (it = Blocks.begin(); it != Blocks.end(); it++)
        {
            if ((*it)->exsistence)
            {
                break;
            }

        }
        if (it == Blocks.end())//胜利的标志
        {
            signal_of_end = true;
            auto label_success = Label::createWithTTF("Congratulations!", "fonts/Marker Felt.ttf", 148);
            label_success->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
            this->addChild(label_success);

            auto label_back = Label::createWithTTF("Back To Your Map", "fonts/Marker Felt.ttf", 48);
            auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::BackToStage, this));
            auto backbuttom = Menu::create(item_back, NULL);
            backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
            this->addChild(backbuttom);
            Balls.back()->existence = false;
            //Director::getInstance()->popScene();
        }
        for (auto i = Balls.begin(); i < Balls.end(); i++)
            if ((*i)->getPositionY() < board->getPositionY() - 50)
            {
                if (it != Blocks.end())
                {
                    auto label_false = Label::createWithTTF("You Loser!", "fonts/Marker Felt.ttf", 148);
                    label_false->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
                    this->addChild(label_false);

                    auto label_back = Label::createWithTTF("Back To Your Map", "fonts/Marker Felt.ttf", 48);
                    auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::BackToStage, this));
                    auto backbuttom = Menu::create(item_back, NULL);
                    backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
                    this->addChild(backbuttom);
                    Balls.back()->existence = false;
                    //Director::getInstance()->popScene();
                }
            }
    }
    else if (!gameStart)
    {
        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW])
        {
            ActionInterval* actionBy = RotateBy::create(0.01, -5);
            arrow->runAction(Sequence::create(actionBy, NULL));
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
        {
            ActionInterval* actionBy = RotateBy::create(0.01, 5);
            arrow->runAction(Sequence::create(actionBy, NULL));
        }
    }
}
void GameScene::BackToStage(cocos2d::Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, StageSelect::create()));
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = true;
    switch (code)
    {   
        case EventKeyboard::KeyCode::KEY_SPACE:
            if (!gameStart)
            {
                Balls.back()->getPhysicsBody()->setRotationEnable(true);
                gameStart = true;
                getPhysicsWorld()->removeJoint(joint);
                Balls.back()->getPhysicsBody()->setVelocity(Vec2(rand() % 1000 - 500, rand() % 1000 + 500));
                break;
            }
    }
}
void GameScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = false;
}

