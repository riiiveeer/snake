#include <string>
#include <iostream>
#include <cmath> 

// For terminal delay
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm> 
#include <chrono>

#include "game.h"
#include "map.h"

Game::Game() : mIsPaused(false)
{
    // Separate the screen to three windows
    this->mWindows.resize(3);
    initscr();
    // If there wasn't any key pressed don't wait for keypress
    nodelay(stdscr, true);
    // Turn on keypad control
    keypad(stdscr, true);
    // No echo for the key pressed
    noecho();
    // No cursor show
    curs_set(0);
    // Get screen and board parameters
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // Initialize the leader board to be all zeros
    this->mLeaderBoard.assign(this->mNumLeaders, 0);

    //maps
    this->mAvailableMaps = GameMap::getDefaultMaps(mGameBoardWidth, mGameBoardHeight);  // 获取默认地图列表
    this->mSelectedMapIndex = 0;                       // 默认选择第一个地图
    this->mCurrentMap = mAvailableMaps[mSelectedMapIndex]; // 设置当前地图

}

Game::~Game()
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        delwin(this->mWindows[i]);
    }
    endwin();
}

// 切换暂停状态
void Game::togglePause() {
    mIsPaused = !mIsPaused;
}

// 获取暂停状态
bool Game::isGamePaused() const {
    return mIsPaused;
}

void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

void Game::renderInformationBoard() const
{
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");
    mvwprintw(this->mWindows[0], 2, 1, "This is a mock version.");
    mvwprintw(this->mWindows[0], 3, 1, "Please fill in the blanks to make it work properly!!");
    mvwprintw(this->mWindows[0], 4, 1, "Implemented using C++ and libncurses library.");
    wrefresh(this->mWindows[0]);
}

void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

void Game::renderGameBoard() const
{
    wrefresh(this->mWindows[1]);
    renderMap();
    wrefresh(this->mWindows[1]);
}

void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

void Game::renderInstructionBoard() const
{
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 2, 1, "Up: W");
    mvwprintw(this->mWindows[2], 3, 1, "Down: S");
    mvwprintw(this->mWindows[2], 4, 1, "Left: A");
    mvwprintw(this->mWindows[2], 5, 1, "Right: D");
    mvwprintw(this->mWindows[2], 6, 1, "Pause: P");
    mvwprintw(this->mWindows[2], 7, 1, "Speed-Up: J");

    mvwprintw(this->mWindows[2], 9, 1, "Difficulty");
    mvwprintw(this->mWindows[2], 12, 1, "Points");

    wrefresh(this->mWindows[2]);
}


void Game::renderLeaderBoard() const
{
    // If there is not too much space, skip rendering the leader board 
    if (this->mScreenHeight - this->mInformationHeight - 14 - 2 < 3 )
    {
        return;
    }
    mvwprintw(this->mWindows[2], 15, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 14 - 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 15 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 15 + (i + 1), 5, pointString.c_str());
    }
    wrefresh(this->mWindows[2]);
}

bool Game::renderRestartMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

int Game::renderPauseMenu() const
{
    WINDOW* menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);

    std::vector<std::string> menuItems = {"Continue", "Restart", "Quit"};

    int index = 0;
    int offset = 3;

    mvwprintw(menu, 1, 1, "Game Paused");
    mvwprintw(menu, 2, 1, "Current Score: ");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 16, pointString.c_str());

    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0+offset, 1, menuItems[0].c_str());
    wattroff(menu,A_STANDOUT);
    mvwprintw(menu, 1+offset, 1, menuItems[1].c_str());
    mvwprintw(menu, 2+offset, 1, menuItems[2].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index = (index - 1 + menuItems.size()) % menuItems.size(); // 循环选择
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }

            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index = (index + 1 + menuItems.size()) % menuItems.size(); // 循环选择
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10) // 空格或回车，即确认
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    return index;
}

void Game::renderPoints() const
{
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(this->mWindows[2], 13, 1, pointString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::renderDifficulty() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 10, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::initializeGame()
{
    // allocate memory for a new snake
		this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));

    if (!has_colors()) {
        printf("do not support colours.");
        refresh();
        return;
    }
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_YELLOW);
        init_pair(3, COLOR_BLUE, COLOR_RED);
    

    this->mPoints = 0;
    this->renderPoints();

    this->mDifficulty = 0;
    this->renderDifficulty();

    this->createRamdonFood();
    this->renderFood();

    this->mBaseDelay = 150;
    this->mIsFastSpeed = false;

    this->mCurrentMap = mAvailableMaps[mSelectedMapIndex];
    //this->renderMap();
}

