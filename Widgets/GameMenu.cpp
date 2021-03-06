#include"GameMenu.h"

//构造/析构函数
GameMenuItem::GameMenuItem(){
	bgColor=borderColor=nullptr;
	onClicked=[&](){//菜单项可以通过点击事件反馈给其所属菜单
		auto menu=dynamic_cast<GameMenu*>(parentObject);
		if(menu){
			menu->onItemClicked(this);
		}
	};
}
GameMenuItem::~GameMenuItem(){}
GameMenu::GameMenu():
	renderItemStart(0),renderItemAmount(0),
	selectingItemIndex(0),recycleMode(true),
	onConfirm(nullptr),onCancel(nullptr),pSpriteSelector(nullptr){
	bgColor = &ColorRGBA::Black;
	borderColor = &ColorRGBA::White;
	forceIntercept=true;//防止事件穿透
	onCancel=[&](GameMenu*){//默认取消事件
		removeFromParentObject();
	};
}
GameMenu::~GameMenu(){}

void GameMenuItem::updateData(SizeType pos){}
void GameMenuItem::setSelected(bool b){}

void GameMenu::cursorMoveUp(){
	if(selectingItemIndex>0){
		--selectingItemIndex;
	}else{
		selectingItemIndex = recycleMode ? itemAmount()-1 : 0;
	}
	updateRenderParameters();
}
void GameMenu::cursorMoveDown(){
	++selectingItemIndex;
	if(selectingItemIndex>=itemAmount()){
		selectingItemIndex = recycleMode ? 0 : selectingItemIndex-1;
	}
	updateRenderParameters();
}

void GameMenu::updateRenderParameters(bool forceUpdate){
	//调整ITEM数量
	auto dataAmount = itemAmount();//数据总数
	auto renderAmount = min(dataAmount,renderItemAmount);//实际渲染数<=数据总数
	removeSubObject(pSpriteSelector);
	if(!forceUpdate)forceUpdate=(subObjects.size()!=renderAmount);//如果数量产生变化,则需要渲染
	while(subObjects.size()<renderAmount)addItem();//添加
	while(subObjects.size()>renderAmount)removeItem();//删除
	//更新ITEM选择状态
	selectingItemIndex = min(selectingItemIndex,dataAmount);
	auto renderEnd = renderItemStart + renderAmount;
	if(renderEnd > dataAmount){//修正renderItemStart防止越界
		renderItemStart = dataAmount - renderAmount;
		renderEnd = dataAmount;
	}
	//更新item内容,更新选择控件
	if(selectingItemIndex < renderItemStart){
		renderItemStart = selectingItemIndex;//往前移动
		forceUpdate=true;
	}else if(selectingItemIndex >= renderEnd){
		renderItemStart = selectingItemIndex - renderAmount + 1;//往后移动
		forceUpdate=true;
	}
	if(forceUpdate)updateItemsData();
	updateSize();
	updateItemsPos();
	//更新选择器状态
	if(pSpriteSelector){addSubObject(pSpriteSelector);}
	updateSelectCursor();
}
//菜单项数
SizeType GameMenu::itemAmount()const{return subObjects.size();}

//菜单可以响应键盘事件(比如方向键选择,回车键确定,退出键关闭菜单等)
bool GameMenu::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	bool ret=false;
	if(!pressed){
		ret=true;//先拦截,根据情况再决定最后是否拦截
		switch(key){
			//上下键
			case Keyboard::Key_Up:cursorMoveUp();break;
			case Keyboard::Key_Down:cursorMoveDown();break;
			case Keyboard::Key_Enter:
				if(onConfirm)onConfirm(this);
			break;
			case Keyboard::Key_Esc:
				if(onCancel)onCancel(this);
			break;
			default:ret=false;
		}
	}
	return ret;
}
bool GameMenu::mouseKey(MouseKey key,bool pressed){
	bool ret=false;
	if(!pressed && !isMouseOnSprite()){
		if(onCancel){//有取消事件则调用之
			onCancel(this);
			ret=true;
		}
	}
	return ret;
}
bool GameMenu::mouseWheel(int angle){
	bool ret=true;
	if(angle>0)cursorMoveUp();
	else if(angle<0)cursorMoveDown();
	else ret=false;
	return ret;
}
void GameMenu::onItemClicked(GameMenuItem *menuItem){
	//先找出是第几个菜单项被点了
	auto index=subObjects.indexOf([&](GameObject* const &obj){return obj==menuItem;});
	if(index<0)return;//按理说不可能找不到...
	//改变当前选择项,或者响应确认事件
	index += renderItemStart;
	if(selectingItemIndex==(SizeType)index){
		if(onConfirm)onConfirm(this);
	}else{
		selectingItemIndex=index;
		updateRenderParameters(true);
	}
}

void GameMenu::updateItemsData(){}
void GameMenu::updateSelectCursor(){
	if(!pSpriteSelector)return;
	//设置位置,指向选中的Item
	auto obj=subObjects.data(selectingItemIndex-renderItemStart);
	if(!obj)return;
	auto item=dynamic_cast<GameSprite*>(*obj);
	if(item){
		pSpriteSelector->position.y = item->position.y;
	}
}
void GameMenu::updateSize(){
	size.setXY(0,0);
	forEachSubObj<GameSprite>([&](GameSprite &sprite){
		size.x = max(size.x,sprite.size.x);
		size.y += sprite.size.y;
	});
}
void GameMenu::updateItemsPos(){
	//调整各个控件位置
	auto y=size.y/2;
	forEachSubObj<GameSprite>([&](GameSprite &sprite){
		sprite.position.y = y - sprite.size.y/2;
		y -= sprite.size.y;
	});
}

//GameMenuItem_IconName
GameMenuItem_IconName::GameMenuItem_IconName(){
	spriteIcon.size.setXY(32,32);
	addSubObject(&spriteIcon);
	addSubObject(&stringName);
}
void GameMenuItem_IconName::setCursorWidth(SizeType cursorWidth){
	//位置
	auto rect=rectF();
	spriteIcon.position.x=rect.p0.x + cursorWidth;
	stringName.position.x=spriteIcon.position.x + spriteIcon.size.x;
	//锚点
	spriteIcon.anchorPoint.x=0;
	stringName.anchorPoint.x=0;
}