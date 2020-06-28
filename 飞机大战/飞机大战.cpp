#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"Winmm.lib")						// 引用 Windows Multimedia API
//预定义长度和宽度
#define Width 480 
#define High 700 
//图形指针
IMAGE background_image;	//背景图片
IMAGE our_plane1; //我方飞机图层1
IMAGE our_plane2; //我方飞机图层2
IMAGE planeExplode1; //我方飞机爆炸图层1
IMAGE planeExplode2; //我方飞机爆炸图层2
IMAGE enemy_plane1; //敌方飞机图层1
IMAGE enemy_plane2; //敌方飞机图层2
IMAGE enemy_bullet1; //敌方飞机子弹图层1
IMAGE enemy_bullet2; //敌方飞机子弹图层2
IMAGE our_bullet1;	// 飞机子弹图层1
IMAGE our_bullet2;	//飞机子弹图层2
//飞机属性结构体
struct Our_plane   //我方飞机
{   
	float x, y;
	int life;
	float w, h;
} our_plane;

struct Enemy_plane //敌方飞机
{
	float x, y;
	float speed;
	int life;
	float w, h;
} enemy_plane[50];

struct Our_bullet //我方飞机子弹
{
	float x, y;
	int life = 0;
	float w, h;
} our_bullet[200];

struct Enemy_bullet  //敌方飞机子弹
{
	float x, y;
	int life;
	float w, h;
	float speed;
} enemy_bullet[200];

int flag = 1;
int enemy_number = 15; //同屏最大飞机数
float enemy_speed[4] = { 0.25,0.30,0.35,0.40 };  //敌机飞行速度列表
float enemy_bullet_speed[3] = { 0.35,0.40,0.45 };  //敌机子弹飞行速度列表
int enemy_bullet_number = 15; //敌方飞机子弹数量
int score = 0; //分数
DWORD t1, t2;

void re_ourplane();
void flash();
void reset_enemy(int i);
int check_ourbullet(Enemy_plane a, Our_bullet b);
int check_enmeybullet(Our_plane a, Enemy_bullet b);
int check_ourenemy(Our_plane a, Enemy_plane b);
void reset_enemy_bullet(Enemy_plane a);

void re_ourplane() {
	our_plane.x = Width * 0.45;
	our_plane.y = High * 0.8;
	our_plane.w = 50;
	our_plane.h = 50;
	our_plane.life = 5;
}

