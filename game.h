#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "snake.h"
#include "map.h"


class Game
{
public:
    Game();
    ~Game();
    
		void createInformationBoard();
    void renderInformationBoard() const;

    void createGameBoard();
    void renderGameBoard() const;
    
		void createInstructionBoard();
    void renderInstructionBoard() const;
		
		void loadLeadBoard();
    void updateLeadBoard();
    bool readLeaderBoard();
    bool updateLeaderBoard();
    bool writeLeaderBoard();
    void renderLeaderBoard() const;
    
		void renderBoards() const;
    
		void initializeGame();
    void runGame();
    void renderPoints() const;
    void renderDifficulty() const;
    
		void createRamdonFood();
    void renderFood() const;
    void renderSnake() const;
    void controlSnake() ; // CD：删去了const
    
		void startGame();
    bool renderRestartMenu() const;
    void adjustDelay();

    int renderPauseMenu() const;  // 创建暂停菜单
    void togglePause();  // 添加暂停/恢复方法
    bool isGamePaused() const;  // 获取当前状态

    enum class GameMode { CLASSIC, ENDLESS, OPTIONS };
    void showMainMenu();
    void runClassicMode();
    void runEndlessMode();
    void showOptions();

    //void renderMap() const;
    void selectMap();

    

private:
    bool mIsPaused;  // CD：暂停状态成员变量
    enum class GameExitReason {
      COLLISION,
      PLAYER_RESTART,
      QUIT
    };
    GameExitReason mExitReason;
    int mScreenWidth;
    int mScreenHeight;
    int mGameBoardWidth;
    int mGameBoardHeight;
    const int mInformationHeight = 6;
    const int mInstructionWidth = 18;
    std::vector<WINDOW *> mWindows;
    // Snake information
    const int mInitialSnakeLength = 2;
    const char mSnakeSymbol = '@';
    std::unique_ptr<Snake> mPtrSnake;
    // Food information
    SnakeBody mFood;
    const char mFoodSymbol = '#';
    int mPoints = 0;
    int mDifficulty = 0;
    int mBaseDelay = 150;
    // int mDelay;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 3;
    GameMode mCurrentMode = GameMode::CLASSIC;
    int mOptionIndex = 0;
    bool mOptionActive = false;
    int mEditableOptionsCount = 2;
    std::vector<int*> mOptionValues = {
      &mInitialLength,
      &mBaseDelay,
    };
    int mInitialLength = 2;
    bool mIsFastSpeed = false;
    int mLastDifficulty = -1;
    //maps:
    std::vector<GameMap> mAvailableMaps;
    int mSelectedMapIndex = 0;
    GameMap mCurrentMap;
    void renderMap() const;
    bool isObstacleAt(int x, int y) const;
};

#endif
