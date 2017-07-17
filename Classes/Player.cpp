#include "Player.h"
Player::Player(EntityData entity)
{
	edata = entity;
	sprite = cocos2d::Sprite::create();
	labelInfo = cocos2d::Label::create();
	sprite->setPosition(entity.x, entity.y);	
	labelInfo->setPosition(sprite->getContentSize().width / 2, sprite->getContentSize().height);
	sprite->addChild(labelInfo);

	char c[32];
	sprintf(c, "%s - %d", entity.name.c_str(), entity.score);
	labelInfo->setString(c);

	sprite->setTexture("art/animationPs3/201/player.png");
}