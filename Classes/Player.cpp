#include "GameScene.h"
//Player* Player::_player=nullptr;
Player* GameScene::_player = nullptr;
void Player::setDirection(char key, float dt) {
	auto vec = this->getPosition();

	_speed = 350.0f;
	_speed *= dt;
	if (_isShift)_speed /= 1.818f;

	if (_prevmoveState != key) {
		this->stopAllActions();
		this->setFlippedX(false);

		if (key == 0) {
			this->setSpriteFrame(_downSprite);
		}
		if (key & isLeft) {

			this->setFlippedX(true);
			this->runAction(
				_aniRight
			);
		}
		else if (key & isRight) {

			this->runAction(
				_aniRight
			);
		}
		else if (key & isUp) {

			this->setSpriteFrame(_upSprite);
			this->runAction(
				_aniUp
			);
		}
		else if (key & isDown) {

			this->setSpriteFrame(_downSprite);
			this->runAction(
				_aniDown
			);

		}
	}
	if (key & isLeft)  vec.x -= _speed;
	if (key & isRight) vec.x += _speed;
	if (key & isUp)    vec.y += _speed;
	if (key & isDown)  vec.y -= _speed;
	{
		Size sz = Director::getInstance()->getVisibleSize();
		if (vec.y < 0)vec.y = 0;
		else if (vec.y > sz.height)vec.y = sz.height;
		if (vec.x < 0)vec.x = 0;
		else if (vec.x > Shared::Get()->designSize.width)vec.x = Shared::Get()->designSize.width;
	}
	_prevmoveState = _moveState;
	this->setPosition(vec);
}

bool Player::init(){
	float scale = Shared::Get()->scale;
	//1. 초기 이미지 설정(아무거나, 보통 정면이나 하단)
	if (!Sprite::initWithSpriteFrameName("chin0.png")) return false;

	auto cache = SpriteFrameCache::getInstance();
	Size sz = Director::getInstance()->getVisibleSize();
	this->setPosition(sz.width / 2.f, sz.height / 2.f - 200);

	// ------------------------------------------
	// A. 단일 이미지 로드 (Up/Down) - Animation 안 만듦
	// ------------------------------------------
	_upSprite = cache->getSpriteFrameByName("chin1.png");
	_downSprite = cache->getSpriteFrameByName("chin0.png");
	//_upSprite->retain();
	//_downSprite->retain();
	// 포인터 저장할 땐 retain 필수 (안 하면 메모리 날아감)
	_aniUp = RepeatForever::create(Sequence::create(
		FlipX::create(false),
		DelayTime::create(0.1f),
		FlipX::create(true),
		DelayTime::create(0.1f),
		nullptr
	));
	_aniDown = RepeatForever::create(Sequence::create(
		FlipX::create(true),
		DelayTime::create(0.1f),
		FlipX::create(false),
		DelayTime::create(0.1f),
		nullptr
	));

	// ------------------------------------------
	// B. 애니메이션 생성 (Side) - chin2 ~ chin6
	// ------------------------------------------
	Vector<SpriteFrame*> sideFrames;
	char str[10] = { 0 };

	for (int i = 2; i <= 6; i++) {
		sprintf(str, "chin%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		if (frame) {
			sideFrames.pushBack(frame);
		}
	}

	// 옆면 애니메이션 생성 (0.1초 간격)
	auto _sideAnim = Animation::createWithSpriteFrames(sideFrames, 0.066f);
	_aniRight = RepeatForever::create(Animate::create(_sideAnim));
	//_aniRight->retain();


	_heart = Sprite::createWithSpriteFrameName("flower.png");
	_heart->setScale(1.0f);
	_heart->runAction(
		RepeatForever::create(
			RotateBy::create(2.0f, 360.0f)
		)

	);
	//_heart->setBlendFunc(BlendFunc::ADDITIVE);
	//_heart->setAnchorPoint(Vec2(0, 0));
	//Size size=this->getContentSize();
	_heart->setOpacity(0);
	_heart->setNormalizedPosition(Vec2(0.5f, 0.5f));
	this->addChild(_heart);

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event* event) {
		this->processKeyPress(key, event);
		};
	listener->onKeyReleased = [this](EventKeyboard::KeyCode key, Event* event) {
		this->processKeyRelease(key, event);
		};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	this->setScale(scale);
	this->scheduleUpdate();
	return true;
	}

void Player::processKeyPress(EventKeyboard::KeyCode key, Event* event) {

	char move = _moveState;

	if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)move |= Player::isRight;
	if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)	move |= Player::isLeft;
	if (key == EventKeyboard::KeyCode::KEY_UP_ARROW)	move |= Player::isUp;
	if (key == EventKeyboard::KeyCode::KEY_DOWN_ARROW)	move |= Player::isDown;
	if (key == EventKeyboard::KeyCode::KEY_SHIFT) {
		_heart->stopActionByTag(TagFade);
		{

			auto fadeIn = FadeTo::create(0.3f, 200);
			fadeIn->setTag(TagFade);
			_heart->runAction(fadeIn);
		}
		_isShift = true;
	}


	_moveState = move;


}
void Player::processKeyRelease(EventKeyboard::KeyCode key, Event* event) {
	char move = _moveState;
	if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)	move = move & (~Player::isRight);
	if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)		move = move & (~Player::isLeft);
	if (key == EventKeyboard::KeyCode::KEY_UP_ARROW)		move = move & (~Player::isUp);
	if (key == EventKeyboard::KeyCode::KEY_DOWN_ARROW)		move = move & (~Player::isDown);
	if (key == EventKeyboard::KeyCode::KEY_SHIFT) {
		{
			_heart->stopActionByTag(TagFade);
			{
				auto fadeOut = FadeOut::create(0.3f);
				fadeOut->setTag(TagFade);
				_heart->runAction(fadeOut);
			}
			_isShift = false;
		}
	}

	_moveState = move;

}