void Game::createRamdonFood()
{
/* TODO 
 * create a food at random places
 * make sure that the food doesn't overlap with the snake.
 */

    int foodX, foodY;
    do {
        foodX = rand() % (this->mGameBoardWidth - 2) +1;
        foodY = rand() % (this->mGameBoardHeight - 2) +1;
    } while (this->mPtrSnake->isPartOfSnake(foodX, foodY)
            || this->isObstacleAt(foodX, foodY));  // 食物不能生成在障碍物上

    SnakeBody food(foodX,foodY);
    this->mFood = food;
    this->mPtrSnake->senseFood(this->mFood);

}

void Game::renderFood() const
{
    wattron(this->mWindows[1], COLOR_PAIR(2));
    mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
    wattroff(this->mWindows[1], COLOR_PAIR(2));
    wrefresh(this->mWindows[1]);
}

void Game::renderSnake() const
{
    wattron(this->mWindows[1], COLOR_PAIR(1));  // 启用颜色

    int snakeLength = this->mPtrSnake->getLength();
    std::vector<SnakeBody>& snake = this->mPtrSnake->getSnake();

    // 绘制蛇身
    for (int i = 0; i < snakeLength; i ++)
    {
        mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
    }
    wattroff(this->mWindows[1], COLOR_PAIR(1));
    
    // 突出蛇头
    if (snakeLength > 0) {
        wattron(this->mWindows[1], COLOR_PAIR(1) | A_BOLD);
        mvwaddch(this->mWindows[1], snake[0].getY(), snake[0].getX(), this->mSnakeSymbol);
        wattroff(this->mWindows[1], COLOR_PAIR(1) | A_BOLD);
    }
    wrefresh(this->mWindows[1]);
}

void Game::controlSnake()   // CD: added pause function
{
    int key = getch();
    bool directionKeyPressed = false;

    switch(key)
    {
        case 'P':
        case 'p':
        case 27:  // 27 standing for esc
            this->togglePause();
            break;
        case 'W':
        case 'w':
        case KEY_UP:
        {
				    // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Left || this->mPtrSnake->getDirection() == Direction::Right)
            {
                this->mPtrSnake->changeDirection(Direction::Up);
            }
            else
            {
                // what are you expecting?
            }
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
				    // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Left || this->mPtrSnake->getDirection() == Direction::Right)
            {
                this->mPtrSnake->changeDirection(Direction::Down);
            }
            else
            {
                // what are you expecting?
            }
            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
				    // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Up || this->mPtrSnake->getDirection() == Direction::Down)
            {
                this->mPtrSnake->changeDirection(Direction::Left);
            }
            else
            {
                // what are you expecting?
            }
            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
				    // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Up || this->mPtrSnake->getDirection()== Direction::Down)
            {
                this->mPtrSnake->changeDirection(Direction::Right);
            }
            else
            {
                // what are you expecting?
            }
            break;
        }
        case 'J':
        case 'j':
        {
            mIsFastSpeed = !mIsFastSpeed;
        }
        default:
        {
            break;
        }
    }

}

void Game::renderBoards() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    this->renderLeaderBoard();

    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
        wrefresh(this->mWindows[i]);
    }
    
}


void Game::adjustDelay()
{
    this->mDifficulty = this->mPoints / 5;
    
    if (mLastDifficulty != mDifficulty)
    {
        mLastDifficulty = mDifficulty;
        this->mBaseDelay = this->mBaseDelay - (10 * this->mDifficulty);
    }
}

void Game::runGame()
{
    bool moveSuccess;
    int key;
    // mExitReason = GameExitReason::COLLISION;

    while (true)
    {
        this->controlSnake();
        this->renderSnake();
        this->renderFood();
        this->renderPoints();
        this->renderDifficulty(); 
        if (mIsPaused) {
            // CD: 弹出暂停菜单:
            int shouldRestart = this->renderPauseMenu();
            if (shouldRestart == 1) {
                mIsPaused = false;
                mExitReason = GameExitReason::PLAYER_RESTART;
                return;
            }
            else if (shouldRestart == 2) {
                mExitReason = GameExitReason::QUIT;
                return; // 返回主菜单
            }
            this->togglePause();
            continue;
        }

        if (!mIsPaused) 
        {        
            this->adjustDelay();  
            if (this->mPtrSnake->checkCollision() 
            || this->mPtrSnake->hitObstacle(mCurrentMap.getObstacles()))
            {
                
                //this->renderBoards();
                mExitReason = GameExitReason::COLLISION;
                //this->renderRestartMenu();
                return;
            }
            else if (!this->mPtrSnake->touchFood())
            {
                this->mPtrSnake->getSnake().pop_back();
            }
            else
            {
                this->createRamdonFood();
                this->mPoints++;
            }

        }

        int actualDelay = mIsFastSpeed ? mBaseDelay / 2 : mBaseDelay;
        std::this_thread::sleep_for(std::chrono::milliseconds(actualDelay));
        this->renderBoards();
        refresh();
        
    }
}

