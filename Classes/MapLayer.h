#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "Treasure.h"
USING_NS_CC;
struct ESMsg {
	std::string areaid;
	std::map <std::string, EntityData> entities;
	std::map <std::string, EntityData> players;
	int width;
	int height;
	std::string playerId;
};//enterscene msg
class MapLayer:public cocos2d::Layer
{
public:
	static MapLayer* create(const char* msg);
	bool init(const char* msg);
	virtual bool init();

	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	Entity* getEntity(int entityId);
	void setMoveMap(bool move);
	bool getMoveMap();
	void movePath(Vec2 tpos, int enitiyId);
	void centerTo(bool force = false);
	void checkPosition(Vec2 sdist, float time, bool osx, bool osy);
	bool canMoveX(Vec2 pos, float dx, float width);
	bool canMoveY(Vec2 pos, float dy, float height);

	bool mapInBottom(Vec2 mapPos);
	bool mapInLeft(Vec2 mapPos);
	bool mapInTop(Vec2 mapPos, float mapHeight, float screenHeight);
	bool mapInRight(Vec2 mapPos, float mapWidth, float screenWidth);
	virtual void update(float delta);

	std::map <int, Entity*> mEntites;
	Player* mPlayer;
	cocos2d::Sprite* bgSprite;

	//msg
	void parseES(const char* msg);

	void removeEntites(std::vector<int> entites);
private:
	bool bmove;
	ESMsg esmsg;
};