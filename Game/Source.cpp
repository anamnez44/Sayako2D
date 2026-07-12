#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<iostream>
#include"view.h"
#include "level.h"
#include <vector>
#include <list>
#include"Animation.h"
#include"healthBar.h"
using namespace std;
using namespace sf;
class Entity {
public:
	vector<Object> obj;
	float dx, dy, x, y, speed, moveTimer;
	int w, h, health;
	bool life, isMove, onGround;
	float timer, timer_end;
	AnimationManager anim;
	String name;

	Entity(AnimationManager& anim2, String Name, float X, float Y) {
		anim = anim2;
		x = X; y = Y; name = Name;
		life = true; onGround = false; isMove = false; dy = 0;
		timer = 0; timer_end = 0;
	}
	void option(string NAME, float SPEED = 0, int HEALTH = 10, std::string FIRST_ANIM = "")
	{
		name = NAME;
		if (FIRST_ANIM != "") anim.set(FIRST_ANIM);
		w = anim.getW();
		h = anim.getH();
		speed=dx = SPEED;
		health = HEALTH;
	}
	void draw(RenderWindow &window)
	{
		anim.draw(window, x, y+h);
	}
	virtual void update(float time) = 0;
	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}
};
class Player :public Entity {
public:
	 bool isTurn,onStairs;
	bool isShoot,isJump,isBeat,isUp,isDown,isDanger,isDangerE;
	float tick;
	enum {run, up, down, jump, stay,turbo,beat,danger} state;
	enum { left2, right2 } stateD;
	int playerScore;

	Player( AnimationManager& anim2,String Name, Level &lev, float X, float Y) :Entity(anim2, Name, X, Y) {
		playerScore = 0; state = stay; obj = lev.GetAllObjects();
		option("Player", 0,1000, "Stay");
		stateD = right2;
	}
	
