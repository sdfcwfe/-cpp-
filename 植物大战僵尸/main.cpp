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
#include <math.h>
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
	bool catched; //�Ƿ񱻽�ʬ��
	int deadTime; //ֲ���Ѫ��
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

	float xoff;
	float yoff;
}; 
bool isSunshineActive = false; // ��ʼΪfalse����ʾû�л�Ծ������
//10��������(ѭ������)
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29]; //����ͼƬ
int sunshine;

//zombie
struct zm {
	int x, y; //��ʬ������
	int type; //��ʬ������ 0:û�н�ʬ 1:��ͨ��ʬ 
	int  walkFrame;//��ʬ��·��֡��
	int eatFrame;//��ʬ��ֲ���֡��
	int blood; //��ʬ��Ѫ��
	int speed; //��ʬ���ƶ��ٶȣ�����/֡��
	bool used; //�Ƿ�ʹ�� 0:δʹ�� 1:��ʹ��
	int row; //��ʬ���ڵ���
	bool dead; // ��ʬ�Ƿ�dead

	bool eating; //��ʬ�Ƿ��ڳ�ֲ��
};
struct zm zms[10]; //10����ʬ
IMAGE imgZM[22]; //��ʬͼƬ
IMAGE imgZMDead[20]; 
IMAGE imgZMEat[21];//��ʬ��ֲ��ͼƬ

// �ӵ������ݽṹ��
struct bullet {
	int x, y; //�ӵ�������
	int speed; //�ӵ����ƶ��ٶȣ�����/֡��
	int row; //�ӵ����ڵ���
	bool used; //�Ƿ�ʹ�� 0:δʹ�� 1:��ʹ��
	bool blast; //�Ƿ��䱬ը;
	int frameIndex;
};
struct bullet bullets[30]; //30���ӵ�
IMAGE imgBulletNormal; //�ӵ�����ͼƬ
IMAGE imgBullBlast[4];//��ը

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

	//���ؽ�ʬͼƬ
	memset(zms, 0, sizeof(zms)); //����ʬ��������
	for(int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/ZM/%d.png", i + 1);
		wchar_t wname[64];
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);//important
		loadimage(&imgZM[i], wname);
	}

	loadimage(&imgBulletNormal, _T("res/bullets/bullet_normal.png")); //�����ӵ�ͼƬ
	memset(bullets, 0, sizeof(bullets)); //���ӵ���������

	//��ʼ���㶹�ӵ�֡ͼƬ����
	loadimage(&imgBullBlast[3], _T("res/bullets/bullet_blast.png"));
	for (int i = 0; i < 3; i++) {
		float k = (i+1) * 0.2;
		loadimage(&imgBullBlast[i], _T("res/bullets/bullet_blast.png"),
			imgBullBlast[3].getwidth() * k,
			imgBullBlast[3].getheight() * k,true);
	}

	//���ؽ�����ͼƬ
	for (int i = 0; i < 20; i++) {
		//ʹ��char�ַ�����_t()���ַ���
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		wchar_t wname[64];// ת�ɿ��ַ�
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);//importamt
		loadimage(&imgZMDead[i], wname);
	}

	//���ؽ�ʬ��ֲ��ͼƬ
	for(int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		wchar_t wname[64];
		MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 64);//important
		loadimage(&imgZMEat[i], wname);
	}
	//�ر�ͼ�δ���
	//closegraph();
}

void drawZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);
	for(int i = 0; i < zmMax; i++) {
		if (zms[i].used) {
			
			IMAGE* imgArray = NULL;
			int frameIndex = 0;

			if (zms[i].dead) {
				imgArray = imgZMDead;//��ʬ����
				frameIndex = zms[i].walkFrame % 20; // ����֡

			}
			else if (zms[i].eating) {
				imgArray = imgZMEat;//��ʬ��ֲ��
				frameIndex = zms[i].eatFrame % 21; // ��ʳ֡������������
			}
			else {
				imgArray = imgZM;//��ͨ��ʬ
				frameIndex = zms[i].walkFrame % 22;  // ��·֡
			}


			/*int maxFrames = zms[i].dead ? 20 : 22;
			int frameIndex = zms[i].frameIndex % maxFrames;*/

			/*putimagePNG(
				zms[i].x, 
				zms[i].y-img->getheight(),
				img
			);*/
			putimagePNG(
				zms[i].x,
				zms[i].y - imgArray[frameIndex].getheight(),
				&imgArray[frameIndex]  // ��ȷ��ȡͼƬ��ַ
			);


			/*int x = zms[i].x;
			int y = zms[i].y;
			int type = zms[i].type;
			int index = zms[i].frameIndex;*/
			//putimagePNG(curX , curY , img);
			//if (type == 1) { //��ͨ��ʬ
			//	if (imgZM[index] != NULL) {
			//		putimagePNG(x, y, &imgZM[index]);
			//	}
			//}else if (type == 2) { //·�Ͻ�ʬ
			//	if (imgZM[11 + index] != NULL) {
			//		putimagePNG(x, y, &imgZM[11 + index]);
			//	}
			//}
		}
	}
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


	//zombie����
	drawZM();

	//bullet����
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for(int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {
				IMAGE* img = &imgBullBlast[bullets[i].frameIndex];
				putimagePNG(bullets[i].x, bullets[i].y, img);
			}
			else {
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
				
			
		}
	}
	 

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
				//��������ƫ����
				float destY = 0;
				float destX = 0;
				float angle = atan((y - destY) / (x - destX));
				balls[i].xoff = 4 * cos(angle);
				balls[i].yoff = 4 * sin(angle);
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
		balls[i].xoff = 0;
		balls[i].yoff = 0;


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
		else if (balls[i].xoff) {
			balls[i].x -= balls[i].xoff;
			balls[i].y -= balls[i].yoff;
			if (balls[i].y < 0 || balls[i].y) {
				balls[i].used = false; // �ƶ�����Ļ����ʧ
				balls[i].xoff = 0; // ����ƫ����
				isSunshineActive = false; // ����������һ������
			}
		}
		
		// ���¡���һ��λ�ø���ʱ�䡱�������ƶ����Ÿ��£������δ�����
		if (now - lastSunMoveTime >= SUN_MOVE_INTERVAL) {
			lastSunMoveTime = now;
		}
	}

}

void createZM(){
	//static DWORD lastZmTime = 0;
	static int zmFre = 100; //��ʬ���ɵ�Ƶ��
	static int count = 0;
	count++;
	if (count >= zmFre) {
		count = 0; 
		zmFre = 150 + rand() % 150;
		if (rand() % 100 < 50) { //2%�ĸ������ɽ�ʬ
			int i;
			int zmMax = sizeof(zms) / sizeof(zms[0]);
			for (i = 0; i < zmMax && zms[i].used; i++);
			if (i < zmMax) {
				memset(&zms[i], 0, sizeof(zms[i]));//����

				zms[i].used = true;
				//zms[i].type = 1; //��ͨ��ʬ

				// ��ʼ������Ķ���֡����ʹ����walkFrame/eatFrame��
				zms[i].walkFrame = 0;  // ����֡
				zms[i].eatFrame = 0;   // ��ʳ֡

				zms[i].blood = 60;
				zms[i].x = WIN_WIDTH;
				zms[i].row = rand() % 3; //0-2//���������0-3
				//int rows = rand() % 3 + 1;
				zms[i].y = 172 + (1 + zms[i].row) * 100; 
				//zms[i].hp = 100;
				zms[i].speed = 1 + rand() % 2; //1-3����/֡

			}
		}
		
	}
}

void updateZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);//����
	static DWORD lastMoveTime = 0;
	//static int count = 0;    
	const int MOVE_INTERVAL = 100; // ÿ 100ms ����һ��
	DWORD now = GetTickCount();

	//���½�ʬλ��
	if (now - lastMoveTime >= MOVE_INTERVAL) {
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if(zms[i].dead) {
					//���Ž�ʬ��������
					if (zms[i].walkFrame < 20) {
						zms[i].walkFrame++;
					}
					else {
						zms[i].used = false; //���������꣬��ʬ��ʧ
						zms[i].dead = false;
					}
					continue; //���������ƶ��Ͷ�������
				}
				/*else if(zms[i].eating){
					zms[i].eatFrame = (zms[i].eatFrame + 1) % 21;

				}*/
				else {
					zms[i].x -= zms[i].speed; // �ٶȱ��� 1-2 ����
					zms[i].walkFrame = (zms[i].walkFrame + 1) % 22;

					if (zms[i].x < 170) {
						MessageBox(NULL, _T("Game Over!"), _T("��ʾ"), MB_OK);
						exit(0);//��Ϸ����
					}
				}
			}
		}
		lastMoveTime = now;
	}
	//for(int i = 0; i < zmMax; i++) {
	//	if (zms[i].used) {
	//		zms[i].x -= zms[i].speed;
	//		if (zms[i].x < 170) {
	//			//zms[i].used = false;
	//			printf("Game Over!\n");
	//			MessageBox(NULL, _T("Game Over!"), _T("��ʾ"), MB_OK);
	//			exit(0);
	//		}
	//		else {
	//			zms[i].frameIndex = (zms[i].frameIndex + 1) % 22;
	//			putimagePNG(zms[i].x, zms[i].y, &imgZM[zms[i].frameIndex]);
	//		}
	//	}
	//} 
}

