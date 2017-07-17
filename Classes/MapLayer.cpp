#include "MapLayer.h"
#include "json\document.h"
#include "AreaScene.h"
USING_NS_CC;
MapLayer* MapLayer::create(const char* msg)
{
	MapLayer *maplayer = new (std::nothrow) MapLayer();
	if (maplayer && maplayer->init(msg))
	{
		maplayer->autorelease();
		return maplayer;
	}
	CC_SAFE_DELETE(maplayer);
	return nullptr;
}

bool MapLayer::init(const char* msg)
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	parseES(msg);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// add "HelloWorld" splash screen"
	bgSprite = Sprite::create("art/map/oasis-mini.jpg");
	bgSprite->setAnchorPoint(Vec2(0, 0));
	bgSprite->setContentSize(Size(esmsg.width, esmsg.height));
	// position the sprite on the center of the screen
	bgSprite->setPosition(Vec2(visibleSize.width / 2- esmsg.width /2, visibleSize.height / 2- esmsg.width /2));

	// add the sprite as a child to this layer
	this->addChild(bgSprite, 0);

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(MapLayer::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(MapLayer::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(MapLayer::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//create entity
	for (auto ite = esmsg.entities.begin(); ite != esmsg.entities.end(); ite++)
	{
		if (ite->second.type == EntityType::treasure)
		{
			Treasure* treasure = new Treasure(ite->second);
			bgSprite->addChild(treasure->getSprite());
			mEntites.insert(std::pair<int, Entity*>(ite->second.entityId, treasure));
		}else if(ite->second.type == EntityType::player)
		{
			Player* player = new Player(ite->second);
			bgSprite->addChild(player->getSprite());
			mEntites.insert(std::pair<int, Player*>(ite->second.entityId, player));
			if (ite->second.id == atoi(esmsg.playerId.c_str()))
				mPlayer = player;
		}
	}
	//center player
	bmove = false;
	centerTo(true);
	this->scheduleUpdate();

	return true;
}

bool MapLayer::init()
{
	return Layer::init();
}

void MapLayer::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	CCLOG("onTouchesEnded");
}

void MapLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	CCLOG("onTouchesEnded");
}

void MapLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	if (touches.size() == 1)
	{
		Vec2 tpos = touches[0]->getLocation();
		Vec2 bgPos = bgSprite->getPosition();
		tpos.x -= bgPos.x;
		tpos.y -= bgPos.y;
		char reqMsg[200];
		sprintf(reqMsg, "{\"targetPos\": {\"x\": %f, \"y\": %f}}", tpos.x, tpos.y);
		//check pick treasure
		for (auto e : mEntites) {
			Entity* p = e.second;
			//entity rect
			Rect rect = Rect(p->getSprite()->getPositionX()- p->getSprite()->getContentSize().width/2,
				p->getSprite()->getPositionY()- p->getSprite()->getContentSize().height/2,
				p->getSprite()->getContentSize().width, p->getSprite()->getContentSize().height);
			
			if (rect.containsPoint(tpos)&& p->edata.type == EntityType::treasure)
			{
				sprintf(reqMsg, "{\"targetPos\": {\"x\": %f, \"y\": %f},\"target\":%d}", tpos.x, tpos.y, e.first);
				break;
			}
		}
		//move player
		PomeloClient::getInstance()->requestWithTimeout("area.playerHandler.move", reqMsg, (void *)0x14, 2, [=](const char *msg) {
			CCLOG("resp %s", msg);
		});
	}	
}

Entity* MapLayer::getEntity(int entityId)
{
	return mEntites[entityId];
}

void MapLayer::setMoveMap(bool move)
{
	bmove = move;
}

bool MapLayer::getMoveMap()
{
	return bmove;
}

void MapLayer::movePath(Vec2 tpos,int enitiyId)
{
	Entity* entity = getEntity(enitiyId);
	Sprite* sprite = entity->getSprite();
	setMoveMap(false);
	sprite->stopAllActions();
	Vec2 curpos = sprite->getPosition();
	Vec2 bgPos = bgSprite->getPosition();

	float disx = tpos.x - curpos.x;
	float disy = tpos.y - curpos.y;
	float dis = sqrt(disx*disx + disy*disy);
	int speed = entity->edata.walkSpeed;
	float time = dis / speed;
	if(entity==mPlayer)
		sprite->runAction(
			Sequence::create(
				CallFunc::create(CC_CALLBACK_0(MapLayer::setMoveMap, this, true)),
				MoveTo::create(time, tpos),
				CallFunc::create(CC_CALLBACK_0(MapLayer::setMoveMap, this, false)),
				nullptr
			)
		);
	else
		sprite->runAction(MoveTo::create(time, tpos));

}

