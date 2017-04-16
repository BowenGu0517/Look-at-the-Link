/********************************
*	   项目名称：WOW连连看		*
*		文件名称：Main.cpp		*
*	 作者：stormG by 九夏师父		*
*		 编译环境：VS2015		*
*		  当前版本：V1.0			*
*********************************/

#include <graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "resource.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")


#ifndef PI
#define PI 3.14159265
#endif 

//宏定义
#define WINDOW_WIDTH 900	//宽度的一半必须可以被PIC_SIZE整除
#define WINDOW_HEIGHT 600   //高度的一半必须可以被PIC_SIZE整除
#define PIC_SIZE 50
#define NUM 10
#define FINISH_CODE 1
#define INTERRUPT_CODE -1
#define ERROR_CODE_MODE_FAILURE -2


//图片数组结构体
typedef struct graph
{
	int mode=1;
	int **initArray=NULL;
	int col=0;
	int row=0;
	int initX = 0;
	int initY = 0;
}GP;

//全局变量
IMAGE img[NUM], bkimg, selectimg[6], bklayerImg[3], transImg[2],finishImg[3],finalImg[2];
bool isMode1Pass = false;
bool isMode2Pass = false;
bool isMode3Pass = false;
volatile bool isMouseClicked = false;
//bool isMode1Pass = true;
//bool isMode2Pass = true;
//bool isMode3Pass = true;

//函数声明
void initLoad();
void welcome();
void initSelect();
void initTransition();
void transition(GP* gIn);
GP* selectMode();
GP* initDraw(GP *gIn);
void reDraw(GP* gIn);
int play(GP* gIn);
bool isFinished(GP* gIn);
void afterFinish(GP* gIn);
void buttonTrans();
void finalScene();

//线程函数
DWORD CALLBACK threadMouse(LPVOID lParam)
{
	MOUSEMSG msg = { 0 };
	while (true)
	{
		msg = GetMouseMsg();
		switch (msg.uMsg)
		{
		case WM_LBUTTONDOWN:
		{
			isMouseClicked = true;
			return 0;
		}
		break;

		default:
			break;
		}
	}
	return 0;
}


//主函数
int main()
{
	srand((unsigned)time(NULL));
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	HWND hWnd = GetHWnd();
	SetWindowText(hWnd, L"欢饮来到 <<WOW连连看>> 作者：stormG by 九夏师父");
	//-----------------函数调用-----------------//
	initLoad();
	welcome();
	initTransition();

	do
	{
		initSelect();
		if (isMode1Pass&&isMode2Pass&&isMode3Pass)
		{
			MessageBox(NULL, L"恭喜你，勇士，所有副本均已通关！\n敬请期待后续版本", L"ALL completed", MB_SYSTEMMODAL);
			break;
		}
		GP* gAll = selectMode();
		gAll = initDraw(gAll);
		int backCode = play(gAll);
	}   while (true);
	finalScene();
	//-----------------------------------------//
	getchar();
	closegraph();
	return 0;
}

