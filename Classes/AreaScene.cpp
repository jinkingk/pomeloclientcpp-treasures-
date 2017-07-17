#include "AreaScene.h"
#include "json\document.h"
#include "cJSON\cJSON.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define flag ((void *)0x12)
#define sendFlag ((void *)0x13)

cocos2d::Scene* AreaScene::createScene(int port,std::string name)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = AreaScene::create();
	layer->scheduleUpdate();
	layer->name = name;
	layer->connectToServer(port);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool AreaScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	return true;
}

void AreaScene::connectToServer(int port)
{
	//PomeloClient::initLib(NULL, NULL, NULL, NULL);
	pomeloClient = PomeloClient::getInstance();
	PomeloClient::getInstance()->registerEventCb(PC_EV_CONNECTED_NAME, [=](const char *msg) {
		char reqMsg[200];
		sprintf(reqMsg, "{\"name\": \"%s\"}", this->name.c_str());
		PomeloClient::getInstance()->requestWithTimeout("connector.entryHandler.entry", reqMsg, (void *)0x14, 2, [=](const char *msg) {
			CCLOG("resp %s", msg);
			using rapidjson::Document;
			Document doc;
			doc.Parse<0>(msg);
			char reqMsg[200];
			sprintf(reqMsg, "{\"name\": \"%s\",\"playerId\": \"%d\"}", this->name.c_str(), doc["playerId"].GetInt());
			PomeloClient::getInstance()->requestWithTimeout("area.playerHandler.enterScene", reqMsg, (void *)0x14, 2, [=](const char *msg) {
				CCLOG("resp %s", msg);

				maplayer = MapLayer::create(msg);

				// add layer as a child to scene
				Director::getInstance()->getRunningScene()->addChild(maplayer);

			});
		});
	});
	this->registMsg();
	PomeloClient::getInstance()->connect("192.168.1.107", port, NULL);
}
void AreaScene::registMsg()
{
	PomeloClient::getInstance()->registerEventCb("addEntities", [=](const char *msg) {
		CCLOG("resp %s", msg);
		//maplayer->addEntitys(entityId);
	});
	PomeloClient::getInstance()->registerEventCb("removeEntities", [=](const char *msg) {
		CCLOG("resp %s", msg);
		using rapidjson::Document;
		Document doc;
		doc.Parse<0>(msg);

		std::vector<int> entities;
		const rapidjson::Value& a = doc["entities"];
		assert(a.IsArray());
		for (rapidjson::SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
			entities.push_back(a[i].GetInt());

		maplayer->removeEntites(entities);
	});
	PomeloClient::getInstance()->registerEventCb("onMove", [=](const char *msg) {
		CCLOG("resp %s", msg);
		using rapidjson::Document;
		Document doc;
		doc.Parse<0>(msg);
		const rapidjson::Value& endPos = doc["endPos"];
		maplayer->movePath(Vec2(endPos["x"].GetFloat(),endPos["y"].GetFloat()),doc["entityId"].GetInt());
	});
	PomeloClient::getInstance()->registerEventCb("onRemoveItem", [=](const char *msg) {
		CCLOG("resp %s", msg);
	});
	PomeloClient::getInstance()->registerEventCb("onPickItem", [=](const char *msg) {
		CCLOG("resp %s", msg);
	});
	PomeloClient::getInstance()->registerEventCb("rankUpdate", [=](const char *msg) {
		CCLOG("resp %s", msg);
	});
	PomeloClient::getInstance()->registerEventCb("onKick", [=](const char *msg) {
		CCLOG("resp %s", msg);
	});
	PomeloClient::getInstance()->registerEventCb("disconnect", [=](const char *msg) {
		CCLOG("resp %s", msg);
	});
	PomeloClient::getInstance()->registerEventCb("onUserLeave", [=](const char *msg) {
		CCLOG("resp %s", msg);
	});
}


void AreaScene::update(float delta)
{
	if (pomeloClient)
		pomeloClient->update(delta);
}

