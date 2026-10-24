
#pragma once

/*
* draw bg
* bullet enemy hittest
* go die scene, show score, text
* tong shauping
*/

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "shared.hpp"
#include "StartScene.h"
#include "UnitDef.hpp"
using namespace cocos2d;


class GameScene;

class Enemy : public Sprite {
public:
	enum EnemyType :uint32_t {
		Type_Man,
		Type_Police,
		Type_Crazy,
		Type_Car,
		Type_Tong,
		Type_Tong2,
	};

	RefPtr<SpriteFrame>frame;
	//RefPtr<RepeatForever>rep;
	float totaltime;
	Vec2 orgPos;
	struct EnemyData {
		int32_t hp : 16;
		EnemyType type : 3;
		uint32_t isDying : 2;//0 live 01 dying 10 dye
		uint32_t isShow : 1;
		EnemyData() :hp(0), type(Type_Man), isDying(0), isShow(0) {}
	}data;
	GameScene* parent;
	//cocos2d::Vector<cocos2d::SpriteFrame*> frames;
	static Enemy* createEnemy(EnemyType type, GameScene* parent);
	void createDieFrame(Enemy* enemy);

	/*bool init() {
		if (!Sprite::init()) return false;

		return true;
	}*/
	virtual void update(float dt) override;
	void updateScore(EnemyType);
	CREATE_FUNC(Enemy);
};


class Player :public cocos2d::Sprite {
private:
	RefPtr <SpriteFrame> _upSprite;
	RefPtr <SpriteFrame> _downSprite;
	RefPtr <RepeatForever> _aniRight;
	RefPtr <RepeatForever> _aniUp;
	RefPtr <RepeatForever> _aniDown;
	Sprite* _heart;

public:
	GameScene* parent;
	struct {
		float _speed;
		uint32_t _moveState : 4;
		uint32_t _prevmoveState : 4;
		uint32_t _isShift : 1;
	};


	enum {
		isLeft = 0b0001,
		isRight = 0b0010,
		isUp = 0b0100,
		isDown = 0b1000,
	};
	enum {
		TagFade
	};
	//static Player* _player;
	static Player* createPlayer(GameScene* parent) {
		Player* _player = Player::create();
		_player->parent = parent;
		return _player;
	}


	bool init();
	void update(float dt) override {
		this->setDirection(_moveState, dt);

	}

	void setDirection(char key, float dt);
	void processKeyPress(EventKeyboard::KeyCode key, Event* event);
	void processKeyRelease(EventKeyboard::KeyCode key, Event* event);
	~Player() {
		_eventDispatcher->removeEventListenersForTarget(this);
	}
	CREATE_FUNC(Player);
};

class GameScene : public cocos2d::Scene
{
	ControllerLayer* _controlLayer;
	cocos2d::Layer* _gameLayer;
public:

	Vector<Enemy*> _enemies;
	Vector<Sprite*> _bullets;
	Vector<Sprite*> _items;
	Vector<TanSprite*> _ebullets;
	Sprite* _scoreSprite[10];
	Sprite* _itemSprite[8];
	SpriteFrame* _numSprite[10];
	SpriteFrame* _itemFrame;
	Vector<Sprite*> _bgSprite;
	Sprite* _nowbgSprite;
	Label* mylabel;
	Label* mylabel2;
	static GameScene* _gamescene;
	static Player* _player;
	float totaltime, timer[5];
	struct {
		uint32_t _chance : 3;
		uint32_t _timechance : 8;
		uint32_t _isSpace : 1;
		uint32_t _isDie : 2;
	};



	static cocos2d::Scene* createScene()
	{
		auto _gamescene = GameScene::create();
		GameScene::_gamescene = _gamescene;
		return _gamescene;
	}
	static GameScene* Get() {
		return _gamescene;
	}
	//GameScene():_chance{}
	~GameScene() {
		_eventDispatcher->removeEventListenersForTarget(this);
		if (_gamescene == this) _gamescene = nullptr;
		_player->removeFromParentAndCleanup(true);
		_player = nullptr;

		Director::getInstance()->getTextureCache()->removeUnusedTextures();
		_enemies.clear();
		_bullets.clear();
		_ebullets.clear();

		Scene::cleanup();
	}

	void createItem(Vec2 enemyPos);
	void createEnemyTan(Vec2 enemyPos, TanType tantype = TanType::tanNormal, TanPattern tanpat = TanPattern::Type_Follow, uint32_t tannum = 1);

