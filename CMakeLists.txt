cmake_minimum_required(VERSION 2.8)
include(../CompileScripts/CMakeLists.txt)

#指定存放源码和头文件的所有路径(游戏工程文件可能会include此文件)
set(GAMECLIENT_DIR
	. #当前目录
	Zipper Zipper/FileStructs #压缩模块,负责解压
	Image Image/FileStructs Image/ColorSpaces #图像模块,主要用于图像解码和处理
	Renderer #渲染模块,主要用于控制显存和绘制
	Font #字体处理模块,文字可以当作一种特殊的图像
	Charset #字符集,处理字符转换过程
	Timer #计时器,主要用于控制物体动作
	Sound #声音处理模块
	Network #网络模块
	Widgets #控件集,类似于各种平台特有的控件
	Dialogs #对话框
	Scenes #场景,提供公共的常见场景(如选择文件场景)
	Layers #图层,提供公共功能的图层(如游戏中的对话)
)

macro(includeEngine)
	set(INCLUDE_DIR_ONLY ON)
	include(../libGamesEngines/CMakeLists.txt)
endmacro()

if(GAME_NAME)
	#编译执行程序或库
	if(COMPILE_GAME_EXE)
		message(编译执行程序)
		project(GamesExe)
		#设置共用库,给不同平台的可执行文件用
		set(LIBS_LIST ${GAME_NAME}Client GamesClient ${GAME_NAME} GamesEngines ${LIBS_LIST} z pthread)#把编译好的引擎添加到库中
		#GLUT库
		set(GLUT_NAME glut)
		set(LIBS_LIST ${LIBS_LIST} ${GLUT_NAME})
		#linux相关依赖
		set(LIBS_LIST ${LIBS_LIST} GLU GL gtk-3 gobject-2.0)
		#以下为各种可执行文件的编译过程,根据情况选择编译自己需要的可执行文件即可
		add_executable(GamesGLUT ./executable/GamesGLUT.cpp)
		target_link_libraries(GamesGLUT ${LIBS_LIST})
		#更新器
		add_executable(updater ./executable/updater.cpp)
		target_link_libraries(updater ${LIBS_LIST})
	else()
		message(编译客户端游戏库${GAME_NAME})
		project(${GAME_NAME}Client)
		include(../lib${GAME_NAME}/CMakeLists.txt)#读取游戏的CMakeLists.txt
		srcIncludeAll(../lib${GAME_NAME}/Client/ ${CLIENT_DIR})
		includeAll(./ ${GAMECLIENT_DIR})
		includeEngine()
		foreach(name ${CMAKE_INCLUDE_PATH})
			message(....${name})
		endforeach()
		#库输出
		add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
		target_link_libraries(${PROJECT_NAME} GamesClient ${GAME_NAME} GamesEngines ${LIBS_LIST})
	endif()
else()
	message(编译客户端库)
	project(GamesClient)
	#添加所有paths下的源码和头文件
	srcIncludeAll(./ ${GAMECLIENT_DIR})
	#包含依赖库的目录
	includeEngine()
	include_directories(
		/usr/include/gtk-3.0
		/usr/include/atk-1.0
		/usr/include/glib-2.0
		/usr/include/pango-1.0
		/usr/include/harfbuzz
		/usr/include/cairo
		/usr/include/gdk-pixbuf-2.0/
		/usr/lib/x86_64-linux-gnu/glib-2.0/include)
	#库输出
	add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
	target_link_libraries(${PROJECT_NAME} GamesEngines ${LIBS_LIST})
	#执行程序输出
	#add_executable(tools/PngJson ./executable/PngJson.cpp)
	#target_link_libraries(tools/PngJson GamesClient GamesEngines GLU GL gtk-3)

	#add_executable(tools/Mp3Viewer ./executable/Mp3Viewer.cpp)
	#target_link_libraries(tools/Mp3Viewer GamesClient GamesEngines GLU GL gtk-3)
endif()