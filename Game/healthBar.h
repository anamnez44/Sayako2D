#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
class Bar
{
public:
	int max=1000;
	Texture t;
	Sprite s;
	Image i;
	RectangleShape bar;
	Bar()
	{
		i.loadFromFile("images/HealthBar.png");
		t.loadFromImage(i);
		s.setTexture(t);
		bar.setFillColor(Color::Black);
	}
	void update(int health)
	{
		if(health>0)
			if (health < max)
			{
				bar.setSize(Vector2f(10,(max-health)*70/max));
			}

	}
	void draw(RenderWindow& window)
	{
		Vector2f center = window.getView().getCenter();
		Vector2f size = window.getView().getSize();

		s.setPosition(center.x - size.x / 2 + 10, center.y - size.y / 2 + 10);
		bar.setPosition(center.x - size.x / 2 + 11, center.y - size.y / 2 + 13);

		window.draw(s);
		window.draw(bar);

	}
};