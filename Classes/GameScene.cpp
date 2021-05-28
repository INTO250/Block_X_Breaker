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

    Vec2 gravity = Vec2(0.0f, 10.0f);
    getPhysicsWorld()->setGravity(gravity);
    this->getPhysicsWorld()->setAutoStep(true);

    auto edge = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(1.0f, 1.0f, 0.0f));
    edge->setTag(0);
    auto edgeNode = Node::create();
    edgeNode->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    edgeNode->setPhysicsBody(edge);
    this->addChild(edgeNode);

    //������Ƭ��ͼ
    auto* map = TMXTiledMap::create("test1.tmx");
    map->setPosition(Vec2(0, 100));
    addChild(map);

    TMXLayer* blocklayer = map->getLayer("normal_block");//��ȡ��Ҫ���PhysicsBody����Ƭ���ڵ�ͼ��
    for (float i = 0; i <= 39; i++)
    {
        for (float j = 0; j <= 39; j++)
        {
            Blockstore[block_order].block_check = blocklayer->getTileAt(Vec2(i, j));//��ȡ�������Ƭ����Ƭ�Ǿ���
            if (Blockstore[block_order].block_check)//����������λ�ö�����Ƭ�����û����Ƭ���ǿ�
            {               
                Blockstore[block_order].exsistence = true;
                Blockstore[block_order].life = 1;
                auto blockBody = PhysicsBody::createBox(Blockstore[block_order].block_check->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));//����PhysicsBody���               
                blockBody->setDynamic(false);
                blockBody->setCategoryBitmask(1);
                blockBody->setCollisionBitmask(1);
                blockBody->setContactTestBitmask(1);
                blockBody->setTag(block_order+1);
                Blockstore[block_order].block_check->setPhysicsBody(blockBody);//����Ƭ�����PhysicsBody���
                block_order++;//start at 0
            }
        }
    }

    //��ȡ��Ҫ�򲻶���ש��
    TMXLayer* toughblocklayer = map->getLayer("tough_block");
    for (float i = 0; i <= 39; i++)
    {
        for (float j = 0; j <= 39; j++)
        {
            toughBlockstore[toughblock_order].block_check = toughblocklayer->getTileAt(Vec2(i, j));//��ȡ�������Ƭ����Ƭ�Ǿ���
            if (toughBlockstore[toughblock_order].block_check)//����������λ�ö�����Ƭ�����û����Ƭ���ǿ�
            {
                toughBlockstore[toughblock_order].exsistence = true;
                auto toughblockBody = PhysicsBody::createBox(toughBlockstore[toughblock_order].block_check->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));//����PhysicsBody���               
                toughblockBody->setDynamic(false);
                toughblockBody->setCategoryBitmask(1);
                toughblockBody->setCollisionBitmask(1);
                toughblockBody->setContactTestBitmask(1);
                toughblockBody->setTag(-1000);//����
                toughBlockstore[toughblock_order].block_check->setPhysicsBody(toughblockBody);//����Ƭ�����PhysicsBody���
                toughblock_order++;//start at 0
            }
        }
    }

    //��ȡbonus
    TMXLayer* bonuslayer = map->getLayer("bonus");
    for (float i = 39; i >=0 ; i--)
    {
        for (float j = 0; j <=39 ; j++)
        {
            bonus[bonus_order].block_check = bonuslayer->getTileAt(Vec2(i, j));//��ȡ�������Ƭ����Ƭ�Ǿ���
            if (bonus[bonus_order].block_check)//����������λ�ö�����Ƭ�����û����Ƭ���ǿ�
            {
                bonus[bonus_order].exsistence = true;
                bonus[bonus_order].life = 0;
                auto bonusBody = PhysicsBody::createBox(bonus[bonus_order].block_check->getContentSize(), PhysicsMaterial(10000.0f, 1.0f, 0.0f));//����PhysicsBody���               
                bonusBody->setDynamic(false);
                bonusBody->setCategoryBitmask(1);
                bonusBody->setCollisionBitmask(1);
                bonusBody->setContactTestBitmask(1);
                bonusBody->setTag(-1* bonus_order-1);
                bonusBody->setGravityEnable(false);
                bonus[bonus_order].block_check->setPhysicsBody(bonusBody);//����Ƭ�����PhysicsBody���
                bonus_order++;//start at 0
            }
        }
    }
    
    //��
    board = Board::createBoard("board.png");
    board->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5));
    this->addChild(board);
    auto boardBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(10000.f, 1.0f, 0.0f));
    boardBody->setCategoryBitmask(1);
    boardBody->setCollisionBitmask(1);
    boardBody->setContactTestBitmask(1);
    boardBody->setRotationEnable(true);
    boardBody->setGravityEnable(false);
    boardBody->setDynamic(false);
    boardBody->setTag(0);
    board->setPhysicsBody(boardBody);

    //������
    powerpng = Arrow::createArrow("power.png");
    powerpng->setPosition(Vec2(visibleSize.width / 3, visibleSize.height / 5+100));
    this->addChild(powerpng);

    //������arrow
    powerArrow = Arrow::createArrow("power_arrow.png");
    powerArrow->setPosition(Vec2(visibleSize.width / 3, visibleSize.height / 5));
    this->addChild(powerArrow);

    //��ͷָ�����䷽��
    arrow = Arrow::createArrow("arrow.png");
    arrow->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + 100));
    this->addChild(arrow);
    auto arrowBody = PhysicsBody::createBox(board->getContentSize(), PhysicsMaterial(0.f, 0.0f, 0.0f));
    arrowBody->setCategoryBitmask(0);
    arrowBody->setCollisionBitmask(1);
    arrowBody->setContactTestBitmask(1);
    arrowBody->setRotationEnable(true);
    arrowBody->setGravityEnable(false);
    arrowBody->setDynamic(false);
    arrowBody->setTag(0);
    arrow->setPhysicsBody(arrowBody);

    //��
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

    auto label_back = Label::createWithTTF("End", "fonts/Marker Felt.ttf", 48);
    auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::BackToStage, this));
    auto backbuttom = Menu::create(item_back, NULL);
    backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height) / 10);
    this->addChild(backbuttom);

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
 
            /*С��ӱ�
            
            */

            /*����
            ActionInterval * scaleto = ScaleTo ::create(2, 0.5);
            board->runAction(scaleto);
            break;
            */ 
}

