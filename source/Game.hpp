#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <list>
#include "level.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "MovingPlatform.hpp"
#include "HealthBar.hpp"
using namespace sf;



void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("files/images/111.png");
	menuTexture3.loadFromFile("files/images/333.png");
	aboutTexture.loadFromFile("files/images/about.png");
	menuBackground.loadFromFile("files/images/Mega-man.jpg");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu3.setPosition(100, 150);
	menuBg.setPosition(345, 0);

	//////////////////////////////����///////////////////
	while (isMenu)
	{
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 0;
		window.clear(Color(129, 181, 221));

		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//���� ������ ������ ������, �� ������� �� ���� 
			if (menuNum == 2) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3) { window.close(); isMenu = false; }

		}

		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);

		window.display();
	}
	////////////////////////////////////////////////////
}







void RunGame()
{
	///////////// ������������� ///////////////////////////
	
	RenderWindow window(VideoMode(1920 , 1080), "MegaMan");
	menu(window);
	View view( FloatRect(0, 0, 450, 280) );

	Level lvl;
	lvl.LoadFromFile("files/Level1.tmx");

	Texture enemy_t, moveplatform_t, megaman_t, bullet_t, bg;
	bg.loadFromFile("files/images/bg.png"); 
	enemy_t.loadFromFile("files/images/enemy.png");
	moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	megaman_t.loadFromFile("files/images/megaman.png");
	bullet_t.loadFromFile("files/images/bullet.png");

    //�������� ���������
	AnimationManager anim;
	anim.loadFromXML("files/anim_megaman.xml",megaman_t);
	anim.animList["jump"].loop = 0;

	AnimationManager anim2;
	anim2.create("move",bullet_t,7,10,8,8,1,0);
	anim2.create("explode",bullet_t,27,7,18,18,4,0.01,29,false);

	AnimationManager anim3;
	anim3.create("move",enemy_t,0,0,16,16,2,0.002,18);
	anim3.create("dead",enemy_t,58,0,16,16,1,0);

	AnimationManager anim4;
	anim4.create("move",moveplatform_t,0,0,95,22,1,0);

    Sprite background(bg);
    background.setOrigin(bg.getSize().x/2,bg.getSize().y/2);

	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;

	std::vector<Object> e = lvl.GetObjects("enemy");
	for (int i=0;i < e.size();i++)
		entities.push_back(new ENEMY(anim3, lvl, e[i].rect.left, e[i].rect.top) );

	e = lvl.GetObjects("MovingPlatform");
	for (int i=0;i < e.size();i++)
		entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top) );

	Object pl = lvl.GetObject("player");
	PLAYER MegaMan(anim, lvl, pl.rect.left, pl.rect.top);

	HealthBar healthBar;

	Clock clock;
	Music music;//������� ������ ������
	music.openFromFile("files/music.ogg");//��������� ����
	music.play();//������������� ������

	/////////////////// �������� ����  /////////////////////
	while (window.isOpen())
	{
		
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		time = time/500;  // ����� ���������� �������� ����

		if (time > 40) time = 40;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::KeyPressed)
				if (event.key.code==Keyboard::Space)
		        	entities.push_back(new Bullet(anim2,lvl,MegaMan.x+18,MegaMan.y+18,MegaMan.dir) );
		}


		if (Keyboard::isKeyPressed(Keyboard::Left)) MegaMan.key["L"]=true;
		if (Keyboard::isKeyPressed(Keyboard::Right)) MegaMan.key["R"]=true;
		if (Keyboard::isKeyPressed(Keyboard::Up)) MegaMan.key["Up"]=true;
		if (Keyboard::isKeyPressed(Keyboard::Down)) MegaMan.key["Down"]=true;
		if (Keyboard::isKeyPressed(Keyboard::Space)) MegaMan.key["Space"]=true;


		for(it=entities.begin();it!=entities.end();)
		{
			Entity *b = *it;
			b->update(time);
			if (b->life==false)	{ it  = entities.erase(it); delete b;}
			else it++;
		}


		MegaMan.update(time);
		healthBar.update(MegaMan.Health);


		for(it=entities.begin();it!=entities.end();it++)
		{
			//���������� ������ �������
			if ((*it)->Name=="Enemy")
			{
				Entity *enemy = *it;

				if (enemy->Health<=0) continue;

				if  (MegaMan.getRect().intersects( enemy->getRect() ))
				    if (MegaMan.dy>0) { enemy->dx=0; MegaMan.dy=-0.2; enemy->Health=0;}
				    else if (!MegaMan.hit) { MegaMan.Health-=5; MegaMan.hit=true;
				                         if (MegaMan.dir) MegaMan.x+=10; else MegaMan.x-=10;}


				for (std::list<Entity*>::iterator it2=entities.begin(); it2!=entities.end(); it2++)
				{
					Entity *bullet = *it2;
					if (bullet->Name=="Bullet")
						if ( bullet->Health>0)
							if  (bullet->getRect().intersects( enemy->getRect() ) )
							 { bullet->Health=0; enemy->Health-=5;}
				}
			}

			//2. ���������� ���������
			if ((*it)->Name=="MovingPlatform")
			{
			    Entity *movPlat = *it;
				if  (MegaMan.getRect().intersects( movPlat->getRect() ) )
				   if (MegaMan.dy>0)
					  if (MegaMan.y+MegaMan.h<movPlat->y+movPlat->h)
					  {MegaMan.y=movPlat->y - MegaMan.h + 3; MegaMan.x+=movPlat->dx*time; MegaMan.dy=0; MegaMan.STATE=PLAYER::stay;}
			}

			//3.. � �.�.
		}


		/////////////////////���������� �� �����/////////////////////
		view.setCenter( MegaMan.x,MegaMan.y);
		window.setView(view);

		background.setPosition(view.getCenter());
		window.draw(background);

		lvl.Draw(window);

		for(it=entities.begin();it!=entities.end();it++)
			(*it)->draw(window);

		MegaMan.draw(window);
		healthBar.draw(window);

		window.display();
	} 

}

#endif GAME_H

