#include <easyx.h>
#include <stdio.h>
#include <conio.h> 
#include <time.h>
#include <mmsystem.h> //播放音乐的头文件
#pragma  comment(lib,"winmm.lib") //加载静态库

#define MAX_LEN 1000 //蛇的最大长度
#define	WINDOW_WIDTH 860
#define WINDOW_HEIGHT 484
#define SIZE 20 // 基准单位 很重要 作为食物的大小以及蛇每一节的大小 蛇移动的速度必须以基准单位 否则会出现蛇身压缩
enum DIR {UP,DOWN,LEFT,RIGHT}; // 枚举类型 四个方向
int score = 0;
IMAGE bgimg;
//表示蛇的每一个节点
struct Node {
	int x, y;
	DWORD color;
};
struct Snake{
	int len, dir; //dir表示方向 len表示蛇的节点数
	struct Node nodes[MAX_LEN];
}snake;
//食物结构体
struct Food {
	int x, y, flag;
	DWORD color;
}food;
void gameInit() {
	//初始化游戏窗口
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&bgimg, "res/bg.jpg");
	//蛇的初始化
	srand((unsigned int)time(NULL));
	snake.len = 3; //默认初始化为三个节点
	snake.dir = RIGHT;
	for (int i = 0; i < snake.len; i++) {
		snake.nodes[i].x = i* SIZE;
		snake.nodes[i].y = 0;
		snake.nodes[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
	}
	//食物初始化
	food.flag = 1;
	food.x = rand() % (WINDOW_WIDTH/SIZE) * SIZE;
	food.y = rand()% (WINDOW_HEIGHT / SIZE) * SIZE;
	food.color = RGB(255,255,255);
	//播放背景音乐
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	
}
void updateWindow() {
	//双缓冲 防止闪屏
	BeginBatchDraw();
	putimage(0, 0, &bgimg);
	//绘制蛇
	for (int i = 0; i < snake.len; i++)
	{
		setfillcolor(snake.nodes[i].color);
		fillrectangle(snake.nodes[i].x, snake.nodes[i].y, snake.nodes[i].x + SIZE, snake.nodes[i].y + SIZE);
	}
	//绘制食物
	if (food.flag == 1) {
		setfillcolor(food.color);
		solidellipse(food.x, food.y, food.x + SIZE, food.y + SIZE);
	}
	//显示分数
	setbkmode(TRANSPARENT); //设置文字背景透明
	char str[100];
	sprintf(str, "分数:%d", score);
	settextstyle(40, 0, _T("微软雅黑"));
	outtextxy(10, 10, str);
	EndBatchDraw();
}
void changeDir() {
	//性能优化：使用Async异步捕获输入 比_getch()快很多 可以让蛇更丝滑
	//往一个方向移动时不可以反向 防止蛇可以吃自己
	if (GetAsyncKeyState(VK_UP) && snake.dir != DOWN) snake.dir = UP;  
	else if (GetAsyncKeyState(VK_LEFT) && snake.dir != RIGHT) snake.dir = LEFT;  
	else if (GetAsyncKeyState(VK_DOWN) && snake.dir != UP) snake.dir = DOWN;  
	else if (GetAsyncKeyState(VK_RIGHT)&& snake.dir != LEFT) snake.dir = RIGHT;  
}
void snakeMove() {
	//蛇移动的关键步骤 将头节点往后的所有节点都赋值为前一个节点的坐标 注：将蛇头定义为下标为0的节点
	for (int i = snake.len - 1; i > 0; i--) {
		snake.nodes[i].x = snake.nodes[i - 1].x;
		snake.nodes[i].y = snake.nodes[i - 1].y;
	}
	//只需要让头节点移动就行 后面的跟上去
	switch (snake.dir)
	{
	case UP:
		snake.nodes[0].y -= SIZE;
		break;
	case LEFT:
		snake.nodes[0].x -= SIZE;
		break;
	case DOWN:
		snake.nodes[0].y += SIZE;
		break;
	case RIGHT:
		snake.nodes[0].x += SIZE;
		break;
	}
}
void eatFood() {
	//判断是否吃到了食物 前面有没有按照SIZE进行移动将影响这里
	if (food.flag && snake.nodes[0].x == food.x && snake.nodes[0].y == food.y) {
		PlaySound("res/eat.wav", 0, SND_FILENAME|SND_ASYNC); //异步播放音效
		food.flag = 0;
		snake.len++;
		//让新节点立马跟上 不然新节点会在左上角闪一下
		snake.nodes[snake.len-1].x = snake.nodes[snake.len - 1 - 1].x;
		snake.nodes[snake.len - 1].y = snake.nodes[snake.len - 1 - 1].y;
		snake.nodes[snake.len-1].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		score += 10;
		//重新初始化食物
		food.flag = 1;
		food.x = rand() % (WINDOW_WIDTH / SIZE) * SIZE;
		food.y = rand() % (WINDOW_HEIGHT / SIZE) * SIZE;
		//记得给食物设置颜色 不然颜色都一样
		food.color = RGB(255, 255, 255);
	}
}
void checkOver() {
	//粗略地判断蛇是否越界 此处可作更严谨的判断 但是这个判断基本够用了
	if (snake.nodes[0].x > WINDOW_WIDTH || snake.nodes[0].x < 0 || snake.nodes[0].y<0 || snake.nodes[0].y>WINDOW_HEIGHT) {
		PlaySound("res/over.wav", 0, SND_FILENAME | SND_ASYNC); //播放失败音效
		Sleep(1000);
		loadimage(0, "res/over.png");
		//保存历史最高分成绩
		FILE* fp = NULL;
		int last = 0;
		fopen_s(&fp, "score.txt", "r");
		fscanf(fp, "%d", &last);
		fclose(fp);
		if (score > last) {
			fopen_s(&fp, "score.txt", "w");
			fprintf(fp, "%d", score);
			last = score;
			fclose(fp);
		}
		//显示分数
		setbkmode(TRANSPARENT);
		char str1[100], str2[100];
		sprintf(str1, "你的分数为:%d", score);
		sprintf(str2, "历史最高分:%d", last);
		settextstyle(40, 0, _T("微软雅黑"));
		outtextxy((WINDOW_WIDTH - textwidth(str1)) / 2, 350, str1);
		outtextxy((WINDOW_WIDTH - textwidth(str2)) / 2, 270, str2);
		system("pause");
		//重置分数  蛇的状态
		score = 0;
		snake.len = 3;
		snake.dir = RIGHT; 
		for (int i = 0; i < snake.len; i++) {
			snake.nodes[i].x = i * SIZE;
			snake.nodes[i].y = 0;
			snake.nodes[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		}
	}
}
int main(void)
{
	//初始化
	gameInit();
	//游戏主循环
	while (1) {
		snakeMove();
		eatFood();
		checkOver();
		if (_kbhit()) {
			changeDir();
		}
		Sleep(50); //控制速度的关键参数 50就够了
		updateWindow();
	}
	return 0;
}