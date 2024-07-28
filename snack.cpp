#include <easyx.h>
#include <stdio.h>
#include <conio.h> 
#include <time.h>
#include <mmsystem.h> //�������ֵ�ͷ�ļ�
#pragma  comment(lib,"winmm.lib") //���ؾ�̬��

#define MAX_LEN 1000 //�ߵ���󳤶�
#define	WINDOW_WIDTH 860
#define WINDOW_HEIGHT 484
#define SIZE 20 // ��׼��λ ����Ҫ ��Ϊʳ��Ĵ�С�Լ���ÿһ�ڵĴ�С ���ƶ����ٶȱ����Ի�׼��λ ������������ѹ��
enum DIR {UP,DOWN,LEFT,RIGHT}; // ö������ �ĸ�����
int score = 0;
IMAGE bgimg;
//��ʾ�ߵ�ÿһ���ڵ�
struct Node {
	int x, y;
	DWORD color;
};
struct Snake{
	int len, dir; //dir��ʾ���� len��ʾ�ߵĽڵ���
	struct Node nodes[MAX_LEN];
}snake;
//ʳ��ṹ��
struct Food {
	int x, y, flag;
	DWORD color;
}food;
void gameInit() {
	//��ʼ����Ϸ����
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&bgimg, "res/bg.jpg");
	//�ߵĳ�ʼ��
	srand((unsigned int)time(NULL));
	snake.len = 3; //Ĭ�ϳ�ʼ��Ϊ�����ڵ�
	snake.dir = RIGHT;
	for (int i = 0; i < snake.len; i++) {
		snake.nodes[i].x = i* SIZE;
		snake.nodes[i].y = 0;
		snake.nodes[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
	}
	//ʳ���ʼ��
	food.flag = 1;
	food.x = rand() % (WINDOW_WIDTH/SIZE) * SIZE;
	food.y = rand()% (WINDOW_HEIGHT / SIZE) * SIZE;
	food.color = RGB(255,255,255);
	//���ű�������
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	
}
void updateWindow() {
	//˫���� ��ֹ����
	BeginBatchDraw();
	putimage(0, 0, &bgimg);
	//������
	for (int i = 0; i < snake.len; i++)
	{
		setfillcolor(snake.nodes[i].color);
		fillrectangle(snake.nodes[i].x, snake.nodes[i].y, snake.nodes[i].x + SIZE, snake.nodes[i].y + SIZE);
	}
	//����ʳ��
	if (food.flag == 1) {
		setfillcolor(food.color);
		solidellipse(food.x, food.y, food.x + SIZE, food.y + SIZE);
	}
	//��ʾ����
	setbkmode(TRANSPARENT); //�������ֱ���͸��
	char str[100];
	sprintf(str, "����:%d", score);
	settextstyle(40, 0, _T("΢���ź�"));
	outtextxy(10, 10, str);
	EndBatchDraw();
}
void changeDir() {
	//�����Ż���ʹ��Async�첽�������� ��_getch()��ܶ� �������߸�˿��
	//��һ�������ƶ�ʱ�����Է��� ��ֹ�߿��Գ��Լ�
	if (GetAsyncKeyState(VK_UP) && snake.dir != DOWN) snake.dir = UP;  
	else if (GetAsyncKeyState(VK_LEFT) && snake.dir != RIGHT) snake.dir = LEFT;  
	else if (GetAsyncKeyState(VK_DOWN) && snake.dir != UP) snake.dir = DOWN;  
	else if (GetAsyncKeyState(VK_RIGHT)&& snake.dir != LEFT) snake.dir = RIGHT;  
}
void snakeMove() {
	//���ƶ��Ĺؼ����� ��ͷ�ڵ���������нڵ㶼��ֵΪǰһ���ڵ������ ע������ͷ����Ϊ�±�Ϊ0�Ľڵ�
	for (int i = snake.len - 1; i > 0; i--) {
		snake.nodes[i].x = snake.nodes[i - 1].x;
		snake.nodes[i].y = snake.nodes[i - 1].y;
	}
	//ֻ��Ҫ��ͷ�ڵ��ƶ����� ����ĸ���ȥ
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
	//�ж��Ƿ�Ե���ʳ�� ǰ����û�а���SIZE�����ƶ���Ӱ������
	if (food.flag && snake.nodes[0].x == food.x && snake.nodes[0].y == food.y) {
		PlaySound("res/eat.wav", 0, SND_FILENAME|SND_ASYNC); //�첽������Ч
		food.flag = 0;
		snake.len++;
		//���½ڵ�������� ��Ȼ�½ڵ�������Ͻ���һ��
		snake.nodes[snake.len-1].x = snake.nodes[snake.len - 1 - 1].x;
		snake.nodes[snake.len - 1].y = snake.nodes[snake.len - 1 - 1].y;
		snake.nodes[snake.len-1].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		score += 10;
		//���³�ʼ��ʳ��
		food.flag = 1;
		food.x = rand() % (WINDOW_WIDTH / SIZE) * SIZE;
		food.y = rand() % (WINDOW_HEIGHT / SIZE) * SIZE;
		//�ǵø�ʳ��������ɫ ��Ȼ��ɫ��һ��
		food.color = RGB(255, 255, 255);
	}
}
void checkOver() {
	//���Ե��ж����Ƿ�Խ�� �˴��������Ͻ����ж� ��������жϻ���������
	if (snake.nodes[0].x > WINDOW_WIDTH || snake.nodes[0].x < 0 || snake.nodes[0].y<0 || snake.nodes[0].y>WINDOW_HEIGHT) {
		PlaySound("res/over.wav", 0, SND_FILENAME | SND_ASYNC); //����ʧ����Ч
		Sleep(1000);
		loadimage(0, "res/over.png");
		//������ʷ��߷ֳɼ�
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
		//��ʾ����
		setbkmode(TRANSPARENT);
		char str1[100], str2[100];
		sprintf(str1, "��ķ���Ϊ:%d", score);
		sprintf(str2, "��ʷ��߷�:%d", last);
		settextstyle(40, 0, _T("΢���ź�"));
		outtextxy((WINDOW_WIDTH - textwidth(str1)) / 2, 350, str1);
		outtextxy((WINDOW_WIDTH - textwidth(str2)) / 2, 270, str2);
		system("pause");
		//���÷���  �ߵ�״̬
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
	//��ʼ��
	gameInit();
	//��Ϸ��ѭ��
	while (1) {
		snakeMove();
		eatFood();
		checkOver();
		if (_kbhit()) {
			changeDir();
		}
		Sleep(50); //�����ٶȵĹؼ����� 50�͹���
		updateWindow();
	}
	return 0;
}