#include "Entity.h"
Entity::Entity()
{
}

cocos2d::Sprite* Entity::getSprite()
{
	return sprite;
}

void Entity::setInfo(const char* info)
{
	labelInfo->setString(info);
}