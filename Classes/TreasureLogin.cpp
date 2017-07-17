#include "TreasureLogin.h"
#include "ui/UIButton.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "json/document.h"
#include "AreaScene.h"

USING_NS_CC;
using namespace cocos2d::ui;

cocos2d::Scene* TeLogin::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = TeLogin::create();
	layer->scheduleUpdate();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool TeLogin::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bgSprite = Sprite::create("treasures/treasures.png");
	bgSprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(bgSprite);

	auto editBoxSize = Size(200, 30);
	auto nameEditBox = ui::EditBox::create(editBoxSize, "orange_edit.png");
	nameEditBox->setPosition(Vec2(visibleSize.width / 2 + 50, 300));
	this->addChild(nameEditBox);

	auto loginBtn = ui::Button::create();
	loginBtn->setTitleText("Start Game");
	loginBtn->setAnchorPoint(Vec2(0.5, 0.5));
	loginBtn->setPosition(Vec2(visibleSize.width / 2, 100));
	loginBtn->addClickEventListener([=](Ref*) {
		pomeloClient->connect("192.168.1.107", 3014, NULL);

		//auto name = nameEditBox->getText();
		//CCLOG("name = %s", name,);
		////auto scene = ChatRoom::createScene(port, name, roomId);
		////Director::getInstance()->replaceScene(scene);
		//pomeloClient->destroy();
	});
	this->addChild(loginBtn, 11);
	// pomelo init
	//PomeloClient::initLib(NULL, NULL, NULL, NULL);
	pomeloClient = PomeloClient::getInstance();
	PomeloClient::getInstance()->registerEventCb(PC_EV_CONNECTED_NAME, [=](const char *msg) {
		char buff[256];
		sprintf(buff, "{\"uid\": \"%s\"}", nameEditBox->getText());
		PomeloClient::getInstance()->requestWithTimeout("gate.gateHandler.queryEntry", buff, (void *)0x14, 2, [=](const char *msg) {
			CCLOG("resp %s = :", msg);
			using rapidjson::Document;
			Document doc;
			doc.Parse<0>(msg);
			port = doc["port"].GetInt();

			//std::string s = msg;

			//std::string searchFora("\"");
			//while(s.find(searchFora) != std::string::npos)
			//	s = s.replace(s.find(searchFora),sizeof(searchFora), """);

			//cJSON *json, *json_value;
			//// 解析数据包  
			//json = cJSON_Parse(msg);
			//json_value = cJSON_GetObjectItem(json, "port");
			//port = json_value->valueint;
			//cJSON_Delete(json);

			pomeloClient->disconnect();
			CCLOG("destroy");
			auto name = nameEditBox->getText();
			auto scene = AreaScene::createScene(port, name);
			Director::getInstance()->replaceScene(scene);
		});
	});
	
	return true;
}

void TeLogin::update(float delta)
{
	if (pomeloClient)
		pomeloClient->update(delta);
}

void TeLogin::onExit() {
	Layer::onExit();
	this->unscheduleUpdate();
	//if (pomeloClient)
		//pomeloClient->destroy();
}

