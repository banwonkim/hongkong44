#include "GameScene.h"
 void GameScene::createEnemyTan(Vec2 enemyPos, TanType tantype, TanPattern tanpat, uint32_t tannum){
	 Vector<SpriteFrame*>frames;
	 SpriteFrame* frame;
	 auto cache = SpriteFrameCache::getInstance();
	 char dir[10] = { 0 };
	 switch (tantype) {
	 case tanNormal: {
		 for (int i = 1; i <= 5; i++) {
			 sprintf(dir, "tan%d.png", i);
			 frame = cache->getSpriteFrameByName(dir);
			 if (frame) {
				 frames.pushBack(frame);
			 }
		 }
	 }break;

	 }
	 auto ani=Animation::createWithSpriteFrames(frames, 1.f / 12.f);
	 auto angle = atan2f(enemyPos.y - _player->getPositionY(), enemyPos.x - _player->getPositionX());

	 switch (tanpat) {
	 case TanPattern::Type_Follow: {
		 log("angle:.2f", angle);
		 auto sec = sqrtf(_player->getPosition().distanceSquared(enemyPos)) / 200.f;
		 //for (uint32_t i = 0; i < tannum; i++) {
			 auto tan = TanSprite::create();
			 tan->setPosition(enemyPos);
			 tan->runAction(RepeatForever::create(Animate::create(ani)));
			 //tan->setOpacity(0);
			 tan->data._r = 0.f;
			 tan->data._angle = angle + MATH_DEG_TO_RAD(180.f);
			 tan->data._sp = 300.f;
			 tan->data._pos = enemyPos;
			 tan->data._pattern = tanpat;
			 tan->data._type = tantype;
			 tan->setInfo(tan->data);
			 tan->setScale(Shared::Get()->scale);
			 this->_ebullets.pushBack(tan);
			 this->addChild(tan);
		 //}
	 }break;
	 case TanPattern::Type_Round: {
		 log("angle:.2f", angle);
		 for (uint32_t idx = 0; idx < tannum; idx++) {
			 auto tan = TanSprite::create();
			 tan->setPosition(enemyPos);
			 tan->runAction(RepeatForever::create(Animate::create(ani)));
			 tan->data._r = 0.f;
			 tan->data._angle = angle+MATH_DEG_TO_RAD(360.f / (float)tannum) * idx+ MATH_DEG_TO_RAD(180.f);
			 tan->data._sp = 300.f;
			 tan->data._pos = enemyPos;
			 tan->data._pattern = tanpat;
			 tan->data._type = tantype;
			 tan->setInfo(tan->data);
			 tan->setScale(Shared::Get()->scale);
			 this->_ebullets.pushBack(tan);
			 this->addChild(tan);
		 }
	 }break;
	 }
}

//ÃÖÀûÈ­ ¾ÈÇØ ±ÍÂú¾Æ
 void GameScene::createItem(Vec2 enemyPos) {
	 if (GameScene::Get()->_chance == 7)return;
	 auto sp = Sprite::createWithSpriteFrameName("piston.png");
	 sp->setPosition(enemyPos);
	 sp->setScale(2.5f * Shared::Get()->scale);
	 sp->runAction(RepeatForever::create(MoveBy::create(1.0f, Vec2(0, -200.f))));
	 sp->runAction(RepeatForever::create(
		 Sequence::create(
			 RotateBy::create(0.016f, 30.f),
			 DelayTime::create(0.016f * 12.f),
			 nullptr
		 )
	 ));
	 _items.pushBack(sp);
	 this->addChild(sp);
 }