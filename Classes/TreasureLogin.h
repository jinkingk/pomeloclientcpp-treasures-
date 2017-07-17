#ifndef __TREASURELOGIN_SCENE_H__
#define __TREASURELOGIN_SCENE_H__

#include "cocos2d.h"
#include "PomeloClient.h"

class TeLogin : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(TeLogin);
	// pomelo
	PomeloClient *pomeloClient;
	int port;
	void update(float delta);

	void onExit();
};

#endif // __TREASURELOGIN_SCENE_H__