//-----------------子函数定义-----------------//
//加载图片
void initLoad()
{
	loadimage(&bkimg, L"JPG", MAKEINTRESOURCE(IDR_BKIMG), WINDOW_WIDTH, WINDOW_HEIGHT);
	
	for (int i = 0; i < 6; i++)
	{
		loadimage(&selectimg[i], L"JPG", MAKEINTRESOURCE(IDR_RAGNAROS_0 + i));
	}

	for (int i = 0; i < NUM; i++)
	{
		loadimage(&img[i], L"JPG", MAKEINTRESOURCE(IDB_WARRIOR+i), PIC_SIZE, PIC_SIZE);
	}

	for (int i = 0; i < 3; i++)
	{
		loadimage(&bklayerImg[i], L"JPG", MAKEINTRESOURCE(IDR_GRASS + i), WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	for (int i = 0; i < 2; i++)
	{
		loadimage(&transImg[i], L"JPG", MAKEINTRESOURCE(IDR_TRANS0 + i), 100, 100);
	}

	for (int i = 0; i < 2; i++)
	{
		loadimage(&finalImg[i], L"JPG", MAKEINTRESOURCE(IDR_FINAL0+i), WINDOW_WIDTH, WINDOW_HEIGHT);
	}
}

//载入界面
void welcome()
{
	putimage(0, 0, &bkimg);
	settextcolor(RGB(0, 100, 255));
	settextstyle(100, 50, L"华文彩云");
	setbkmode(TRANSPARENT);
	outtextxy(30, 100, L"《WOW连连看》");
	settextstyle(40, 20, L"黑体");
	settextcolor(RGB(200, 200, 0));
	outtextxy(820, 545, L"V1.0");
	settextstyle(50, 30, L"华文琥珀");
	settextcolor(RGB(0, 200, 255));
	outtextxy(125, 480, L"请点击任意键以继续");
	Sleep(500);

	HANDLE hThreadMouse = CreateThread(NULL, 0, threadMouse, NULL, 0, 0);
	CloseHandle(hThreadMouse);

	clock_t nowTime = clock();
	clock_t prevTime = clock();
	bool isFlash = false;
	while (true)
	{
		nowTime = clock();
		if (nowTime - prevTime >= 500)
		{
			if (isFlash)
			{
				settextcolor(RGB(rand() % 255, rand() % 255, 255));
				outtextxy(125, 480, L"请点击任意键以继续");
				prevTime = nowTime;
			}
			else
			{
				putimage(0, 0, &bkimg);
				settextcolor(RGB(0, 100, 255));
				settextstyle(100, 50, L"华文彩云");
				setbkmode(TRANSPARENT);
				outtextxy(30, 100, L"《WOW连连看》");
				settextstyle(40, 20, L"黑体");
				settextcolor(RGB(200, 200, 0));
				outtextxy(820, 545, L"V1.0");
				settextstyle(50, 30, L"华文琥珀");
				prevTime = nowTime;
			}
			isFlash = !isFlash;
		}

		if (isMouseClicked)
		{
			break;
		}
	}
}

//难度选择界过渡
void initTransition()
{
	float ratio = 0.0f;
	for (int i = 0; i < 500; i++)
	{
		ratio = (float)i / 500;
		BeginBatchDraw();
		cleardevice();
		setaspectratio(ratio,ratio);
		putimage(0, 0, &selectimg[0]);
		putimage(WINDOW_WIDTH / 2, 0, &selectimg[2]);
		putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
		setaspectratio(1, 1);
		EndBatchDraw();
	}
}

//难度选择界面贴图
void initSelect()
{
	if (isMode1Pass)
	{
		putimage(0, 0, &selectimg[1]);
	}
	else
	{
		putimage(0, 0, &selectimg[0]);
	}

	if (isMode2Pass)
	{
		putimage(WINDOW_WIDTH / 2, 0, &selectimg[3]);
	}
	else
	{
		putimage(WINDOW_WIDTH / 2, 0, &selectimg[2]);
	}

	if (isMode3Pass)
	{
		putimage(0, WINDOW_HEIGHT / 2, &selectimg[5]);
	}
	else
	{
		putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
	}

	settextcolor(RGB(200, 200, 0));
	settextstyle(40, 20, L"华文琥珀");
	setbkmode(TRANSPARENT);
	if (isMode1Pass)
	{
		outtextxy(10, 10, L"普通难度(已通关)");
	}
	else
	{
		outtextxy(10, 10, L"普通难度");
	}


	settextcolor(RGB(0, 0, 255));
	settextstyle(40, 20, L"华文琥珀");
	setbkmode(TRANSPARENT);

	if (isMode2Pass)
	{
		outtextxy(545, 10, L"英雄难度(已通关)");
	}
	else
	{
		outtextxy(705, 10, L"英雄难度");
	}

	settextcolor(RGB(255, 0, 0));
	settextstyle(40, 20, L"华文琥珀");
	setbkmode(TRANSPARENT);
	if (isMode3Pass)
	{
		outtextxy(10,550, L"史诗难度(已通关)");
	}
	else
	{
		outtextxy(10,550, L"史诗难度");
	}
}

//选择相应难度后的过渡界面
void transition(GP* gIn)
{
	switch (gIn->mode)
	{
	case 1:
	{
		float ratio = 0.0f;
		int maxiNum = 200;
		if (!isMode1Pass)
		{
			for (int i = 0; i < maxiNum; i++)
			{
				BeginBatchDraw();
				cleardevice();
				if (isMode2Pass)
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[3]);
				}
				else
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[2]);
				}
				if (isMode3Pass)
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[5]);
				}
				else
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
				}
				ratio = (float)i / maxiNum;
				setaspectratio(1.0f - ratio, 1.0f);
				putimage(0, 0, &selectimg[0]);
				EndBatchDraw();
				setaspectratio(1, 1);
				Sleep(10);
			}
			Sleep(100);
			for (int i = 0; i < maxiNum; i++)
			{
				BeginBatchDraw();
				cleardevice();
				ratio = (float)(i + 1) / maxiNum;
				setaspectratio(ratio, 1.0f);
				putimage(0, 0, &selectimg[1]);
				setaspectratio(1, 1);
				if (isMode2Pass)
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[3]);
				}
				else
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[2]);
				}
				if (isMode3Pass)
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[5]);
				}
				else
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
				}
				EndBatchDraw();
				Sleep(10);
			}
			Sleep(1000);
		}

	}
		break;

	case 2:
	{
		float ratio = 0.0f;
		int maxiNum = 200;
		if (!isMode2Pass)
		{
			for (int i = 0; i < maxiNum; i++)
			{
				BeginBatchDraw();
				cleardevice();
				if (isMode1Pass)
				{
					putimage(0, 0, &selectimg[1]);
				}
				else
				{
					putimage(0, 0, &selectimg[0]);
				}

				if (isMode3Pass)
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[5]);
				}
				else
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
				}
				ratio = (float)(i + 1) / maxiNum;
				putimage(WINDOW_WIDTH / 2 + (int)(ratio*WINDOW_WIDTH), 0, &selectimg[2]);
				EndBatchDraw();
				Sleep(10);
			}
			Sleep(100);
			for (int i = 0; i < maxiNum; i++)
			{
				BeginBatchDraw();
				cleardevice();
				if (isMode1Pass)
				{
					putimage(0, 0, &selectimg[1]);
				}
				else
				{
					putimage(0, 0, &selectimg[0]);
				}

				if (isMode3Pass)
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[5]);
				}
				else
				{
					putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
				}
				ratio = (float)(i + 1) / maxiNum;
				putimage(WINDOW_WIDTH - (int)(WINDOW_WIDTH / 2 * ratio), 0, &selectimg[3]);
				EndBatchDraw();
				Sleep(10);
			}
			Sleep(1000);
		}
	}
	break;

	case 3:
	{
		float ratio = 0.0f;
		int maxiNum = 200;
		if (!isMode3Pass)
		{
			for (int i = 0; i < maxiNum; i++)
			{
				BeginBatchDraw();
				cleardevice();
				if (isMode1Pass)
				{
					putimage(0, 0, &selectimg[1]);

				}
				else
				{
					putimage(0, 0, &selectimg[0]);
				}

				if (isMode2Pass)
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[3]);

				}
				else
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[2]);
				}
				ratio = (float)i / maxiNum;
				setaspectratio(1.0f - ratio, 1.0f);
				putimage(0, WINDOW_HEIGHT / 2, &selectimg[4]);
				EndBatchDraw();
				setaspectratio(1, 1);
				Sleep(5);
			}
			Sleep(100);
			for (int i = 0; i < maxiNum; i++)
			{
				BeginBatchDraw();
				cleardevice();
				ratio = (float)(i + 1) / maxiNum;
				setaspectratio(ratio, 1.0f);
				putimage(0, WINDOW_HEIGHT / 2, &selectimg[5]);
				setaspectratio(1, 1);
				if (isMode1Pass)
				{
					putimage(0, 0, &selectimg[1]);

				}
				else
				{
					putimage(0, 0, &selectimg[0]);
				}

				if (isMode2Pass)
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[3]);

				}
				else
				{
					putimage(WINDOW_WIDTH / 2, 0, &selectimg[2]);
				}
				EndBatchDraw();
				Sleep(5);
			}
			Sleep(1000);
		}

	}
	break;

	default:
		break;
	}
}

