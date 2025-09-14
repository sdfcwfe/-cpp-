/*
	1.�����µ���Ŀ
	2.���ͼ�ο�
	3.ʵ���ʼ����Ϸ����
	4.ʵ����Ϸ�����Ĺ�����
	5.ʵ�ֹ������Ŀ���
*/
#define _CRT_SECURE_NO_WARNINGS  // ������������������ܲ���Ч
#include<stdio.h>
#include<graphics.h>//����ͼ�ο�ͷ�ļ�
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include"tools.h" 

#include <mmsyscom.h>
#pragma comment(lib, "winmm.lib")

#define WIN_WIDTH 900 //��Ϸ���ڿ��
#define WIN_HEIGHT 600 //��Ϸ���ڸ߶�

enum {WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT};

IMAGE imgBg; //��ʾ����ͼƬ
IMAGE imgBar; //��ʾ��Ϸ�е�������
IMAGE imgCards[ZHI_WU_COUNT];
IMAGE* imgZhiWu[ZHI_WU_COUNT][20]; //��ʾֲ��ͼƬ



int curX, curY; //��ʾ��ǰ��������
int curZhiWu = 0; //��ʾ��ǰѡ�е�ֲ�� 0δѡ��,1�㶹����,2���տ�

struct zhiwu{
	int type; //0:û��ֲ��;1:��ֲ��;
	int frameIndex; //��ǰ���ŵ��ڼ�֡
};

struct zhiwu map[3][9]; //��ʾ��Ϸ�е�ֲ��

struct sunshineBall {
	int x, y; //��������� 
	int frameIndex; //��ǰ���ŵ��ڼ�֡
	int destY; //��������Ŀ��y����
	int used; //�Ƿ�ʹ�� 0:δʹ�� 1:��ʹ�� 
	//int speed; //����������ٶȣ�����/֡��
	//int timer; //�������ɵ�ʱ��
	DWORD landTime; // ��������¼������ص�ʱ�䣨���룩
}; 
bool isSunshineActive = false; // ��ʼΪfalse����ʾû�л�Ծ������
//10��������(ѭ������)
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29]; //����ͼƬ
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
	//������Ϸ����
	loadimage(&imgBg, _T("res/bg.jpg")); //���ر���ͼƬ
	loadimage(&imgBar, _T("res/bar5.png")); //���ؾ�����ͼƬ

	memset(imgZhiWu, 0, sizeof(imgZhiWu)); //��ֲ��ͼƬ��������
	memset(map, 0, sizeof(map)); //��ֲ���ͼ����

	//��ʼ��ֲ�￨��
	char name[64];
	for (int i =  0;i < ZHI_WU_COUNT; i++)
	{
		//����ֲ�￨�Ƶ��ļ���
		wchar_t wname[64];
		// 2. ����խ�ַ�·����ԭ�߼����䣩
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		// 3. ת��խ�ַ������ַ�����Ҫ���� Windows.h��
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);
		// 4. ������ַ�·��
		loadimage(&imgCards[i], wname);

		for(int j = 0; j < 20; j++){
			sprintf_s(name, sizeof(name), "res/ZhiWu/%d/%d.png", i , j+1);
			MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);
			//imgZhiWu[i][j] = new IMAGE;
			//�ж��ļ��Ƿ���� 
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], wname);
			}else{
				imgZhiWu[i][j] = NULL;
				break; //�ļ������ڣ�����ѭ��
			}
			
		}
	}
	curZhiWu = 0; //��ʼ����ǰѡ�е�ֲ��Ϊ0
	sunshine = 50; //��ʼ��������Ϊ50
	
	memset(balls, 0, sizeof(balls)); //����������������
	// �޸�˼·��loadimage������Ҫ�ڶ�������ΪLPCTSTR���ͣ���TCHAR*��������Ĵ��봫�����char*���͡�
	// �����������name��char���飩ת��Ϊwchar_t�����ַ����飩�������롣
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		wchar_t wname[64];
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);
		loadimage(&imgSunshineBall[i], wname);
	}

	//�����������
	srand(time(NULL));

	//������Ϸ����:
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1); //����800*600�Ĵ��ڣ���ʾ����̨����ֹ�رպ���С��
	
	// set font��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	//strcpy(f.lfFaceName, "Segee UI Black");
	// �����������Ƶ�խ����ת��
	const char* fontName = "Segoe UI Black"; // ����������ƴд��Segoe ��΢�����޳������壩
	int wideCharLen = MultiByteToWideChar(CP_ACP, 0, fontName, -1, NULL, 0);
	wchar_t wideFontName[LF_FACESIZE];
	MultiByteToWideChar(CP_ACP, 0, fontName, -1, wideFontName, wideCharLen);
	wcscpy(f.lfFaceName, wideFontName);

	f.lfQuality = ANTIALIASED_QUALITY; //�����Ч��
	settextstyle(&f);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);

	//�ر�ͼ�δ���
	//closegraph();
}

