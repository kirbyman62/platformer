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
#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>
#include <string>
#include "../constants.h"

class Player
{
	private:
		//Static members relating to the loading of the image files for use
		//as textures for the sprite:
		static const short int SPRITES = 1;
		static const std::string SPRITE_PATHS[SPRITES];
		static const sf::Color COLOUR_MASK;
		static sf::Image _sprites[SPRITES];

		//The texture and the sprite:
		sf::Texture _texture;
		sf::Sprite _sprite;

		//Movement related variables:
		bool _isJumping;
		Direction _facing;

		//Score related variables:
		static const char* HIGHSCORE_FILE;
		unsigned int _score;
		unsigned int _highscore;

	public:
		//Load the images:
		static bool init();

		//Constructor:
		Player();

		//Check for a collision between the player and the given sprite:
		bool checkCollision(sf::Sprite&) const;

		//Signal the player to jump:
		void jump();

		//Signal the player to move in the given direction:
		void move(Direction);

		//Process events at the end of the frame:
		void handleMovement(float);
		void handleAnimation(float);

		//Getters:
		sf::Sprite& getSprite();
		unsigned int getScore();
		unsigned int getHighScore();
};

#endif
