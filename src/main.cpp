#include "Geode/ui/BasedButtonSprite.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/modify/CustomSongLayer.hpp>
#include <UIBuilder.hpp>

using namespace geode::prelude;


class $modify(CustomSongLayer) {
	CCMenuItemSpriteExtra* m_randBtn;
	LoadingCircle* m_loadCircle;

	bool init(LevelSettingsObject* lso) {
		if (!CustomSongLayer::init(lso)) return false;

		Build(CircleButtonSprite::create(CCSprite::create("dice.png"_spr), CircleBaseColor::Pink))
			.scale(0.8)
			.intoMenuItem([this](auto) {
				m_fields->m_randBtn->setVisible(false);
				m_fields->m_loadCircle->setVisible(true);

				Build<CCRotateBy>::create(1.0, 360)
					.repeatForever()
					.runFor(m_fields->m_loadCircle);

				this->retain();
				findRandomSong();
			})
			.store(m_fields->m_randBtn)
			.pos(170, -20)
			.intoNewParent(CCMenu::create())
			.parent(m_mainLayer);

		Build<LoadingCircle>::create()
			.store(m_fields->m_loadCircle)
			.pos(170, -20)
			.scale(0.6)
			.visible(false)
			.parent(m_mainLayer);

		return true;
	}

	void stopLoading() {
		m_fields->m_randBtn->setVisible(true);
		m_fields->m_loadCircle->setVisible(false);
		m_fields->m_loadCircle->stopAllActions();
		release();
	}

	void findRandomSong() {
		if (this->retainCount() == 1) {
			stopLoading();
			return;
		}

		int songID = rand() % 1000000;
		web::AsyncWebRequest()
			.postRequest()
			.postFields(fmt::format("songID={}&secret=Wmfd2893gb7", songID))
			.fetch("http://www.boomlings.com/database/getGJSongInfo.php")
			.text()
			.then([=](std::string const& text) {
				if (text[0] == '-') {
					findRandomSong();
				} else {
					this->m_songIDInput->setString(std::to_string(songID));
					this->onSearch(nullptr);
					stopLoading();
				}
			});
	}
};