void updateWindow()
{
	BeginBatchDraw(); //��ʼ������ͼ����ֹ��˸(˫����
	//���Ʊ���ͼƬ
	putimage(0, 0, &imgBg); //������ͼƬ���Ƶ����ڵ�(0,0)λ��
	putimagePNG(250, 0, &imgBar); //��������ͼƬ���Ƶ����ڵײ�

	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		int x = 338 + i * 65; //����ÿ�ſ��Ƶ�x����
		int y = 6; //���Ƶ�y����̶�Ϊ6
		putimage(x, y, &imgCards[i]); //��ֲ�￨�ƻ��Ƶ���������
		//putimagePNG(50 + i * 80, 10, &imgCards[i]); //��ֲ�￨�ƻ��Ƶ���������
	}

	//��Ⱦ�϶�����
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
	// 1. ������ַ������賤��
	int wideLen = MultiByteToWideChar(CP_ACP, 0, scoreText, -1, NULL, 0);
	// 2. ������ַ���������������+1ȷ�����ɽ�������
	wchar_t wideScoreText[8];
	// 3. ת��խ�ַ��������ַ���
	MultiByteToWideChar(CP_ACP, 0, scoreText, -1, wideScoreText, wideLen);
	outtextxy(276, 67, wideScoreText);

	//ˢ��ͼ�δ���
	//FlushBatchDraw(); //ˢ��ͼ�δ��ڣ���ʾ���л��Ƶ�����
	EndBatchDraw(); //����������ͼ����ʾ���л��Ƶ�����
	
	//sleep(20); //��ʱ20���룬����֡��
}

// �޸� collectSunshine �����е� mciSendString ����
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
				// �ؼ��޸������ⱻ�ռ��󣬱�ǡ��޻�Ծ���⡱
				isSunshineActive = false;
				// �޸�����խ�ַ�ת��Ϊ���ַ�
				wchar_t cmd[64];
				MultiByteToWideChar(CP_ACP, 0, "play res/sunshine.mp3", -1, cmd, 64);
				mciSendString(cmd, 0, 0, 0);
			}
		}
	}
}

//�û��ĵ���¼�
void userClick(){
	ExMessage msg; //ר�������洢ͼ�δ����з�������Ϣ
	static int status = 0; //������ʾ��ǰ��״̬��0��ʾû�е���κο��ƣ�1��ʾ�����ĳ�ſ���

	//peekmessage(); //���������Ϣ
	if(peekmessage(&msg)){
		if (msg.message == WM_LBUTTONDOWN) { //�������
			if(msg.x> 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y > 6 && msg.y < 96){
				int index = (msg.x - 338) / 65; //�����û�������ǵڼ��ſ���
				status = 1; //��ʾ�û������ĳ�ſ���
				curZhiWu = index + 1; //��¼��ǰѡ�е�ֲ��
			}
			else {
				collectSunshine(&msg);
			}//or catch sunshine
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) { //����ƶ�
			curX = msg.x;
			curY = msg.y;

		}
		else if (msg.message == WM_LBUTTONUP) { //������̧��
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102; //�����û��ɿ����ʱ��ֲ��Ӧ����ֲ����һ��
				int col = (msg.x - 256) / 81; //�����û��ɿ����ʱ��ֲ��Ӧ����ֲ����һ��

				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex;

				}
				//status = 0; // ���á��϶�״̬��
				//curZhiWu = 0; // ���á�ѡ��ֲ��������ɸ�Ϊ���ж���ֲ�������á�
			}
			
		}
		else if( msg.message == WM_RBUTTONDOWN) { //�Ҽ�����
			status = 0; //����״̬
			curZhiWu = 0; //���õ�ǰѡ�е�ֲ��
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

		//����������������һ��δʹ�õ�������
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 260);
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90; //
		balls[i].landTime = 0; // ��ʼ�����ʱ��Ϊ0

		// ��ǡ��л�Ծ���⡱
		isSunshineActive = true;
	}

}