void shoot() {
	int lines[3] = { 0 };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int dangerX = WIN_WIDTH - imgZM[0].getwidth();
	int bulletCount = sizeof(bullets) / sizeof(bullets[0]);

	for(int i = 0; i < zmCount; i++) {
		//for(int j = 0; j < 9; j++) {
		if (zms[i].used && zms[i].x < dangerX) { //
			//�ȴ���ʬ are out;������ָ��λ��;
			lines[zms[i].row] = 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if(map[i][j].type == WAN_DOU + 1 && lines[i] ) {
				//��ֲ���Ҹ����н�ʬ
				static int count = 0;
				count++;
				if (count >= 500) { //ÿ20֡����һ���ӵ�
					count = 0; 
					int k;
					for (k = 0; k < bulletCount && bullets[k].used; k++);
					if (k < bulletCount) {
						bullets[k].used = true;
						bullets[k].x = 256 + j * 81 + imgZhiWu[0][0]->getwidth();
						bullets[k].y = 179 + i * 102 + 50;
						bullets[k].speed = 1; //5����/֡
						bullets[k].row = i; 
						//explode
						bullets[k].blast = false;
						bullets[k].frameIndex = 0;

						int zwX = 256 + j * 81;
						int zwY = 179 + i * 102 + 14;
						bullets[k].x = zwX + imgZhiWu[map[i][j].type - 1][0]->getwidth() - 10;
						bullets[k].y = zwY + 5; 
					}
				}
			}
		}
	}
}

void updateBullets() {
	int countMax = sizeof(bullets) / sizeof(bullets[0]);
	for(int i = 0; i < countMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			// putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}
			//else {
			//	//����ӵ��Ƿ���н�ʬ
			//	for(int j = 0; j < sizeof(zms) / sizeof(zms[0]); j++) {
			//		if (zms[j].used && zms[j].row == bullets[i].row) {
			//			//ͬһ��
			//			int zmX = zms[j].x;
			//			int zmY = zms[j].y - imgZM[0].getheight();
			//			int zmW = imgZM[0].getwidth();
			//			int zmH = imgZM[0].getheight();
			//			int bulletX = bullets[i].x;
			//			int bulletY = bullets[i].y;
			//			int bulletW = imgBulletNormal.getwidth();
			//			int bulletH = imgBulletNormal.getheight();
			//			if (bulletX + bulletW > zmX && bulletX < zmX + zmW &&
			//				bulletY + bulletH > zmY && bulletY < zmY + zmH) {
			//				//��ײ���
			//				bullets[i].used = false; //�ӵ���ʧ
			//				zms[j].used = false; //��ʬ��ʧ
			//				break; //������ʬѭ��
			//			}
			//		}
			//	}
			//}
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex >= 4) {
					bullets[i].used = false;
				}
			}
		}
	}
}

void checkBullet2Zm() {
	int bCount = sizeof(bullets) / sizeof(bullets[0]);
	int zCount = sizeof(zms) / sizeof(zms[0]);

	for (int i = 0; i < bCount; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;

		for (int k = 0; k < zCount; k++) {
			if (zms[k].used == false) continue;
			int x1 = zms[k].x + 50;
			int x2 = zms[k].x + 100;
			int x = bullets[i].x;
			if (zms[k].dead == false && bullets[i].row == zms[k].row && x > x1 && x < x2) {
				zms[k].blood -= 20;
				bullets[i].blast = true;
				bullets[i].speed = 0;
				bullets[i].frameIndex = 0;  // ���ñ�ը֡

				if (zms[k].blood <= 0) {
					// zms[k].used = false; �������̱��,Ҫ������������
					zms[k].speed = 0;
					zms[k].dead = true;
					zms[k].walkFrame = 0;   // ����֡����
					zms[k].eatFrame = 0;    // ��ʳ֡���ã���ʹ���˶���������
				}
				break; // �ӵ��ѻ��У�������ʬѭ��
			}
		}
	}
}

