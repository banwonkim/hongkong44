#ifndef __SHARED_HPP__
#define __SHARED_HPP__
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

struct GamePlayData {
	uint32_t score = 0U;
	uint32_t killEnemy = 0U;
	uint32_t killTong = 0U;
	uint32_t killReds = 0U;
	uint32_t nowTong = 0U;
	uint32_t playtime = 0U;

	GamePlayData() {}
	void AllClear() { killTong = 0; killEnemy = 0, killReds = 0, score = 0; nowTong = 0; playtime = 0; }
	void GameClear() { nowTong = 0u; }
};
enum LangMode{
	LANG_KO,
	LANG_EN,
};
class Shared {
private:
	static Shared* _shared;

	// 생성자를 private로 설정하여 싱글턴 보장
	Shared() :_isHack(false) {
		gpdata = GamePlayData();
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		scale = 1.5f;
	#else
		scale = 1.0f;
	#endif
	}

	// 복사 생성자와 대입 연산자 삭제
	Shared(const Shared&) = delete;
	Shared& operator=(const Shared&) = delete;

public:
	
	Map<std::string, RepeatForever*> _actionMap;
	Map<std::string, SpriteFrame*> _spriteFrameMap;
	GamePlayData gpdata;
	cocos2d::Size designSize;
	static const uint32_t redscore = 10000;
	float scale;
	LangMode nLangMode = LANG_EN;
	bool bFullScreen = false;
	bool _isHack;
	static Shared* Get() {
		if (_shared == nullptr) {
			_shared = new Shared();
		}
		return _shared;
	}
	Vec2 getScreenLeft() { return Director::getInstance()->getVisibleOrigin(); }
	Vec2 getScreenRight() { return Director::getInstance()->getVisibleOrigin() + Vec2(Director::getInstance()->getVisibleSize().width, 0); }
	Vec2 getScreenCenter() { return Director::getInstance()->getVisibleOrigin() + Director::getInstance()->getVisibleSize() / 2; }
	float getMarginX() { return (Director::getInstance()->getVisibleSize().width - this->designSize.width) / 2.f; }
	// 명시적으로 Release 호출하도록 수정
	static void Release() {
		if (_shared != nullptr) {
			// Map의 모든 요소들을 명시적으로 해제
			_shared->_actionMap.clear();
			_shared->_spriteFrameMap.clear();

			delete _shared;
			_shared = nullptr;
		}
	}

	// 소멸자에서 리소스 정리
	~Shared() {
		_actionMap.clear();
		_spriteFrameMap.clear();
	}

	static std::string getPath(const char* dir) {
		std::string path = R"(bg/)";
		path += dir;
		return path;
	}

	static std::string getSoundPath(const char* dir) {
		std::string path = R"(bgm/)";
		path += dir;
		return path;
	}

	static Sprite* createSpriteFromDir(const char* dir) {
		std::string path = getPath(dir);
		return Sprite::create(path.c_str());
	}

	static void playBgmFromDir(const char* dir) {
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(getSoundPath(dir).c_str(), true);
	}
	static void ChangeScreen() {
		auto glview = dynamic_cast<GLViewImpl*>(Director::getInstance()->getOpenGLView());
		bool& bFullScreen = Shared::Get()->bFullScreen;
		if (glview)
		{
			bFullScreen = !bFullScreen;
			if (bFullScreen) glview->setFullscreen();
			else glview->setWindowed(Shared::Get()->designSize.width, Shared::Get()->designSize.height);
		}
	}
	void SaveGameData() {
		std::string path = cocos2d::FileUtils::getInstance()->getWritablePath();

		// 2. 파일명을 붙여서 전체 경로 완성
		path += "save_data.bin";

		// 3. 여기서부터는 익숙한 C언어 함수입니다.
		// 주의: 그냥 "save_data.bin"만 쓰면 안 되고, 위에서 만든 '전체 경로(path)'를 넣어야 합니다.
		FILE* fp = fopen(path.c_str(), "wb");

		if (fp) {
			if (CheckHack()) {
				//MessageBox(0, L"Hack!!!", L"Hack!!!", MB_ICONERROR);
			}
			else fwrite(&gpdata, sizeof(GamePlayData), 1, fp);

			fclose(fp);
			CCLOG("저장 성공: %s", path.c_str());
		}
		else {
			CCLOG("저장 실패! 경로 권한 문제일 수 있음");
		}
	}
	void SaveGameEnding() {
		std::string path = cocos2d::FileUtils::getInstance()->getWritablePath();

		// 2. 파일명을 붙여서 전체 경로 완성
		path += "baka.bin";

		// 3. 여기서부터는 익숙한 C언어 함수입니다.
		// 주의: 그냥 "save_data.bin"만 쓰면 안 되고, 위에서 만든 '전체 경로(path)'를 넣어야 합니다.
		FILE* fp = fopen(path.c_str(), "wb");

		if (fp) {
			if (CheckHack()) {
				//MessageBox(0, L"Hack!!!", L"Hack!!!", MB_ICONERROR);
				fwrite("YouIdiotHacker:)", strlen("YouIdiotHacker:)"), 1, fp);
			}

			else { fwrite("Thankyouforplayingbaka", strlen("Thankyouforplayingbaka"), 1, fp); }
			fwrite(&gpdata, sizeof(GamePlayData), 1, fp);

			fclose(fp);
			CCLOG("저장 성공: %s", path.c_str());
		}
		else {
			CCLOG("저장 실패! 경로 권한 문제일 수 있음");
		}
	}
	bool CheckHack() {
		bool flag = true;
		float score_per_T = (float)((double)((int64_t)gpdata.score - (int64_t)gpdata.killReds * 120000000) / ((double)gpdata.playtime + 0.0001));
		flag &= score_per_T < 15;

		flag &= gpdata.killReds < 101;
		flag &= gpdata.killTong < 4000000;
		//fuck you
		const int64_t expect =
			gpdata.killEnemy * 10
			+ gpdata.killTong * 300
			+ gpdata.killReds * 120000000;
		//const uint32_t eps = gpdata.killEnemy * 5;

		flag &= gpdata.score >= gpdata.killReds * 120000000;
		flag &= ((int64_t)gpdata.score >= (expect - (int64_t)gpdata.killEnemy * 5))
			&& ((int64_t)gpdata.score <= expect);
		//	||( - eps < (gpdata.score - (int64_t)expect) && ((int64_t)gpdata.score - (int64_t)expect) < (int64_t)eps);
		flag &= (gpdata.nowTong == 0);
		return !flag;
	}
	void LoadGameData() {
		std::string path = cocos2d::FileUtils::getInstance()->getWritablePath();
		path += "save_data.bin";

		FILE* fp = fopen(path.c_str(), "rb+");

		if (fp) {
			fread(&gpdata, sizeof(GamePlayData), 1, fp);
			if (CheckHack()) {
				this->_isHack = true;
				//MessageBox(0, L"Hack!!!", L"Hack!!!", MB_ICONERROR);
				gpdata.AllClear();
			}
			fclose(fp);
			// 데이터 적용...
		}
		else {
			// 파일이 없으면? (최초 실행) -> 초기값 세팅
		}
	}
	bool isRedAble() { return (this->gpdata.score > this->redscore); }
	bool isSkipAble() { return (this->gpdata.killTong > 1 || this->gpdata.killReds > 1); }
	bool isGameClear() { return (this->gpdata.score > 1200000000); }
};

#endif