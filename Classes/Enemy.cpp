#include "GameScene.h"
GameScene* GameScene::_gamescene = nullptr;
class EnemyTong :public Enemy {
public:
	float target_x;
	float totaltime;
	int fadeno;
	enum {
		FadeStart,
		FadeMove,
		FadeDown,
		FadeUp,
		FadeEnd,
		//TagFade,
	};
	inline void initTong() {
		auto cache = SpriteFrameCache::getInstance();
		this->data.isShow = 1;
		this->totaltime = 0.f;
		this->target_x = random(70.f, 900.f);

		Size sz = Director::getInstance()->getVisibleSize();
		this->setPosition(
			GameScene::_player->getPositionX(),
			sz.height / 2.f + 250);
		orgPos = this->getPosition();

		Vector<SpriteFrame*>frames;
		frames.pushBack(cache->getSpriteFrameByName("tong1.png"));
		frames.pushBack(cache->getSpriteFrameByName("tong2.png"));
		frames.pushBack(cache->getSpriteFrameByName("tong3.png"));
		this->runAction(
			RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(frames, 1.f / 4.f)))
		);
		auto fade = Sequence::create(
			FadeTo::create(0.05f, 80),
			DelayTime::create(0.2f),
			FadeTo::create(0.05f, 255),
			DelayTime::create(0.2f),
			FadeTo::create(0.05f, 80),
			DelayTime::create(0.2f),
			FadeTo::create(0.05f, 255),
			DelayTime::create(0.2f),
			FadeTo::create(0.05f, 80),
			DelayTime::create(0.2f),
			FadeTo::create(0.05f, 255),
			DelayTime::create(0.2f),
			CallFunc::create([=]() {this->data.isShow = 0; }),
			//RemoveSelf::create(),
			nullptr
		);
		if (this->data.type == Type_Tong2) {
			Sprite* sp = Sprite::createWithSpriteFrameName("ugly.png");
			sp->setNormalizedPosition(Vec2(0.5f, 0.5f));
			this->addChild(sp);
		}
		//fade->setTag(TagFade);
		this->runAction(fade);
		fadeno = FadeStart;
	}
	inline void updateTong(float dt) {
		if (data.hp < 0)return;
		auto ppos = GameScene::Get()->_player->getPosition();
		float tantime = fmodf(totaltime, 4.f);
		this->totaltime += dt;
		if (fadeno <= FadeMove)
			this->setPositionY(sin(totaltime * 360.f * 3.1415926535 / 180.f) * 100.f + orgPos.y);
		if (fadeno==FadeStart) {

#define ABS(A) ((A)>0?(A):-(A))
#define BETWEEN(A,T) (A)> (T) && (A) < (T + 0.016666f)
			if (ABS(this->getPositionX() - target_x) < 20.f * Shared::Get()->scale) {
				target_x = ppos.x;
				if (this->getPositionX() > target_x)this->setScaleX(1.5f * Shared::Get()->scale);
				else this->setScaleX(-1.5f*Shared::Get()->scale);
				log("%.2f", target_x);
			}
			this->setPositionX(this->_position.x+(target_x - this->getPositionX()) / 2.f * dt);

			if (totaltime >= 8.0f && ABS(ppos.x - this->getPositionX()) < 50.f*Shared::Get()->scale)
				fadeno = FadeMove;
			for (int i = 0; i < 4; i++) {
				if (BETWEEN(tantime, 1.0f+(float)i*0.5f)) {
					GameScene::Get()->createEnemyTan(this->getPosition(), TanType::tanNormal, Type_Round, 5);
				}
			}

			/*if (BETWEEN(tantime, 3.f)) {
				GameScene::Get()->createEnemyTan(this->getPosition(), TanType::tanNormal, Type_Follow, 5);
			}*/
#undef ABS
		}
		else {
		#define movetime 10.f
			if (totaltime > movetime && fadeno == FadeMove) {
				totaltime = movetime;
				this->runAction(EaseOut::create(MoveTo::create(0.5f, Vec2(this->getPositionX(), 70)), 0.4f));
				fadeno = FadeDown;
			}
			else if (BETWEEN(totaltime, movetime + 0.5f)) {
				GameScene::Get()->createEnemyTan(this->getPosition(), TanType::tanNormal, Type_Round,
					(this->data.type==Type_Tong2 ? 11 : 5));
			}
			else if (BETWEEN(totaltime, movetime + 0.8f)) {
				GameScene::Get()->createEnemyTan(this->getPosition(), TanType::tanNormal, Type_Round,
					(11));
			}
			else if (totaltime > movetime + 1.5f && fadeno == FadeDown) {
				this->runAction(MoveTo::create(1.0f, Vec2(this->getPositionX(), orgPos.y)));
				fadeno = FadeUp;
			}
			else if (totaltime > movetime + 3.5f && fadeno == FadeUp) {
				target_x = ppos.x;
				fadeno = FadeStart; totaltime -= (movetime + 3.5f);
			}
		#undef movetime
		}
		//this->setPosition(Vec2(rand()%950, rand()%720));
	}
	CREATE_FUNC(EnemyTong);
};
void Enemy::update(float dt) {
	totaltime += dt;
	if (data.isDying == 0) {
		if (data.hp < 0) {
			log("start die");
			data.isDying = 1;
			this->stopAllActions();
			log("pos %.2f, %.2f", this->getPositionX(), this->getPositionY());
			if (data.type >= Type_Tong || data.type == Type_Car)
				GameScene::Get()->createItem(this->getPosition());
			else if(cocos2d::random()%47==1)
				GameScene::Get()->createItem(this->getPosition());
			else if(cocos2d::random() < RAND_MAX / 2)
				GameScene::Get()->createEnemyTan(this->getPosition());
			
				//TanType::tanNormal,TanPattern::Type_Round,16);
			Enemy::createDieFrame(this);
			Enemy::updateScore(data.type);
				//this->runAction(frames);
		}
		else {
			if (data.type >= Type_Tong) {
				((EnemyTong*)this)->updateTong(dt);
			}
		}
	}
	//else if (data.isDying)this->removeFromParent();
}

