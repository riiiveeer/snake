#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

struct Obstacle {
    int x, y;
};

class GameMap {
public:
    GameMap(std::string name, const std::vector<Obstacle>& obstacles);
    GameMap() = default;
    const std::string& getName() const;
    const std::vector<Obstacle>& getObstacles() const;

    // 静态方法：提供一些预设地图
    static std::vector<GameMap> getDefaultMaps(int boardX, int boardY);

private:
    std::string mName;
    std::vector<Obstacle> mObstacles;
};

#endif