void updateSunshine() {
	// ��̬������ֻ��ʼ��һ�Σ���¼��һ�θ���λ�õ�ʱ��
	static DWORD lastSunMoveTime = 0;
	// ���Ƹ���Ƶ�ʣ�ÿ100�������һ��λ�ã��ɵ�����ֵԽ���ٶ�Խ����
	const int SUN_MOVE_INTERVAL = 100;
	const int STAY_TIME = 2000; // ������غ�ͣ��2000����
	// 2. ���� now ����ֵ����ȡ��ǰϵͳʱ�䣨���뼶��
	DWORD now = GetTickCount();

	int ballMax = sizeof(balls) / sizeof(balls[0]); // ��������Ԫ�ص��ܸ���
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			//����������Ķ���
			if (balls[i].used) {
				if (now - lastSunMoveTime >= 50) {
					balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
				}
				//balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
				// 2. λ�ø��£�����ʱ��������100����ʱ���ƶ�
				if (now - lastSunMoveTime >= SUN_MOVE_INTERVAL) {
					balls[i].y += 4; // �����ɱ���2�������Ƶ�ʽ��ͣ�ʵ���ٶȱ���
				}

				if(balls[i].y >= balls[i].destY) {
					// ��һ�����ʱ����¼��ǰʱ�䣨ֻ��¼һ�Σ�
					if ((balls[i].landTime == 0)) {
						balls[i].y = balls[i].destY; // �̶���Ŀ��λ��
						balls[i].landTime = now; // ��¼���ʱ��
					}

					// ������ͣ����ʱ�䣺��ǰʱ�� - ���ʱ��
					if (now - balls[i].landTime >= STAY_TIME) {
						balls[i].used = false; // ͣ����5�����ʧ
						isSunshineActive = false; // ����������һ������
					}
				}
			}
			
		}
		// ���¡���һ��λ�ø���ʱ�䡱�������ƶ����Ÿ��£������δ�����
		if (now - lastSunMoveTime >= SUN_MOVE_INTERVAL) {
			lastSunMoveTime = now;
		}
	}

}

//�ı���Ϸ��״̬
void updateGame(){
	// 1. ��̬������¼��һ�θ��¶�����ʱ�䣨ֻ��ʼ��һ�Σ�
	static DWORD lastAnimTime = 0;
	// 2. ��ǰʱ�䣨���룩
	DWORD now = GetTickCount();
	// 3. �������¼����100���루�ɵ�����ֵԽ�󶯻�Խ����
	const int ANIM_INTERVAL = 100;


	// ֻ�е�ʱ��������100���룬�Ÿ��¶���֡
	if (now - lastAnimTime >= ANIM_INTERVAL) {
		// ����ֲ��Ķ�����ԭ�߼����䣬ֻ�ǰ���ʱ���ж��
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 9; j++) {
				if (map[i][j].type > 0) {
					int zhiwutype = map[i][j].type - 1;
					// �޸���ԭ�����жϵ��ǡ�����ǰ��֡��������ĳɡ����º��֡��
					map[i][j].frameIndex++;
					// �����һ֡�Ƿ���ڣ�������������Ϊ0
					if (imgZhiWu[zhiwutype][map[i][j].frameIndex] == NULL) {
						map[i][j].frameIndex = 0;
					}
				}
			}
		}
		// 4. ���¡���һ�ζ���ʱ�䡱��Ϊ�´��ж���׼��
		lastAnimTime = now;
	}
	createSunshine();//��������iu
	updateSunshine(); //��������״̬
}
void startUI(){
	IMAGE imgBg,imgMenu1, imgMenu2;
	loadimage(&imgBg, _T("res/menu.png"));
	loadimage(&imgMenu1, _T("res/menu1.png"));
	loadimage(&imgMenu2, _T("res/menu2.png"));

	int flag = 0;

	while (1){
		BeginBatchDraw(); //��ʼ������ͼ����ֹ��˸(˫����
		putimage(0, 0, &imgBg); //������ͼƬ���Ƶ����ڵ�(0,0)λ��
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		EndBatchDraw(); //����������ͼ����ʾ���л��Ƶ����� 

		ExMessage msg; //ר�������洢ͼ�δ����з�������Ϣ
		if(peekmessage(&msg)){
			if (msg.message == WM_MOUSEMOVE) { //����ƶ�
				if (msg.x > 474 && msg.x < 474 + 300 && msg.y > 75 && msg.y < 75 + 100) {
					flag = 1;
				}
				else {
					flag = 0;
				}
			}
			else if (msg.message == WM_LBUTTONDOWN) { //�������
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

		// �������ö�ʱ������֡�ʣ�16�����60FPS�����������룩
		static DWORD lastFrameTime = 0;
		DWORD now = GetTickCount();
		if (now - lastFrameTime < 16) {  // ȷ��ÿ֡���ټ��16����
			Sleep(now - lastFrameTime);  // ֻ�ڡ�̫֡�족ʱ��΢��ʱ������������
		}
		lastFrameTime = now;

		
	}
	

	system("pause"); //�ȴ��û��������
	return 0;
}