	void control() {
	
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Up)) {
			  isUp = true;
			
			}

			if (Keyboard::isKeyPressed(Keyboard::Space) && onGround)
			{
				 onGround = false; isJump = true;
				 if (state == stay || state == run) { state = jump; dy = -0.6; dx = 0; }
			}
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				stateD = left2; isTurn = true; 
				if (state != down) dx = -0.2;
				if (state == stay) { state = run; }
			}
			if (Keyboard::isKeyPressed(Keyboard::Down))
			{
				isDown = true;
				if (state == stay || state == run) { state = down; dx = 0; }
				
			}
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				stateD = right2; isTurn = false;
				if (state != down) dx = 0.2; 
				if (state == stay ) { state = run;}
		
			}
			
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				isBeat = true;
			}
			
			
			
		}
	}



	void checkCollisionWithMap(float Dx, float Dy)
	{
	
	
		for (int i = 0; i < obj.size(); i++) {
			if (getRect().intersects(obj[i].rect))
			{
				if (obj[i].name == "solid")
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true;}
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left -  w; }
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; }
				}
				if (obj[i].name == "leftSlope")
				{
					FloatRect r = obj[i].rect;
					float y0 = (x + w/ 2 - r.left) * r.height / r.width + r.top - h;
					if (y > y0) {
						if (x + w / 2 > r.left)
						{
							y = y0;  state = stay;
						}
						
					}
					
					//onGround = true;
				
				}
				if (obj[i].name == "rightSlope")
				{
					FloatRect r = obj[i].rect;
					float y0 = -(x + w / 2 - r.left) * r.height / r.width + r.top + r.height - h;
					if (y > y0) {
						if (x + w / 2 < r.left + r.width)
						{
							y = y0; state = stay;
						}
						
					}
					
					//onGround = true;
					
				}
				if (obj[i].name == "stairs")
				{
					onStairs = true;   dy = 0; dx = 0;
					if (isUp) { dy = -0.2; state = up; onGround = false; x = (obj[i].rect.left + obj[i].rect.width / 2) - w / 2; }
					if (isDown) { dy = 0.2; state = up; onGround = false;}
				   
				}
				if (obj[i].name == "danger")
				{
					isDanger = true;	
				}
			}

		}
	}

	void update(float time)
	{
		if(onGround)state = stay;
		if (state == jump)dx = 0;
		
		isJump=isBeat=isDanger= isUp = isDown = onStairs=false;
		checkCollisionWithMap(dx, dy);
		control();

		if (onStairs&&onGround) { if (dx == 0)state = stay;}
		if (onStairs && !onGround) { state = up; }
	
		switch (state)
		{
		case run:if(!onStairs)anim.set("Run"); break;
		case jump:anim.set("Jump"); if (isJump)anim.animList[anim.currentAnim].currentFrame = 0;   if (anim.animList[anim.currentAnim].currentFrame > 18) { anim.animList[anim.currentAnim].currentFrame = 18; } break;
		case up:anim.set("Up"); anim.pause(); if (isUp || isDown)anim.play(); break;
		case down:anim.set("Down"); break;
		case stay:anim.set("Stay");  dx = 0; break;
		}
		if (isBeat) {anim.set("Beat"); if (state ==run)anim.set("RunBeat"); if (state == up)anim.set("UpBeat"); }
		if (isDanger || isDangerE) { anim.set("Danger"); health -= 5;  }
		if (state != up) {w = anim.getW(); h = anim.getH();}else h = 41;
		if (isTurn)anim.flip();

		x += dx * time;    
		checkCollisionWithMap(dx, 0);
		y += dy * time;
		checkCollisionWithMap(0, dy);
	
		if (health <= 0) { life = false; }
		GetView(x + w / 2, y + h / 2);
		//	if (life) { GetView(x, y, time); }
		dy = dy + 0.001*time;
	    anim.tick(time);
	}
};
class Enemy :public Entity {
public:
	Enemy(AnimationManager& anim2, String Name, Level &lvl, float X, float Y) :Entity(anim2, Name, X, Y) {
		obj = lvl.GetAllObjects();	
		option("enemy", 0.01, 100, "move");
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		for (int i = 0; i < obj.size(); i++)
			if (getRect().intersects(obj[i].rect))
			{
				if (obj[i].name == "solid" || obj[i].name == "stop") {
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w;  dx = -0.1; anim.flip(); }
					if (Dx < 0) {
						x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; anim.flip();
					}
				}

			}
	}

	void update(float time)
	{
		checkCollisionWithMap(dx, 0);
		x += dx * time;
		
		if (health <= 0) {
			anim.set("dead"); dx = 0;
			timer_end += time;
			if (timer_end > 4000) life = false;
		}
		anim.tick(time);
	}
};
class Platform :public Entity
{
public:

