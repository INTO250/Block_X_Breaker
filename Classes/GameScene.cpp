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

    auto boardBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(10000.f, 1.0f, 0.0f));
    boardBody->setCategoryBitmask(0xFFFFFFFF);
    boardBody->setCollisionBitmask(0xFFFFFFFF);
    boardBody->setContactTestBitmask(0xFFFFFFFF);
    boardBody->setRotationEnable(true);
    boardBody->setGravityEnable(false);
    boardBody->setDynamic(false);
    boardBody->setTag(0);
    board->setPhysicsBody(boardBody);
    
    Balls.pushBack(Ball::createBall("ball.png"));
    Balls.back()->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + Balls.back()->getContentSize().height / 2 + board->getContentSize().height / 2));
    Balls.back()->existence = true;
    Balls.back()->type = 0;
    this->addChild(Balls.back());

    auto ballBody = PhysicsBody::createCircle(Balls.back()->getContentSize().width / 2, PhysicsMaterial(1.0f, 1.0f, 0.0f));
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
            break;
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
    bool gameSuccess = 0;
    for (auto it = Blocks.begin(); it != Blocks.end(); it++)
    {
        if ((*it)->exsistence)
        {
            break;
        }
        gameSuccess = 1;
    }
    if (gameSuccess)
    {
        Director::getInstance()->popScene();
    }
    return true;  //返回true，才干够继续监听其他碰撞
}

void GameScene::update(float dt)
{
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
    if (Balls.back()->getPositionY() < board->getPositionY() - 50)
    {
        Director::getInstance()->popScene();
    }
   
   
}
void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    keys[code] = true;
    switch (code)
    {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            if (board->getPosition().x > 50)
            {
                board->setPositionX(board->getPosition().x - 10);
            }
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            if (board->getPosition().x < 950)
            {
                board->setPositionX(board->getPosition().x + 10);
            }
            break;
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
