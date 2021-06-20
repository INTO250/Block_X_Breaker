#include "GameScene.h"
#include "StageSelectScene.h"
#include "AudioEngine.h"
#include <cmath>
extern float volumeSound;
extern int BGM;
USING_NS_CC;

Scene* GameScene::createScene(int stage, int oriscore)
{
    GameScene* rua = new GameScene();
    if (rua->initWithPhysics(stage, oriscore))
    {
        rua->autorelease();
        return rua;
    }
    return NULL;
}
bool GameScene::initWithPhysics(int stage, int oriscore)
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

    auto edge = PhysicsBody::createEdgeBox(Size(visibleSize.width, visibleSize.height - 216), PhysicsMaterial(10000.0f, 1.0f, 0.0f));
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

    score = oriscore;
    Score = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
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
    board->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + 160));
    if (stage != 0 || !isar)
        board->life = 3;
    else
        board->life = 1;
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

    life = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
    life->setColor(Color3B::WHITE); //设置颜色
    life->setPosition(Vec2(350, 1274));
    life->setString(GameScene::trans(board->life));
    this->addChild(life);

    auto lifeLabel = Label::createWithTTF("Life", "fonts/Marker Felt.ttf", 50);
    lifeLabel->setColor(Color3B::WHITE);
    lifeLabel->setPosition(Vec2(350, 1354));
    this->addChild(lifeLabel);


    //蓄力槽
    powerpng = Arrow::createArrow("power.png");
    powerpng->setPosition(Vec2(visibleSize.width / 3, visibleSize.height / 5 + 260));
    this->addChild(powerpng);

    //蓄力槽arrow
    powerArrow = Arrow::createArrow("power_arrow.png");
    powerArrow->setPosition(Vec2(visibleSize.width / 3, visibleSize.height / 5 + 160));
    this->addChild(powerArrow);

    //箭头指引发射方向
    arrow = Arrow::createArrow("arrow.png");
    arrow->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + 260));
    this->addChild(arrow);

    //球
    Balls[0] = Ball::createBall("ball.png");
    Balls[0]->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 5 + Balls[0]->getContentSize().height / 2 + board->getContentSize().height / 2 + 160));
    Balls[0]->existence = true;
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

    if (!stage)
    {
        isar = true;
        auto rand = RandomHelper();
        stage = rand.random_int(1, 5);
    }
    blocks_create(stage);

    char stageBackgroundString[30];
    sprintf(stageBackgroundString, "stage_%d_background.png", stage);
    auto background = Sprite::create(stageBackgroundString);
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(background, -10);

    char stageBGMString[30];
    sprintf(stageBGMString, "stage_%d_BGM.mp3", stage);
    float volumeBGM = AudioEngine::getVolume(BGM);
    AudioEngine::stop(BGM);
    BGM = AudioEngine::play2d(stageBGMString, true, volumeBGM);

    this->scheduleUpdate();

    return true;
}

void GameScene::blocks_create(int stage)
{
    char stageString[20];
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
                int k = 1 + rand() % 3;
                rua.bonus = k;//1 变小 2 旋转
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
            auto sound = AudioEngine::play2d("sound_crash.mp3", false, volumeSound);
            float volumeBGM = AudioEngine::getVolume(BGM);
            Blocks[bodyB->getTag() - 1].exsistence = false;
            auto effect = ParticleFire::create();
            effect->setDuration(0.5);


            if (Blocks[bodyB->getTag() - 1].bonus)
            {
                effect->setPosition(map->getLayer("bonusBlock")->getTileAt(Blocks[bodyB->getTag() - 1].position)->getPosition());
                bonus_create(Blocks[bodyB->getTag() - 1].bonus, map->getLayer("bonusBlock")->getTileAt(Blocks[bodyB->getTag() - 1].position)->getPosition());
                map->getLayer("bonusBlock")->removeTileAt(Blocks[bodyB->getTag() - 1].position);
            }
            else
            {
                effect->setPosition(map->getLayer("normalBlock")->getTileAt(Blocks[bodyB->getTag() - 1].position)->getPosition());
                map->getLayer("normalBlock")->removeTileAt(Blocks[bodyB->getTag() - 1].position);
            }
            this->addChild(effect);
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
            auto effect = ParticleFire::create();
            effect->setDuration(0.5);


            if (Blocks[bodyA->getTag() - 1].bonus)
            {
                effect->setPosition(map->getLayer("bonusBlock")->getTileAt(Blocks[bodyA->getTag() - 1].position)->getPosition());
                bonus_create(Blocks[bodyA->getTag() - 1].bonus, map->getLayer("bonusBlock")->getTileAt(Blocks[bodyA->getTag() - 1].position)->getPosition());
                map->getLayer("bonusBlock")->removeTileAt(Blocks[bodyA->getTag() - 1].position);
            }
            else
            {
                effect->setPosition(map->getLayer("normalBlock")->getTileAt(Blocks[bodyA->getTag() - 1].position)->getPosition());
                map->getLayer("normalBlock")->removeTileAt(Blocks[bodyA->getTag() - 1].position);
            }
            this->addChild(effect);
            auto sound = AudioEngine::play2d("sound_crash.mp3", false, volumeSound);
            float volumeBGM = AudioEngine::getVolume(BGM);
            score++;
            Score->setString(GameScene::trans(score));



            check_win();
        }
    }

    return true;  //返回true，才干够继续监听其他碰撞
}