	Platform(AnimationManager& anim2, String Name, Level &lvl, float X, float Y) :Entity(anim2, Name, X, Y) {
		obj = lvl.GetAllObjects();
		option("platform", 0.2, 0, "move");
	}
	void update(float time)
	{
			checkCollisionWithMap(dx, 0);
			x += dx * time;
			anim.tick(time);
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		for (int i = 0; i < obj.size(); i++)
			if (getRect().intersects(obj[i].rect))
			{
				if (obj[i].name == "solid" ) {
					if (Dx > 0) { x = obj[i].rect.left - w;  dx = -0.1; }
					if (Dx < 0) {
						x = obj[i].rect.left + obj[i].rect.width; dx = 0.1;
					}
				}

			}
	}
	float GetDX()
	{
		return dx;
	}

};
class Bullet :public Entity
{
public:
	int direct;
	Bullet(AnimationManager& anim2, String Name, Level &lvl, float X, float Y,int direction) :Entity(anim2, Name, X, Y) {
		obj = lvl.GetObjects("solid");
		direct = direction;
		option("bullet", 0.5, 0, "move");
		
	}
	void update(float time)
	{
		switch (direct)
		{
		
		case 0:dx = -speed; break;
		case 1:dx = speed; break;
		}
		x += dx * time;
		for (int i = 0; i < obj.size(); i++)
		{
			if (getRect().intersects(obj[i].rect))
			{
				anim.set("explode"); speed = 0;
					if (anim.isPlaying() == false) life = false;
			}
		}
		
		anim.tick(time);
	}
};
void menu(RenderWindow& window,bool& sound,int&change)
{
	Texture t1, t2, t3,tb,t4,t5,t6,t7;
	t1.loadFromFile("images/111.png");
	t2.loadFromFile("images/222.png");
	t3.loadFromFile("images/333.png");
	tb.loadFromFile("images/bg2.png");
	t4.loadFromFile("images/soundOn.png");
	t5.loadFromFile("images/soundOff.png");
	t6.loadFromFile("images/megalovania.png");
	t7.loadFromFile("images/mario.png");
	Sprite s1, s2, s3,s4,s5,s6,s7,s8;
	s1.setTexture(t1);
	s2.setTexture(t2);
	s3.setTexture(t3);
	s4.setTexture(tb);
	s4.setScale(2, 2);
	s5.setTexture(t4);
	s6.setTexture(t5);
	s7.setTexture(t6);
	s8.setTexture(t7);


	s1.setPosition(100, 30);
	s2.setPosition(100, 90);
	s3.setPosition(100, 150);
	s4.setPosition(0, 0);
	s7.setColor(Color::White);
	s8.setColor(Color::White);
	bool isMenu=true;
	int menuNum=0;
	while (isMenu)
	{
		s1.setColor(Color::White);
		s2.setColor(Color::White);
		s3.setColor(Color::White);
		s5.setColor(Color::White);
		s6.setColor(Color::White);
		
		if (sound) {
			s5.setPosition(1200, 1200);
			s6.setPosition(100, 330);
			s7.setPosition(150, 390);
			s8.setPosition(150, 450);
		}
		else {
			s5.setPosition(100, 330);
			s6.setPosition(1200, 1000);
			s7.setPosition(1200,1000);
			s8.setPosition(1200,1200);
		}
		
		if (IntRect(100, 30, 131, 60).contains(Mouse::getPosition(window))) { s1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(100, 90, 131, 60).contains(Mouse::getPosition(window))) { s2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(100, 150, 131, 60).contains(Mouse::getPosition(window))) { s3.setColor(Color::Blue); menuNum = 3; }
		if (sound)
		{
			if (IntRect(100, 330, 131, 60).contains(Mouse::getPosition(window))) { s6.setColor(Color::Blue); menuNum = 6;}
			if (IntRect(150, 390, 131, 60).contains(Mouse::getPosition(window))) { menuNum = 7; }
			if (IntRect(150, 450, 131, 60).contains(Mouse::getPosition(window))) {   menuNum = 8;  }
     	}
		else
		{
			if (IntRect(100, 330, 131, 60).contains(Mouse::getPosition(window))) { s5.setColor(Color::Blue); menuNum = 5; }
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) { isMenu = false; }
			if (menuNum == 2) { isMenu = false; }
			if (menuNum == 3) { window.close(); isMenu = false; }
			if (menuNum == 5) { sound = true; }
			if (menuNum == 6) { sound = false; }
			if (menuNum == 7) { change = 1; s7.setColor(Color::Blue);
			; s8.setColor(Color::White);
			}
			if (menuNum == 8) {
				change = 2; s7.setColor(Color::White);
			s8.setColor(Color::Blue);
			}
		}
		window.draw(s4);
		window.draw(s1);
		window.draw(s2);
		window.draw(s3);
		window.draw(s5);
		window.draw(s6);
		window.draw(s7);
		window.draw(s8);
		window.display();
	}
}

