#pragma warning(disable:4819)
#include "StartScene.h"
#include "HelloWorldScene.h"
#include "GameScene.h"
USING_NS_CC;
void StoryScene::goScene(uint32_t story) {
	log("gostory "); log(nowstory); log(" to ");
	nowstory = story;
	if (bg != nullptr) {
		bg->removeFromParent();
		bg = nullptr;
	}
	switch (story) {
	case Story0:
		bg = Shared::createSpriteFromDir("story0.jpg");
		break;
	case Story1:
		if (Shared::Get()->nLangMode == LANG_EN)
			bg = Shared::createSpriteFromDir("story1_en.jpg");
		else bg = Shared::createSpriteFromDir("story1.jpg");
		break;
	case Story2:
		if (Shared::Get()->nLangMode == LANG_EN)
			bg = Shared::createSpriteFromDir("story2_en.jpg");
		else bg = Shared::createSpriteFromDir("story2.jpg");
		break;
	case Story3:
		if (Shared::Get()->nLangMode == LANG_EN)
			bg = Shared::createSpriteFromDir("story3_en.jpg");
		else bg = Shared::createSpriteFromDir("story3.jpg");
		break;
	case Story4:
		if (Shared::Get()->nLangMode == LANG_EN)
			bg = Shared::createSpriteFromDir("story4_en.jpg");
		else bg = Shared::createSpriteFromDir("story4.jpg");
		break;
	case StoryGame:
		Director::getInstance()->replaceScene(GameScene::createScene());
		return;
	case StoryDie:
		bg = Shared::createSpriteFromDir("scenedie.jpg");
		break;
	case StoryClear:
		Shared::Get()->SaveGameEnding();
		bg = Shared::createSpriteFromDir("clear.png");
		{
			auto blink = RepeatForever::create(
				Sequence::create(FadeOut::create(0.2f), FadeIn::create(0.2f), nullptr));
			//blink->setTag(TagBlink);
			bg->runAction(blink);
		}
		break;
	case StoryNextHongkong:
		//bg->stopActionByTag(TagBlink);
		Shared::Get()->gpdata.AllClear();
		bg = Shared::createSpriteFromDir("nexthongkong.png");
		bg->runAction(FadeIn::create(3.f));

		break;


	case StoryEnd:
		//
		Director::getInstance()->end();
		log("END");
		return;
	}
	bg->setPosition(this->pos_center);
	bg->setOpacity(0);
	bg->runAction(Sequence::create(
		FadeIn::create((Shared::Get()->isGameClear() ? 3.0f : 0.25f)),
		CallFunc::create([this]() {
			this->is_change = false;
			}),
		nullptr)
	);

	this->addChild(bg);
	log(nowstory);
}
void StoryScene::makeAction(uint32_t story) {
	this->is_change = true;
	if(Shared::Get()->isGameClear())bg->stopAllActions();
	bg->runAction(
		Sequence::create(
			FadeOut::create((Shared::Get()->isGameClear() ? 3.0f : 0.25f)),
			DelayTime::create(0.15f),
			CallFunc::create([this, story]() {
				this->goScene(story);
				}),
			nullptr
		)
	);
}