void checkZm2ZhiWu() {
	int zCount = sizeof(zms) / sizeof(zms[0]);
	// 1. ���峣����ͳһ���ƿ�ʳƵ�ʺ�ֲ��Ѫ��
	const int EAT_FRAME_THRESHOLD = 200;  // ��ʳ�����300
	const int PLANT_MAX_HP = 200;      // ֲ����Ѫ����300
	const int EAT_DAMAGE = 100;          // ÿ�ο�ʳ�˺���100
	const int EAT_ANIM_INTERVAL = 100; // ÿ100����
	static DWORD lastEatAnimTime[10] = { 0 }; // ÿ����ʬ�����Ŀ�ʳ������ʱ����10����ʬ��λ��
	for (int i = 0; i < zCount; i++) {
		if (zms[i].dead) continue;

		int row = zms[i].row;
		bool isEatingAnyPlant = false;  // ��ǵ�ǰ��ʬCHIֲ��
		DWORD now = GetTickCount();
		for(int j = 0; j < 9; j++) {
			if (map[row][j].type == 0) continue;
			
			int zhiWuX = 256 + j * 81; //ֲ�����Ͻ�x����
			//ԭ��
			int x1 = zhiWuX + 10;//ֲ����ײ���������߽�
			int x2 = zhiWuX + 60;//��
			int zmX = zms[i].x + 80; //��ʬ��ײ���������߽�
			//�Ż���
			//int zhiWuW = imgZhiWu[map[row][j].type - 1][0]->getwidth();  // ֲ��ʵ�ʿ��
			//int x1 = zhiWuX;                     // ֲ����߽�
			//int x2 = zhiWuX + zhiWuW;            // ֲ���ұ߽�
			//int zmX = zms[i].x;                  // ��ʬ��߽�
			if(zmX  > x1 && zmX < x2) { //+ imgZM[0].getwidth()
				isEatingAnyPlant = true;  // ��ǽ�ʬ���ڿ�ʳ
				zms[i].eating = true;
				zms[i].speed = 0; // ֹͣ�ƶ�

				if(map[row][j].catched) {
					// 2. ���ƿ�ʳ���������ٶȣ���ʱ�������£���������ѭ����
					//zms[i].eatFrame++;
					if(now - lastEatAnimTime[i] >= EAT_ANIM_INTERVAL) {
						zms[i].eatFrame = (zms[i].eatFrame + 1) % 21; // 21֡����ѭ��
                        lastEatAnimTime[i] = now; // ���¼�ʱ��
					}

					if(zms[i].eatFrame >= EAT_FRAME_THRESHOLD) {
						zms[i].eatFrame = 0;
						map[row][j].deadTime += EAT_DAMAGE; //ÿ�γ�ֲ�ֲ��Ѫ������100
						if (map[row][j].deadTime >= PLANT_MAX_HP) { //ֲ��Ѫ��Ϊ500
							map[row][j].deadTime = 0;
							map[row][j].type = 0; //ֲ������
							map[row][j].catched = false;

							zms[i].eating = false;
							zms[i].speed = 1;// + rand() % 2; //��ʬ�ָ��ƶ�
							lastEatAnimTime[i] = 0; // ���øý�ʬ�Ķ�����ʱ��
							// zms[i].frameIndex = 0; //���ý�ʬ���߶���
						}
					}
				}
				else {
					map[row][j].catched = true;
					map[row][j].deadTime = 0; //ֲ��Ѫ��
					zms[i].eating = true;
					zms[i].speed = 0; //��ʬֹͣ�ƶ�
					zms[i].eatFrame = 0; //���ý�ʬ��ֲ�ﶯ��
					lastEatAnimTime[i] = now; // ��ʼ��������ʱ��
				}
				break; // ����ֲ��ѭ���������ظ����ͬһ��ʬ
			} 
		}
		// �����ǰ��ʬû�п�ʳ�κ�ֲ���֮ǰ���ڿ�ʳ״̬����ָ�����״̬
		if (!isEatingAnyPlant && zms[i].eating) {
			zms[i].eating = false;
			zms[i].speed = 1 + rand() % 2;
			zms[i].walkFrame = 0;
			lastEatAnimTime[i] = 0; // ���ü�ʱ��
		}
	}
}

void collisionCheck() {
	checkBullet2Zm();// �ӵ�dui��ʬ��ײ���
	checkZm2ZhiWu(); // ��ʬ��ֲ����ײ���
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

	createZM(); //������ʬ
	updateZM();//���½�ʬ״̬

	shoot();//���
	updateBullets();//�����ӵ�״̬

	collisionCheck(); //�㶹�ӵ���ײ���

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