void MapLayer::centerTo(bool force)
{
	if (getMoveMap()||force)
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 bgPos = bgSprite->getPosition();
		Vec2 tpos = mPlayer->getSprite()->getPosition();
		tpos.add(bgPos);
		tpos.subtract(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		float ex = bgPos.x - tpos.x;
		float ey = bgPos.y - tpos.y;

		bool sx = canMoveX(Vec2(ex, ey), -tpos.x, bgSprite->getContentSize().width);
		bool sy = canMoveY(Vec2(ex, ey), -tpos.y, bgSprite->getContentSize().height);

		bgSprite->setPosition(sx ? ex : bgPos.x, sy ? ey : bgPos.y);
		if (force) {
			//check boundary
			if (ex > 0)
				ex = 0;
			if (ex < visibleSize.width - bgSprite->getContentSize().width)
				ex = visibleSize.width - bgSprite->getContentSize().width;
			if (ey > 0)
				ey = 0;
			if (ey < visibleSize.height - bgSprite->getContentSize().height)
				ey = visibleSize.height - bgSprite->getContentSize().height;
			bgSprite->setPosition(ex, ey);
		}
	}	
}

bool MapLayer::canMoveX(Vec2 pos,float dx,float width) {
	if (dx == 0) {
		return false;
	}

	if (dx > 0) {
		return !mapInLeft(pos);
	}

	return !mapInRight(pos, width, Director::getInstance()->getVisibleSize().width);
};

bool MapLayer::canMoveY(Vec2 pos,float dy,float height) {
	if (dy == 0) {
		return false;
	}

	if (dy > 0) {
		return !mapInBottom(pos);
	}

	return !mapInTop(pos, height, Director::getInstance()->getVisibleSize().height);
};

bool MapLayer::mapInBottom(Vec2 mapPos) {
	return  mapPos.y >= -10;
}
bool MapLayer::mapInLeft(Vec2 mapPos) {
	return  mapPos.x >= -10;
}
bool MapLayer::mapInTop(Vec2 mapPos,float mapHeight,float screenHeight) {
	return  (mapHeight + mapPos.y - screenHeight) <= 10;
}
bool MapLayer::mapInRight(Vec2 mapPos, float mapWidth, float screenWidth) {
	return  (mapWidth + mapPos.x - screenWidth) <= 10;
}

void MapLayer::update(float delta)
{
	centerTo();
}

void MapLayer::parseES(const char* msg)
{
	using rapidjson::Document;
	Document doc;
	doc.Parse<0>(msg);
	const rapidjson::Value& data = doc["data"];
	esmsg.playerId = data["playerId"].GetString();
	const rapidjson::Value& area = data["area"];
	esmsg.areaid = area["id"].GetString();
	esmsg.width = area["width"].GetInt();
	esmsg.height = area["height"].GetInt();
	const rapidjson::Value& entities = area["entities"];
	Document::ConstMemberIterator ite = entities.MemberBegin();
	for (; ite != entities.MemberEnd(); ++ite)
	{
		EntityData entitydata = EntityData();
		/*if(ite->HasMember("score"))
			entity->score = ite->score.GetInt();*/
		const char* name = ite->name.GetString();
		const rapidjson::Value& value = ite->value;
		if(value.HasMember("x"))
			entitydata.x = value["x"].GetInt();
		if (value.HasMember("y"))
			entitydata.y = value["y"].GetInt();
		if (value.HasMember("entityId"))
			entitydata.entityId = value["entityId"].GetInt();
		if (value.HasMember("kindId")&& value["kindId"].IsInt())
			entitydata.kindId = value["kindId"].GetInt();
		if (value.HasMember("kindId") && value["kindId"].IsString())
			entitydata.kindId = atoi(value["kindId"].GetString());
		if (value.HasMember("type"))
		{
			std::string type = value["type"].GetString();
			if (type.compare("treasure") == 0 ) {
				entitydata.type = EntityType::treasure;
			}else{
				entitydata.type = EntityType::player;
			}
		}
		if (value.HasMember("id") && value["id"].IsInt())
			entitydata.id = value["id"].GetInt();
		if (value.HasMember("id") && value["id"].IsString())
			entitydata.id = atoi(value["id"].GetString());
		if (value.HasMember("name"))
			entitydata.name = value["name"].GetString();
			//strcpy(entity->name, value["name"].GetString());
		if (value.HasMember("walkSpeed"))
			entitydata.walkSpeed = value["walkSpeed"].GetInt();
		if (value.HasMember("score"))
			entitydata.score = value["score"].GetInt();
		if (value.HasMember("areaId"))
			entitydata.areaId = value["areaId"].GetInt();
		if (value.HasMember("imgId"))
			entitydata.imgId = value["imgId"].GetInt();
		if (value.HasMember("kindName"))
			entitydata.kindName = value["kindName"].GetString();
			//strcpy(entity->kindName, value["kindName"].GetString());
		char buf[32];
		sprintf(buf, "%d", entitydata.entityId);
		esmsg.entities[buf]=entitydata;
		if (entitydata.type == EntityType::player)
		{
			std::pair<std::map<std::string, EntityData>::iterator, bool> ret;
			sprintf(buf, "%d", entitydata.id);
			ret = esmsg.players.insert(std::pair<std::string, EntityData>(buf, entitydata));
			if (ret.second == false)
				CCLOG("add player twice! player : %s", entitydata.name);
		}			
	}
}

void MapLayer::removeEntites(std::vector<int> entites)
{
	for (auto ite = entites.begin(); ite != entites.end(); ite++) {
		std::map<int, Entity*>::iterator it;
		it = mEntites.find(*ite);
		if (it != mEntites.end())
		{
			Entity* entity = (*it).second;
			entity->getSprite()->removeFromParent();
			mEntites.erase(it);
		}		
	}
}
