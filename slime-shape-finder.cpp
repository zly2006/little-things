#include <iostream>
#include <vector>
#include <cmath>
#include <thread>

using ll = long long;

class Random {
    static constexpr ll multiplier = 0x5DEECE66DL;
    static constexpr ll addend = 0xBL;
    static constexpr ll mask = (1L << 48) - 1;
    ll seed;
public:
    explicit Random(ll seed) : seed((seed ^ multiplier) & mask) {}

    int nextInt() {
        return (int) next(32);
    }
    int nextIntBound10() {
        unsigned int r = next(31);
        unsigned int m = 9;
        for (int u = r;
             u - (r = u % 10) + m < 0;
             u = next(31));
        return r;
    }
    unsigned int next(unsigned int bits) {
        seed = (seed * multiplier + addend) & mask;
        return (int)(seed >> (48 - bits));
    }
};

bool isSlime(long worldSeed, int chunkX, int chunkZ) {
    Random rng(
            worldSeed +
            (chunkX * chunkX) * 0x4C1906 +
            (chunkX * 0x5AC0DB) +
            (chunkZ * chunkZ) * 0x4307A7L +
            (chunkZ * 0x5F24F) ^ 0x3AD8025F
    );
    return rng.nextIntBound10() == 0;
}

std::vector<std::vector<bool>> shape = {
        {0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1}, // 上北下南左西右东
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
};

bool matchHere(long worldSeed, int chunkX, int chunkZ) {
    for (int z = 0; z < shape.size(); z++) {
        for (int x = 0; x < shape[z].size(); x++) {
            if (shape[z][x] != isSlime(worldSeed, chunkX + x, chunkZ + z)) {
                return false;
            }
        }
    }
    return true;
}

class SearchTask {
public:
    class Result {
    public:
        long worldSeed;
        int chunkX;
        int chunkZ;
    };
    long worldSeed;
    int startX;
    int startZ;
    int endX;
    int endZ;
    std::vector<Result> results;
    SearchTask(long worldSeed, int startX, int startZ, int endX, int endZ) :
            worldSeed(worldSeed), startX(startX), startZ(startZ), endX(endX), endZ(endZ) {}

    std::thread start() {
        results.clear();
        return std::thread([this] {
            for (int z = startZ; z <= endZ; z++) {
                for (int x = startX; x <= endX; x++) {
                    if (matchHere(worldSeed, x, z)) {
                        std::cout << "[" << std::this_thread::get_id() << "] Found at " << x << ", " << z << std::endl;
                        results.push_back({worldSeed, x, z});
                    }
                }
            }
        });
    }
};

int main() {
    int count = 0;
    int nearestX = 0, nearestZ = 0;
    double nearestDistance = 1e9;
    ll worldSeed = -6367689914077029021;
    int startX = -10000;
    int startZ = -10000;
    int endX = 10000;
    int endZ = 10000;
    int taskCount = 20;
    time_t startTime = time(nullptr);
    std::vector<SearchTask> tasks;
    for (int i = 0; i < taskCount; ++i) {
        int start = (endX - startX) / taskCount * i + startX;
        int end = (endX - startX) / taskCount * (i + 1) + startX;
        SearchTask task(worldSeed, start, startZ, end, endZ);
        tasks.push_back(task);
    }
    for (auto &item: tasks) {
        item.start().join();
    }
    std::vector<SearchTask::Result> results;
    for (auto &item: tasks) {
        results.insert(results.end(), item.results.begin(), item.results.end());
    }
    std::sort(results.begin(), results.end(), [](const SearchTask::Result &a, const SearchTask::Result &b) {
        return a.worldSeed < b.worldSeed;
    });
    for (auto &item: results) {
        std::cout << "Shape at " << item.chunkX << ", " << item.chunkZ << std::endl;
        for (int dz = -5; dz < 15; ++dz) {
            for (int dx = -5; dx < 15; ++dx) {
                std::cout << (isSlime(item.worldSeed, item.chunkX + dx, item.chunkZ + dz) ? "X" : " ");
            }
            std::cout << std::endl;
        }
        double distance = sqrt(pow(item.chunkX, 2) + pow(item.chunkZ, 2));
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestX = item.chunkX;
            nearestZ = item.chunkZ;
        }
    }
    std::cout << "total: " << count << std::endl;
    std::cout << "nearest: " << nearestX << " " << nearestZ << std::endl;
    std::cout << "distance: " << nearestDistance << std::endl;
    std::cout << "time: " << time(nullptr) - startTime << "seconds" << std::endl;
    return 0;
}
