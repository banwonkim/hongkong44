#include "UnitDef.hpp"
#include "GameScene.h"
#include <base/ZipUtils.h>
// ControllerLayer.cpp
bool ControllerLayer::init() {
    if (!Layer::init()) return false;

    // 1. UI 배치 (왼쪽, 오른쪽)
    // ... _dpad, _btnA 위치 잡기 ...
    auto winSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    //this->setPosition();
    this->setPositionX(-Shared::Get()->getMarginX());

    auto cache=SpriteFrameCache::getInstance();
    _btnU = Sprite::createWithSpriteFrameName("padbtn.png");
    _btnR = Sprite::createWithSpriteFrameName("padbtn.png");
    _btnD = Sprite::createWithSpriteFrameName("padbtn.png");
    _btnL = Sprite::createWithSpriteFrameName("padbtn.png");
    _btnShift = Sprite::createWithSpriteFrameName("padshift.png");
    //_btnZ = Sprite::createWithSpriteFrameName("padZ.png");

    _btnU->setScale(4.f);
    _btnR->setScale(4.f);
    _btnD->setScale(4.f);
    _btnL->setScale(4.f);
    _btnShift->setScale(2.f,4.f);

    _btnU->setOpacity(175);
    _btnR->setOpacity(175);
    _btnD->setOpacity(175);
    _btnL->setOpacity(175);
    _btnShift->setOpacity(175);

    _btnR->setRotation(90);
    _btnD->setRotation(180);
    _btnL->setRotation(270);
    
    {
        float r = 40.f/2.f//반지름
            *_btnU->getScale();//스케일
        this->radius = 40.f / 2.f//반지름
            * _btnU->getScale();
        r *= sqrt(2);
        
        const float margin = 30;

        _btnD->setPosition(origin.x + winSize.width - 180 - margin, 60 + margin);
        _btnU->setPosition(_btnD->getPosition() + Vec2(0, r * 2));
        _btnL->setPosition(_btnD->getPosition() + Vec2(-r, r));
        _btnR->setPosition(_btnD->getPosition() + Vec2(r, r));
        
        _btnShift->setPosition(origin.x + 180-r/2.f + margin, 60 + margin);
    }
    this->addChild(_btnU);
    this->addChild(_btnD);
    this->addChild(_btnL);
    this->addChild(_btnR);
    this->addChild(_btnShift);
    // 2. 멀티터치 리스너 등록 (AllAtOnce 사용 권장)
    auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(ControllerLayer::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(ControllerLayer::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(ControllerLayer::onTouchesEnded, this);

    // Z-Order가 높은 UI 레이어이므로 이벤트를 삼켜야 뒤쪽 게임 화면이 터치되지 않음 (옵션)
    // listener->setSwallowTouches(true); 

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}
bool checkCircleCollision(Vec2 &btnloc, const Vec2& mouseloc, float radius) {
    // 1. 두 캐릭터의 위치 가져오기
    // 2. 거리의 제곱 계산 (getDistanceSq 사용)
    float distanceSq = btnloc.getDistanceSq(mouseloc);

    // 3. 반지름 합의 제곱 계산
    float radiusSum = radius + 0.5;
    float radiusSumSq = radiusSum * radiusSum;

    // 4. 비교 (실제 거리 계산인 sqrt를 피함)
    if (distanceSq < radiusSumSq) {
        return true; // 충돌
    }
    return false;
}
void ControllerLayer::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {
    
    for (auto& touch : touches) {
        cocos2d::Vec2 loc = touch->getLocation();
        int id = touch->getID();
        if (_moveTouchID == -1) {
            if (checkCircleCollision(loc,_btnU->getPosition(), this->radius)) {
                _moveTouchID = id;
                _btnU->setOpacity(100);
                GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_UP_ARROW, nullptr);
            }
            else if (checkCircleCollision(loc, _btnD->getPosition(), this->radius)) {
                _moveTouchID = id;
                _btnD->setOpacity(100);
                GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_DOWN_ARROW, nullptr);
            }
            else if (checkCircleCollision(loc, _btnL->getPosition(), this->radius)) {
                _moveTouchID = id;
                _btnL->setOpacity(100);
                GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_LEFT_ARROW, nullptr);
            }
            else if (checkCircleCollision(loc, _btnR->getPosition(), this->radius)) {
                _moveTouchID = id;
                _btnR->setOpacity(100);
                GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_RIGHT_ARROW, nullptr);
            }
        }
        if (_actionTouchID == -1) {
            if (_btnShift->getBoundingBox().containsPoint(loc)) {
                _actionTouchID = id;
                _btnShift->setOpacity(100);
                GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_SHIFT, nullptr);
            }
        }
    }
}

void ControllerLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {
    for (auto& touch : touches) {
        cocos2d::Vec2 loc = touch->getLocation();
        if (touch->getID() == _moveTouchID) {
            auto isU = checkCircleCollision(loc,_btnU->getPosition(),this->radius);
            auto isD = checkCircleCollision(loc,_btnD->getPosition(),this->radius);
            auto isL = checkCircleCollision(loc,_btnL->getPosition(),this->radius);
            auto isR = checkCircleCollision(loc,_btnR->getPosition(),this->radius);
            
            if (isU) { GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_UP_ARROW, nullptr); _btnU->setOpacity(100); }
            else {
                GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_UP_ARROW, nullptr); _btnU->setOpacity(175);
            }
            if (isD) { GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_DOWN_ARROW, nullptr); _btnD->setOpacity(100); }
            else {
                GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_DOWN_ARROW, nullptr);      _btnD->setOpacity(175);
            }
            if (isL) { GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_LEFT_ARROW, nullptr); _btnL->setOpacity(100); }
            else {
                GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_LEFT_ARROW, nullptr); _btnL->setOpacity(175);
            }
            if (isR) { GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_RIGHT_ARROW, nullptr); _btnR->setOpacity(100); }
            else {
                GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_RIGHT_ARROW, nullptr); _btnR->setOpacity(175);
            }
        }
        if (touch->getID() == _actionTouchID) {
            auto isS = _btnShift->getBoundingBox().containsPoint(loc);
            if (isS) { GameScene::Get()->_player->processKeyPress(EventKeyboard::KeyCode::KEY_SHIFT, nullptr); _btnShift->setOpacity(100); }
            else {
                GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_SHIFT, nullptr); _btnShift->setOpacity(175);
            }
        }
    }
}

void ControllerLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {
    for (auto& touch : touches) {

        if (touch->getID() == _moveTouchID) {
            _moveTouchID = -1;
            GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_UP_ARROW, nullptr);
            GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_DOWN_ARROW, nullptr);
            GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_LEFT_ARROW, nullptr);
            GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_RIGHT_ARROW, nullptr);

            _btnU->setOpacity(175);
            _btnD->setOpacity(175);
            _btnL->setOpacity(175);
            _btnR->setOpacity(175);
        }
        if (touch->getID() == _actionTouchID) {
            _actionTouchID = -1;
            GameScene::Get()->_player->processKeyRelease(EventKeyboard::KeyCode::KEY_SHIFT, nullptr);
            _btnShift->setOpacity(175);
            
        }
    }
}