//难度选择界面
GP* selectMode()
{
	GP *newGraph = (GP*)malloc(sizeof(GP));
	MOUSEMSG msg = { 0 };
	int prevX=0;
	int prevY=0;
	//BeginBatchDraw();
	while (true)
	{
		msg = GetMouseMsg();
		if (msg.x >= 0 && msg.x < WINDOW_WIDTH / 2
			&& msg.y >= 0 && msg.y < WINDOW_HEIGHT / 2)
		{
			if (!(prevX >= 0 && prevX < WINDOW_WIDTH / 2
				&& prevY >= 0 && prevY < WINDOW_HEIGHT / 2))
			{
				initSelect();
				settextcolor(RGB(0, 0, 0));
				settextstyle(40, 20, L"华文琥珀");
				setbkmode(TRANSPARENT);
				if (isMode1Pass)
				{
					outtextxy(10, 10, L"普通难度(已通关)");
				}
				else
				{
					outtextxy(10, 10, L"普通难度");
				}
				setlinestyle(PS_SOLID, 10);
				setlinecolor(RGB(0, 0, 0));
				rectangle(5, 5, WINDOW_WIDTH/2 - 5,WINDOW_HEIGHT/2- 5);
			}
		}

		if (msg.x >= WINDOW_WIDTH / 2 && msg.x < WINDOW_WIDTH
			&& msg.y >= 0 && msg.y < WINDOW_HEIGHT / 2)
		{
			if (!(prevX >= WINDOW_WIDTH / 2 && prevX < WINDOW_WIDTH
				&& prevY >= 0 && prevY < WINDOW_HEIGHT / 2))
			{
				initSelect();
				settextcolor(RGB(0, 0, 0));
				settextstyle(40, 20, L"华文琥珀");
				setbkmode(TRANSPARENT);

				if (isMode2Pass)
				{
					outtextxy(545, 10, L"英雄难度(已通关)");
				}
				else
				{
					outtextxy(705, 10, L"英雄难度");
				}
				setlinestyle(PS_SOLID, 10);
				setlinecolor(RGB(0, 0, 0));
				rectangle(WINDOW_WIDTH / 2+5, 5, WINDOW_WIDTH - 5, WINDOW_HEIGHT/2 - 5);
			}
		}

		if (msg.x >=0 && msg.x < WINDOW_WIDTH
			&& msg.y >= WINDOW_HEIGHT / 2 && msg.y < WINDOW_HEIGHT)
		{
			if (!(prevX >= 0 && prevX < WINDOW_WIDTH
				&& prevY >= WINDOW_HEIGHT / 2 && prevY < WINDOW_HEIGHT))
			{
				initSelect();
				settextcolor(RGB(0, 0, 0));
				settextstyle(40, 20, L"华文琥珀");
				setbkmode(TRANSPARENT);
				if (isMode3Pass)
				{
					outtextxy(10, 550, L"史诗难度(已通关)");
				}
				else
				{
					outtextxy(10, 550, L"史诗难度");
				}
				setlinestyle(PS_SOLID, 10);
				setlinecolor(RGB(0, 0, 0));
				rectangle(5, WINDOW_HEIGHT / 2+5, WINDOW_WIDTH - 5, WINDOW_HEIGHT - 5);
			}
		}

		prevX = msg.x;
		prevY = msg.y;

		switch (msg.uMsg)
		{
		case WM_LBUTTONDOWN:
		{
			if (msg.x >= 0 && msg.x < WINDOW_WIDTH / 2
				&& msg.y >= 0 && msg.y < WINDOW_HEIGHT / 2)
			{
				newGraph->mode = 1;
				transition(newGraph);
				return newGraph;
			}
			if (msg.x >= WINDOW_WIDTH/2 && msg.x < WINDOW_WIDTH
				&& msg.y >= 0 && msg.y < WINDOW_HEIGHT / 2)
			{
				newGraph->mode = 2;
				transition(newGraph);
				return newGraph;

			}
			if (msg.x >= 0 && msg.x < WINDOW_WIDTH
				&& msg.y >= WINDOW_HEIGHT / 2 && msg.y < WINDOW_HEIGHT)
			{
				newGraph->mode = 3;
				transition(newGraph);
				return newGraph;
			}
		}
			break;

		default:
			break;
		}
	}
	return newGraph;
}

