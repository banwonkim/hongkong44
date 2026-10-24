#pragma once
#ifndef __UNIT_DEF_H__
#define __UNIT_DEF_H__
#include "cocos2d.h"
#include "shared.hpp"

using namespace cocos2d;

enum TanPattern :uint32_t{
	Type_Follow,
	Type_Round,
};

enum TanType :uint32_t {
	tanNormal,
	tanGreen,
	tanRed,
	tanPurple,
};

class ControllerLayer : public cocos2d::Layer {
public:
	virtual bool init() override;
	CREATE_FUNC(ControllerLayer);

	// 터치 콜백
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	//std::function<void(Vec2)>;

private:
	// UI 스프라이트들
	cocos2d::Sprite* _btnL;   // 왼쪽 이동키
	cocos2d::Sprite* _btnR;   // 왼쪽 이동키
	cocos2d::Sprite* _btnU;   // 오른쪽 버튼
	cocos2d::Sprite* _btnD;   // 오른쪽 버튼

	cocos2d::Sprite* _btnShift;   // 오른쪽 버튼
	cocos2d::Sprite* _btnZ;   // 오른쪽 버튼

	// 현재 터치 ID 관리 (어떤 손가락이 무엇을 누르고 있는지)
	int _moveTouchID = -1;
	int _actionTouchID = -1;
	int _zTouchID = -1;
	float radius;
};
struct TanData {
	Vec2 _pos;
	float _angle;
	float _r;
	float _sp;
	TanPattern _pattern : 8;
	TanType _type : 8;
	TanData() :_pos(0, 0), _angle(0.f), _r(0.f), _sp(0.f), _pattern(Type_Follow), _type(tanNormal) {}
	Vec2 calcPos() {
		return Vec2(cos(_angle) * _r, sin(_angle) * _r) + _pos;
	}
};
class TanSprite :public Sprite {
public:
	TanData data;
	CREATE_FUNC(TanSprite);
	//virtual bool init() override {

	//}
	void setInfo(TanData _data) {
		data = _data;
		switch (data._pattern) {
		case TanPattern::Type_Follow:
			this->scheduleUpdate();
			break;
		default:
			this->scheduleUpdate();
		}
	}
	void update(float dt) override {
		switch (data._pattern) {
		case TanPattern::Type_Follow:
		case TanPattern::Type_Round:
			this->setPosition(data.calcPos());
			data._r += data._sp * dt;
			if (data._sp > 150)data._sp-=100*dt;
			break;
		}
	}
};

#endif
