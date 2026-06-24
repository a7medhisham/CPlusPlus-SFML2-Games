#include "LevelConfig.hpp"
#include <algorithm>

LevelConfig getLevelConfig(int level) {
    static const LevelConfig configs[] = {
        {1,  8,  0,  0,  0, false, 0.4f, 0.4f, "First Wave"},
        {2, 10,  2,  0,  0, false, 0.2f, 0.2f, "Speed Up!"},
        {3, 12,  3,  1,  0, false, 1.2f, 1.2f, "Tanks Ahead"},
        {4, 10,  4,  2,  1, false, 1.3f, 1.3f, "Spinners!"},
        {5,  6,  2,  1,  1, true,  1.4f, 1.4f, "BOSS FIGHT"},
        {6, 14,  5,  2,  2, false, 1.5f, 1.5f, "Chaos"},
        {7, 16,  6,  3,  2, false, 1.6f, 1.6f, "Elite Squad"},
        {8, 14,  8,  4,  3, false, 1.7f, 1.7f, "Swarm"},
        {9, 12,  8,  4,  4, false, 1.8f, 1.8f, "Nightmare"},
        {10, 8,  4,  2,  2, true,  2.0f, 2.0f, "FINAL BOSS"},
    };
    return configs[std::min(std::max(level, 1), 11) - 1];
}