	void changeBg() {

		_nowbgSprite->runAction(
			Sequence::create(FadeOut::create(0.5f),
				CallFunc::create([=]() {
					_nowbgSprite->setSpriteFrame(_bgSprite.at(cocos2d::random(0, 6))->getSpriteFrame());
					}),
				FadeTo::create(0.5f, 180), nullptr)
		);
	}
	void makeCache() {
		Shared::Get()->_spriteFrameMap.insert("die.png", SpriteFrameCache::getInstance()->getSpriteFrameByName("die.png"));

		const char* _bgDirs[7] = { "bg_china1.jpg","bg_china2.jpg","bg_china3.jpg",
		"bg_mao1.jpg", "bg_mao2.jpg", "bg_mao3.jpg","bigmao.jpg" };
		for (int i = 0; i < 7; i++)
			this->_bgSprite.pushBack(Shared::Get()->createSpriteFromDir(_bgDirs[i]));

		_nowbgSprite = Sprite::create();
		//_nowbgSprite->setSpriteFrame(_bgSprite[cocos2d::random(0, 6)]->getSpriteFrame()); 
		_nowbgSprite->setOpacity(0);

		changeBg();
		_nowbgSprite->setAnchorPoint(Vec2(0, 0));
		_nowbgSprite->setScale(960.f / 600.f, 720.f / 600.f);
		this->addChild(_nowbgSprite);

		char dirbuf[10];
		for (int i = 0; i < 10; i++) {
			sprintf(dirbuf, "k%d.png", i);
			this->_numSprite[i] = SpriteFrameCache::getInstance()->getSpriteFrameByName(dirbuf);
		}

		//Shared::Get()->_actionMap.insert(;
	}
	void makeScore() {
		uint32_t score = Shared::Get()->gpdata.score;
		for (int i = 0; i < 10; i++) {
			_scoreSprite[i] = Sprite::create();
			_scoreSprite[i]->setSpriteFrame(_numSprite[0]);
			_scoreSprite[i]->setScale(2);
			_scoreSprite[i]->setPosition(925.f - (9 - i) * 25 * 2.f, 720.f - 17.f * 3.f);
			this->addChild(_scoreSprite[i], 103 + i);
		}
		_itemFrame = SpriteFrameCache::getInstance()
			->getSpriteFrameByName("piston.png");
		for (int i = 0; i < 7; i++) {
			_itemSprite[i] = Sprite::create();
			_itemSprite[i]->setOpacity(0);
			_itemSprite[i]->setRotation(90);
			_itemSprite[i]->setSpriteFrame(_itemFrame);
			_itemSprite[i]->setScale(2* Shared::Get()->scale);
			_itemSprite[i]->setPosition(
				925.f - (6 - i) * 15 * 2.f * Shared::Get()->scale, 620.f);
			this->addChild(_itemSprite[i], 113 + i);
		}


	}
	inline void updateScore() {
		uint32_t score = Shared::Get()->gpdata.score;
		int loc = 9;
		while (score > 0 && loc >= 0) {
			_scoreSprite[loc--]->setSpriteFrame(_numSprite[score % 10]);
			score /= 10;
		}

		for (int i = 0; i < 7; i++) {
			_itemSprite[i]->setOpacity(
				((uint32_t)i < _chance) ? (
					(i == _chance - 1) ?
					(12 - (_timechance % 12)) * 20
					: 255
					) : 100
			);
		}

	}
	inline void showPanel(bool flag) {
		if (!flag) return;
		this->mylabel->setString("ebullet : " + std::to_string(_ebullets.size()));
		if (Shared::Get()->gpdata.nowTong >= 1) {
			{
				Enemy* enemy;
				for (int i = 0; i < _enemies.size(); i++) {
					enemy = _enemies.at(i);
					if (enemy->data.type == Enemy::Type_Tong) {
						break;
					}
				}
				this->mylabel2->setString("tong : (" + std::to_string((int)enemy->getPositionX()) + "," +
					std::to_string((int)enemy->getPositionY()) + ")");
			}
		}
	}
	void ZKeyPress(bool flag) {
		this->_isSpace = flag;
	}
	bool checkCircleCollision(const Vec2& btnloc, const Vec2& mouseloc, float radius) {
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
	virtual bool init() {
		Shared::Get()->gpdata.GameClear();
		//Shared::Get()->gpdata.score = 1199999500;
		if (!Scene::init()) return false;
		this->setPositionX((Shared::Get()->getScreenRight().x - Shared::Get()->designSize.width) / 2.f);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		_controlLayer = ControllerLayer::create();
		this->addChild(_controlLayer, 999);
#else	
		/*_controlLayer->onMoveCallback = [this](cocos2d::Vec2 dir) {
			this->onControllerInput(dir);
			};*/
#endif
		auto label = Label::createWithTTF("Hello Cocos2d-x!", "fonts/Marker Felt.ttf", 24);
		label->setPosition(Vec2(900, 500));
		label->setString("emenies : " + std::to_string(_enemies.size()));
		auto label2 = Label::createWithTTF("Hello Cocos2d-x!", "fonts/Marker Felt.ttf", 24);
		label2->setPosition(Vec2(800, 400));
		label2->setAlignment(TextHAlignment::RIGHT);
		label->setOpacity(0);
		label2->setOpacity(0);

		// Add it to the scene
		this->addChild(label, 101);
		this->addChild(label2, 102);
		this->mylabel = label;
		this->mylabel2 = label2;

		this->totaltime = 0.f;
		for (int i = 0; i < 5; i++) this->timer[i] = 0.f;
		this->_isSpace = 0u;
		this->_isDie = 0u;
		this->_chance = 0u;



		this->makeCache();
		this->makeScore();

		_player = Player::createPlayer(this);
		_player->setLocalZOrder(100u);
		this->addChild(_player);


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#else
		auto listener = EventListenerKeyboard::create();
		listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
			switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_SPACE: case EventKeyboard::KeyCode::KEY_Z: 
				this->ZKeyPress(true);
			break;
			}
			};
		listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
			switch (keyCode) { 
			case EventKeyboard::KeyCode::KEY_SPACE: case EventKeyboard::KeyCode::KEY_Z: {
				this->ZKeyPress(false);
			}
			};
			};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