//随机生成当前难度的初始画面
GP* initDraw(GP* gIn)
{
	GP* newGraph = gIn;
	//GP* newGraph = (GP*)malloc(sizeof(GP));
	switch (newGraph->mode)
	{
	case 1:
		newGraph->row = 4;
		newGraph->col = 4;
		putimage(0, 0, &bklayerImg[0]);
		break;


	case 2:
		newGraph->row = 6;
		newGraph->col = 6;
		putimage(0, 0, &bklayerImg[1]);
		break;

	case 3:
		newGraph->row = 8;
		newGraph->col = 8;
		putimage(0, 0, &bklayerImg[2]);
		break;

	default:
		MessageBox(NULL, L"模式选择初始化错误！", L"MODE error", MB_OK);
		exit(ERROR_CODE_MODE_FAILURE);
	}
	newGraph->initX = WINDOW_WIDTH / 2 - newGraph->col / 2 * PIC_SIZE;
	newGraph->initY = WINDOW_HEIGHT / 2 - newGraph->row / 2 * PIC_SIZE;
	newGraph->initArray = (int**)malloc(newGraph->row * sizeof(int*));
	for (int r = 0; r < newGraph->row; r++)
	{
		newGraph->initArray[r] = (int*)malloc(newGraph->col * sizeof(int));
	}

	for (int r = 0; r < newGraph->row; r++)
	{
		for (int c = 0; c < newGraph->col; c++)
		{
			newGraph->initArray[r][c] = 0;
		}
	}

	BeginBatchDraw();
	for (int r = 0; r < newGraph->row; r++)
	{
		for (int c = 0; c < newGraph->col; c++)
		{
			if (newGraph->initArray[r][c] == 0)
			{
				int temp = rand() % NUM+1;
				putimage(newGraph->initX+c*PIC_SIZE, newGraph->initY+r*PIC_SIZE, &img[temp-1]);
				newGraph->initArray[r][c] = temp;

				int rr;
				int cc;
				do
				{
					 rr = rand() % newGraph->row;
					 cc = rand() % newGraph->col;
				} 
				while (newGraph->initArray[rr][cc] != 0);
				putimage(newGraph->initX + cc*PIC_SIZE, newGraph->initY + rr*PIC_SIZE, &img[temp-1]);
				newGraph->initArray[rr][cc] = temp;
			}
		}
	}
	EndBatchDraw();
	return newGraph;
}

