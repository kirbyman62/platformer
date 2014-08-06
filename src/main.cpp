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
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "player/player.h"
#include "block/block.h"
#include "block/staticBlock.h"
#include "block/dynamicBlock.h"
#include "block/deathBlock.h"
#include "interface/text.h"
#include "interface/arrow.h"
#include "layout/layout.h"

//The height and width of the window:
const unsigned int WINDOW_X = 600;
const unsigned int WINDOW_Y = 400;

const sf::Color PURPLE(182, 48, 227);

int main()
{
	//Attempt to load all the nessecary files:
	if(! Player::init())
		return -1;

	if(! SoundEffect::init())
		return -1;

	if(! Text::init())
		return -1;

	if(! Arrow::init())
		return -1;

	//Seed the random number generator:
	std::srand(unsigned(std::time(0)));

	sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "Platformer");
	sf::Event event;
	sf::Clock frameTimer;
	float frameTime = 0.016;

	Player p;

	//Create the text items:
	Text fps("FPS: ", 16, TOP_LEFT, &window, 5, 5);
	Text score("SCORE: ", 16, TOP_RIGHT, &window, 5, 5);
	Text high("HIGH:  ", 16, TOP_RIGHT, &window, 5, 23);

	//Used to time the difference between the player pressing and releasing space,
	//allowing for 'short hops' if the player releases space bar quick enough:
	sf::Clock jumpTimer;

	//Initialises the two 'target' blocks that stand at either side of the
	//screen. These are the blocks the player must jump back and forth between
	//to score points. At any time, the 'target' block is stored in the pointer
	//with the same name:
	StaticBlock b1(100, 250, 0, (WINDOW_Y - 250));
	StaticBlock b2(100, 250, (WINDOW_X - 100), (WINDOW_Y - 250));
	StaticBlock* targets[2] = { &b1, &b2 };
	StaticBlock* target = targets[1];

	//The block at the bottom that kills the player on contact and ends the game:
	DeathBlock deathBlock(400, 15, 100, 385);

	//Two arrows to teach the player at the beginning of the game:
	Arrow helpArrow1(SOUTH, (WINDOW_X - 50), (WINDOW_Y - 280));
	Arrow helpArrow2(SOUTH, 50, (WINDOW_Y - 280));

	//The master layouts vector, containing all possible combinations of blocks
	//that go in the middle. A pointer is used to reference the currently 
	//selected layout:
	std::vector <std::vector<Block*>* >* layouts = initLayouts();
	std::vector <Block*>* layout = layouts->front();

	while(window.isOpen())
	{
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				//Write highscore to file if needed:
				if(p.getScore() > p.getHighScore())
				{
					if(! p.writeScoreToFile())
					{
						cleanup(layouts);
						return -1;
					}
				}
				window.close();
			}
			if(event.type == sf::Event::KeyReleased)
				if(event.key.code == sf::Keyboard::Space)
					p.setMaxJumpHeight(jumpTimer.getElapsedTime().asSeconds());
		}

		//Handle keypresses:
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			p.move(LEFT);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			p.move(RIGHT);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			p.jump();
			jumpTimer.restart();
		}

		//Handle the block events:
		for(unsigned int i = 0; i < layout->size(); i++)
			layout->at(i)->handleEvents(frameTime);

		//Check if the player is on a dynamicBlock, and move them if so:
		for(unsigned int i = 0; i < layout->size(); i++)
		{
			if(layout->at(i)->isPlayerOnTop(p.getSprite()))
			{
				DynamicBlock* b = dynamic_cast <DynamicBlock*>(layout->at(i));
				if(b != NULL)
					p.move(b->getDistanceMoved());
			}
		}

		//Handle the player's movement:
		//Check collisions for the static target blocks and the layout blocks:
		for(unsigned int i = 0; i < 2; i++)
			p.handleCollision(targets[i]->getShape(), frameTime);
		for(unsigned int i = 0; i < layout->size(); i++)
			p.handleCollision(layout->at(i)->getShape(), frameTime);
		p.handleCollision(&window, frameTime);
		p.handleMovement(frameTime);

		//Handle animation for the death block:
		deathBlock.handleEvents(0);

		//Check if the player has reached the target block:
		if(target->isPlayerOnTop(p.getSprite()))
		{
			//Give the player a point:
			p.addPoint();

			//Change the target block to the other one:
			if(target == targets[0])
				target++;
			else
				target--;

			layout = shuffleLayouts(layouts);
		}
		//Otherwise, check if the player is on top of the death block:
		else if(deathBlock.isPlayerOnTop(p.getSprite()))
		{
			//Kill the player:
			p.kill();

			//Write highscore to file if needed:
			if(p.getScore() > p.getHighScore())
			{
				if(! p.writeScoreToFile())
				{
					cleanup(layouts);
					return -1;
				}
			}

			//Create new text items:
			Text gameover("GAME OVER", 34, TOP_LEFT, &window, 150, 125);
			Text scoreFinal("SCORE: ", 16, TOP_LEFT, &window, 150, 165);
			Text highFinal("HIGH: ", 16, TOP_LEFT, &window, 330, 165);
			Text retry("RETRY", 16, TOP_LEFT, &window, 250, 220);
			Text quit("QUIT", 16, TOP_LEFT, &window, 260, 250);
			Text* menu[2] = { &retry, &quit };
			Text* selected = menu[0];

			//Create arrows to point at the current selection:
			Arrow menuArrow1(EAST, 225, 231);
			Arrow menuArrow2(EAST, 225, 261);

			//Make a new loop for the game over screen:
			while(window.isOpen())
			{
				while(window.pollEvent(event))
					if(event.type == sf::Event::Closed)
						window.close();

				//If the user presses enter, restart or quit:
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
				{
					if(selected == menu[0])
					{
						p.reset();
						target = targets[1];
						layout = shuffleLayouts(layouts);
						break;
					}
					else
						window.close();
				}

				//Change the selection if the player presses an arrow key:
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					if(selected == menu[1])
						selected = menu[0];

				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					if(selected == menu[0])
						selected = menu[1];

				//Draw everything:
				window.clear(PURPLE);
				window.draw(gameover.updateText());
				window.draw(scoreFinal.updateText(p.getScore()));
				window.draw(highFinal.updateText(p.getHighScore()));
				window.draw(retry.updateText());
				window.draw(quit.updateText());
				if(selected == menu[0])
					window.draw(menuArrow1.getSprite());
				else
					window.draw(menuArrow2.getSprite());
				window.display();
			}
		}

		//Clear the screen and draw everything:
		window.clear(PURPLE);
		window.draw(b1.getShape());
		window.draw(b2.getShape());
		window.draw(deathBlock.getShape());
		window.draw(fps.updateText(static_cast <unsigned>(1 / frameTime)));
		window.draw(score.updateText(p.getScore()));
		window.draw(high.updateText(p.getHighScore()));
		if(p.getScore() == 0)
			window.draw(helpArrow1.getSprite());
		if(p.getScore() == 1)
			window.draw(helpArrow2.getSprite());
		for(unsigned int i = 0; i < layout->size(); i++)
			window.draw(layout->at(i)->getShape());
		window.draw(p.getSprite());
		window.display();

		//Get the time of that frame:
		frameTime = frameTimer.restart().asSeconds();
	}
	cleanup(layouts);
	return 0;
}