void Enemy::createDieFrame(Enemy* enemy)
{
	Vector<SpriteFrame*>frames;
	auto cache = SpriteFrameCache::getInstance();
	char str[10] = { 0 };

	for (int i = 1; i <= 6; i++) {
		sprintf(str, "bomb%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);

		if (frame) {
			frames.pushBack(frame);
		}
	}
	float scale = Shared::Get()->scale;
	auto _sideAnim = Animation::createWithSpriteFrames(frames, 1.f / 15.f);
	for (int i = 0; i < (enemy->data.type == Type_Tong ? 8 : (enemy->data.type == Type_Car ? 4 : 1)); i++) {
		auto bombSprite = Sprite::createWithSpriteFrame(frames.front());
		if (i > 0) {
			bombSprite->setNormalizedPosition(Vec2(0.5f, 0.5f));
			bombSprite->setPosition(
				enemy->getPosition() + 
				Vec2(cocos2d::random(-90.f*scale, 90.f*scale), cocos2d::random(-70.f*scale, 70.f*scale)));
		}
		else {
			bombSprite->setPosition(enemy->getPosition());
		}
		bombSprite->setScale(Shared::Get()->scale);
		auto seq = Sequence::create(
			FadeTo::create(0.01f,0),
			DelayTime::create((float)i * 0.25f),
			FadeTo::create(0.01f, 200),
			Animate::create(_sideAnim),
			RemoveSelf::create(),
			nullptr);
		enemy->getParent()->addChild(bombSprite, this->getLocalZOrder()+i);
		bombSprite->runAction(seq);
		bombSprite->setOpacity(200);
	}
	if(enemy->data.type<Type_Tong&&enemy->data.type!=Type_Car)
		enemy->setOpacity(0);
	this->runAction(Sequence::create(
		DelayTime::create(6.f/15.f),
		CallFunc::create([enemy]() {
			enemy->setOpacity(255);
			enemy->setSpriteFrame(Shared::Get()->_spriteFrameMap.at("die.png"));
			
			}),
		DelayTime::create(0.50f),
		FadeTo::create(0.03f, 0),
		DelayTime::create(0.15f),
		FadeTo::create(0.03f, 200),
		DelayTime::create(0.08f),
		FadeTo::create(0.03f, 0),
		DelayTime::create(0.08f),
		FadeTo::create(0.03f, 200),
		DelayTime::create(0.30f),
		CallFunc::create([enemy]() {
			enemy->data.isDying = 2;
			}),
		nullptr));
	//_sideAnim->retain(); // 이것도 retain 필수

	//_aniRight->retain();
}
Enemy* Enemy::createEnemy(Enemy::EnemyType type, GameScene* parent) {

	Vector<SpriteFrame*>enemyframe;
	auto cache = SpriteFrameCache::getInstance();
	auto enemy = (type >= Type_Tong) ? EnemyTong::create() : Enemy::create();
	enemy->data.type = (EnemyType)(uint32_t)type;
	enemy->data.isDying = 0;
	enemy->parent = parent;
	enemy->setScale(1.0f);
	switch (enemy->data.type) {
	case Type_Man:
		enemy->data.hp = 20;
		enemy->frame = cache->getSpriteFrameByName("enemy1.png");
		enemy->setSpriteFrame(enemy->frame);
		enemy->setPosition(rand() % 900 + 30.f, 900);
		enemy->runAction(RepeatForever::create(Sequence::create(
			CallFunc::create([enemy]() {
				enemy->setFlippedX(true); }),
				DelayTime::create(0.2f),
				CallFunc::create([enemy]() {
				enemy->setFlippedX(false); }),
				DelayTime::create(0.2f),
				nullptr
				)));
		enemy->runAction(MoveBy::create((rand() % 6) / 4.f + 3.5f, Vec2(0, -1200.f)));
		break;
	case Type_Police:
		enemy->data.hp = 30;
		enemy->frame = cache->getSpriteFrameByName("enemy2.png");
		enemy->setSpriteFrame(enemy->frame);
		enemy->setPosition(rand() % 900 + 30.f, 900);
		enemy->setScaleX(1.3f);
		enemy->runAction(RepeatForever::create(Sequence::create(
			CallFunc::create([enemy]() {enemy->setFlippedX(true); }),
			DelayTime::create(0.2f),
			CallFunc::create([enemy]() {enemy->setFlippedX(false); }),
			DelayTime::create(0.2f),
			nullptr
		)));
		enemy->runAction(MoveBy::create((rand() % 6) / 4.f + 3.f, Vec2(0, -1200.f)));
		break;
	case Type_Crazy:
		enemy->data.hp = 10;
		enemy->frame = cache->getSpriteFrameByName("enemy3.png");
		enemy->setSpriteFrame(enemy->frame);
		enemy->setPosition(rand() % 900 + 30.f, 900);
		enemy->orgPos = enemy->getPosition();
		enemy->runAction(RepeatForever::create(Sequence::create(
			CallFunc::create([enemy]() {enemy->setFlippedX(true); }),
			DelayTime::create(0.2f),
			CallFunc::create([enemy]() {enemy->setFlippedX(false); }),
			DelayTime::create(0.2f),
			nullptr
		)));
		enemy->runAction(
			RepeatForever::create(Sequence::create(
				CallFunc::create([enemy]() {
					enemy->setPositionX(::sinf((enemy->totaltime / (1.f / 60.f)) * 8
						* 3.1415926535f / 180.f) * 80 + enemy->orgPos.x);
					enemy->_position.y -= 5.f;
					}), 
				DelayTime::create(1.f / 60.f), nullptr)));
		//enemy->runAction(MoveBy::create(5.f, Vec2(0, -1000.f)));
		break;
	case Type_Car:
		log("car create");
		enemy->data.hp = 30;
		enemy->frame = cache->getSpriteFrameByName("car.png");
		enemy->setSpriteFrame(enemy->frame);
		enemy->setPosition(1000, GameScene::_player->getPositionY());
		enemy->setScale(1.35f);
		enemy->setOpacity(100);
		enemy->data.isShow = 1;
		enemy->runAction(
			Sequence::create(
				MoveBy::create(0.5f, Vec2(-50.f, 0)),
				CallFunc::create([enemy]() {enemy->data.isShow = 0; enemy->setOpacity(255); }),
				MoveBy::create(1.0f, Vec2(-100.f, 0)),
				MoveTo::create(2.5f, Vec2(-210.f, enemy->getPositionY())),
				CallFunc::create([enemy]() {enemy->data.hp = -10; log("car fucked"); }),
				nullptr)
		);
		break;
	case Type_Tong:
		enemy->data.hp = 1250;
		enemy->setScale(1.5f);
		((EnemyTong*)(enemy))->initTong();
		break;
	case Type_Tong2:
		enemy->data.hp = 10000;
		enemy->setScale(1.5f);
		((EnemyTong*)(enemy))->initTong();
		break;
	}
	enemy->setScaleX(enemy->getScaleX() * Shared::Get()->scale);
	enemy->setScaleY(enemy->getScaleY() * Shared::Get()->scale);
	enemy->scheduleUpdate();
	return enemy;
}
void Enemy::updateScore(EnemyType type) {
	switch (type) {
	case Type_Tong2:	Shared::Get()->gpdata.score += 120000000; break;
	case Type_Tong:		Shared::Get()->gpdata.score += 300; break;
	case Type_Man:		Shared::Get()->gpdata.score += 5; break;
	case Type_Police:	Shared::Get()->gpdata.score += 6; break;
	case Type_Car:		Shared::Get()->gpdata.score += 9; break;
	case Type_Crazy:	Shared::Get()->gpdata.score += 8; break;
	}
}