//重绘
void reDraw(GP* gIn)
{
	GP* newGraph = gIn;
	BeginBatchDraw();
	switch (newGraph->mode)
	{
	case 1:
	{
		putimage(0, 0, &bklayerImg[0]);
		loadimage(&finishImg[0], L"JPG", MAKEINTRESOURCE(IDR_MALFURION), 4 * PIC_SIZE, 4 * PIC_SIZE);
		putimage(newGraph->initX, newGraph->initY, 4 * PIC_SIZE, 4 * PIC_SIZE, &finishImg[0], 0, 0, SRCCOPY);
	}
		break;

	case 2:
	{
		putimage(0, 0, &bklayerImg[1]);
		loadimage(&finishImg[1], L"JPG", MAKEINTRESOURCE(IDR_FORDRING), 6 * PIC_SIZE, 6 * PIC_SIZE);
		putimage(newGraph->initX, newGraph->initY, 6 * PIC_SIZE, 6 * PIC_SIZE, &finishImg[1], 0, 0, SRCCOPY);
	}
		break;

	case 3:
	{
		putimage(0, 0, &bklayerImg[2]);
		loadimage(&finishImg[2], L"JPG", MAKEINTRESOURCE(IDR_THROLL), 8 * PIC_SIZE, 8 * PIC_SIZE);
		putimage(newGraph->initX, newGraph->initY, 8 * PIC_SIZE, 8 * PIC_SIZE, &finishImg[2], 0, 0, SRCCOPY);
	}
		break;

	default:
		break;
	}

	for (int r = 0; r < newGraph->row; r++)
	{
		for (int c = 0; c < newGraph->col; c++)
		{
			if (newGraph->initArray[r][c] != 0)
			{
				putimage(newGraph->initX + c*PIC_SIZE, newGraph->initY + r*PIC_SIZE, &img[newGraph->initArray[r][c] - 1]);
			}
		}
	}
	EndBatchDraw();
}