void Game::startGame()
{
    refresh();
    bool choice;
    while (true)
    {
        this->showMainMenu();
        switch (mCurrentMode) {
            case GameMode::CLASSIC:
                this->runClassicMode();
                break;
            case GameMode::ENDLESS:
                this->runEndlessMode();
                break;
            case GameMode::OPTIONS:
                this->showOptions();
                break;
        }
        /*this->readLeaderBoard();
        this->renderBoards();
        this->initializeGame();
        this->runGame();
        switch (mExitReason) {
            case GameExitReason::PLAYER_RESTART:
                continue; // 直接重启
            case GameExitReason::COLLISION:
                if (!renderRestartMenu()) return;
                continue;
            case GameExitReason::QUIT:
                break;
        }*/
        
        this->updateLeaderBoard();
        this->writeLeaderBoard();
    }
}

// https://en.cppreference.com/w/cpp/io/basic_fstream
bool Game::readLeaderBoard()
{
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }
    int temp;
    int i = 0;
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        this->mLeaderBoard[i] = temp;
        i ++;
    }
    fhand.close();
    return true;
}

bool Game::updateLeaderBoard()
{
    bool updated = false;
    int newScore = this->mPoints;
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        if (this->mLeaderBoard[i] >= this->mPoints)
        {
            continue;
        }
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        updated = true;
    }
    return updated;
}

bool Game::writeLeaderBoard()
{
    // trunc: clear the data file
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    if (!fhand.is_open())
    {
        return false;
    }
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        fhand.write(reinterpret_cast<char*>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));;
    }
    fhand.close();
    return true;
}

void Game::showMainMenu() 
{
    clear();
    refresh();

    // create menu window
    int height = 10;
    int width = 30;
    int startY = (mScreenHeight - height) / 2;
    int startX = (mScreenWidth - width) / 2;

    WINDOW* menuWin = newwin(height, width, startY, startX);
    box(menuWin, 0, 0);

    // menu options
    std::vector<std::string> menuOptions = {
        "Classic Mode",
        "Endless Mode",
        "Options",
        "Quit"
    };

    int highlight = 0;
    keypad(menuWin, true);

    while (true) {
        for (int i = 0; i < menuOptions.size(); i++) {
            if (i == highlight) 
                wattron(menuWin, A_REVERSE);
            mvwprintw(menuWin, i+2, 2, menuOptions[i].c_str());
            if (i == highlight)
                wattroff(menuWin, A_REVERSE);
        }

        wrefresh(menuWin);

        int choice = wgetch(menuWin);

        switch(choice) {
            case 'W':
            case 'w':
            case KEY_UP:
                highlight = ((highlight - 1 + menuOptions.size()) % menuOptions.size());
                break;
            case 'S':
            case 's':
            case KEY_DOWN:
                highlight = ((highlight + 1 + menuOptions.size()) % menuOptions.size());
                break;
            case 10:
            case ' ':
                delwin(menuWin);
                switch (highlight) {
                    case 0:
                        mCurrentMode = GameMode::CLASSIC;
                        //runClassicMode();
                        break;
                    case 1:
                        mCurrentMode = GameMode::ENDLESS;
                        //runEndlessMode();
                        break;
                    case 2:
                        mCurrentMode = GameMode::OPTIONS;
                        break;  // return to main menu
                    case 3:
                        endwin();
                        exit(0);
                }
            return;
            // 还有快捷键：
            case '1':
                delwin(menuWin);
                runClassicMode();
                return;
            case '2':
                delwin(menuWin);
                runEndlessMode();
                return;
            case '3':
                delwin(menuWin);
                showOptions();
                break;
            case 27:
                endwin();
                exit(0);
        }
    }
}

void Game::runClassicMode() {
    // 经典模式初始化
    mCurrentMode = GameMode::CLASSIC;
    this->selectMap();

    if (mAvailableMaps.empty()) {
        return;  // 回到主菜单
    }

    while (true) {
        this->readLeaderBoard();
        this->renderBoards();
        this->initializeGame();
        mIsPaused = false;
        this->runGame(); 
        switch (mExitReason) {
            case GameExitReason::PLAYER_RESTART:
                continue; // 直接重启
            case GameExitReason::COLLISION:
                if (!this->renderRestartMenu()) {
                    return;
                }
                break;
            case GameExitReason::QUIT:
                return;
        }
    }
}

void Game::runEndlessMode() {
    // 无尽模式，用激光造成最高分
}