//装载图片并初始化数据
void loadpic() {
	srand(time(NULL));   //利用时间产生随机数种子
	t1 = t2 = GetTickCount();
	initgraph(Width, High); //绘制图形界面
	//装载各种图片
	loadimage(&background_image, "image\\back2.png", 480, 700);
	loadimage(&our_plane1, "image\\plane1.jpg",50,50);
	loadimage(&our_plane2, "image\\plane2.jpg",50,50);
	loadimage(&enemy_plane1, "image\\enemy1.png",40,40);
	loadimage(&enemy_plane2, "image\\enemy2.png",40,40);
	loadimage(&our_bullet1, "image\\bullet1_a.jpg");
	loadimage(&our_bullet2, "image\\bullet1_b.jpg");
	loadimage(&enemy_bullet1, "image\\enemy_bullet1_a.png");
	loadimage(&enemy_bullet2, "image\\enemy_bullet1_b.png");
	loadimage(&planeExplode1, "image\\planeExplode1.jpg",50,50);
	loadimage(&planeExplode2, "image\\planeExplode2.jpg", 50, 50);
	//播放音乐
	mciSendString("open music\\game_music.mp3 alias bgm", NULL, 0, NULL);
	mciSendString("play bgm repeat", NULL, 0, NULL);
	// 初始化我方飞机
	re_ourplane();
	//初始化敌方飞机
	for (int i = 0; i < enemy_number; i++) {
		reset_enemy(i);
	}
}
//显示图片，开始游戏
void start() {
	//背景图片
	putimage(0, 0, &background_image);
	//显示分数
	setbkmode(TRANSPARENT);  //设置文字背景填充色为透明
	settextcolor(RED);	//设置文字颜色
	outtextxy(10, 20, "得分：");
	char sc[10];
	sprintf_s(sc, "%d", score);
	outtextxy(50, 20, sc);
	//显示剩余生命值
	outtextxy(10, 45, "生命值：");
	char li[10];
	sprintf_s(li, "%d", our_plane.life);
	outtextxy(66, 45, li);
	//两图层组合显示我方飞机
	if (our_plane.life > 0) {
		putimage(our_plane.x, our_plane.y, &our_plane1, NOTSRCERASE);
		putimage(our_plane.x, our_plane.y, &our_plane2, SRCINVERT);
	}
	else {
		putimage(our_plane.x, our_plane.y, &planeExplode1, NOTSRCERASE);
		putimage(our_plane.x, our_plane.y, &planeExplode2, SRCINVERT);
	}
	//绘制敌方飞机
	for (int i = 0; i < enemy_number; i++) {
		if (enemy_plane[i].x < 10 || enemy_plane[i].x > 470) {
			continue;
		}
		if (enemy_plane[i].life == 0) {
			continue;
		}
		else{
			putimage(enemy_plane[i].x, enemy_plane[i].y, &enemy_plane1,NOTSRCERASE);
			putimage(enemy_plane[i].x, enemy_plane[i].y, &enemy_plane2, SRCINVERT);
		}
	}
	//绘制我方飞机子弹
	for (int i = 0; i < 20; i++) {
		if (our_bullet[i].life == 0) {
			continue;
		}
		putimage(our_bullet[i].x, our_bullet[i].y, &our_bullet1, NOTSRCERASE);
		putimage(our_bullet[i].x, our_bullet[i].y, &our_bullet2, SRCINVERT);
	}
	//绘制敌方子弹
	for (int i = 0; i < enemy_bullet_number; i++) {
		if (enemy_bullet[i].x < 30 || enemy_bullet[i].x>470) {
			continue;
		}
		if (enemy_bullet[i].life == 0) {
			continue;
		}
		putimage(enemy_bullet[i].x, enemy_bullet[i].y, &enemy_bullet1, NOTSRCERASE);
		putimage(enemy_bullet[i].x, enemy_bullet[i].y, &enemy_bullet2, SRCINVERT);
	}
	//让移动更加平滑
	FlushBatchDraw();
	Sleep(1);
}
//读取键盘输入
void check_keyboard() {
	//上下左右控制飞机移动并确保飞机不会飞出屏幕外
	if (GetAsyncKeyState(VK_UP)&&our_plane.y>0) {
		our_plane.y -= 1;
	}
	if (GetAsyncKeyState(VK_DOWN)&&our_plane.y+50<High) {
		our_plane.y += 1;
	}
	if (GetAsyncKeyState(VK_RIGHT)&&our_plane.x+50<Width) {
		our_plane.x += 1;
	}
	if (GetAsyncKeyState(VK_LEFT) && our_plane.x > 0) {
		our_plane.x -= 1;
	}
	//空格发射子弹
	if (GetAsyncKeyState(VK_SPACE) && (t2-t1>200)) {
		for (int i = 0; i < 20; i++) {
			if (our_bullet[i].life == 0) {
				our_bullet[i].w = 8;
				our_bullet[i].h = 20;
				our_bullet[i].x = our_plane.x + 22;
				our_bullet[i].y = our_plane.y - 20;
				our_bullet[i].life = 1;
				t1 = t2;
				break;
			}
		}
	}
	t2 = GetTickCount();
}
//非输入画面刷新
void flash() {
	//我方子弹移动
	for (int i = 0; i < 20; i++) {
		if (our_bullet[i].life == 0) {
			continue;
		}
		if (our_bullet[i].y > 0) {
			our_bullet[i].y -= 3;
		}
		else {
			our_bullet[i].life = 0;
		}
	}
	//敌方飞机移动
	for (int i = 0; i < enemy_number; i++) {
		if (enemy_plane[i].y < High-20) {
			enemy_plane[i].y += enemy_plane[i].speed;
		}
		else {
			reset_enemy(i);
		}
	}
	//敌方飞机子弹移动
	for (int i = 0; i < enemy_bullet_number; i++) {
		if (enemy_bullet[i].life == 0) {
			continue;
		}
		if (enemy_bullet[i].y < High - 10) {
			enemy_bullet[i].y += enemy_bullet[i].speed;
		}
		else {
			enemy_bullet[i].life = 0;
		}
	}
	//我方子弹击中敌方飞机
	for (int i = 0; i < enemy_number; i++) {
		for (int j = 0; j < 20; j++) {
			if (our_bullet[j].life == 0) {
				continue;
			}
			if (check_ourbullet(enemy_plane[i], our_bullet[j])) {
				our_bullet[j].life = 0;
				enemy_plane[i].life -= 1;
				if (enemy_plane[i].life == 0) {
					reset_enemy(i);
					score++;
				}
			}
		}
	}
	//敌方子弹击中我方飞机
	for (int i = 0; i < enemy_bullet_number; i++) {
		if (enemy_bullet[i].life == 0) {
			continue;
		}
		if (check_enmeybullet(our_plane, enemy_bullet[i])) {
			enemy_bullet[i].life = 0;
			our_plane.life -= 1;
		}
	}
	//飞机碰撞
	for (int i = 0; i < enemy_number; i++) {
		if (enemy_plane[i].life == 0) {
			continue;
		}
		if (check_ourenemy(our_plane, enemy_plane[i])) {
			enemy_plane[i].life = 0;
			our_plane.life -= 1;
		}
	}
}
//初始化敌方飞机
void reset_enemy(int i) {
	enemy_plane[i].w = 40;
	enemy_plane[i].h = 40;
	enemy_plane[i].x = (rand() % Width)-40;
	enemy_plane[i].y = -100;
	enemy_plane[i].speed = enemy_speed[rand() % 4];    //让飞机有随机的速度
	enemy_plane[i].life = 2;
	reset_enemy_bullet(enemy_plane[i]);
}
//初始化敌方飞机子弹
void reset_enemy_bullet(Enemy_plane a) {
	for (int i = 0; i < enemy_bullet_number; i++) {
		if (enemy_bullet[i].life == 0) {
			enemy_bullet[i].w = 8;
			enemy_bullet[i].h = 20;
			enemy_bullet[i].x = a.x + 18;
			enemy_bullet[i].y = a.y - 10;
			enemy_bullet[i].speed = a.speed + enemy_bullet_speed[rand() % 3]; //敌机子弹速度随机
			enemy_bullet[i].life = 1;
			break;
		}
	}
}
//检测我方子弹与敌方飞机是否碰撞
int check_ourbullet(Enemy_plane a, Our_bullet b) {
	int rex = (a.w + b.w) / 2;
	int rey = (a.h + b.h) / 2;
	int lx = abs((int)(a.x + a.w / 2 - b.x - b.w / 2));
	int ly = abs((int)(a.y + a.h / 2 - b.y - b.h / 2));
	if (lx < rex && ly < rey)
	{
		return 1;
	}
	return 0;
}
//检测我方飞机与敌方子弹是否碰撞
int check_enmeybullet(Our_plane a,Enemy_bullet b) {
	int rex = (a.w + b.w) / 2;
	int rey = (a.h + b.h) / 2;
	int lx = abs((int)(a.x + a.w / 2 - b.x - b.w / 2));
	int ly = abs((int)(a.y + a.h / 2 - b.y - b.h / 2));
	if (lx < rex && ly < rey)
	{
		return 1;
	}
	return 0;
}
//检测我方飞机与敌方飞机是否碰撞
int check_ourenemy(Our_plane a, Enemy_plane b) {
	int rex = (a.w + b.w) / 2;
	int rey = (a.h + b.h) / 2;
	int lx = abs((int)(a.x + a.w / 2 - b.x - b.w / 2));
	int ly = abs((int)(a.y + a.h / 2 - b.y - b.h / 2));
	if (lx < rex && ly < rey)
	{
		return 1;
	}
	return 0;
}
//游戏结束
void over() {
	EndBatchDraw();
	_getch();
	closegraph();
}
int main() {
	MessageBox(NULL, "上下左右控制飞机移动，空格发射子弹\n点击\"确定\"开始游戏", "game start", 0);
	loadpic();
	while (1) {
		start();
		check_keyboard();
		flash();
		if (our_plane.life==0){
			mciSendString("open music\\explode.mp3 alias die", NULL, 0, NULL);
			mciSendString("play die", NULL, 0, NULL);
			mciSendString("close bgm", NULL, 0, NULL);
			break;
		}
	}
	Sleep(1);
	//弹窗显示分数，游戏结束
	TCHAR s[100];
	sprintf_s(s, TEXT("%d"), score);
	MessageBox(NULL, TEXT(s), "game over", 0);
	over();
}
