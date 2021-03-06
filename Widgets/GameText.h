#ifndef GAMETEXT_H
#define GAMETEXT_H

#include"GameString.h"

/*游戏文本,用于在屏幕上显示,用于显示多行
*如果需要做文字逐渐出现的效果(比如对话文字的逐字出现),可以考虑使用此类
*/
class GameText:public GameString{
public:
	//构造/析构函数
	GameText();
	GameText(const string &text);
	~GameText();

	//重写
	virtual void setString(const string &str);
	virtual SizeType stringWidth()const;
	virtual void renderX()const;//实时渲染文字
	//换行相关
	void setLineCharAmount(SizeType renderLineAmount,SizeType charAmountPerLine);//设置显示的行数和每行的字符数,超过行数的内容不会被绘制
	//逐字打印
	void readyPrint();//准备逐个文字打印
	bool printNextLine();//准备打印下一行,返回换行是否成功
	bool printNextChar();//打印下一个字符,屏幕上显示,返回文字是否渲染完了需要的区域
protected:
	void updateRenderParameter();//更新渲染参数
	SizeType charAmountPerLine;//一行的字母数量,超过的话绘制成自动换行
	Array<uint> lineStartList;//保存每行的起始位置
	//逐字出现的效果控制
	SizeType renderLineStart;//开始渲染的行
	SizeType renderLineAmount;//渲染的行数
};
#endif