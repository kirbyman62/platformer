/* Copyright (c) Alex Kerr 2014
* This file is part of 'platformer'
*
* 'platformer' is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* 'platformer' is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRENTY; without the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Purpose License
* along with 'platformer'. If not, see <http://www.gnu.org/licenses/>.
*/
#include <SFML/Graphics.hpp>
#include "player/player.h"
#include "block/block.h"
#include "block/staticBlock.h"

const sf::Color PURPLE(182, 48, 227);

int main()
{
	if(! Player::init())
		return -1;

	sf::RenderWindow window(sf::VideoMode(600, 400), "Platformer");
	sf::Event event;
	sf::Clock fps;
//	float frameTime = 0.016;

	Player p;

	StaticBlock b1(100, 250, 0, (400 - 250));
	StaticBlock b2(100, 250, (600 - 100), (400 - 250));

	while(window.isOpen())
	{
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				//write highscore to file if needed
				window.close();
			}
		}
		window.clear(PURPLE);
		window.draw(b1.getShape());
		window.draw(b2.getShape());
		window.draw(p.getSprite());
		window.display();
	}
	return 0;
}