void GameScene::speedUp()
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
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameScene::isBoostChange, this));
        CallFunc* func2 = func->clone();
        board->runAction(Sequence::create(func, delay, func2, NULL));
    }
}
void GameScene::isBoostChange()
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

void GameScene::transform()
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
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameScene::isSmallChange, this));
        CallFunc* func2 = func->clone();
        auto sequence = Sequence::create(func, scaleto, delay, scaleback, func2, NULL);
        board->runAction(sequence);
    }
}
void GameScene::rotation()
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
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameScene::isRotatingChange, this));
        CallFunc* func2 = func->clone();
        board->runAction(Sequence::create(func, actionTo, func2, NULL));
    }
}
void GameScene::isRotatingChange()
{
    if (isRotating)
        isRotating = false;
    else
        isRotating = true;
}

void GameScene::update(float dt)
{
    getPhysicsWorld()->setSpeed(speed * MIN(1.0f + 0.02f * score, 3.0f));

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
                if (Balls[i]->getPosition().y < visibleSize.height / 5 - 50)
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
                powerpng->setPosition(Vec2(board->getPosition().x - 70, visibleSize.height / 5 + 260));
                powerArrow->setPosition(Vec2(board->getPosition().x - 70, visibleSize.height / 5 + 160));
                arrow->setPosition(Vec2(board->getPosition().x, visibleSize.height / 5 + 260));
                this->addChild(powerpng);
                this->addChild(arrow);
                this->addChild(powerArrow);
                gameStart = false;
            }
            else
            {
                if (!isar)
                {
                    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
                    auto label_false = Label::createWithTTF("You Lose!", "fonts/Marker Felt.ttf", 148);
                    label_false->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
                    this->addChild(label_false);
                }


                if (isar)
                {
                    auto blockboard = Sprite::create("blockboard.png");
                    blockboard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
                    this->addChild(blockboard);

                    auto Rank = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 60); //初始文本，字体，字号
                    Rank->setColor(Color3B::WHITE); //设置颜色
                    Rank->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 95));
                    int ranklist = rank(score);
                    if (ranklist < 6)
                        Rank->setString("Rank  " + GameScene::trans(ranklist));
                    else
                        Rank->setString("Rank Too Low");
                    this->addChild(Rank);

                    auto Rank1 = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
                    Rank1->setColor(Color3B::WHITE); //设置颜色
                    Rank1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 260 - 50));
                    Rank1->setString("Rank 1  " + GameScene::trans(UserDefault::getInstance()->getIntegerForKey("rank1")));
                    this->addChild(Rank1);

                    auto Rank2 = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
                    Rank2->setColor(Color3B::WHITE); //设置颜色
                    Rank2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 210 - 50));
                    Rank2->setString("Rank 2  " + GameScene::trans(UserDefault::getInstance()->getIntegerForKey("rank2")));
                    this->addChild(Rank2);

                    auto Rank3 = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
                    Rank3->setColor(Color3B::WHITE); //设置颜色
                    Rank3->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 160 - 50));
                    Rank3->setString("Rank 3  " + GameScene::trans(UserDefault::getInstance()->getIntegerForKey("rank3")));
                    this->addChild(Rank3);

                    auto Rank4 = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
                    Rank4->setColor(Color3B::WHITE); //设置颜色
                    Rank4->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 110 - 50));
                    Rank4->setString("Rank 4  " + GameScene::trans(UserDefault::getInstance()->getIntegerForKey("rank4")));
                    this->addChild(Rank4);

                    auto Rank5 = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 50); //初始文本，字体，字号
                    Rank5->setColor(Color3B::WHITE); //设置颜色
                    Rank5->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 60 - 50));
                    Rank5->setString("Rank 5  " + GameScene::trans(UserDefault::getInstance()->getIntegerForKey("rank5")));
                    this->addChild(Rank5);

                }
                Score = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 60); //初始文本，字体，字号
                Score->setColor(Color3B::RED); //设置颜色
                Score->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 160));
                Score->setString("Score  " + GameScene::trans(score));
                this->addChild(Score);

                auto label_back = Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 48);
                auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::onButtonPressed, this));
                auto backbuttom = Menu::create(item_back, NULL);
                backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3 - 10));
                this->addChild(backbuttom);

                gameStart = false;
                gameEnd = true;

            }
        }
        for (unsigned int i = 0; i < Bonus.size(); i++)
        {
            Bonus[i]->setPosition(Vec2(Bonus[i]->getPosition().x, Bonus[i]->getPosition().y - 1));
            if (board->getBoundingBox().containsPoint(Bonus[i]->getPosition()))
            {

                switch (Bonus[i]->getTag())
                {
                    case 1:
                        transform();
                        break;
                    case 2:
                        rotation();
                        break;
                    case 3:
                        speedUp();
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

                Balls[0]->getPhysicsBody()->setDynamic(true);
                gameStart = true;

                Balls[0]->getPhysicsBody()->setVelocity(Vec2((float)2.0f * (power + 50.f) * (float)sin(5.f * shootvec * 3.14 / 180), (power + 50.f) * 2.0f * (float)cos(5 * shootvec * 3.14 / 180)));
                removeChild(arrow, true);
                removeChild(powerArrow, true);
                removeChild(powerpng, true);
                power = 0;
                shootvec = 0;
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
        if (isar)
        {
            Director::getInstance()->pushScene(TransitionFade::create(2.0f, GameScene::createScene(0, score)));
        }
        else
        {
            auto label_success = Label::createWithTTF("Congratulations!", "fonts/Marker Felt.ttf", 68);
            label_success->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
            this->addChild(label_success);

            Score = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 60); //初始文本，字体，字号
            Score->setColor(Color3B::RED); //设置颜色
            Score->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 120));
            Score->setString("Your Score  " + GameScene::trans(score));
            this->addChild(Score);

            auto label_back = Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 48);
            auto item_back = MenuItemLabel::create(label_back, CC_CALLBACK_1(GameScene::onButtonPressed, this));
            auto backbuttom = Menu::create(item_back, NULL);
            backbuttom->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
            this->addChild(backbuttom);
        }
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
    if (!isar)
    {
        BGM = AudioEngine::play2d("stage_select_BGM.mp3", true, volumeBGM);
        Director::getInstance()->replaceScene(TransitionFade::create(2.0f, StageSelect::createScene()));
    }
    else
    {
        BGM = AudioEngine::play2d("MainMenu.mp3", true, volumeBGM);
        Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MenuScene::createScene()));
    }
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