void Pause(RenderWindow& window,bool&isPause,bool&sound,int&change,bool&menuGo)
{
	isPause = true;
	int PauseNum = 0;
	Texture t1,  t3;
	t1.loadFromFile("images/continue.png");
	t3.loadFromFile("images/exit.png");
	Sprite s1,  s3;
	s1.setTexture(t1);
	s3.setTexture(t3);
	s1.setPosition(window.getView().getCenter().x-50, window.getView().getCenter().y-50);
	s3.setPosition(window.getView().getCenter().x-50 , window.getView().getCenter().y);
	while (isPause)
	{
		s1.setColor(Color::White);
	
		s3.setColor(Color::White);
		if (IntRect(550,280, 280,60).contains(Mouse::getPosition(window))) { s1.setColor(Color::Blue); PauseNum = 1; }
		if (IntRect(595,420, 170,50).contains(Mouse::getPosition(window))) { s3.setColor(Color::Blue); PauseNum = 3; }
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (PauseNum == 1)
			{
				isPause = false;
			}

			if (PauseNum == 3)
			{
				window.close(); isPause = false;
			}
		}
		window.draw(s1);
		
		window.draw(s3);
		window.display();
	}
}

void changeLvl(Level&lvl,int&num)
{
	if (num == 1)lvl.LoadFromFile("map.tmx");
	if (num == 2)lvl.LoadFromFile("map2.tmx");
}