#endif/*
		*///create enemy 

		auto moveAction = MoveBy::create(1.5f, Vec2(0, 1000));
		//Shared::Get()->_actionMap.insert("bullet", RepeatForever::create(moveAction));
		this->runAction(
			RepeatForever::create(
				Sequence::create(
					DelayTime::create(0.15f),
					CallFunc::create([&]() {
						if (this->_isSpace) {
							auto bullet = Sprite::createWithSpriteFrameName("mytan.png");
							bullet->setScale(Shared::Get()->scale);
							// 총알은 플레이어 위치에서 시작
							bullet->setPosition(this->_player->getPosition());

							// 총알 자체의 이동 액션 (Scene update에서 좌표 바꿔도 되고, Action 써도 됨)
							auto moveAction = MoveBy::create(1.5f, Vec2(0, 1000));
							bullet->runAction(RepeatForever::create(moveAction));
							this->addChild(bullet);
							_bullets.pushBack(bullet); // 리스트에 등록해야 충돌 체크 가능!

						}

						}),
					nullptr)
			)
		);

		this->scheduleUpdate();
		return true;
	}

	void update(float dt) override {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

		GameScene::Get()->ZKeyPress(true);
#else
#endif
		if (this->_isDie == 2)return;
		totaltime += dt;
		cocos2d::Rect rectPlayer = _player->getBoundingBox();

		this->updateScore();
		this->showPanel(false);
		//debug




		while (totaltime > 1.f) {
			Shared::Get()->gpdata.playtime += 1;
			totaltime -= 1.f;

		}
		if (Shared::Get()->gpdata.playtime % 60 == 0)
			Director::getInstance()->getTextureCache()->removeUnusedTextures();

		if (_enemies.size() < (Shared::Get()->gpdata.nowTong >= 1 ? 3 : 20)) {

			for (int i = 0; i < 5; i++)timer[i] += dt;
			if (timer[Enemy::Type_Man] > 0.5f)
			{
				timer[Enemy::Type_Man] -= 0.5f;
				auto enemy = Enemy::createEnemy(Enemy::EnemyType(rand() % 3), this);
				_enemies.pushBack(enemy);
				this->addChild(enemy);
			}
			if (timer[Enemy::Type_Car] > 20.0f)
			{
				timer[Enemy::Type_Car] -= 20.0f;
				auto enemy = Enemy::createEnemy(Enemy::EnemyType(Enemy::Type_Car), this);
				_enemies.pushBack(enemy);
				this->addChild(enemy);
			}
			if (timer[Enemy::Type_Tong] > 30.f
				&& Shared::Get()->gpdata.nowTong < (Shared::Get()->isRedAble() ? 1 : 1))
			{
				timer[Enemy::Type_Tong] = 0.f;
				auto enemy = Enemy::createEnemy(
					Shared::Get()->isRedAble() ?
					Enemy::Type_Tong2 :
					Enemy::Type_Tong
					, this);
				_enemies.pushBack(enemy);
				Shared::Get()->gpdata.nowTong++;
				log("enemy tong %d", _enemies.size());
				this->addChild(enemy);
			}
		}
		for (ssize_t i = _items.size() - 1; i >= 0; i--) {
			auto it = _items.at(i);
			if (it->getBoundingBox().intersectsCircle(_player->getPosition(), 20.f * Shared::Get()->scale)) {
				if (this->_chance < 7)this->_chance += 1;
				it->removeFromParent();
				_items.erase(i);
			}
			else if (it->getPositionX() < -200.f
				|| it->getPositionX() > 1500.f
				|| it->getPositionY() > 1000.0f
				|| it->getPositionY() < -200.f) {
				it->removeFromParent();
				_items.erase(i);
			}
		}
		for (ssize_t i = _ebullets.size() - 1; i >= 0; i--) {
			auto bul = _ebullets.at(i);
			if (bul->getBoundingBox().intersectsCircle(_player->getPosition(), 15.f)) {
				this->_isDie = true;
				break;
			}
			else if (bul->getPositionX() < -200.f
				|| bul->getPositionX() > 1500.f
				|| bul->getPositionY() > 1000.0f
				|| bul->getPositionY() < -200.f) {
				bul->removeFromParent();
				_ebullets.erase(i);
			}
		}
		for (ssize_t i = _enemies.size() - 1; i >= 0; i--) {
			auto enemy = _enemies.at(i);
			if (enemy->data.isDying == 2) {
				if (enemy->data.type == Enemy::Type_Tong) {
					Shared::Get()->gpdata.nowTong--;
					Shared::Get()->gpdata.killTong++;
				}
				else if (enemy->data.type == Enemy::Type_Tong2) {
					Shared::Get()->gpdata.nowTong--;
					Shared::Get()->gpdata.killReds++;
				}
				else {
					Shared::Get()->gpdata.killEnemy++;
				}
				enemy->removeFromParent();
				_enemies.erase(i);
			}
			else if (enemy->getPositionX() < -200.f
				|| enemy->getPositionX() > 1500.f
				|| enemy->getPositionY() > 1000.0f
				|| enemy->getPositionY() < -200.f) {
				enemy->removeFromParent();
				_enemies.erase(i);
			}
			else {
				{
					if (enemy->data.isDying == 0 && enemy->data.isShow == 0u
						&& enemy->getBoundingBox().intersectsCircle(_player->getPosition(), 15.f)) {
						if (enemy->data.type < Enemy::Type_Tong) {
							this->_isDie = true;
							break;
						}
						else if (this->checkCircleCollision(enemy->getPosition(), _player->getPosition(),
							140.f * Shared::Get()->scale)) {
							this->_isDie = true;
							break;
						}
					}
				}
				for (ssize_t j = _bullets.size() - 1; j >= 0; j--) {
					auto bul = _bullets.at(j);
					if (bul->getPosition().y > 900) {
						bul->removeFromParent();
						_bullets.erase(j);
					}
					else if (enemy->data.isDying == 0 && enemy->getBoundingBox().intersectsRect(
						bul->getBoundingBox())) {
						enemy->data.hp -= 10;
						bul->removeFromParent();
						_bullets.erase(j);
						break;
					}
				}
			}
		}

		if (this->_isDie == 1) {

			if (this->_chance > 0) {
				if (this->_timechance == 0) {
					this->_timechance = 255;
					this->changeBg();
					log("start! inochi");
				}
				else {
					_player->setColor(Color3B(255, 128, 128));
					_player->setOpacity((12 - (_timechance % 12)) * 20);
					if (_timechance == 1) {
						this->_chance--;
						this->_isDie = 0;
						_player->setColor(Color3B(255, 255, 255));
						log("end! inochi");
						_player->setOpacity(255);
					}
					this->_timechance--;
				}

			}
			else {
				Shared::Get()->gpdata.nowTong = 0u;
				this->stopAllActions();
				_player->stopAllActions();
				_player->unscheduleUpdate();
				//
				auto effect = TransitionFade::create(1.0f, StartScene::createDieScene(), Color3B::BLACK);
				Director::getInstance()->replaceScene(effect);
				this->_isDie = 2;
			}
		}
		else if (Shared::Get()->isGameClear()) {
			auto effect = TransitionFade::create(3.0f, StartScene::createClearScene(), Color3B::BLACK);
			Shared::Get()->gpdata.nowTong = 0u;
			this->stopAllActions();
			_player->stopAllActions();
			_player->unscheduleUpdate();
			Director::getInstance()->replaceScene(effect);
			this->_isDie = 2;
		}
	}
	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);
private:
public:

};

#endif