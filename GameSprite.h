#ifndef GAMESPRITE_H
#define GAMESPRITE_H

#include"GameObject.h"
#include"Texture.h"
#include"ColorRGB.h"

/*游戏中的精灵,可以用于表现人物,物体等各种东西*/
class GameSprite:public GameObject{
public:
	GameSprite();
	virtual ~GameSprite();

	//显示内容
	Texture texture;//精灵的纹理,基本上每个精灵都有纹理,如果2D游戏要变换显示的内容,简单改变此值就好
	ColorRGBA color;//精灵可以有颜色,可以通过变色来区分
	Point2D<int> size;//精灵的大小,没有纹理的情况下可以考虑使用此值
	const ColorRGBA *bgColor,*borderColor;//背景色与边框色,在有值的情况下进行绘制
	//几何变量
	Point3D<int> position;//位置(用来表现在屏幕上的位置)
	double rotateAngle;//旋转角度
	Point3D<int> rotation;//旋转方向
	Point3D<float> scale;//缩放
	Point2D<float> anchorPoint;//锚点(百分比)

	//设置纹理
	void setTexture(const Texture *tex);
	void setTexture(const Texture &tex);
	void setTexture_FilePNG(const string &filename,WhenErrorString whenError=nullptr);

	virtual void setColor(const ColorRGBA &clr);//设置颜色
	bool isMouseOnSprite()const;//判断当前的鼠标位置有没有在精灵上
	//布局相关,针对所有子物体
	void horizontalLayout(int start,int spacing);
	void verticalLayout(int start,int spacing);

	//屏幕相关
	decltype(position) screenPosition()const;//屏幕位置
	void setScreenPosition(const decltype(position) &pos);//设置精灵在屏幕上的位置
	//移动到角落
	void setScreenPosition_Corner(bool right,bool top);

	//以下函数有可能子类差异化
	virtual void render()const;//渲染texture
	virtual void renderX()const;//子类特殊的渲染方式
	virtual Rectangle2D<float> rectF()const;//返回精灵在屏幕上的矩形区域,值由成员变量推断
};
#endif