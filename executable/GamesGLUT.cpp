#include"Game.h"
#include"PrintF.h"

//Windows特性
#ifdef __i386
#include"WinAPI.h"
#define WINAPI_CODE(code) code
#else
#define WINAPI_CODE(code)
#endif

#include<GL/glut.h>
#include<stdio.h>
#include<time.h>

Game *game=nullptr;//游戏本体
int window=0;//glut的window
const Keyboard keyboard;//键盘对象
static int fps=0;//帧每秒
//时间片类型
enum{
	TimerCPU,
	TimerFPS,
	Timer_Amount
};
int timerInterval[Timer_Amount]={16,1000};//microsecond
//检查OpenGL执行是否出错
void checkGLerror(const char *name){
	int error=glGetError();
	if(error){
		PRINT_ERROR("%s: 出错%d",name,error)
	}else{
		if(name)PRINT_OK(name)
	}
}
#define CHECK_GL_ERROR(name,code)\
code;\
checkGLerror(#name);

void glutTimerFunction(int timerID){
	switch(timerID){
		case TimerCPU:
			game->addTimeSlice(timerInterval[timerID]);//随时更新
			glutPostRedisplay();//更新完后开始渲染
		break;
		case TimerFPS:
			srand(time(NULL));//随时改变随机序列
			PRINT_INFO("fps:%d",fps);//显示帧数
			fps=0;
		break;
	}
	glutTimerFunc(timerInterval[timerID],glutTimerFunction,timerID);
}
void glutIdleFunction(){
	//输出OpenGL的错误信息
	int error=glGetError();
	if(error){
		PRINT_ERROR("GL get error %d",error);fflush(stdout);
	}
	//处理网络消息
	if(game->gameClient){
		game->gameClient->addTimeSlice();
	}
	//处理界面控件
	//GameInputBox::inputing();
	WINAPI_CODE(WinAPI::addTimeSlice();)
}

//input-keyboard
void keyboardFunction(unsigned char key,bool pressed){
	auto k=keyboard.asciiKey[key];
	if(k<Keyboard::Amount_KeyboardKey){
		game->keyboardKey(k,pressed);
	}else{
		PRINT_INFO("keyboardFunction: unknown %d",key);
	}
}
void glutKeyboardFunction(unsigned char key,int x,int y){keyboardFunction(key,true);}
void glutKeyboardUpFunction(unsigned char key,int x,int y){keyboardFunction(key,false);}

//input-special key
#define CASE(num) case GLUT_KEY_F##num:k=Keyboard::Key_F##num;break;
void specialFunction(int key,bool pressed){
	Keyboard::KeyboardKey k=Keyboard::Amount_KeyboardKey;
	switch(key){
		case GLUT_KEY_UP:k=Keyboard::Key_Up;break;
		case GLUT_KEY_DOWN:k=Keyboard::Key_Down;break;
		case GLUT_KEY_LEFT:k=Keyboard::Key_Left;break;
		case GLUT_KEY_RIGHT:k=Keyboard::Key_Right;break;
		case GLUT_KEY_F1:{
			k=Keyboard::Key_F1;
			//启动编辑界面
			if(!pressed){
				WINAPI_CODE(WinAPI::showWindow_Game(game);)
			}
		}break;
		CASE(2)CASE(3)CASE(4)CASE(5)CASE(6)CASE(7)CASE(8)CASE(9)CASE(10)CASE(11)CASE(12)
		case GLUT_KEY_PAGE_UP:k=Keyboard::Key_PageUp;break;
		case GLUT_KEY_PAGE_DOWN:k=Keyboard::Key_PageDown;break;
		case GLUT_KEY_HOME:k=Keyboard::Key_Home;break;
		case GLUT_KEY_END:k=Keyboard::Key_End;break;
		case GLUT_KEY_INSERT:k=Keyboard::Key_Insert;break;
		case 0x70:k=Keyboard::Key_ShiftL;break;
		case 0x71:k=Keyboard::Key_ShiftR;break;
		case 0x72:k=Keyboard::Key_CtrlL;break;
		case 0x73:k=Keyboard::Key_CtrlR;break;
		case 0x74:k=Keyboard::Key_AltL;break;
		case 0x75:k=Keyboard::Key_AltR;break;
		default:PRINT_INFO("specialFunction: unknown key %.8X",key);//do nothing
	}
	game->keyboardKey(k,pressed);
}
#undef CASE
void glutSpecialFunction(int key,int x,int y){specialFunction(key,true);}
void glutSpecialUpFunction(int key,int x,int y){specialFunction(key,false);}

//input-mouse
#define GAME_MOUSE_MOVE game->mouseMove(x-game->resolution.x/2,game->resolution.y/2-y);
void glutMouseFunction(int button,int state,int x,int y){
	GAME_MOUSE_MOVE
	switch(button){
		case GLUT_LEFT_BUTTON:game->mouseKey(Game::Mouse_LeftButton,state==GLUT_DOWN);break;
		case GLUT_MIDDLE_BUTTON:game->mouseKey(Game::Mouse_MiddleButton,state==GLUT_DOWN);break;
		case GLUT_RIGHT_BUTTON:game->mouseKey(Game::Mouse_RightButton,state==GLUT_DOWN);break;
		case 3:case 4://鼠标滚轮,3为往前,4为往后(没办法没找到滚轮的宏)
			if(state==GLUT_UP){
				game->mouseWheel(button==3 ? 1 : -1);
			}
		break;
		default:PRINT_INFO("unknown mouse button: %d",button);
	}
}
void glutMotionFunction(int x,int y){
	GAME_MOUSE_MOVE
}
void glutPassiveMotionFunction(int x,int y){
	GAME_MOUSE_MOVE
}
//input-joystick
void glutJoystickFunction(unsigned int buttonMask,int x, int y, int z){
	if(buttonMask&GLUT_JOYSTICK_BUTTON_A){}
	if(buttonMask&GLUT_JOYSTICK_BUTTON_B){}
	if(buttonMask&GLUT_JOYSTICK_BUTTON_C){}
	if(buttonMask&GLUT_JOYSTICK_BUTTON_D){}
}
//input-spaceball
void glutSpaceballMotionFunction(int x,int y,int z){}
void glutSpaceballRotateFunction(int x,int y,int z){}
void glutSpaceballButtonFunction(int button,int state){}
//input-others
void glutButtonBoxFunction(int button,int state){printf("glutButtonBoxFunction\n");}
void glutDialsFunction(int dial, int value){printf("glutDialsFunction\n");}
void glutTabletMotionFunction(int x,int y){printf("glutTabletMotionFunction\n");}
void glutTabletButtonFunction(int button, int state,int x, int y){printf("glutTabletButtonFunction\n");}

void glutMenuStateFunction(int status){printf("glutMenuStateFunction\n");}
void glutMenuStatusFunction(int status,int x,int y){printf("glutMenuStatusFunction\n");}
void glutWindowStatusFunction(int status){printf("glutWindowStatusFunction\n");}

//event
void glutReshapeFunction(int width,int height){printf("glutReshapeFunction\n");}
void glutVisibilityFunction(int state){
	switch(state){
		case GLUT_VISIBLE:break;
		case GLUT_NOT_VISIBLE:break;
		default:;
	}
}
void glutDisplayFunction(){
	glClear(GL_DEPTH_BUFFER_BIT|GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,1);
	game->render();
	glFlush();
	glutSwapBuffers();
	++fps;
}
void glutOverlayDisplayFunction(){}
void glutEntryFunction(int state){
	switch(state){
		case GLUT_LEFT:break;
		case GLUT_ENTERED:break;
		default:;
	}
}

void printGlutGet(){
	printf("Window:\n");
	printf("X,Y==%d,%d W*H==%d*%d\n",glutGet(GLUT_WINDOW_X),glutGet(GLUT_WINDOW_Y),glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
	printf("Buffer Size: %d\n",glutGet(GLUT_WINDOW_BUFFER_SIZE));
	printf("Stencil Size: %d\n",glutGet(GLUT_WINDOW_STENCIL_SIZE));
	printf("Depth Size: %d\n",glutGet(GLUT_WINDOW_DEPTH_SIZE));
	printf("Size(R,G,B,A): %d,%d,%d,%d\n",glutGet(GLUT_WINDOW_RED_SIZE),glutGet(GLUT_WINDOW_GREEN_SIZE),glutGet(GLUT_WINDOW_BLUE_SIZE),glutGet(GLUT_WINDOW_ALPHA_SIZE));
	printf("AccumSize(R,G,B,A): %d,%d,%d,%d\n",glutGet(GLUT_WINDOW_ACCUM_RED_SIZE),glutGet(GLUT_WINDOW_ACCUM_GREEN_SIZE),glutGet(GLUT_WINDOW_ACCUM_BLUE_SIZE),glutGet(GLUT_WINDOW_ACCUM_ALPHA_SIZE));
	printf("DoubleBuffer: %d\n",glutGet(GLUT_WINDOW_DOUBLEBUFFER));
	printf("RGBA: %d\n",glutGet(GLUT_WINDOW_RGBA));
	printf("Parent: %d\n",glutGet(GLUT_WINDOW_PARENT));
	printf("NumChildren: %d\n",glutGet(GLUT_WINDOW_NUM_CHILDREN));
	printf("ColorMap Size: %d\n",glutGet(GLUT_WINDOW_COLORMAP_SIZE));
	//printf("Num Sample: %d\n",glutGet(GLUT_WINDOW_NUM_SAMPLES));//会报枚举错误
	printf("Stereo: %d\n",glutGet(GLUT_WINDOW_STEREO));
	printf("Cursor: %d\n",glutGet(GLUT_WINDOW_CURSOR));
	printf("FormatID: %d\n",glutGet(GLUT_WINDOW_FORMAT_ID));

	printf("\nScreen(W*H)==%d*%d %dmm*%dmm\n",glutGet(GLUT_SCREEN_WIDTH),glutGet(GLUT_SCREEN_HEIGHT),glutGet(GLUT_SCREEN_WIDTH_MM),glutGet(GLUT_SCREEN_HEIGHT_MM));
	printf("Menu Num Item: %d\n",glutGet(GLUT_MENU_NUM_ITEMS));
	printf("Display Mode Possible: %d\n",glutGet(GLUT_MENU_NUM_ITEMS));
	printf("InitWindow: X,Y==%d,%d W*H==%d*%d\n",glutGet(GLUT_INIT_WINDOW_X),glutGet(GLUT_INIT_WINDOW_Y),glutGet(GLUT_INIT_WINDOW_WIDTH),glutGet(GLUT_INIT_WINDOW_HEIGHT));
	printf("Init Display Mode: %d\n",glutGet(GLUT_INIT_DISPLAY_MODE));
	printf("Elapsed Time: %d\n",glutGet(GLUT_ELAPSED_TIME));
}
void printGlutDeviceGet(){
	printf("\nDevice:\n");
	printf("Has Keyboard: %d\n",glutDeviceGet(GLUT_HAS_KEYBOARD));
	printf("Has Mouse: %d\n",glutDeviceGet(GLUT_HAS_MOUSE));
	printf("Has SpaceBall: %d\n",glutDeviceGet(GLUT_HAS_SPACEBALL));
	printf("Has Dial and Button Box: %d\n",glutDeviceGet(GLUT_HAS_DIAL_AND_BUTTON_BOX));
	printf("Has Tablet: %d\n",glutDeviceGet(GLUT_HAS_TABLET));
	printf("Has Joystick: %d\n",glutDeviceGet(GLUT_HAS_JOYSTICK));
	printf("Owns Joystick: %d\n",glutDeviceGet(GLUT_OWNS_JOYSTICK));
	printf("Joystick Buttons: %d\n",glutDeviceGet(GLUT_JOYSTICK_BUTTONS));
	printf("Joystick Axes: %d\n",glutDeviceGet(GLUT_JOYSTICK_AXES));
	printf("Joystick Poll Rate: %d\n",glutDeviceGet(GLUT_JOYSTICK_POLL_RATE));
	printf("Num Mouse Buttons: %d\n",glutDeviceGet(GLUT_NUM_MOUSE_BUTTONS));
	printf("Num Spaceball Buttons: %d\n",glutDeviceGet(GLUT_NUM_SPACEBALL_BUTTONS));
	printf("Num Button Box Buttons: %d\n",glutDeviceGet(GLUT_NUM_BUTTON_BOX_BUTTONS));
	printf("Num Dials: %d\n",glutDeviceGet(GLUT_NUM_DIALS));
	printf("Num Tablet Buttons: %d\n",glutDeviceGet(GLUT_NUM_TABLET_BUTTONS));
	printf("Device ignore key repeat: %d\n",glutDeviceGet(GLUT_DEVICE_IGNORE_KEY_REPEAT));
	printf("Device key repeat: %d\n",glutDeviceGet(GLUT_DEVICE_KEY_REPEAT));
}

#define GAMESGLUT_GLUTFUNC(name) glut##name##Func(glut##name##Function)

static void whenExit(){
	WINAPI_CODE(WinAPI::deleteThis();)
	delete game;
}

int main(int argc,char* argv[]){
	//游戏参数初始化
	game=Game::newGame();
	game->reset();
	//glut初始化
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	int width=640,height=480,resWidth=width,resHeight=height;
	if(game->gameSettings){//有配置的时候,使用配置中的分辨率
		auto sz=game->gameSettings->windowSize;
		if(sz.x>0)width=sz.x;
		if(sz.y>0)height=sz.y;
		sz=game->gameSettings->resolution;
		if(sz.x>0)resWidth=sz.x;
		if(sz.y>0)resHeight=sz.y;
	}
	int resZ=max(resWidth,resHeight);
	glutInitWindowSize(width,height);
	glutInitWindowPosition(100,100);
	window=glutCreateWindow(game->gameName().data());
	//计时器回调函数
	GAMESGLUT_GLUTFUNC(Idle);
	glutTimerFunc(timerInterval[TimerCPU],glutTimerFunction,TimerCPU);
	glutTimerFunc(timerInterval[TimerFPS],glutTimerFunction,TimerFPS);
	//主要输入回调
	GAMESGLUT_GLUTFUNC(Keyboard);
	GAMESGLUT_GLUTFUNC(KeyboardUp);
	GAMESGLUT_GLUTFUNC(Special);
	GAMESGLUT_GLUTFUNC(SpecialUp);
	GAMESGLUT_GLUTFUNC(Mouse);
	GAMESGLUT_GLUTFUNC(Motion);
	GAMESGLUT_GLUTFUNC(PassiveMotion);
	glutJoystickFunc(glutJoystickFunction,1);
	//空间球输入回调
	GAMESGLUT_GLUTFUNC(SpaceballMotion);
	GAMESGLUT_GLUTFUNC(SpaceballRotate);
	GAMESGLUT_GLUTFUNC(SpaceballButton);
	//其它回调
	GAMESGLUT_GLUTFUNC(ButtonBox);
	GAMESGLUT_GLUTFUNC(Dials);
	GAMESGLUT_GLUTFUNC(TabletMotion);
	GAMESGLUT_GLUTFUNC(TabletButton);

	GAMESGLUT_GLUTFUNC(MenuState);
	GAMESGLUT_GLUTFUNC(MenuStatus);
	GAMESGLUT_GLUTFUNC(WindowStatus);
	//输出回调
	GAMESGLUT_GLUTFUNC(Reshape);
	GAMESGLUT_GLUTFUNC(Visibility);
	GAMESGLUT_GLUTFUNC(Display);
	GAMESGLUT_GLUTFUNC(OverlayDisplay);
	GAMESGLUT_GLUTFUNC(Entry);

	//输出调试信息
	printGlutGet();
	printGlutDeviceGet();
	//OpenGL初始化
	CHECK_GL_ERROR(缩放原点,glScalef(2.0/resWidth,2.0/resHeight,2.0/resZ))//以原点为缩放源进行缩放,使得整个屏幕的坐标范围变成(-width/2,-height/2 ~ width/2,height/2)
	CHECK_GL_ERROR(启用1D纹理,glEnable(GL_TEXTURE_1D))
	CHECK_GL_ERROR(启用2D纹理,glEnable(GL_TEXTURE_2D))
	CHECK_GL_ERROR(启用混合,glEnable(GL_BLEND))
	//CHECK_GL_ERROR(启用深度测试,glEnable(GL_DEPTH_TEST))
	CHECK_GL_ERROR(设置混合函数,glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA))//设置混合功能对透明度的处理
	CHECK_GL_ERROR(启用客户端状态-顶点数组,glEnableClientState(GL_VERTEX_ARRAY))
	CHECK_GL_ERROR(启用客户端状态-纹理坐标数组,glEnableClientState(GL_TEXTURE_COORD_ARRAY))
	//CHECK_GL_ERROR(启用客户端状态-法线数组,glEnableClientState(GL_NORMAL_ARRAY))
	//开始事件循环
	game->restart();
	atexit(whenExit);
	WINAPI_CODE(WinAPI::newThis();)
	glutMainLoop();
	//永远不会执行到这里,可以考虑用glutLeaveMainLoop(),但某些老版本可能没有此函数
	//网上说有种方案是在调用glutMainLoop()前执行下面一行代码
	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	return 0;
}