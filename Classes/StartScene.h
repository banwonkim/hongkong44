#pragma once

#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"
#include "shared.hpp"
class StoryScene :public cocos2d::Node {
public:
	enum {
		Story0,
		Story1,
		Story2,
		Story3,
		Story4,
		StoryGame,
		StoryDie,
		StoryClear,
		StoryNextHongkong,
		StoryEnd,

		StoryMax = StoryEnd,
		TagBlink=0,
	};
	Sprite* bg;
	Vec2 pos_center;
	uint32_t nowstory;
	bool is_change;

	static StoryScene* createStory() {
		return StoryScene::create();
	}


	bool init()
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		auto keyListener = cocos2d::EventListenerKeyboard::create();

		bg = nullptr;
		pos_center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

		//////////////////////////////
		// 1. super init first
		if (!Node::init())
		{
			return false;
		}

		nowstory = Story0;
		is_change = false;
		this->goScene(nowstory);


		keyListener->onKeyPressed = [this](EventKeyboard::KeyCode keycode, Event* event) {
			if (this->is_change)return;
			switch (keycode) {
			case EventKeyboard::KeyCode::KEY_ENTER:
			case EventKeyboard::KeyCode::KEY_SPACE:
				this->nextScene();
				break;
			/*case EventKeyboard::KeyCode::KEY_SHIFT:
				makeAction(StoryGame);
				break;*/
			case EventKeyboard::KeyCode::KEY_ESCAPE:
				makeAction(StoryEnd);
				break;
			case EventKeyboard::KeyCode::KEY_F4:
				Shared::ChangeScreen();
				break;

			};
			};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(
			keyListener, this
		);
		return true;
	}
	void makeAction(uint32_t story);
	void nextScene() {
		
		if(nowstory==StoryDie && !Shared::Get()->isGameClear()){
			if (Shared::Get()->isSkipAble()) { makeAction(StoryGame); }
			else{ makeAction(Story0); }
		}
		else makeAction((nowstory + 1) % (StoryMax + 1));
	}
	void goScene(uint32_t story);
	CREATE_FUNC(StoryScene);
};
class StartScene : public cocos2d::Scene
{
public:
    StoryScene* story;
    float _dt;
    static cocos2d::Scene* createScene()
    {
        return StartScene::create();
    }
    static cocos2d::Scene* createDieScene()
    {
        auto scene=StartScene::create();
        scene->story->goScene(StoryScene::StoryDie);

		return scene;
    }
	static cocos2d::Scene* createClearScene()
	{
		auto scene = StartScene::create();
		scene->story->goScene(StoryScene::StoryClear);

		return scene;
	}
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) {
		if (story->is_change)return;
		story->nextScene();
	}

    virtual bool init() {
		/*[&]() {
			auto effect = TransitionFade::create(1.5f, StartScene::createScene(), Color3B::BLACK);
			Director::getInstance()->replaceScene(effect);
			};*/
		this->story = StoryScene::createStory();
		this->addChild(this->story);
		this->scheduleUpdate();

		auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(StartScene::onTouchesBegan, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		return true;
	}
    void update(float dt) override {
        _dt += dt;
        if (_dt > 0.1f) {
			if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying() == false)
				Shared::playBgmFromDir("hongkong.mp3");
            this->unscheduleUpdate();
        }
    }
    // implement the "static create()" method manually
    CREATE_FUNC(StartScene);
private:
};

#endif 