void Game::showOptions() {
    int height = 10;
    int width = 60;
    int startY = (mScreenHeight - height) / 2;
    int startX = (mScreenWidth - width) / 2;

    WINDOW* optionsWin = newwin(height, width, startY, startX);
    box(optionsWin, 0, 0);

    std::vector<std::string> options = {
        "Initial Length",
        "Speed",
        "Color Theme",
        "Back"
    };

    int highlight = 0;
    keypad(optionsWin, true);
    mOptionIndex = 0; // CD: 添加选项索引
    mOptionActive = false; // CD: 添加选项激活状态

    while (true) {
        for (int i = 0; i < options.size(); i++) {
            int y = i+2;
            wmove(optionsWin, y, 1);
            wclrtoeol(optionsWin);

            if (i == highlight)
                wattron(optionsWin, A_REVERSE);
            if (i < mEditableOptionsCount) {
                if (mOptionActive && mOptionIndex == i) {
                    std::string optionText = options[i] + ": <" + std::to_string(*mOptionValues[i]) + ">";
                    mvwprintw(optionsWin, i+2, 2, optionText.c_str());
                } else {
                    std::string optionText = options[i] + ": " + std::to_string(*mOptionValues[i]);
                    mvwprintw(optionsWin, i+2, 2, optionText.c_str());
                }  
            } else {
                mvwprintw(optionsWin, i+2, 2, options[i].c_str());
            }
            if (i == highlight)
                wattroff(optionsWin, A_REVERSE);
        }

        wrefresh(optionsWin);

        int key = wgetch(optionsWin);

        if (mOptionActive) {
            switch (key) {
                case 'a':
                case 'A':
                case KEY_LEFT:
                    *mOptionValues[mOptionIndex] = std::max(1, (*mOptionValues[mOptionIndex]) - 1);
                    break;
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    (*mOptionValues[mOptionIndex]) += 1;
                    break;
                case 10:
                case ' ':
                    mOptionActive = false; // 退出编辑状态
                    break;
            }
        } else {
            switch (key) {
                case 'w':
                case 'W':
                case KEY_UP:
                    highlight = (highlight - 1 + options.size()) % options.size();
                    break;
                case 'S':
                case 's':
                case KEY_DOWN:
                    highlight = (highlight + 1) % options.size();
                    break;
                case 10:
                case ' ':
                    if (highlight == options.size() - 1) {
                        delwin(optionsWin);
                        return;
                    } else {
                        mOptionActive = true;
                        mOptionIndex = highlight; 
                    }
                    break;
            }
        }
    }
}

void Game::renderMap() const {
    wattron(this->mWindows[1], COLOR_PAIR(3));
    for (const auto& obs:mCurrentMap.getObstacles()) {
        mvwaddch(this->mWindows[1], obs.y, obs.x, '%');
    }
    wattroff(this->mWindows[1], COLOR_PAIR(3));
}

void Game::selectMap() {
    clear();
    refresh();

    int height = 8;
    int width = 30;
    int startY = (mScreenHeight - height) / 2;
    int startX = (mScreenWidth - width) / 2;

    WINDOW* mapWin = newwin(height, width, startY, startX);
    box(mapWin, 0, 0);

    std::vector<std::string> mapNames = {
        "Empty Field", "Boxed Arena", "Crossing Field", "Random", "Back"
    };

    int highlight = 0;
    keypad(mapWin, true);

    mvwprintw(mapWin, 1, 2, "Choose a Map:");

    while (true) {
        
        for (int i = 0; i < mapNames.size(); i++) {
            if (i == highlight)
                wattron(mapWin, A_REVERSE);
            mvwprintw(mapWin, i + 2, 4, mapNames[i].c_str());
            if (i == highlight)
                wattroff(mapWin, A_REVERSE);
        }

        wrefresh(mapWin);
        int key = wgetch(mapWin);
        switch (key) {
            case 'w':
            case 'W':
            case KEY_UP:
                highlight = (highlight - 1 + mapNames.size()) % mapNames.size();
                break;
            case 's':
            case 'S':
            case KEY_DOWN:
                highlight = (highlight + 1) % mapNames.size();
                break;
            case 10:
            case ' ':
                //mCurrentMap.loadFromFile(mapFiles[highlight]);
                if (highlight == mapNames.size()-1) {
                    return;
                } 
                else if (highlight == mapNames.size()-2) {
                    mSelectedMapIndex = rand() % mAvailableMaps.size();
                }
                else {
                    mSelectedMapIndex = highlight;
                }
                delwin(mapWin);
                return;
        }
    }
}

bool Game::isObstacleAt(int x, int y) const {
    for (const auto& obs : this->mCurrentMap.getObstacles()) {
        if (obs.x == x && obs.y == y) {
            return true;
        }
    }
    return false;
}




