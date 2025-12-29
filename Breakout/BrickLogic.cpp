#include "BrickLogic.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include "UpgradePickup.h"

// (same overlaps helper as you already have)
static bool overlaps(const SDL_FRect& a, const SDL_FRect& b)
{
    return a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y;
}

static bool isBlankLine(const std::string& s)
{
    for (unsigned char ch : s) {
        if (!std::isspace(ch)) return false;
    }
    return true;
}

static void stripCR(std::string& s)
{
    if (!s.empty() && s.back() == '\r') s.pop_back();
}

std::vector<Brick> BrickLogic::spawnBricks(
    int screenWidth,
    int screenHeight,
    int brickW,
    int brickH,
    int margin,
    int padding,
    const std::string& mapsPath,
    int levelIndex
) {
    // --- Load all levels from file 
    std::ifstream file(mapsPath);
    if (!file.is_open()) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
            "WARNING: Could not open '%s'. No bricks will be spawned.",
            mapsPath.c_str());
        return {}; // empty level
    }


    std::vector<std::vector<std::string>> levels;
    std::vector<std::string> current;

    std::string line;
    int lineNumber = 0;

    auto flushLevel = [&]() {
        if (!current.empty()) {
            levels.push_back(current);
            current.clear();
        }
        };

    while (std::getline(file, line)) {
        lineNumber++;
        stripCR(line);

        if (isBlankLine(line)) {
            flushLevel();
            continue;
        }

        // Validate characters (only 0..3 allowed)
        for (char c : line) {
            if (c < '0' || c > '9') {
                std::ostringstream oss;
                oss << "Maps.txt error at line " << lineNumber
                    << ": illegal character '" << c << "'. Only digits 0-3 allowed.";
                SDL_Log("%s", oss.str().c_str());
                throw std::runtime_error(oss.str());
            }
            int v = c - '0';
            if (v > 3) {
                std::ostringstream oss;
                oss << "Maps.txt error at line " << lineNumber
                    << ": illegal value '" << c << "'. Only 0-3 allowed.";
                SDL_Log("%s", oss.str().c_str());
                throw std::runtime_error(oss.str());
            }
        }

        current.push_back(line);
    }
    flushLevel();

    if (levels.empty()) {
        std::string err = "Maps.txt contains no levels.";
        SDL_Log("%s", err.c_str());
        throw std::runtime_error(err);
    }
    if (levelIndex < 0 || levelIndex >= (int)levels.size()) {
        std::ostringstream oss;
        oss << "Requested levelIndex " << levelIndex << " but Maps.txt has "
            << levels.size() << " level(s).";
        SDL_Log("%s", oss.str().c_str());
        throw std::runtime_error(oss.str());
    }

    const auto& lvl = levels[levelIndex];

    // --- Validate consistent row widths --- idk only if you guys need it
    const int rows = (int)lvl.size();
    const int cols = (int)lvl[0].size();
    /*for (int r = 0; r < rows; ++r) {
        if ((int)lvl[r].size() != cols) {
            std::ostringstream oss;
            oss << "Maps.txt error in level " << (levelIndex + 1)
                << ": inconsistent row width at row " << (r + 1)
                << ". Expected " << cols << " columns but got " << lvl[r].size() << ".";
            SDL_Log("%s", oss.str().c_str());
            throw std::runtime_error(oss.str());
        }
    }*/

    // --- Compute placement (grid to world) ---
    const int totalWidth = cols * brickW + (cols - 1) * padding;
    const int totalHeight = rows * brickH + (rows - 1) * padding;

    // Use margin as top padding; center horizontally if possible
    float startX = (float)margin;
    if (totalWidth + 2 * margin <= screenWidth) {
        startX = (screenWidth - totalWidth) * 0.5f;
    }

    float startY = (float)margin;

    // --- Create bricks ---
    std::vector<Brick> bricks;
    bricks.reserve(rows * cols);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int v = lvl[r][c] - '0';
            if (v == 0) continue; // empty space

            Brick b;
            b.alive = true;
            b.state = 1;
            b.maxState = v;

            b.rect = SDL_FRect{
                startX + c * (brickW + padding),
                startY + r * (brickH + padding),
                (float)brickW,
                (float)brickH
            };

            bricks.push_back(b);
        }
    }

    return bricks;
}

void BrickLogic::handleBallCollision(
    Ball& ball,
    std::vector<Brick>& bricks,
    std::vector<UpgradePickup>& pickups
) {
    const SDL_FRect ballRect = ball.getRect();

    for (auto& brick : bricks)
    {
        if (!brick.alive) continue;

        if (overlaps(ballRect, brick.rect))
        {
            // One collision per frame
            // If ball is strong, apply multiple "hits" depending on your design.
    
            brick.hit();

            if (!brick.alive) {
                if (brick.hasUpgrade)
                    BrickLogic::spawnPickupFromBrick(brick, pickups);
            }

            if (!ball.isStrong()) {
                ball.bounceVertical();
            }
            else {
                // strong ball doesn't bounce
                ball.consumeStrongHit();
            }

            break;
        }
    }
}
void BrickLogic::spawnPickupFromBrick(
    const Brick& brick,
    std::vector<UpgradePickup>& pickups
) {
    UpgradePickup p;
    p.type = brick.upgradeType;
    p.alive = true;

    p.rect = SDL_FRect{
        brick.rect.x + brick.rect.w * 0.25f,
        brick.rect.y + brick.rect.h * 0.25f,
        brick.rect.w * 0.5f,
        brick.rect.h * 0.5f
    };

    pickups.push_back(p);
}