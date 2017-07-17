#pragma once
#include "cocos2d.h"
#include "PomeloClient.h"
#include <vector>
#include <mutex>
#include <string.h>
#include "MapLayer.h"
struct EventCb {
	pc_client_t* client;
	int ev_type;
	void* ex_data;
	std::string arg1;
	std::string arg2;
};
class AreaScene : public cocos2d::Layer
{
public:

	static cocos2d::Scene* createScene(int port, std::string name);
	virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(AreaScene);
// pomelo
	PomeloClient* pomeloClient;

	std::string name;
	void connectToServer(int port);
	void registMsg();
	void addMsg(std::string router, std::string msg);
	std::vector<EventCb> eventCbArray;
	std::mutex eventMutex;
	void update(float delta);

	MapLayer* maplayer;
};