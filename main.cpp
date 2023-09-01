#include <iostream>
#include <graphics.h>
#include <vector>
#pragma comment( lib, "MSIMG32.LIB")
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 1100;

//透明贴图
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg) //新版png
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}
//鼠标点击
bool PointInRect(int x, int y, RECT& r)
{
	return (r.left <= x && x << r.right && r.top <= y && y <= r.bottom);
}
//判断相撞
bool RectDuangRect(RECT& r1, RECT& r2)
{
	RECT r;
	r.left = r1.left-70;//- (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top-50;//- (r2.bottom - r2.top);
	r.bottom = r1.bottom;

	return (r.left < r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
}

//开始界面
void Welcome()
{
	LPCTSTR title = _T("飞机大战");
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");

	RECT tplayr, texitr;
	setbkcolor(WHITE);
	cleardevice();
	settextstyle(60, 0, _T("黑体"));
	settextcolor(BLACK);
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);
	settextstyle(0, 0, _T("黑体"));
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.right = tplayr.left+ textwidth(tplay);
	tplayr.top = sheight / 5*2.5;
	tplayr.bottom = tplayr.top + textheight(tplay);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5*3;
	texitr.bottom = texitr.top + textheight(texit);

	
	outtextxy(tplayr.left, tplayr.top, tplay);
	outtextxy(texitr.left, texitr.top, texit);


	EndBatchDraw();

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_MOUSE);
		if (mess.lbutton)
		{
			if (PointInRect(mess.x, mess.y, tplayr))
				return;
			else if (PointInRect(mess.x, mess.y, texitr))
				exit(0);
		}
	}
}

//背景、敌机、英雄、子弹
class BK
{
public:
	BK(IMAGE& img)
		:img(img) ,y(-sheight)
	{

	}
	void Show()
	{
		if (y == 0)
			y = -sheight;
		y += 4;
		putimage(0, y, &img);
	}

private:
	IMAGE& img;
	int  y;
};
class Hero
{
public:
	Hero(IMAGE& img)
		:img(img)
	{
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.top = sheight / 2 - img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = sheight;
	}
	void Show()
	{
		transparentimage(NULL,rect.left, rect.top, &img);
	}

	void Control()
	{
		ExMessage mess;
		if (peekmessage(&mess, EM_MOUSE))
		{
			rect.left = mess.x - img.getwidth() / 2;
			rect.top = mess.y - img.getheight() / 2;
			rect.right = rect.right + img.getwidth();
			rect.bottom = rect.top + img.getheight();
		}
	}
	RECT& GetRect()
	{
		return rect;
	}

private:
	IMAGE& img;
	RECT rect;

};
class Enemy
{
public:
	Enemy(IMAGE& img, int x)
		:img(img)
	{
		rect.left = x;
		rect.right = rect.left + img.getwidth();
		rect.top = -img.getheight();
		rect.bottom = 0;
	}
	bool Show()
	{
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 4;
		rect.bottom += 4;
		transparentimage(NULL,rect.left, rect.top, &img);

		return true;
	}
	RECT& GetRect()
	{
		return rect;
	}
private:
	IMAGE& img;
	RECT rect;

};
class Bullet
{
public:
	Bullet(IMAGE& img, RECT pr)
		:img(img)
	{
		rect.left = pr.left + (pr.right - pr.left) / 2 - (img.getwidth()) / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = pr.top - img.getheight();
		rect.bottom = rect.top + img.getheight();
	}
	bool Show()
	{
		if (rect.bottom <= 0)
		{
			return false;
		}
		rect.top -= 4;
		rect.bottom -= 4;
		transparentimage(NULL, rect.left, rect.top, &img);
	}
	RECT& GetRect()
	{
		return rect;
	}
private:
	IMAGE& img;
	RECT rect;

};

//敌机不重合
bool AddEnemy(vector<Enemy*>& es,IMAGE& enemyimg)
{
	Enemy* e =new  Enemy(enemyimg, abs(rand() % (swidth - enemyimg.getwidth())));
	for (auto& i : es)
	{
		if (RectDuangRect(i->GetRect(), e->GetRect()))
		{
			delete e;
			return false;
		}
	}
	es.push_back(e);
	return true;
}

bool Play()
{
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;

	IMAGE heroimg, enemyimg, bkimg, bulletimg;
	loadimage(&heroimg, _T("C:\\Users\\86183\\Desktop\\计算机高级程序语言\\C++\\大作业\\game\\images\\me1.png"));
	loadimage(&enemyimg, _T("C:\\Users\\86183\\Desktop\\计算机高级程序语言\\C++\\大作业\\game\\images\\enemy1.png"));
	loadimage(&bkimg, _T("C:\\Users\\86183\\Desktop\\计算机高级程序语言\\C++\\大作业\\game\\images\\bk2.png"),swidth,sheight*2);
	loadimage(&bulletimg, _T("C:\\Users\\86183\\Desktop\\计算机高级程序语言\\C++\\大作业\\game\\images\\bullet1.png"));

	BK bk = BK(bkimg);
	Hero hp = Hero(heroimg);

	vector<Enemy*> es;
	vector<Bullet*>bs;
	int bsing = 0;

	//生成敌机
	for (int i = 0; i < 5; i++)
	{
		AddEnemy(es, enemyimg);
	}
	while (is_play)
	{
		bsing++;
		if (bsing == 20)
		{
			bsing = 0;
			bs.push_back(new Bullet(bulletimg, hp.GetRect()));
		}
		BeginBatchDraw();

		bk.Show();
		Sleep(6);
		flushmessage();
		Sleep(2);
		hp.Control();
		hp.Show();

		for (auto & i : bs)
		{
			i->Show();
		}

		auto it = es.begin();
		while (it != es.end())
		{
			//敌我相撞
			if (RectDuangRect((*it)->GetRect(), hp.GetRect()))
			{
				is_play = false;
			}

			auto bit = bs.begin();
			while (bit != bs.end())
			{
				//子弹敌机
				if (RectDuangRect((*bit)->GetRect(), (*it)->GetRect()))
				{
					delete (*it);
					es.erase(it);
					it = es.begin();
					delete(*bit);
					bs.erase(bit);
				//	bit = bs.begin();
					break;
				}
				bit++;
			}
			//敌机销毁
	    	if (!(*it)->Show())
			{
				delete(*it);
				es.erase(it);
				it = es.begin();
			}
			it++;
		}
		//随机生成敌机数
		for (int i = 0; i < 5 - es.size(); i++)
		{
			es.push_back(new Enemy(enemyimg, abs(rand()) % (swidth - enemyimg.getwidth())));
		}
		EndBatchDraw();
	}

	return true;
}

int main()
{
	//easyx初始化
	initgraph(swidth, sheight, EX_NOMINIMIZE | EX_SHOWCONSOLE);
	bool is_live = true;
	while (is_live)
	{
		Welcome();
		is_live=Play();
	}
	return 0;
}