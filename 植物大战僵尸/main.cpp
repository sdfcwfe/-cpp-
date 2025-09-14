/*
	1.创建新的项目
	2.添加图形库
	3.实现最开始的游戏场景
	4.实现游戏顶部的工具栏
	5.实现工具栏的卡牌
*/
#define _CRT_SECURE_NO_WARNINGS  // 必须放在最顶部，否则可能不生效
#include<stdio.h>
#include<graphics.h>//引用图形库头文件
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include"tools.h" 

#include <mmsyscom.h>
#pragma comment(lib, "winmm.lib")

#define WIN_WIDTH 900 //游戏窗口宽度
#define WIN_HEIGHT 600 //游戏窗口高度

enum {WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT};

IMAGE imgBg; //表示背景图片
IMAGE imgBar; //表示游戏中的任务栏
IMAGE imgCards[ZHI_WU_COUNT];
IMAGE* imgZhiWu[ZHI_WU_COUNT][20]; //表示植物图片



int curX, curY; //表示当前鼠标的坐标
int curZhiWu = 0; //表示当前选中的植物 0未选中,1豌豆射手,2向日葵

struct zhiwu{
	int type; //0:没有植物;1:有植物;
	int frameIndex; //当前播放到第几帧
};

struct zhiwu map[3][9]; //表示游戏中的植物

struct sunshineBall {
	int x, y; //阳光的坐标 
	int frameIndex; //当前播放到第几帧
	int destY; //阳光掉落的目标y坐标
	int used; //是否被使用 0:未使用 1:已使用 
	//int speed; //阳光下落的速度（像素/帧）
	//int timer; //阳光生成的时间
	DWORD landTime; // 新增：记录阳光落地的时间（毫秒）
}; 
bool isSunshineActive = false; // 初始为false，表示没有活跃的阳光
//10个阳光球(循环出现)
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29]; //阳光图片
int sunshine;

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if(fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

void gameInit()
{
	//加载游戏背景
	loadimage(&imgBg, _T("res/bg.jpg")); //加载背景图片
	loadimage(&imgBar, _T("res/bar5.png")); //加载矩形条图片

	memset(imgZhiWu, 0, sizeof(imgZhiWu)); //将植物图片数组清零
	memset(map, 0, sizeof(map)); //将植物地图清零

	//初始化植物卡牌
	char name[64];
	for (int i =  0;i < ZHI_WU_COUNT; i++)
	{
		//生成植物卡牌的文件名
		wchar_t wname[64];
		// 2. 生成窄字符路径（原逻辑不变）
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		// 3. 转换窄字符到宽字符（需要包含 Windows.h）
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);
		// 4. 传入宽字符路径
		loadimage(&imgCards[i], wname);

		for(int j = 0; j < 20; j++){
			sprintf_s(name, sizeof(name), "res/ZhiWu/%d/%d.png", i , j+1);
			MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);
			//imgZhiWu[i][j] = new IMAGE;
			//判断文件是否存在 
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], wname);
			}else{
				imgZhiWu[i][j] = NULL;
				break; //文件不存在，跳出循环
			}
			
		}
	}
	curZhiWu = 0; //初始化当前选中的植物为0
	sunshine = 50; //初始化阳光数为50
	
	memset(balls, 0, sizeof(balls)); //将阳光球数组清零
	// 修复思路：loadimage函数需要第二个参数为LPCTSTR类型（即TCHAR*），而你的代码传入的是char*类型。
	// 解决方法：将name（char数组）转换为wchar_t（宽字符数组），并传入。
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		wchar_t wname[64];
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);
		loadimage(&imgSunshineBall[i], wname);
	}

	//配置随机种子
	srand(time(NULL));

	//创建游戏窗口:
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1); //创建800*600的窗口，显示控制台，禁止关闭和最小化
	
	// set font设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	//strcpy(f.lfFaceName, "Segee UI Black");
	// 处理字体名称的窄→宽转换
	const char* fontName = "Segoe UI Black"; // 修正字体名拼写（Segoe 是微软常用无衬线字体）
	int wideCharLen = MultiByteToWideChar(CP_ACP, 0, fontName, -1, NULL, 0);
	wchar_t wideFontName[LF_FACESIZE];
	MultiByteToWideChar(CP_ACP, 0, fontName, -1, wideFontName, wideCharLen);
	wcscpy(f.lfFaceName, wideFontName);

	f.lfQuality = ANTIALIASED_QUALITY; //抗锯齿效果
	settextstyle(&f);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);

	//关闭图形窗口
	//closegraph();
}