//游戏进行逻辑判断
int play(GP* gIn)
{
	GP* newGraph = gIn;
	int currentX = gIn->initX / PIC_SIZE;
	int currentY = gIn->initY / PIC_SIZE;
	setlinestyle(PS_SOLID, 2);
	bool isFirstValid = false;
	int FirstX;
	int FirstY;
	int SecondX;
	int SecondY;
	MOUSEMSG msg = {0};
	while (1)
	{
		msg = GetMouseMsg();
		if (msg.x >= newGraph->initX && msg.x < newGraph->initX + newGraph->col*PIC_SIZE
			&& msg.y >= newGraph->initY && msg.y < newGraph->initY + newGraph->row*PIC_SIZE)
		{
			if (newGraph->initArray[msg.y / PIC_SIZE - newGraph->initY / PIC_SIZE][msg.x / PIC_SIZE - newGraph->initX / PIC_SIZE] != 0)
			{
				if (newGraph->initArray[currentY - newGraph->initY / PIC_SIZE][currentX - newGraph->initX / PIC_SIZE] != 0)
				{
					setlinecolor(RGB(255, 255, 255));
					rectangle(currentX*PIC_SIZE + 1, currentY*PIC_SIZE + 1, currentX*PIC_SIZE + PIC_SIZE - 1, currentY*PIC_SIZE + PIC_SIZE - 1);
				}
				currentX = msg.x / PIC_SIZE;
				currentY = msg.y / PIC_SIZE;
				setlinecolor(RGB(50, 50, 255));
				rectangle(currentX*PIC_SIZE + 1, currentY*PIC_SIZE + 1, currentX*PIC_SIZE + PIC_SIZE - 1, currentY*PIC_SIZE + PIC_SIZE - 1);
			}
		}
		else
		{
			if (newGraph->initArray[currentY - newGraph->initY / PIC_SIZE][currentX - newGraph->initX / PIC_SIZE] != 0)
			{
				setlinecolor(RGB(255, 255, 255));
				rectangle(currentX*PIC_SIZE + 1, currentY*PIC_SIZE + 1, currentX*PIC_SIZE + PIC_SIZE - 1, currentY*PIC_SIZE + PIC_SIZE - 1);
			}
		}

		if (isFirstValid)
		{
			setlinecolor(RGB(255, 50, 50));
			rectangle(FirstX*PIC_SIZE + 1, FirstY*PIC_SIZE + 1, FirstX*PIC_SIZE + PIC_SIZE - 1, FirstY*PIC_SIZE + PIC_SIZE - 1);
		}
		
		switch (msg.uMsg)
		{
		case WM_LBUTTONUP:
		{
			if (msg.x >= newGraph->initX && msg.x < newGraph->initX + newGraph->col*PIC_SIZE
				&& msg.y >= newGraph->initY && msg.y < newGraph->initY + newGraph->row*PIC_SIZE
				&& newGraph->initArray[msg.y / PIC_SIZE - newGraph->initY / PIC_SIZE][msg.x / PIC_SIZE - newGraph->initX / PIC_SIZE] != 0)
			{
				if (isFirstValid)
				{
					SecondX = msg.x / PIC_SIZE;
					SecondY = msg.y / PIC_SIZE;
					setlinecolor(RGB(255, 50, 50));
					rectangle(SecondX*PIC_SIZE + 1, SecondY*PIC_SIZE + 1, SecondX*PIC_SIZE + PIC_SIZE - 1, SecondY*PIC_SIZE + PIC_SIZE - 1);
					Sleep(200);
					if (!(SecondX == FirstX && SecondY == FirstY)
						&& (newGraph->initArray[FirstY - newGraph->initY / PIC_SIZE][FirstX - newGraph->initX / PIC_SIZE]== newGraph->initArray[SecondY - newGraph->initY / PIC_SIZE][SecondX - newGraph->initX / PIC_SIZE]))
					{
						newGraph->initArray[FirstY - newGraph->initY / PIC_SIZE][FirstX - newGraph->initX / PIC_SIZE] = 0;
						newGraph->initArray[SecondY - newGraph->initY / PIC_SIZE][SecondX - newGraph->initX / PIC_SIZE] = 0;
					}
					isFirstValid = false;
				}
				else
				{
					FirstX = msg.x / PIC_SIZE;
					FirstY = msg.y / PIC_SIZE;
					isFirstValid = true;
				}
				reDraw(newGraph);
			}
		}
		break;

		default:
			break;
		}

		if (isFinished(newGraph))
		{
			afterFinish(newGraph);
			free(newGraph);
			return FINISH_CODE;
			//MessageBox(NULL, L"finished", L"finished", MB_OK);
		}
	}
	return 0;
}

//判断是否完成当前难度
bool isFinished(GP* gIn)
{
	for (int r = 0; r < gIn->row; r++)
	{
		for (int c = 0; c < gIn->col; c++)
		{
			if (gIn->initArray[r][c] != 0)
			{
				return false;
			}
		}
	}
	return true;
}

