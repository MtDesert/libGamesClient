#include"GameSettings.h"
#include"Game.h"

GameSettings::GameSettings(){}
GameSettings::~GameSettings(){}

#define READ_STR(name) luaState.getGlobalString(#name,name);
#define READ_XY(name) \
luaState.getGlobalTable(#name,[&](){\
	return luaState.getTableForEach([&](int idx){\
		switch(idx){\
			case 0:name.x=luaState.getTopInteger();break;\
			case 1:name.y=luaState.getTopInteger();break;\
			default:return false;\
		}\
		return true;\
	});\
});\
luaState.clearStack();

bool GameSettings::loadFile(const string &filename){
//尝试打开配置文件,出错了就直接返回错误信息
	LuaState luaState;
	luaState.whenError=Game::whenError;
	if(!luaState.doFile(filename))return false;
	//读取通用配置
	READ_XY(windowSize)
	READ_XY(resolution)
	READ_STR(language)
	READ_STR(headImagePath)
	READ_STR(bodyImagePath)
	READ_STR(serverIP)
	serverPort=luaState.getGlobalInteger("serverPort");
	readCustom(luaState);
	//结束
	luaState.clearStack();
	return true;
}
void GameSettings::readCustom(LuaState &state){}