void updateWindow()
{
	BeginBatchDraw(); //开始批量绘图，防止闪烁(双缓冲
	//绘制背景图片
	putimage(0, 0, &imgBg); //将背景图片绘制到窗口的(0,0)位置
	putimagePNG(250, 0, &imgBar); //将任务栏图片绘制到窗口底部

	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		int x = 338 + i * 65; //计算每张卡牌的x坐标
		int y = 6; //卡牌的y坐标固定为6
		putimage(x, y, &imgCards[i]); //将植物卡牌绘制到任务栏上
		//putimagePNG(50 + i * 80, 10, &imgCards[i]); //将植物卡牌绘制到任务栏上
	}

	//渲染拖动窗口
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth()/2, curY -img->getheight()/2, img);
		//putimagePNG(curX , curY , img);
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 179 + i * 102+10;
				int zhiwutype = map[i][j].type-1;
				int index = map[i][j].frameIndex;
				//putimagePNG(curX , curY , img);
				if (imgZhiWu[zhiwutype][index] != NULL) {
					putimagePNG(x, y, imgZhiWu[zhiwutype][index]);
				}
			}
		}
	}

	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for(int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x, balls[i].y, img);
		}
	}

	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	// 1. 计算宽字符串所需长度
	int wideLen = MultiByteToWideChar(CP_ACP, 0, scoreText, -1, NULL, 0);
	// 2. 定义宽字符串缓冲区（长度+1确保容纳结束符）
	wchar_t wideScoreText[8];
	// 3. 转换窄字符串到宽字符串
	MultiByteToWideChar(CP_ACP, 0, scoreText, -1, wideScoreText, wideLen);
	outtextxy(276, 67, wideScoreText);

	//刷新图形窗口
	//FlushBatchDraw(); //刷新图形窗口，显示所有绘制的内容
	EndBatchDraw(); //结束批量绘图，显示所有绘制的内容
	
	//sleep(20); //延时20毫秒，控制帧率
}

// 修复 collectSunshine 函数中的 mciSendString 调用
void collectSunshine(ExMessage* msg) {
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used) {
			int x = balls[i].x;
			int y = balls[i].y;
			if (msg->x > x && msg->x < x + w &&
				msg->y > y && msg->y < y + h) {
				balls[i].used = false;
				sunshine += 25;
				// 关键修复：阳光被收集后，标记“无活跃阳光”
				isSunshineActive = false;
				// 修复：将窄字符转换为宽字符
				wchar_t cmd[64];
				MultiByteToWideChar(CP_ACP, 0, "play res/sunshine.mp3", -1, cmd, 64);
				mciSendString(cmd, 0, 0, 0);
			}
		}
	}
}

//用户的点击事件
void userClick(){
	ExMessage msg; //专门用来存储图形窗口中发生的消息
	static int status = 0; //用来表示当前的状态，0表示没有点击任何卡牌，1表示点击了某张卡牌

	//peekmessage(); //处理鼠标消息
	if(peekmessage(&msg)){
		if (msg.message == WM_LBUTTONDOWN) { //左键按下
			if(msg.x> 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y > 6 && msg.y < 96){
				int index = (msg.x - 338) / 65; //计算用户点击的是第几张卡牌
				status = 1; //表示用户点击了某张卡牌
				curZhiWu = index + 1; //记录当前选中的植物
			}
			else {
				collectSunshine(&msg);
			}//or catch sunshine
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) { //鼠标移动
			curX = msg.x;
			curY = msg.y;

		}
		else if (msg.message == WM_LBUTTONUP) { //鼠标左键抬起
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102; //计算用户松开鼠标时，植物应该种植在哪一行
				int col = (msg.x - 256) / 81; //计算用户松开鼠标时，植物应该种植在哪一列

				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex;

				}
				//status = 0; // 重置“拖动状态”
				//curZhiWu = 0; // 重置“选中植物”（后续可改为“判断种植后再重置”
			}
			
		}
		else if( msg.message == WM_RBUTTONDOWN) { //右键按下
			status = 0; //重置状态
			curZhiWu = 0; //重置当前选中的植物
		}
	}
	
}

void createSunshine() {
	static int count = 0;
	static int frc = 400;
	count++;
	if (count >= frc && !isSunshineActive) {
		frc = 200 + rand() % 200;
		count = 0;

		//从阳光球数组中找一个未使用的阳光球
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 260);
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90; //
		balls[i].landTime = 0; // 初始化落地时间为0

		// 标记“有活跃阳光”
		isSunshineActive = true;
	}

}