//完成当前难度后的过渡动画
void afterFinish(GP* gIn)
{
	IMAGE imgTemp;
	switch (gIn->mode)
	{
	case 1:
	{
		for (int i = 0; i < 60; i++)
		{
			rotateimage(&imgTemp, &finishImg[0], PI / 6 * i);
			putimage(gIn->initX, gIn->initY, &imgTemp);
			Sleep(10);
		}
		putimage(gIn->initX, gIn->initY, 4 * PIC_SIZE, 4 * PIC_SIZE, &finishImg[0], 0, 0, SRCCOPY);
		Sleep(1000);

		int count =gIn->initX > gIn->initY ? gIn->initY : gIn->initX;
		int dis = 0;
		for (int i = 1; i <= count;i++)
		{
			IMAGE imgTemp1;
			loadimage(&imgTemp1, L"JPG",MAKEINTRESOURCE(IDR_MALFURION), 4 * PIC_SIZE + 2*i, 4 * PIC_SIZE + 2*i);
			putimage(gIn->initX-i, gIn->initY-i, &imgTemp1);
			dis = gIn->initX - i;
			Sleep(5);
		}
		Sleep(1000);
		IMAGE imgTemp2;
		loadimage(&imgTemp2, L"JPG", MAKEINTRESOURCE(IDR_MALFURION), WINDOW_HEIGHT, WINDOW_HEIGHT);
		for (int i = 1; i <= dis; i++)
		{	
			BeginBatchDraw();
			putimage(0, 0, &bklayerImg[0]);
			putimage(dis - i, 0, &imgTemp2);
			EndBatchDraw();
			Sleep(5);
		}
		Sleep(2000);

		wchar_t * p1 = L"保护大地是我的职责,你无法抵挡大自然的力量!";
		wchar_t * p2 = L"大德鲁伊┃玛法里奥·怒风";
		setlinestyle(0, 3, NULL, 0);
		settextstyle(40, 0, L"华文行楷");
		setbkmode(TRANSPARENT);
		int X = WINDOW_HEIGHT + 50;
		int Y = 20;
		int charNum = 0;
		settextcolor(RGB(40, 255, 0));
		while (p1[charNum] != L'\0')
		{
			outtextxy(X, Y, p1[charNum]);
			charNum++;
			Y += 40;
			if (Y > WINDOW_HEIGHT - 200)
			{
				Y = 20;
				X += 60;
			}
			Sleep(150);
		}

		Sleep(1000);
		setlinestyle(0, 3, NULL, 0);
		settextstyle(25, 0, L"华文行楷");
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255,140,0));
		X+= 60;
		Y = 115;
		charNum = 0;
		while (p2[charNum] != L'\0')
		{
			outtextxy(X, Y, p2[charNum]);
			charNum++;
			Y += 25;
			Sleep(100);
		}

		buttonTrans();
		isMode1Pass = true;
	}
		break;

	case 2:
	{
		for (int i = 0; i < 60; i++)
		{
			rotateimage(&imgTemp, &finishImg[1], PI / 6 * i);
			putimage(gIn->initX, gIn->initY, &imgTemp);
			Sleep(10);
		}
		putimage(gIn->initX, gIn->initY, 6 * PIC_SIZE, 6 * PIC_SIZE, &finishImg[1], 0, 0, SRCCOPY);
		Sleep(1000);

		int count = gIn->initX > gIn->initY ? gIn->initY : gIn->initX;
		int dis = 0;
		for (int i = 1; i <= count; i++)
		{
			IMAGE imgTemp1;
			loadimage(&imgTemp1, L"JPG", MAKEINTRESOURCE(IDR_FORDRING), 6 * PIC_SIZE + 2 * i, 6 * PIC_SIZE + 2 * i);
			putimage(gIn->initX - i, gIn->initY - i, &imgTemp1);
			dis = gIn->initX - i;
			Sleep(5);
		}
		Sleep(1000);
		IMAGE imgTemp2;
		loadimage(&imgTemp2, L"JPG", MAKEINTRESOURCE(IDR_FORDRING), WINDOW_HEIGHT, WINDOW_HEIGHT);
		for (int i = 1; i <= dis; i++)
		{
			BeginBatchDraw();
			putimage(0, 0, &bklayerImg[1]);
			putimage(dis - i, 0, &imgTemp2);
			EndBatchDraw();
			Sleep(5);
		}
		Sleep(2000);

		wchar_t * p1 = L"勇气会让你的灵魂比千颗太阳还要明亮!";
		wchar_t * p2 = L"大领主┃提里奥·弗丁";
		setlinestyle(0, 3, NULL, 0);
		settextstyle(40, 0, L"华文行楷");
		setbkmode(TRANSPARENT);
		int X = WINDOW_HEIGHT + 50;
		int Y = 20;
		int charNum = 0;
		settextcolor(RGB(230, 220, 0));
		while (p1[charNum] != L'\0')
		{
			outtextxy(X, Y, p1[charNum]);
			charNum++;
			Y += 40;
			if (Y > WINDOW_HEIGHT - 200)
			{
				Y = 20;
				X += 60;
			}
			Sleep(150);
		}

		Sleep(1000);
		setlinestyle(0, 3, NULL, 0);
		settextstyle(25, 0, L"华文行楷");
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 127, 174));
		X += 60;
		Y = 115;
		charNum = 0;
		while (p2[charNum] != L'\0')
		{
			outtextxy(X, Y, p2[charNum]);
			charNum++;
			Y += 25;
			Sleep(100);
		}

		buttonTrans();
		isMode2Pass = true;
	}
	break;

	case 3:
	{
		for (int i = 0; i < 60; i++)
		{
			//putimage(0, 0, &bklayerImg);
			rotateimage(&imgTemp, &finishImg[2], PI / 6 * i);
			putimage(gIn->initX, gIn->initY, &imgTemp);
			Sleep(10);
		}
		putimage(gIn->initX, gIn->initY, 8 * PIC_SIZE, 8 * PIC_SIZE, &finishImg[2], 0, 0, SRCCOPY);
		Sleep(1000);

		int count = gIn->initX > gIn->initY ? gIn->initY : gIn->initX;
		int dis = 0;
		for (int i = 1; i <= count; i++)
		{
			IMAGE imgTemp1;
			loadimage(&imgTemp1, L"JPG", MAKEINTRESOURCE(IDR_THROLL), 8 * PIC_SIZE + 2 * i, 8 * PIC_SIZE + 2 * i);
			putimage(gIn->initX - i, gIn->initY - i, &imgTemp1);
			dis = gIn->initX - i;
			Sleep(5);
		}
		Sleep(1000);

		IMAGE imgTemp2;
		loadimage(&imgTemp2, L"JPG", MAKEINTRESOURCE(IDR_THROLL), WINDOW_HEIGHT, WINDOW_HEIGHT);
		for (int i = 1; i <= dis; i++)
		{
			BeginBatchDraw();
			putimage(0, 0, &bklayerImg[2]);
			putimage(dis - i, 0, &imgTemp2);
			EndBatchDraw();
			Sleep(5);
		}
		Sleep(2000);

		wchar_t * p1 = L"为了部落!";
		wchar_t * p2 = L"大酋长┃世界守护者|萨尔·古伊尔";
		setlinestyle(0, 3, NULL, 0);
		settextstyle(40, 0, L"华文行楷");
		setbkmode(TRANSPARENT);
		int X = WINDOW_HEIGHT + 50;
		int Y = 20;
		int charNum = 0;
		settextcolor(RGB(190, 190, 225));
		while (p1[charNum] != L'\0')
		{
			outtextxy(X, Y, p1[charNum]);
			charNum++;
			Y += 40;
			if (Y > WINDOW_HEIGHT - 200)
			{
				Y = 20;
				X += 60;
			}
			Sleep(150);
		}

		Sleep(1000);
		setlinestyle(0, 3, NULL, 0);
		settextstyle(25, 0, L"华文行楷");
		setbkmode(TRANSPARENT);
		settextcolor(RGB(5, 0, 230));
		X += 60;
		Y = 115;
		charNum = 0;
		while (p2[charNum] != L'\0')
		{
			outtextxy(X, Y, p2[charNum]);
			charNum++;
			Y += 25;
			Sleep(100);
		}

		buttonTrans();
		isMode3Pass = true;
	}
	break;

	default:
		break;
	}
}

