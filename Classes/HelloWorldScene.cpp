/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "StartScene.h"
USING_NS_CC;

class SetLangModeScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void update(float dt) override;
    // implement the "static create()" method manually
    CREATE_FUNC(SetLangModeScene);
private:
    RefPtr<Sprite> bg;
};
bool SetLangModeScene::init(){
    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = [=](EventKeyboard::KeyCode keycode, Event* event) {
        auto fn= CallFunc::create(
            [=]() {

                auto effect = TransitionFade::create(1.5f, StartScene::createScene(), Color3B::BLACK);
                Director::getInstance()->replaceScene(effect);
            }
        );
        switch (keycode) {
        case EventKeyboard::KeyCode::KEY_ESCAPE:
            Director::getInstance()->end();
            break;

        case EventKeyboard::KeyCode::KEY_F4:
            Shared::ChangeScreen();
            break;
        case EventKeyboard::KeyCode::KEY_1:
            Shared::Get()->nLangMode = LangMode::LANG_KO;
            this->runAction(fn);
            break;
        case EventKeyboard::KeyCode::KEY_2:
        {
            Shared::Get()->nLangMode = LangMode::LANG_EN;
            this->runAction(fn);
        }
        break;
        }
        };
    this->scheduleUpdate();
    _eventDispatcher->addEventListenerWithSceneGraphPriority(
        keyListener, this
    );
    bg = Shared::Get()->createSpriteFromDir("setlang.jpg");
    bg->setPosition(Shared::Get()->getScreenCenter());
    this->addChild(bg);
    return true;
}
Scene* SetLangModeScene::createScene()
{
    return SetLangModeScene::create();
}
void SetLangModeScene::update(float dt) {
    
}

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    //this->setPosition(Vec2(100.f, 100.f));

    
    //this->addChild();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _xijin = Sprite::createWithSpriteFrameName("jongun.png");
   // _xijin->setPosition(400, 300);
    _xijin->setScale(0.5f);
    _xijin->setOpacity(0);
    _xijin->runAction(RepeatForever::create(Sequence::create(
        FadeIn::create(1.5f),
        MoveTo::create(3.0f, Vec2(visibleSize.width / 2 + origin.x, 450)),
        nullptr
    )));
    auto sprite = Sprite::createWithSpriteFrameName("pepe.png");
    sprite->setPosition(Vec2(visibleSize.width/2+origin.x, visibleSize.height/2+origin.y));
    auto moveRight = MoveBy::create(2.0f, Vec2(100, 0));
    auto moveLeft = MoveBy::create(2.0f, Vec2(-100, 0));
    auto seq = Sequence::create(moveRight, moveLeft, nullptr);
    sprite->runAction(RepeatForever::create(seq));
    this->addChild(sprite);
    this->addChild(_xijin);

    auto hack = Shared::createSpriteFromDir("fraud.jpg");
    hack->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    hack->runAction(RepeatForever::create(Sequence::create(
        FadeOut::create(0.15f),
        FadeIn::create(0.15f), 
        nullptr)));
    if (Shared::Get()->_isHack)this->addChild(hack);

    _sprite = sprite;
    _timer = 0.f;

    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = [](EventKeyboard::KeyCode keycode, Event* event) {
        if (keycode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            Director::getInstance()->end();
        }
        else if (keycode == EventKeyboard::KeyCode::KEY_F4) {
            Shared::ChangeScreen();
        }

        };

    this->scheduleUpdate();
    _eventDispatcher->addEventListenerWithSceneGraphPriority(
        keyListener, this

    );
    return true;
}
void HelloWorld::update(float dt) {
    if (_xijin == nullptr)return;

    _timer += dt;
    if (_isDie) {
        return;
    }

	Rect spriteRect = _sprite->getBoundingBox();
	Rect enemyRect = _xijin->getBoundingBox();
	if (spriteRect.intersectsRect(enemyRect)) {
        _isDie = true;
		this->runAction(RepeatForever::create(Sequence::create(
			CallFunc::create([&]() {
				this->setPosition(Vec2(rand() % 10, rand() % 10));
				}),
			DelayTime::create(1.f / 60.f),
			nullptr
		)));
		_xijin->runAction(Sequence::create(
			Spawn::create(
                RotateBy::create(1.0f, 200.f), 
                ScaleBy::create(2.0f, 3.0f), 
                FadeOut::create(3.f), 
                nullptr
            ),
			CallFunc::create([=]() {
					auto effect = TransitionFade::create(1.5f, SetLangModeScene::createScene(), Color3B::BLACK);
					Director::getInstance()->replaceScene(effect);
			}),
            nullptr
        ));
	}

}

    /*auto rep = RepeatForever::create(
        Sequence::create(
            RotateBy::create(3.f, 150.f),
            RotateBy::create(1.f, -150.f),
            ScaleTo::create(2.f, 8.f),
            ScaleTo::create(2.f, 1.f),
            nullptr));
    sprite->runAction(rep);*/