void updateSunshine() {
	// 静态变量：只初始化一次，记录上一次更新位置的时间
	static DWORD lastSunMoveTime = 0;
	// 控制更新频率：每100毫秒更新一次位置（可调整，值越大速度越慢）
	const int SUN_MOVE_INTERVAL = 100;
	const int STAY_TIME = 2000; // 阳光落地后停留2000毫秒
	// 2. 定义 now 并赋值：获取当前系统时间（毫秒级）
	DWORD now = GetTickCount();

	int ballMax = sizeof(balls) / sizeof(balls[0]); // 是数组中元素的总个数
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			//更新阳光球的动画
			if (balls[i].used) {
				if (now - lastSunMoveTime >= 50) {
					balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
				}
				//balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
				// 2. 位置更新：仅当时间间隔超过100毫秒时才移动
				if (now - lastSunMoveTime >= SUN_MOVE_INTERVAL) {
					balls[i].y += 4; // 步长可保留2，因更新频率降低，实际速度变慢
				}

				if(balls[i].y >= balls[i].destY) {
					// 第一次落地时，记录当前时间（只记录一次）
					if ((balls[i].landTime == 0)) {
						balls[i].y = balls[i].destY; // 固定在目标位置
						balls[i].landTime = now; // 记录落地时间
					}

					// 计算已停留的时间：当前时间 - 落地时间
					if (now - balls[i].landTime >= STAY_TIME) {
						balls[i].used = false; // 停留够5秒后消失
						isSunshineActive = false; // 允许生成下一个阳光
					}
				}
			}
			
		}
		// 更新“上一次位置更新时间”（仅当移动过才更新，避免多次触发）
		if (now - lastSunMoveTime >= SUN_MOVE_INTERVAL) {
			lastSunMoveTime = now;
		}
	}

}

//改变游戏的状态
void updateGame(){
	// 1. 静态变量记录上一次更新动画的时间（只初始化一次）
	static DWORD lastAnimTime = 0;
	// 2. 当前时间（毫秒）
	DWORD now = GetTickCount();
	// 3. 动画更新间隔：100毫秒（可调整，值越大动画越慢）
	const int ANIM_INTERVAL = 100;


	// 只有当时间间隔超过100毫秒，才更新动画帧
	if (now - lastAnimTime >= ANIM_INTERVAL) {
		// 更新植物的动画（原逻辑不变，只是包在时间判断里）
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 9; j++) {
				if (map[i][j].type > 0) {
					int zhiwutype = map[i][j].type - 1;
					// 修复：原代码判断的是“更新前的帧”，这里改成“更新后的帧”
					map[i][j].frameIndex++;
					// 检查下一帧是否存在，不存在则重置为0
					if (imgZhiWu[zhiwutype][map[i][j].frameIndex] == NULL) {
						map[i][j].frameIndex = 0;
					}
				}
			}
		}
		// 4. 更新“上一次动画时间”，为下次判断做准备
		lastAnimTime = now;
	}
	createSunshine();//创建阳光iu
	updateSunshine(); //更新阳光状态
}
void startUI(){
	IMAGE imgBg,imgMenu1, imgMenu2;
	loadimage(&imgBg, _T("res/menu.png"));
	loadimage(&imgMenu1, _T("res/menu1.png"));
	loadimage(&imgMenu2, _T("res/menu2.png"));

	int flag = 0;

	while (1){
		BeginBatchDraw(); //开始批量绘图，防止闪烁(双缓冲
		putimage(0, 0, &imgBg); //将背景图片绘制到窗口的(0,0)位置
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		EndBatchDraw(); //结束批量绘图，显示所有绘制的内容 

		ExMessage msg; //专门用来存储图形窗口中发生的消息
		if(peekmessage(&msg)){
			if (msg.message == WM_MOUSEMOVE) { //鼠标移动
				if (msg.x > 474 && msg.x < 474 + 300 && msg.y > 75 && msg.y < 75 + 100) {
					flag = 1;
				}
				else {
					flag = 0;
				}
			}
			else if (msg.message == WM_LBUTTONDOWN) { //左键按下
				if (msg.x > 474 && msg.x < 474 + 300 && msg.y > 75 && msg.y < 75 + 100) {
					break;
				}
			}
		}

	}
}

int main(){
	gameInit();

	startUI();

	int timer = 0;
	int flag = true;

	while (1) {
		userClick();
		timer += getDelay();

		updateGame();
		updateWindow();

		// 新增：用定时器控制帧率（16毫秒≈60FPS，不阻塞输入）
		static DWORD lastFrameTime = 0;
		DWORD now = GetTickCount();
		if (now - lastFrameTime < 16) {  // 确保每帧至少间隔16毫秒
			Sleep(now - lastFrameTime);  // 只在“帧太快”时轻微延时，不阻塞输入
		}
		lastFrameTime = now;

		
	}
	

	system("pause"); //等待用户按任意键
	return 0;
}