//按传送门返回难度选择界面
void buttonTrans()
{
	putimage(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, &transImg[0]);
	MOUSEMSG msg = { 0 };
	int prevX=0;
	int prevY=0;
	while (true)
	{
		msg = GetMouseMsg();
		if (msg.x >= WINDOW_WIDTH - 100 && msg.x < WINDOW_WIDTH
			&& msg.y >= WINDOW_HEIGHT - 100 && msg.y < WINDOW_HEIGHT)
		{
			if (!(prevX >= WINDOW_WIDTH - 100 && prevX < WINDOW_WIDTH
				&& prevY >= WINDOW_HEIGHT - 100 && prevY < WINDOW_HEIGHT))
			{
				putimage(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, &transImg[1]);
			}
		}
		else
		{
			if (prevX >= WINDOW_WIDTH - 100 && prevX < WINDOW_WIDTH
				&& prevY >= WINDOW_HEIGHT - 100 && prevY < WINDOW_HEIGHT)
			{
				putimage(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, &transImg[0]);
			}
		}
		prevX = msg.x;
		prevY = msg.y;

		switch (msg.uMsg)
		{
		case WM_LBUTTONDOWN:
		{
			if (msg.x >= WINDOW_WIDTH - 100 && msg.x < WINDOW_WIDTH
				&& msg.y >= WINDOW_HEIGHT - 100 && msg.y < WINDOW_HEIGHT)
			{
				return;
			}
		}
			break;

		default:
			break;
		}
	}
}

//结束画面
void finalScene()
{
	putimage(0, 0, &finalImg[0]);
	Sleep(2000);
	putimage(0, 0, &finalImg[1]);
}