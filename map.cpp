#include <set>
#include <utility>

#include "map.h"

GameMap::GameMap(std::string name, const std::vector<Obstacle>& obstacles)
    : mName(std::move(name)), mObstacles(obstacles) {}

const std::string& GameMap::getName() const {
    return mName;
}

const std::vector<Obstacle>& GameMap::getObstacles() const {
    return mObstacles;
}

std::vector<GameMap> GameMap::getDefaultMaps(int boardX, int boardY) {
    std::vector<GameMap> maps;

    // 地图1：空地
    maps.emplace_back("Empty Field", std::vector<Obstacle>{});

    // 地图2：四周围墙
    std::vector<Obstacle> box;
    for (int x = 1; x < boardX; ++x) {
        box.push_back({x, 1});
        box.push_back({x, boardY-2});
    }
    for (int y = 1; y <= boardY; ++y) {
        box.push_back({1, y});
        box.push_back({boardX-2, y});
    }
    maps.emplace_back("Boxed Arena", box);

    //地图3：随机生成6个十字架
    std::vector<Obstacle> crosses;
    std::set<std::pair<int, int>> usedCenters;

    int centerX = boardX / 2;
    int centerY = boardY / 2;

    auto isInSpawnArea = [&](int x, int y) {
        return abs(x - centerX) <= 3 && abs(y - centerY) <= 2;
    };

    int crossCount = 0;
    while (crossCount < 6) {
        int cx = rand() % (boardX - 4) + 3; // 保证左右不会越界
        int cy = rand() % (boardY - 4) + 2;

        if (isInSpawnArea(cx, cy)) continue;
        if (usedCenters.count({cx, cy})) continue;

        // 记录中心坐标
        usedCenters.insert({cx, cy});

        // 加入十字形障碍
        crosses.push_back({cx, cy});
        crosses.push_back({cx - 1, cy});
        crosses.push_back({cx + 1, cy});
        crosses.push_back({cx - 2, cy});
        crosses.push_back({cx + 2, cy});
        crosses.push_back({cx, cy - 1});
        crosses.push_back({cx, cy + 1});

        crossCount++;
    }

    maps.emplace_back("Crossing Field", crosses);

    // 可继续添加其他地图...

    return maps;
}