int GameScene::rank(int score)
{
    int rankscore[100] = {};
    int ranklist = 0;
    if (isHaveSaveFile())//如果存在存档
    {
        char temp[50] = {};
        for (int i = 1; i <= 5; i++)
        {
            snprintf(temp, sizeof(temp) / sizeof(char), "rank%d", i);
            rankscore[i] = UserDefault::getInstance()->getIntegerForKey(temp);
        }
        int trans = 0;
        rankscore[6] = score;
        for (int i = 1; i < 7; i++)
        {
            for (int j = i + 1; j < 7; j++)
            {
                if (rankscore[i] < rankscore[j])
                {
                    trans = rankscore[j];
                    rankscore[j] = rankscore[i];
                    rankscore[i] = trans;
                }
            }

        }
        for (int j = 6; j > 0; j--)
        {
            snprintf(temp, sizeof(temp) / sizeof(char), "rank%d", j);
            UserDefault::getInstance()->setIntegerForKey(temp, rankscore[j]);
            UserDefault::getInstance()->flush();
            if (score == rankscore[j])
                ranklist = j;
        }
    }
    else
    {
        isHaveSaveFile();
        UserDefault::getInstance()->setBoolForKey("isHaveSaveFileXml", true);//写入bool判断位
        char temp[50] = {};
        for (int i = 1; i <= 100; i++)
        {
            snprintf(temp, sizeof(temp) / sizeof(char), "rank%d", i);
            UserDefault::getInstance()->setIntegerForKey(temp, 0);//写入初始分数0
        }
        UserDefault::getInstance()->flush();//设置完一定要调用flush，才能从缓冲写入io
        UserDefault::getInstance()->setIntegerForKey("rank1", score);
        ranklist = 1;
    }
    return ranklist;
}

bool GameScene::isHaveSaveFile()//判断存档是否存在
{
    if (!UserDefault::getInstance()->getBoolForKey("isHaveSaveFileXml"))//通过设置的bool型标志位判断，如果不存在
    {
        UserDefault::getInstance()->setBoolForKey("isHaveSaveFileXml", true);//写入bool判断位
        char temp[50] = {};
        for (int i = 1; i <= 100; i++)
        {
            snprintf(temp, sizeof(temp) / sizeof(char), "rank%d", i);
            UserDefault::getInstance()->setIntegerForKey(temp, 0);//写入初始分数0
        }
        UserDefault::getInstance()->flush();//设置完一定要调用flush，才能从缓冲写入io
        return false;
    }
    else
    {
        return true;
    }
}

int GameScene::getHighestHistorySorce()
{
    int highestHistoryScore = 0;
    if (isHaveSaveFile())//如果存在存档
    {
        highestHistoryScore = UserDefault::getInstance()->getIntegerForKey("HighestScore");//读取历史最高分
        if (score > highestHistoryScore)
        {
            UserDefault::getInstance()->setIntegerForKey("HighestScore", score);//修改存档
            highestHistoryScore = score;//最高分重新设定
        }
    }
    return highestHistoryScore;
}
