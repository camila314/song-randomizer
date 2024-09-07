#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/modify/CustomSongLayer.hpp>
#include <UIBuilder.hpp>

using namespace geode::prelude;


class $modify(CustomSongLayer) {
	struct Fields {
		CCMenuItemSpriteExtra* m_randBtn;
		LoadingCircle* m_loadCircle;
	};

	bool init(CustomSongDelegate* del) {
		if (!CustomSongLayer::init(del)) return false;

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
			.pos(390, -155)
			.parent(m_buttonMenu);

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

		unsigned int songID = ((uint32_t)rand() * (uint32_t)rand()) % 1200000;
		web::WebRequest()
			.bodyString(fmt::format("songID={}&secret=Wmfd2893gb7", songID))
			.post("https://www.boomlings.com/database/getGJSongInfo.php")
			.listen([=](web::WebResponse* response) {
				if (response->code() != 200) {
					FLAlertLayer::create("Timed Out", "Could not connect to the server. Please try again later.", "OK")->show();
					return;
				}

				auto text = response->string().unwrapOr("-1");
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
