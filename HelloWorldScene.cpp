#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    
    auto layer = HelloWorld::create();

	scene->addChild(layer);
    
    return scene;
}


bool HelloWorld::init()
{

    if ( !Layer::init() )
    {
        return false;
    }

	auto touchevent = EventListenerTouchOneByOne::create();//创建单点触摸事件
	touchevent->setSwallowTouches(true);
	touchevent->onTouchBegan = [=](Touch *touch, Event *event) {
		log("onTouchBegan");
		return true;
	};

	
	LayerColor *color = LayerColor::create(Color4B(0, 0, 0, 100));
	color->setContentSize(Size(1280, 720));
	color->setOpacity(100);
	this->addChild(color, 1000);
	color->setTouchEnabled(true);
	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchevent, color);
	
	Node * pNode = CSLoader::createNode("platform/LobbyUI/LobbyLayer.csb");
	this->addChild(pNode);
    

	//Layout *color = Layout::create();
	//color->setContentSize(Size(1280, 720));
	//color->setBackGroundColorType(LAYOUT_COLOR_SOLID);
	//color->setBackGroundColor(Color3B(0, 0, 0));
	//color->setOpacity(100);
	//this->addChild(color, 1000);
	//color->setTouchEnabled(true);

    
	return true;
	socket = new Socket();
	socket->setAddr("192.168.1.100",1020);
	socket->start();
	
	
	scheduleOnce(schedule_selector(HelloWorld::sendDate,this),0.5f);

    return true;
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *event)
{

}

void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{

}

void HelloWorld::sendDate(float)
{
	char curBuff[SOCKET_TCP_BUFFER] = {0};

	TCP_Head test;
	test.CommandInfo.wMainCmdID = 100;
	test.CommandInfo.wSubCmdID = 101;
	test.TCPInfo.cbCheckCode = 0;
	test.TCPInfo.cbDataKind = 1;
	test.TCPInfo.wPacketSize = 0;
	memcpy(curBuff,&test,sizeof(test));
	socket->sendbuf(curBuff, sizeof(TCP_Head));
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