void GameScene::onEnter()
{
    Scene::onEnter();
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);  //��ʼ��ײ����


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

    if (bodyA->getTag() == 0 && bodyB->getTag() >= 1)//b is a block
    {
        Blockstore[bodyB->getTag() - 1].life -= 1;
        if (Blockstore[bodyB->getTag() - 1].life <= 0)
        {
            Blockstore[bodyB->getTag() - 1].block_check->removeFromParent();
            Blockstore[bodyB->getTag() - 1].exsistence = false;
        }
    }
    else if (bodyB->getTag() == 0 && bodyA->getTag() >= 1)
    {
        Blockstore[bodyA->getTag() - 1].life -= 1;
        if (Blockstore[bodyA->getTag() - 1].life <= 0)
        {
            Blockstore[bodyA->getTag() - 1].block_check->removeFromParent();
            Blockstore[bodyA->getTag() - 1].exsistence = false;
        }
    }
    else if (bodyB->getTag() == 0 && bodyA->getTag() < 0&& bodyA->getTag() !=-1000)
    {
        
        bodyA->setGravityEnable(true);
        bodyA->setVelocity(Vec2(0,100));
        GameScene::transform();//��type�ж�
    }
    else if (bodyA->getTag() == 0 && bodyB->getTag() < 0 && bodyB->getTag() != -1000)
    {
        
        bodyB->setGravityEnable(true);
        bodyB->setVelocity(Vec2(0, -100)); 
    }
    else if (bodyA->getTag() == 0 && bodyA->getTag() < 0 && bodyA->getTag() != -1000)
    {
        removeChild(bonus[bodyA->getTag() * (-1) - 1].block_check);
        bonus[bodyA->getTag() * (-1) - 1].exsistence = false;
        bodyA->removeFromWorld();
        bonus[bodyA->getTag() * (-1) - 1].block_check->removeFromParent();
        GameScene::transform();//��type�ж�
    }
    else if (bodyA->getTag() == 0 && bodyB->getTag() < 0 && bodyB->getTag() != -1000)
    {
        removeChild(bonus[bodyB->getTag() * (-1) - 1].block_check);
        bonus[bodyB->getTag() * (-1) - 1].exsistence = false;
        bodyB->removeFromWorld();
        bonus[bodyB->getTag() * (-1) - 1].block_check->removeFromParent();
        GameScene::transform();//��type�ж�
    }
    return true;  //����true���Ÿɹ���������������ײ
}

void GameScene::boardrotation()
{
    ActionInterval* actionTo = RotateTo::create(50, 180);
    board->runAction(Sequence::create(actionTo, NULL));
}

void GameScene::transform()
{
    ActionInterval* scaleto = ScaleTo::create(2, 0.5);
    board->runAction(scaleto);
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

        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW])
        {
            if (board->getPosition().x > 0)
            {
                board->setPositionX(board->getPosition().x - 20);
            }
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
        {
            if (board->getPosition().x < 1000)
            {
                board->setPositionX(board->getPosition().x + 20);
            }
        }

        int it = 0;
        for (it = 0; it != block_order; it++)
        {
            if (Blockstore[it].exsistence)
            {
                break;
            }
        }

        if (it == block_order)//ʤ���ı�־
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

        }       

        for (auto i = Balls.begin(); i < Balls.end(); i++)
            if ((*i)->getPositionY() < board->getPositionY() - 50)
            {
                if (it != block_order)
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
                }
            }
    }
    else if (!gameStart)
    {
        if (keys[EventKeyboard::KeyCode::KEY_LEFT_ARROW])
        {
            if (shootvec >= -12)
            {
                ActionInterval* actionBy = RotateBy::create(0.01f, -5.f);
                arrow->runAction(Sequence::create(actionBy, NULL));
                shootvec--;
            }
        }
        if (keys[EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
        {
            if (shootvec <= 12)
            {
                ActionInterval* actionBy = RotateBy::create(0.01f, 5.f);
                arrow->runAction(Sequence::create(actionBy, NULL));
                shootvec++;
            }
        }       

        if (keys[EventKeyboard::KeyCode::KEY_SPACE])
        {
            if (power <= 50)
            {
                power++;                
                powerArrow->setPositionY(powerArrow->getPosition().y + 3);
            }
        }
    }

    //��ʾ����
    int score = 0;
    for (auto i = Blocks.begin(); i < Blocks.end(); i++)
    {
        if ((*i)->exsistence == false)
            score++;
    }
    auto Score = Label::createWithTTF("TextAlas", "fonts/Marker Felt.ttf", 50); //��ʼ�ı������壬�ֺ�
    Score->setColor(Color3B(159, 168, 176)); //������ɫ
    Score->setPosition(Vec2(visibleSize.width + 100, visibleSize.height) / 10);
    removeChild(Score);
    addChild(Score);
    Score->setString(GameScene::trans(score));

}
std::string GameScene::trans(long long int value)
{
    char buff[16];
    snprintf(buff, sizeof(buff), "%lld", value);
    return buff;
}
void GameScene::BackToStage(cocos2d::Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(2.0f, StageSelect::create()));
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

