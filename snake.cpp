#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"


SnakeBody::SnakeBody()
{
}


SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
	// DONE overload the == operator for SnakeBody comparision.
    return (this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY());
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength): mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)
{
	// DONE check if a given point with axis x, y is on the body of the snake.
    for (int i = 0; i < this->mSnake.size(); i++) {
        if (this->mSnake[i] == SnakeBody(x,y)) {
            return true;
        }
    }
    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
	// DONE check if the snake has hit the wall
    int headX = this->mSnake[0].getX();
    int headY = this->mSnake[0].getY();
    
    switch (this->mDirection) {
        case Direction::Up:
            if (headY == 1) {
                return true; // hit the top
            }
            break;
        case Direction::Down:
            if (headY == this->mGameBoardHeight - 2) {
                return true; // hit the bottom
            }
            break;
        case Direction::Left:
            if (headX == 1) {
                return true; // hit the left wall
            }
            break;
        case Direction::Right:
            if (headX == this->mGameBoardWidth - 2) {
                return true; // hit the right wall
            }
            break;
        }
    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()
{
	// DONE check if the snake has hit itself.
    int headX = this->mSnake[0].getX();
    int headY = this->mSnake[0].getY();
    switch (this->mDirection) {
        case Direction::Up:
            if (this->isPartOfSnake(headX, headY - 1)) {
                return true; 
            }
            break;
        case Direction::Down:
            if (this->isPartOfSnake(headX, headY + 1)) {
                return true;
            }
            break;
        case Direction::Left:
            if (this->isPartOfSnake(headX - 1, headY)) {
                return true;
            }
            break;
        case Direction::Right:
            if (this->isPartOfSnake(headX + 1, headY)) {
                return true;
            }
            break;
    }
		return false;
}


bool Snake::touchFood()
{
    SnakeBody newHead = this->createNewHead();
    if (this->mFood == newHead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Snake::senseFood(SnakeBody food)
{
    this->mFood = food;
}

std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

bool Snake::changeDirection(Direction newDirection)
{
    switch (this->mDirection)
    {
        case Direction::Up:
        {
						// what you need to do when the current direction of the snake is Up
						// and the user inputs a new direction?  DONE
            // can only change to left or right
            switch (newDirection) {
                case Direction::Left:
                case Direction::Right:
                    this->mDirection = newDirection;
            }
        }

        case Direction::Down:
        {
						// what you need to do when the current direction of the snake is Down
						// and the user inputs a new direction? DONE
            // can only change to left or right
            switch (newDirection) {
                case Direction::Left:
                case Direction::Right:
                    this->mDirection = newDirection;
            }
        }

        case Direction::Left:
        {
						// what you need to do when the current direction of the snake is Left
						// and the user inputs a new direction? DONE
            // can only change to up or down
            switch (newDirection) {
                case Direction::Up:
                case Direction::Down:
                    this->mDirection = newDirection;
            }
        }

        case Direction::Right:
        {
						// what you need to do when the current direction of the snake is Right
						// and the user inputs a new direction? TODO
            // can only change to up or down
            switch (newDirection) {
                case Direction::Up:
                case Direction::Down:
                    this->mDirection = newDirection;
            }    
        }
    }

    return false;
}


SnakeBody Snake::createNewHead()
{
		/* TODO
		 * read the position of the current head
		 * read the current heading direction 
		 * add the new head according to the direction
		 * return the new snake
		 */
    int headX = this->mSnake[0].getX();
    int headY = this->mSnake[0].getY();

    switch (this->mDirection) {
        case Direction::Up:
            this->mSnake.insert(this->mSnake.begin(), SnakeBody(headX, headY - 1));
            break;
        case Direction::Down:
            this->mSnake.insert(this->mSnake.begin(), SnakeBody(headX, headY + 1));
            break;
        case Direction::Left:
            this->mSnake.insert(this->mSnake.begin(), SnakeBody(headX - 1, headY));
            break;
        case Direction::Right:
            this->mSnake.insert(this->mSnake.begin(), SnakeBody(headX + 1, headY));
            break;
    }
    SnakeBody newHead = this->mSnake[0];
    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
bool Snake::moveFoward()
{
    /* 
		 * TODO 
		 * move the snake forward. 
     * If eat food, return true, otherwise return false
     */
    // ?
    if (this->touchFood()) {
        return true;
    } else {
        return false;
    }
    return false;
}

bool Snake::checkCollision()
{
    if (this->hitWall() || this->hitSelf())
    {
        return true;
    }
    else
    {
        return false;
    }
}


int Snake::getLength()
{
    return this->mSnake.size();
}

