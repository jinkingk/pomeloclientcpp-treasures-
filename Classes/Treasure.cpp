#include "Treasure.h"
Treasure::Treasure(EntityData entity)
{
	edata = entity;
	sprite = cocos2d::Sprite::create();
	labelInfo = cocos2d::Label::create();
	sprite->setPosition(entity.x, entity.y);

	labelInfo->setPosition(sprite->getContentSize().width / 2, sprite->getContentSize().height);
	sprite->addChild(labelInfo);

	char c[32];
	sprintf(c, "%s - %d", entity.kindName.c_str(), entity.score);
	labelInfo->setString(c);
	sprite->setTexture("art/equipment/item_90472.png");
}