bool start(int&num, RenderWindow&window, int&change, bool&sound)
{
	bool GotoMenu = false;
	bool isPause = false;
	menuGo:
	if(num==1||GotoMenu)
	
	menu(window,sound,change);
	view.reset(FloatRect(0, 0, 600,360));
	bool isLvl = false;

    Texture heroImage;
	heroImage.loadFromFile("images/megaman.png");
	AnimationManager anim;
	anim.loadFromXML("animation.xml",heroImage);

	Texture EnemyImage;
	EnemyImage.loadFromFile("images/enemy.png");
	AnimationManager animE;
	animE.create("move", EnemyImage, 0, 0, 16, 16, 2, 0.002, 18);
	animE.create("dead", EnemyImage, 58, 0, 16, 16, 2,0.01,0,false) ;

	Texture bulletImage;
	bulletImage.loadFromFile("images/bullet.png");
	AnimationManager animB;
	animB.create("move", bulletImage, 7, 10, 8, 8, 1, 0);
	animB.create("explode", bulletImage, 27, 7, 18, 18, 4,0.09, 29, false);

	Texture platformImage;
	platformImage.loadFromFile("images/movingPlatform.png");
	AnimationManager animP;
	animP.create("move", platformImage, 0, 0, 95, 22, 1, 0);
	
	Level lvl;
	changeLvl(lvl, num);

	Object player = lvl.GetObject("player");
	Player p(anim,"Player1", lvl, player.rect.left, player.rect.top);
	
	
	list<Entity*>enemys;
	list<Entity*>entetys;
	vector<Object>enemyObjects;
	enemyObjects = lvl.GetObjects("enemy");
	for (auto it = enemyObjects.begin(); it != enemyObjects.end(); ++it)
	{
		entetys.push_back(new Enemy(animE, "enemy", lvl, it->rect.left, it->rect.top));
	
	}
	enemyObjects = lvl.GetObjects("platform");
	for (auto it = enemyObjects.begin(); it != enemyObjects.end(); ++it)
	{
		entetys.push_back(new Platform(animP, "platform", lvl, it->rect.left, it->rect.top));
	}
	
	Clock clock;
	int dir = 0;
	
	Clock timerF; float fpst;
	int ifps = 0; int fps; 

	Bar bar;

	Texture bg; bg.loadFromFile("images/bg.png");
	Sprite background(bg);
	background.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);
	background.scale(2, 2);

	Music musicMario, musicMegalovania;
	musicMario.openFromFile("Mario_Theme.ogg");
	musicMegalovania.openFromFile("megalovania.ogg");
	musicMario.setLoop(true);
	musicMegalovania.setLoop(true);
	if (sound)
	{
		if (change == 1)
			musicMegalovania.play();
		if (change == 2)
			musicMario.play();
	}
	while (window.isOpen())
	{
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			Pause(window,isPause, sound, change,GotoMenu);
			if (GotoMenu)goto menuGo;
		}
			ifps++;
			float time = clock.getElapsedTime().asMicroseconds();
			fpst = timerF.getElapsedTime().asSeconds();
			clock.restart();

			time = time / 800;

			if (fpst > 1)
			{
				timerF.restart(); ifps = 0;
			}
			fps = ifps / fpst;
			Event event;
			
		
			while (window.pollEvent(event))
			{

				if (event.type == sf::Event::Closed)
					window.close();

				dir = p.stateD;
				if (p.isBeat)
				{
					p.isBeat = false;
					if (dir)
						entetys.push_back(new Bullet(animB, "bullet", lvl, p.x + 33, p.y + 13, dir));
					else
						entetys.push_back(new Bullet(animB, "bullet", lvl, p.x, p.y + 13, dir));
				}
				
			}
			if(!isPause)
			p.update(time);
			if (Keyboard::isKeyPressed(Keyboard::Q))
			{
				p.x = player.rect.left;
				p.y = player.rect.top;
				p.health = 1000;
			}
			if (Keyboard::isKeyPressed(Keyboard::T) || isLvl)
			{
				return true;
			}

			cout << "X : " << p.x << " Y : " << p.y << "\n";

			for (auto it = entetys.begin(); it != entetys.end();)
			{
				Entity *b = *it;
				if (b->name == "bullet")
				{
					if (b->x > view.getCenter().x + window.getSize().x / 2 || b->x < view.getCenter().x - window.getSize().x / 2)b->life = false;
					if (b->y > view.getCenter().y + window.getSize().y / 2 || b->y < view.getCenter().y - window.getSize().y / 2)b->life = false;
				}
				if (!isPause)
				b->update(time);

				if (b->life == false) { it = entetys.erase(it); delete b; }
				else it++;
			}

			p.isDangerE = false;
			for (auto it = entetys.begin(); it != entetys.end(); it++)
			{

				if (((*it)->name == "platform") && ((*it)->getRect().intersects(p.getRect())))
				{
					Entity *movPlat = *it;
					if ((p.dy > 0) || (p.onGround == false))
						if (p.y + p.h < movPlat->y + movPlat->h)
						{
							p.y = movPlat->y - p.h + 3; p.x += movPlat->dx*time; p.dy = 0; p.onGround = true;
						}
				}


				if ((*it)->getRect().intersects(p.getRect()))
				{
					if ((*it)->name == "enemy") {

						if (p.dy > 0 && !p.onGround) { (*it)->dx = 0; p.dy = -0.2; (*it)->health = 0; }
						else {
							p.isDangerE = true;
						}


					}
				}
				if ((*it)->name == "enemy")
				{
					for (auto i = entetys.begin(); i != entetys.end(); i++)
					{
						Entity* bullet = *i;
						if (bullet->name == "bullet")
						{
							if ((*it)->getRect().intersects(bullet->getRect()))
							{
								bullet->life = false;
								(*it)->health -= 35;
							}
						}
					}
				}
				if ((*it)->name == "door")
				{
					if ((*it)->getRect().intersects(p.getRect()))
					{
						isLvl = true;
					}
				}
			}

			//    cout << fps << "\n";
			//	cout << time << endl;
			bar.update(p.health);
			
			window.setView(view);
			window.clear(Color(37, 196, 252));
			background.setPosition(view.getCenter());
			window.draw(background);
			lvl.Draw(window);
			for (auto it = entetys.begin(); it != entetys.end(); it++)
			{
				(*it)->draw(window);
			}
			p.draw(window);
			bar.draw(window);
			
			window.display();
		
	}
	
}
void runGame(int& num,RenderWindow&window,int&change,bool&sound)
{
	if (start(num,window,change,sound)) { num++; runGame(num,window,change,sound); }
}
int main()
{
	RenderWindow window(VideoMode(1000,625), "Game", Style::Fullscreen);
	int num = 1;
	int change; bool sound=false;
	runGame(num,window,change,sound);
	return 0;
}
