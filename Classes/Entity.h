#pragma once
#include<iostream>
#include "cocos2d.h"
enum EntityType
{
	treasure,
	player
};
struct EntityData
{
	float x;
	float y;
	int entityId;
	int kindId;
	EntityType type;

	int id;
	std::string name;
	int walkSpeed;
	int score;

	int areaId;
	int imgId;
	std::string kindName;
};
class Entity
{
public:
	Entity();
public:
	EntityData edata;
public:
	void setInfo(const char* info);
	cocos2d::Sprite* getSprite();
protected:
	cocos2d::Sprite* sprite;
	cocos2d::Label* labelInfo;
};