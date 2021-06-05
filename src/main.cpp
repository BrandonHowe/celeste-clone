#include "raylib.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <nlohmann/json.hpp>
#include "main.hpp"
#include <math.h>

// for convenience
using json = nlohmann::json;

json levelEditor;

constexpr auto SCREEN_WIDTH  = 800;
constexpr auto SCREEN_HEIGHT = 450;

constexpr int G = 1000;
constexpr float PLAYER_JUMP_SPD = 350.0f;
constexpr float PLAYER_HOR_SPD = 200.0f;
constexpr float PLAYER_DASH_SPD = 666.6f;
constexpr float PLAYER_DASH_SPD_DIAG = 353.5f;
constexpr float PLAYER_DASH_DIST = 75.0f;

const auto HAIR_COLOR_NORMAL = ColorFromHSV({ 0, 0.7093, 0.6745 });
const auto HAIR_COLOR_DASHING = ColorFromHSV({ 203.11200, 0.7333, 1.0000 });

Scene gameScene = LevelSelect;

std::ostream& operator<<(std::ostream& stream, const Vector2& vec)
{
    stream << "Vector { x: " << vec.x << ", y: " << vec.y << " }";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Rectangle& rect)
{
    stream << "Rectangle { x: " << rect.x << ", y: " << rect.y << ", width: " << rect.width << ", height: " << rect.height << " }";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const EnvItem& ei)
{
    stream << "EnvItem { rect: " << ei.rect << ", type: " << ei.type << " };";
    return stream;
}

Orientation& operator++(Orientation& orientation)
{
    switch (orientation)
    {
        case Orientation::Top: return orientation = Orientation::Bottom;
        case Orientation::Bottom: return orientation = Orientation::Left;
        case Orientation::Left: return orientation = Orientation::Right;
        case Orientation::Right: return orientation = Orientation::HorizMiddle;
        case Orientation::HorizMiddle: return orientation = Orientation::VertMiddle;
        case Orientation::VertMiddle: return orientation = Orientation::VertTop;
        case Orientation::VertTop: return orientation = Orientation::VertBottom;
        case Orientation::VertBottom: return orientation = Orientation::HorizLeft;
        case Orientation::HorizLeft: return orientation = Orientation::HorizRight;
        case Orientation::HorizRight: return orientation = Orientation::Block;
        case Orientation::Block: return orientation = Orientation::TopLeft;
        case Orientation::TopLeft: return orientation = Orientation::TopRight;
        case Orientation::TopRight: return orientation = Orientation::BottomLeft;
        case Orientation::BottomLeft: return orientation = Orientation::BottomRight;
        case Orientation::BottomRight: return orientation = Orientation::IGBR;
        case Orientation::IGBR: return orientation = Orientation::IGUR;
        case Orientation::IGUR: return orientation = Orientation::IGBL;
        case Orientation::IGBL: return orientation = Orientation::IGUL;
        case Orientation::IGUL: return orientation = Orientation::IGRight;
        case Orientation::IGRight: return orientation = Orientation::IGTop;
        case Orientation::IGTop: return orientation = Orientation::IGLeft;
        case Orientation::IGLeft: return orientation = Orientation::IGBottom;
        case Orientation::IGBottom: return orientation = Orientation::IGSBL;
        case Orientation::IGSBL: return orientation = Orientation::IGSBR;
        case Orientation::IGSBR: return orientation = Orientation::IGSUR;
        case Orientation::IGSUR: return orientation = Orientation::IGSUL;
        case Orientation::IGSUL: return orientation = Orientation::IGCircle;
        case Orientation::IGCircle: return orientation = Orientation::IGDiagDR;
        case Orientation::IGDiagDR: return orientation = Orientation::IGDiagUR;
        case Orientation::IGDiagUR: return orientation = Orientation::Inground1;
        case Orientation::Inground1: return orientation = Orientation::Inground2;
        case Orientation::Inground2: return orientation = Orientation::Inground3;
        case Orientation::Inground3: return orientation = Orientation::Inground4;
        case Orientation::Inground4: return orientation = Orientation::Inground5;
        case Orientation::Inground5: return orientation = Orientation::Inground6;
        case Orientation::Inground6: return orientation = Orientation::Inground7;
        case Orientation::Inground7: return orientation = Orientation::Inground8;
        case Orientation::Inground8: return orientation = Orientation::Inground9;
        case Orientation::Inground9: return orientation = Orientation::Inground10;
        case Orientation::Inground10: return orientation = Orientation::Inground11;
        case Orientation::Inground11: return orientation = Orientation::Inground12;
        case Orientation::Inground12: return orientation = Orientation::Inground13;
        case Orientation::Inground13: return orientation = Orientation::Inground14;
        case Orientation::Inground14: return orientation = Orientation::Inground15;
        case Orientation::Inground15: return orientation = Orientation::Top;
    }
}

Orientation& operator--(Orientation& orientation)
{
    switch (orientation)
    {
        case Orientation::Top: return orientation = Orientation::Inground15;
        case Orientation::Bottom: return orientation = Orientation::Top;
        case Orientation::Left: return orientation = Orientation::Bottom;
        case Orientation::Right: return orientation = Orientation::Left;
        case Orientation::HorizMiddle: return orientation = Orientation::Right;
        case Orientation::VertMiddle: return orientation = Orientation::HorizMiddle;
        case Orientation::VertTop: return orientation = Orientation::VertMiddle;
        case Orientation::VertBottom: return orientation = Orientation::VertTop;
        case Orientation::HorizLeft: return orientation = Orientation::VertBottom;
        case Orientation::HorizRight: return orientation = Orientation::HorizLeft;
        case Orientation::Block: return orientation = Orientation::HorizRight;
        case Orientation::TopLeft: return orientation = Orientation::Block;
        case Orientation::TopRight: return orientation = Orientation::TopLeft;
        case Orientation::BottomLeft: return orientation = Orientation::TopRight;
        case Orientation::BottomRight: return orientation = Orientation::BottomLeft;
        case Orientation::IGBR: return orientation = Orientation::BottomRight;
        case Orientation::IGUR: return orientation = Orientation::IGBR;
        case Orientation::IGBL: return orientation = Orientation::IGUR;
        case Orientation::IGUL: return orientation = Orientation::IGBL;
        case Orientation::IGRight: return orientation = Orientation::IGUL;
        case Orientation::IGTop: return orientation = Orientation::IGRight;
        case Orientation::IGLeft: return orientation = Orientation::IGTop;
        case Orientation::IGBottom: return orientation = Orientation::IGLeft;
        case Orientation::IGSBL: return orientation = Orientation::IGBottom;
        case Orientation::IGSBR: return orientation = Orientation::IGSBL;
        case Orientation::IGSUR: return orientation = Orientation::IGSBR;
        case Orientation::IGSUL: return orientation = Orientation::IGSUR;
        case Orientation::IGCircle: return orientation = Orientation::IGSUL;
        case Orientation::IGDiagDR: return orientation = Orientation::IGCircle;
        case Orientation::IGDiagUR: return orientation = Orientation::IGDiagDR;
        case Orientation::Inground1:  return orientation = Orientation::IGDiagUR;
        case Orientation::Inground2:  return orientation = Orientation::Inground1;
        case Orientation::Inground3:  return orientation = Orientation::Inground2;
        case Orientation::Inground4:  return orientation = Orientation::Inground3;
        case Orientation::Inground5:  return orientation = Orientation::Inground4;
        case Orientation::Inground6:  return orientation = Orientation::Inground5;
        case Orientation::Inground7:  return orientation = Orientation::Inground6;
        case Orientation::Inground8:  return orientation = Orientation::Inground7;
        case Orientation::Inground9:  return orientation = Orientation::Inground8;
        case Orientation::Inground10: return orientation = Orientation::Inground9;
        case Orientation::Inground11: return orientation = Orientation::Inground10;
        case Orientation::Inground12: return orientation = Orientation::Inground11;
        case Orientation::Inground13: return orientation = Orientation::Inground12;
        case Orientation::Inground14: return orientation = Orientation::Inground13;
        case Orientation::Inground15: return orientation = Orientation::Inground14;
    }
}

Player player = { 0 };

Rectangle editorPreviewRectangle = {10, 10, 100, 30 };
Rectangle editorLevelnameRectangle = { SCREEN_WIDTH - 110, 10, 100, 30 };

Vector2 editorLastSelectedSquare;
int editorCurrentTileType = 1;
Orientation editorCurrentOrientation = Orientation::Top;
std::string editorLevelChain;

std::vector<EnvItem> envItems = {
    {{ 290, 440, 20, 20 }, EnvItemType::Crystal },
    {{ 0, 0, 1000, 400 }, EnvItemType::Nonsolid, { LIGHTGRAY } },
    {{ 0, 400, 200, 200 }, EnvItemType::Solid, { GRAY } },
    {{ 400, 400, 200, 200 }, EnvItemType::Solid, { GRAY } },
    {{ 800, 400, 200, 200 }, EnvItemType::Solid, { GRAY } },
    {{ 250, 300, 100, 10 }, EnvItemType::Solid, { GRAY } },
    {{ 650, 300, 100, 20 }, EnvItemType::Hazard, { RED } },
    {{ 200, 580, 200, 20 }, EnvItemType::Hazard, { RED } },
    {{ 600, 580, 200, 20 }, EnvItemType::Hazard, { RED } },
    {{ -1000, 1000, 3000, 20 }, EnvItemType::Hazard, { RED } }
};

std::vector<char> levelName;

class Button
{
private:
    Rectangle b_rect;
    Color b_color;

public:
    Button(Rectangle rect, Color color)
    {
        b_rect = rect;
        b_color = color;
    }

    bool isDown() const
    {
        return IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), b_rect);
    }

    bool isPressed() const
    {
        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), b_rect);
    }

    void render() const
    {
        DrawRectangleRec(b_rect, b_color);
    }
};

std::vector<Button> editorButtons = {
   { { 300, 10, 30, 30 }, BLACK },
   { { 330, 10, 30, 30 }, RED },
   { { 360, 10, 30, 30 }, LIME },
   { { 390, 10, 30, 30 }, BLUE },
   { { 420, 10, 30, 30 }, YELLOW },
   { { 450, 10, 30, 30 }, ORANGE }
};

class Tileset
{
private:
    Image t_rawImg{};
    std::vector<Texture2D> t_imgs{};
public:
    Tileset(const std::string& tilesetName)
    {
        t_rawImg = LoadImage((ASSETS_PATH"tilesets/" + tilesetName + ".png").c_str());
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                t_imgs.push_back(LoadTextureFromImage(ImageFromImage(t_rawImg, { static_cast<float>(8 * j), static_cast<float>(8 * i), 8, 8 })));
            }
        }
        for (int i = 0; i < 15; i++)
        {
            t_imgs.push_back(LoadTextureFromImage(ImageFromImage(t_rawImg, { 32, static_cast<float>(8 * i), 8, 8 })));
        }
        for (int i = 0; i < 15; i++)
        {
            t_imgs.push_back(LoadTextureFromImage(ImageFromImage(t_rawImg, { 40, static_cast<float>(8 * i), 8, 8 })));
        }
        std::cout << t_imgs.size() << std::endl;
    }
    Texture2D get(const Orientation& orientation, int variant)
    {
        switch (orientation)
        {
            case Orientation::Top:          return t_imgs[0 + variant];
            case Orientation::Bottom:       return t_imgs[4 + variant];
            case Orientation::Left:         return t_imgs[8 + variant];
            case Orientation::Right:        return t_imgs[12 + variant];
            case Orientation::HorizMiddle:  return t_imgs[16 + variant];
            case Orientation::VertMiddle:   return t_imgs[20 + variant];
            case Orientation::VertTop:      return t_imgs[24 + variant];
            case Orientation::VertBottom:   return t_imgs[28 + variant];
            case Orientation::HorizLeft:    return t_imgs[32 + variant];
            case Orientation::HorizRight:   return t_imgs[36 + variant];
            case Orientation::Block:        return t_imgs[40 + variant];
            case Orientation::TopLeft:      return t_imgs[44 + variant];
            case Orientation::TopRight:     return t_imgs[48 + variant];
            case Orientation::BottomLeft:   return t_imgs[52 + variant];
            case Orientation::BottomRight:  return t_imgs[56 + variant];
            case Orientation::IGBR:         return t_imgs[60];
            case Orientation::IGUR:         return t_imgs[61];
            case Orientation::IGBL:         return t_imgs[62];
            case Orientation::IGUL:         return t_imgs[63];
            case Orientation::IGRight:      return t_imgs[64];
            case Orientation::IGTop:        return t_imgs[65];
            case Orientation::IGLeft:       return t_imgs[66];
            case Orientation::IGBottom:     return t_imgs[67];
            case Orientation::IGSBL:        return t_imgs[68];
            case Orientation::IGSBR:        return t_imgs[69];
            case Orientation::IGSUR:        return t_imgs[70];
            case Orientation::IGSUL:        return t_imgs[71];
            case Orientation::IGCircle:     return t_imgs[72];
            case Orientation::IGDiagDR:     return t_imgs[73];
            case Orientation::IGDiagUR:     return t_imgs[74];
            case Orientation::Inground1:    return t_imgs[75];
            case Orientation::Inground2:    return t_imgs[76];
            case Orientation::Inground3:    return t_imgs[77];
            case Orientation::Inground4:    return t_imgs[78];
            case Orientation::Inground5:    return t_imgs[79];
            case Orientation::Inground6:    return t_imgs[80];
            case Orientation::Inground7:    return t_imgs[81];
            case Orientation::Inground8:    return t_imgs[82];
            case Orientation::Inground9:    return t_imgs[83];
            case Orientation::Inground10:   return t_imgs[84];
            case Orientation::Inground11:   return t_imgs[85];
            case Orientation::Inground12:   return t_imgs[86];
            case Orientation::Inground13:   return t_imgs[87];
            case Orientation::Inground14:   return t_imgs[88];
            case Orientation::Inground15:   return t_imgs[89];
        }
    }
};

void OnDeath(Player& p)
{
    for (auto& ei : envItems)
    {
        if (ei.type == EnvItemType::Crystal)
        {
            ei.respawning = false;
            ei.respawnTimer = 0.0f;
        }
    }
    p.rect.x = p.respawnPoint.x;
    p.rect.y = p.respawnPoint.y;
    p.speed.x = 0;
    p.speed.y = 0;
}

CollisionResult CollisionFinnaHappen(EnvItem& ei, int envIndex, float delta)
{
    if (ei.type == EnvItemType::Nonsolid)
    {
        return CollisionResult::None;
    }

    Rectangle oldPos = player.rect;
    Rectangle newPosX = { oldPos.x + player.speed.x * delta, oldPos.y, oldPos.width, oldPos.height };
    Rectangle newPosY = { oldPos.x, oldPos.y + player.speed.y * delta, oldPos.width, oldPos.height };
    bool collidedX = CheckCollisionRecs(newPosX, ei.rect);
    bool collidedY = CheckCollisionRecs(newPosY, ei.rect);

    if (collidedX || collidedY)
    {
        switch (ei.type)
        {
            case EnvItemType::Crystal:
            {
                if (!ei.respawning)
                {
                    ei.respawning = true;
                    ei.respawnTimer = 5.0f;
                    player.dashRemaining = PLAYER_DASH_DIST;
                }

                return CollisionResult::DashRecharge;
            }
            case EnvItemType::Hazard:
            {
                return CollisionResult::Death;
            }
            case SwitchLevel:
                return CollisionResult::SwitchLevel;
        }
    }

    if (collidedX)
    {
        Rectangle collisionRecX = GetCollisionRec(newPosX, ei.rect);
        if (oldPos.x < ei.rect.x)
        {
            player.rect.x -= collisionRecX.width - player.speed.x * delta;
        }
        else
        {
            player.rect.x += collisionRecX.width + player.speed.x * delta;
        }
        player.speed.x = 0;
        if (player.dashing)
        {
            player.dashRemaining = 0.0f;
        }
        player.dashing = false;
    }
    if (collidedY)
    {
        Rectangle collisionRecY = GetCollisionRec(newPosY, ei.rect);
        if (oldPos.y < ei.rect.y)
        {
            player.canJump = true;
            player.dashRemaining = PLAYER_DASH_DIST;
            player.rect.y -= collisionRecY.height - player.speed.y * delta;
            player.falling = false;
        }
        else
        {
            if (player.dashing)
            {
                player.dashRemaining = 0.0f;
            }
            player.rect.y += collisionRecY.height + player.speed.y * delta;
        }
        player.speed.y = 0;
        player.dashing = false;
    }

    if (collidedX)
    {
        if (player.rect.y + player.rect.height / 2 >= ei.rect.y)
        {
            if (IsKeyDown(KEY_Z))
            {
                player.climbing = true;
                player.climbingOn = envIndex;
                player.speed.y = 0;
                player.speed.x = 0;
            }
            else
            {
                player.climbing = false;
                player.climbingOn = -1;
            }
        }
    }

    return CollisionResult::None;
}

void UpdatePlayer(float delta)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(GetMousePosition(), { 10, 10, 100, 30 }))
        {
            gameScene = LevelSelect;
        }
    }

    player.frameCounter++;

    if (player.frameCounter >= (60/8))
    {
        player.frameCounter = 0;
        player.currentFrame++;

        if (player.currentFrame > 15) player.currentFrame = 0;
    }

    if (player.animationFramesRemaining > 0)
    {
        player.rect.x += player.animationMovement.x;
        player.rect.y += player.animationMovement.y;
        player.animationFramesRemaining--;

        std::array<Vector2, 5> newHair = {};

        Vector2 stepPerSegment = { 0, 4 };
        stepPerSegment.x = player.facingLeft ? 2 : -2;

        int currentHairOffset = player.facingLeft ? -1 : 11;
        Vector2 currentHairCoords = { player.rect.x + (int)currentHairOffset, player.rect.y - 5 };

        for (int i = 0; i < 5; i++)
        {
            if ((player.speed.x == 0 && player.speed.y == 0 && player.currentFrame % 9 < 4)
                || (player.currentFrame % 12 == 0 || player.currentFrame % 12 == 5 || player.currentFrame % 12 == 10 || player.currentFrame % 12 == 11)
                    )
            {
                currentHairCoords.y -= 2;
            }
            float offsetRatio = 0.1f * i / 3.0f;
            player.hair[i] = { currentHairCoords.x + (offsetRatio * 30) + stepPerSegment.x * i, currentHairCoords.y + (offsetRatio * 30) + stepPerSegment.y * i };
        }
        return;
    }

    if (player.speed.x < 0)
    {
        player.facingLeft = true;
    }
    else if (player.speed.x > 0)
    {
        player.facingLeft = false;
    }

    if (player.canMove && !player.dashing)
    {
        if (IsKeyDown(KEY_LEFT)) player.speed.x = -200;
        if (IsKeyDown(KEY_RIGHT)) player.speed.x = 200;
        if (IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_RIGHT)) player.speed.x = 0;
    }
    if (IsKeyPressed(KEY_SPACE) && player.canJump)
    {
        player.speed.y = -PLAYER_JUMP_SPD;
        player.canJump = false;
    }
    if (IsKeyPressed(KEY_X) && player.dashRemaining > 0.0f)
    {
        player.dashing = true;
        if (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_UP))
        {
            player.speed.x = -PLAYER_DASH_SPD_DIAG;
            player.speed.y = -PLAYER_DASH_SPD_DIAG;
            player.dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_UP))
        {
            player.speed.x = PLAYER_DASH_SPD_DIAG;
            player.speed.y = -PLAYER_DASH_SPD_DIAG;
            player.dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_DOWN))
        {
            player.speed.x = PLAYER_DASH_SPD_DIAG;
            player.speed.y = PLAYER_DASH_SPD_DIAG;
            player.dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_DOWN))
        {
            player.speed.x = -PLAYER_DASH_SPD_DIAG;
            player.speed.y = PLAYER_DASH_SPD_DIAG;
            player.dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            player.speed.x = -PLAYER_DASH_SPD;
            player.speed.y = 0;
            player.dashRemaining -= PLAYER_DASH_SPD * delta;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            player.speed.x = PLAYER_DASH_SPD;
            player.speed.y = 0;
            player.dashRemaining -= PLAYER_DASH_SPD * delta;
        }
        else if (IsKeyDown(KEY_UP))
        {
            player.speed.x = 0;
            player.speed.y = -PLAYER_DASH_SPD;
            player.dashRemaining -= PLAYER_DASH_SPD * delta;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            player.speed.x = 0;
            player.speed.y = PLAYER_DASH_SPD;
            player.dashRemaining -= PLAYER_DASH_SPD * delta;
        }
    }

    if (player.dashing && player.dashRemaining <= 0.0f)
    {
        player.dashing = false;
        player.speed.x = 0;
        player.speed.y = 0;
    }

    if (player.climbing)
    {
        EnvItem& climbingEnvItem = envItems[player.climbingOn];
        if (IsKeyDown(KEY_UP))
        {
            if (player.rect.y > climbingEnvItem.rect.y)
            {
                player.stamina -= delta * 45.45f;
                player.speed.y = -100;
            }
            else
            {
                EnvItem newClimbingEI;
                bool switchedEI = false;
                for (int i = 0; i < envItems.size(); i++)
                {
                    const auto& ei = envItems[i];
                    if (ei.type != Solid)
                    {
                        continue;
                    }
                    if (ei.rect.x != climbingEnvItem.rect.x)
                    {
                        continue;
                    }
                    if (ei.rect.y + ei.rect.height == climbingEnvItem.rect.y)
                    {
                        player.climbingOn = i;
                        switchedEI = true;
                        break;
                    }
                }
                if (switchedEI == false)
                {
                    player.speed.y = 0;
                }
            }
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            if (player.rect.y < envItems[player.climbingOn].rect.y + envItems[player.climbingOn].rect.height - player.rect.height)
            {
                player.speed.y = 100;
            }
            else
            {
                EnvItem newClimbingEI;
                bool switchedEI = false;
                for (int i = 0; i < envItems.size(); i++)
                {
                    const auto& ei = envItems[i];
                    if (ei.type != Solid)
                    {
                        continue;
                    }
                    if (ei.rect.x != climbingEnvItem.rect.x)
                    {
                        continue;
                    }
                    if (ei.rect.y == climbingEnvItem.rect.y + climbingEnvItem.rect.height)
                    {
                        player.climbingOn = i;
                        switchedEI = true;
                        break;
                    }
                }
                if (switchedEI == false)
                {
                    player.speed.y = 0;
                }
            }
        }
        else
        {
            if (player.stamina > 0)
            {
                player.stamina -= delta * 10.0f;
            }
            player.speed.y = 0;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            bool isLeft = player.rect.x < envItems[player.climbingOn].rect.x;
            player.speed.y = -PLAYER_JUMP_SPD;
            player.speed.x = isLeft ? -200.0f : 200.0f;
            player.climbing = false;
            player.climbingOn = -1;
            player.canMove = false;
            player.moveTimer = 0.1f;
            player.stamina -= 27.5f;
        }
    }

    player.canJump = false;
    for (int i = 0; i < envItems.size(); i++)
    {
        auto& ei = envItems[i];
        CollisionResult collisionResult = CollisionFinnaHappen(ei, i, delta);
        switch (collisionResult)
        {
            case CollisionResult::Death:
                OnDeath(player);
                break;
            case CollisionResult::SwitchLevel:
                LoadLevelFile(ei.levelName);
                break;
        }
        if (ei.type == EnvItemType::Crystal && ei.respawning)
        {
            ei.respawnTimer -= delta;
            if (ei.respawnTimer <= 0.0f)
            {
                ei.respawning = false;
            }
        }
    }

    if (!player.climbing)
    {
        player.rect.x += player.speed.x * delta;
    }
    else
    {
        if (IsKeyReleased(KEY_Z))
        {
            player.climbing = false;
            player.climbingOn = -1;
            player.speed.y = 0;
            player.speed.x = 0;
        }
    }

    player.rect.y += player.speed.y * delta;

    if (player.dashing)
    {
        player.dashRemaining -= PLAYER_DASH_SPD_DIAG * delta;
    }
    else if (!player.climbing)
    {
        player.speed.y += G * delta;
    }

    if (!player.canMove)
    {
        player.moveTimer -= delta;
        if (player.moveTimer <= 0.0f)
        {
            player.canMove = true;
        }
    }
    std::array<Vector2, 5> newHair = {};

    Vector2 stepPerSegment = { 0, 4 };
    stepPerSegment.x = player.facingLeft ? 2 : -2;

    int currentHairOffset = player.facingLeft ? -1 : 11;
    Vector2 currentHairCoords = { player.rect.x + (int)currentHairOffset, player.rect.y - 5 };

    for (int i = 0; i < 5; i++)
    {
        if ((player.speed.x == 0 && player.speed.y == 0 && player.currentFrame % 9 < 4)
            || (player.currentFrame % 12 == 0 || player.currentFrame % 12 == 5 || player.currentFrame % 12 == 10 || player.currentFrame % 12 == 11)
            )
        {
            currentHairCoords.y -= 2;
        }
        float offsetRatio = 0.1f * i / 3.0f;
        player.hair[i] = { currentHairCoords.x + (offsetRatio * 30) + stepPerSegment.x * i, currentHairCoords.y + (offsetRatio * 30) + stepPerSegment.y * i };
    }
}

// Load the file currently in the editor
void LoadEditorLevel(Camera2D& camera)
{
    std::string levelStr;

    for (char i : levelName)
    {
        levelStr += i;
    }

    std::ofstream levelFile;
    levelFile.open(SAVES_PATH + levelStr + ".txt");
    levelFile << levelEditor.dump();
    levelFile.close();

    LoadLevelFile(levelStr);
}

void AnimatePlayerEnteringLevel()
{
    Vector2 displacement = { player.respawnPoint.x - player.rect.x, player.respawnPoint.y - player.rect.y };
    player.animationMovement = { displacement.x / 15, displacement.y / 15 };
    player.animationFramesRemaining = 15;
}

// Load a level based on a file
void LoadLevelFile(const std::string& file)
{
    std::cout << "Loading " << file << ".txt" << std::endl;

    std::string fileContent;
    std::getline(std::ifstream(SAVES_PATH + file + ".txt"), fileContent, '\0');

    levelEditor = json::parse(fileContent);

    envItems.clear();
    for (auto& [x, row] : levelEditor.items())
    {
        for (auto& [y, blockData] : row.items())
        {
            bool oldFileFormat = blockData.is_number();
            int blockType = oldFileFormat ? blockData.get<int>() : blockData["type"].get<int>();
            if (blockType == 1)
            {
                EnvItem newItem;
                newItem.rect = { std::stof(x) * 15, std::stof(y) * 15, 15, 15 };
                newItem.type = EnvItemType::Solid;
                newItem.color = GRAY;
                newItem.orientation = blockData["orientation"].get<Orientation>();
                envItems.push_back(newItem);
            }
            else if (blockType == 2)
            {
                envItems.push_back({ { std::stof(x) * 15, std::stof(y) * 15, 15, 15 }, EnvItemType::Hazard, { RED } });
            }
            else if (blockType == 3)
            {
                Vector2 newVec = { std::stof(x) * 15, std::stof(y) * 15 };
                player.respawnPoint = newVec;
            }
            else if (blockType == 4)
            {
                EnvItem switchItem;
                switchItem.rect = { std::stof(x) * 15, std::stof(y) * 15, 15, 15 };
                switchItem.type = EnvItemType::SwitchLevel;
                switchItem.levelName = blockData["switchName"].get<std::string>();
                envItems.push_back(switchItem);
            }
            else if (blockType == 5)
            {
                EnvItem newItem;
                newItem.rect = { std::stof(x) * 15, std::stof(y) * 15, 15, 15 };
                newItem.type = EnvItemType::LevelEntry;
                newItem.orientation = blockData["orientation"].get<Orientation>();
                envItems.push_back(newItem);
            }
            else if (blockType == 6)
            {
                Vector2 newVec = { std::stof(x) * 15, std::stof(y) * 15 };
                player.rect.x = newVec.x;
                player.rect.y = newVec.y;
                EnvItem newItem;
                newItem.rect = { std::stof(x) * 15, std::stof(y) * 15, 15, 15 };
                newItem.type = EnvItemType::LevelEntrySpawn;
                newItem.orientation = blockData["orientation"].get<Orientation>();
                envItems.push_back(newItem);
            }
        }
    }

    player.speed.x = 0;
    player.speed.y = 0;

    gameScene = Game;

    AnimatePlayerEnteringLevel();
}

void LoadFileEditor (const std::string& file)
{
    std::string fileContent;
    std::getline(std::ifstream(SAVES_PATH + file + ".txt"), fileContent, '\0');

    levelEditor = json::parse(fileContent);

    gameScene = LevelEditor;
}

void UpdateLevelCamera(Camera2D& camera)
{
    if (IsKeyDown(KEY_LEFT))
    {
        camera.target.x -= 1;
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        camera.target.x += 1;
    }
    else if (IsKeyDown(KEY_UP))
    {
        camera.target.y -= 1;
    }
    else if (IsKeyDown(KEY_DOWN)) {
        camera.target.y += 1;
    }
    if (IsKeyPressed(KEY_Q))
    {
        --editorCurrentOrientation;
    }
    if (IsKeyPressed(KEY_E))
    {
        ++editorCurrentOrientation;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), editorPreviewRectangle))
    {
        if (levelName.empty())
        {
            gameScene = LevelSelect;
            return;
        }
        LoadEditorLevel(camera);
        gameScene = Game;
        return;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        int buttonIdx = 1;
        for (const auto& button : editorButtons)
        {
            if (button.isPressed())
            {
                if (buttonIdx == 4)
                {
                    gameScene = LevelSelectEditor;
                }
                else
                {
                    editorCurrentTileType = buttonIdx;
                    break;
                }
            }
            buttonIdx++;
        }
    }
    else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
//        Vector2 offsetPos = { mousePos.x + camera.offset.x, mousePos.y + camera.offset.y };
        float coordX = (int)mousePos.x / 15;
        float coordY = (int)mousePos.y / 15;
        Vector2 coords = { coordX, coordY };
        if (coords.x == editorLastSelectedSquare.x && coords.y == editorLastSelectedSquare.y)
        {
            return;
        }
        else
        {
            editorLastSelectedSquare = coords;
        }
        EnvItemJson newItem;
        switch (editorCurrentTileType)
        {
            case 1:
                newItem.type = Solid;
                break;
            case 2:
                newItem.type = Hazard;
                break;
            case 3:
                newItem.type = RespawnPoint;
                break;
            case 4:
                newItem.type = SwitchLevel;
                newItem.switchName = editorLevelChain;
                break;
            case 5:
                newItem.type = LevelEntry;
                break;
            case 6:
                newItem.type = LevelEntrySpawn;
                newItem.enteringFrom = editorLevelChain;
                break;
            default:
                break;
        }
        json newItemJson;
        newItemJson["type"] = newItem.type;
        newItemJson["switchName"] = editorLevelChain;
        newItemJson["orientation"] = editorCurrentOrientation;
        if (levelEditor.contains(std::to_string(coords.x)))
        {
            levelEditor[std::to_string(coords.x)][std::to_string(coords.y)] = newItemJson;
        }
        else
        {
            levelEditor[std::to_string(coords.x)] = {};
            levelEditor[std::to_string(coords.x)][std::to_string(coords.y)] = newItemJson;
        }
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
//        Vector2 offsetPos = { mousePos.x + camera.offset.x, mousePos.y + camera.offset.y };
        float coordX = (int)mousePos.x / 15;
        float coordY = (int)mousePos.y / 15;
        Vector2 coords = { coordX, coordY };
        if (levelEditor.contains(std::to_string(coords.x)))
        {
            levelEditor[std::to_string(coords.x)].erase(std::to_string(coords.y));
        }
    }
};

LevelSelectResult UpdateLevelSelect()
{
    std::vector<Rectangle> rects;
    std::vector<Rectangle> editorRects;

    float row = 0.0f;
    for (const auto& save : std::filesystem::directory_iterator(SAVES_PATH))
    {
        rects.push_back({ 10, 10 + (++row * 50), 100, 30 });
        editorRects.push_back({ 120, 10 + (row * 50), 35, 30 });
    }

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mousePos, { 10, 10, 100, 30 }))
        {
            levelEditor.clear();
            gameScene = LevelEditor;
        }

        int saveNum = 0;
        for (const auto& save : std::filesystem::directory_iterator(SAVES_PATH))
        {
            if (CheckCollisionPointRec(mousePos, rects[saveNum]))
            {
                std::cout << "Clicked " << save.path().stem().string() << std::endl;
                return { LevelSelectResultType::LoadLevel, save.path().stem().string() };
            }
            if (CheckCollisionPointRec(mousePos, editorRects[saveNum]))
            {
                return { LevelSelectResultType::EditLevel, save.path().stem().string() };
            }
            saveNum++;
        }
    }

    return { LevelSelectResultType::None, "" };
};

LevelSelectResult UpdateLevelSelectEditor()
{
    std::vector<Rectangle> rects;

    float row = 0.0f;
    for (const auto& save : std::filesystem::directory_iterator(SAVES_PATH))
    {
        rects.push_back({ 10, 10 + (row++ * 50), 100, 30 });
    }

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        int saveNum = 0;
        for (const auto& save : std::filesystem::directory_iterator(SAVES_PATH))
        {
            if (CheckCollisionPointRec(mousePos, rects[saveNum]))
            {
                return { LevelSelectResultType::LoadLevel, save.path().stem().string() };
            }
            saveNum++;
        }
    }

    return { LevelSelectResultType::None, "" };
};

void UpdateParticles(std::vector<CityParticle>& particles)
{
    int newParticleChance = rand() % 4;
    if (newParticleChance == 0)
    {
        int colorInt = rand() % 3;
        Color newColor = colorInt == 0 ? WHITE : colorInt == 1 ? LIGHTGRAY : PURPLE;
        float yDir = ((float)rand() / (float)RAND_MAX) - 0.5;
        particles.push_back({ { SCREEN_WIDTH, static_cast<float>(rand() % SCREEN_HEIGHT) }, newColor, yDir });
    }
    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i].pos.x <= 0)
        {
            particles.erase(particles.begin() + i);
        }
        else
        {
            particles[i].pos.x -= 8;
            particles[i].pos.y += particles[i].yDir;
        }
    }
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    std::array<std::string, 12> walkPaths;

    for (int i = 0; i < 12; i++)
    {
        if (i < 10)
        {
            walkPaths[i] = ASSETS_PATH"sprites/player/walk0" + std::to_string(i) + ".png";
        }
        else
        {
            walkPaths[i] = ASSETS_PATH"sprites/player/walk" + std::to_string(i) + ".png";
        }
    }

    InitWindow(screenWidth, screenHeight, "raylib [core] example - mouse input");

    std::array<Texture2D, 12> walkSprites{};

    for (int i = 0; i < walkPaths.size(); i++)
    {
        walkSprites[i] = LoadTexture(walkPaths[i].c_str());
    }

    std::array<Texture2D, 9> idleSprites{};

    for (int i = 0; i < idleSprites.size(); i++)
    {
        idleSprites[i] = LoadTexture((ASSETS_PATH"sprites/player/idle0" + std::to_string(i) + ".png").c_str());
    }

    std::array<Texture2D, 15> climbingSprites{};

    for (int i = 0; i < climbingSprites.size(); i++)
    {
        if (i < 10)
        {
            climbingSprites[i] = LoadTexture((ASSETS_PATH"sprites/player/climb0" + std::to_string(i) + ".png").c_str());
        }
        else
        {
            climbingSprites[i] = LoadTexture((ASSETS_PATH"sprites/player/climb" + std::to_string(i) + ".png").c_str());
        }
    }

    const Texture2D rightSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_right00.png");
    const Texture2D upSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_up00.png");
    const Texture2D leftSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_left00.png");
    const Texture2D downSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_down00.png");

    const Texture2D hair = LoadTexture(ASSETS_PATH"sprites/player/hair00.png");

    const Texture2D background = LoadTexture(ASSETS_PATH"bgs/bg1.png");

    Tileset snowTileset = Tileset("snow");

    const std::array<const Texture2D*, 4> spikeSprites = { &upSpikes, &rightSpikes, &downSpikes, &leftSpikes };

    std::vector<CityParticle> particles;

    player.rect = {0, 0, 40, 40 };
    player.speed.x = 0;
    player.speed.y = 0;
    player.respawnPoint = { 20, 300 };
    player.canJump = false;
    player.canMove = true;
    player.currentFrame = 0;
    player.frameCounter = 0;
    player.stamina = 110.0f;

    Camera2D camera = { 0 };
    camera.target = { player.rect.x, player.rect.y };
    camera.offset = { screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        if (gameScene == Game)
        {
            UpdatePlayer(deltaTime);
            UpdateParticles(particles);
            camera.offset = { 0.0f, 0.0f };
        }
        else if (gameScene == LevelEditor)
        {
            UpdateLevelCamera(camera);
            camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        }
        else if (gameScene == LevelSelect)
        {
            auto loadFile = UpdateLevelSelect();
            if (loadFile.type == LevelSelectResultType::LoadLevel)
            {
                LoadLevelFile(loadFile.filename);
            }
            else if (loadFile.type == LevelSelectResultType::EditLevel)
            {
                LoadFileEditor(loadFile.filename);
            }
            camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        }
        else if (gameScene == LevelSelectEditor)
        {
            auto loadFile = UpdateLevelSelectEditor();
            if (loadFile.type != LevelSelectResultType::None)
            {
                editorLevelChain = loadFile.filename;
                gameScene = LevelEditor;

                editorCurrentTileType = 4;
            }

            camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        if (gameScene == Game)
        {
            ClearBackground(BLACK);
            DrawTextureEx(background, { 0, 0 }, 0, (float)SCREEN_WIDTH / background.width, WHITE);
        }
        else
        {
            ClearBackground(LIGHTGRAY);
        }

        BeginMode2D(camera);

        if (gameScene == Game)
        {
            for (const auto& ei : envItems) {
                int variant = (int)(ei.rect.x + ei.rect.y) % 4;
                switch (ei.type)
                {
                    case EnvItemType::Crystal:
                        DrawRectangleRec(ei.rect, ei.respawning ? LIME : GREEN);
                        break;
                    case EnvItemType::Nonsolid:
                        DrawRectangleRec(ei.rect, ei.color);
                        break;
                    case EnvItemType::Solid:
//                        DrawRectangleRec(ei.rect, ei.color);
                        DrawTexturePro(snowTileset.get(ei.orientation, variant), { 0, 0, 8, 8 }, ei.rect, { 0, 0 }, 0, WHITE);
                        break;
                    case EnvItemType::LevelEntrySpawn:
                    case EnvItemType::LevelEntry:
                        if (player.animationFramesRemaining <= 0)
                        {
                            DrawTexturePro(snowTileset.get(ei.orientation, variant), { 0, 0, 8, 8 }, ei.rect, { 0, 0 }, 0, WHITE);
                        }
                        break;
                    case EnvItemType::Hazard:
                        DrawTextureQuad(*spikeSprites[ei.direction], { ei.rect.width / 20, 1 }, { 0, 0 }, ei.rect, WHITE);
                        break;
                }
            }

            Rectangle playerRect = { player.rect.x, player.rect.y, 40, 40 };

            Color playerColor;

            if (player.dashing) playerColor = ORANGE;
            else if (player.dashRemaining <= 0) playerColor = BLUE;
            else playerColor = RED;

            for (int i = 0; i < player.hair.size(); i++)
            {
                const Vector2 hairCoord = player.hair[i];
                DrawTextureEx(hair, hairCoord, 0, 3.0f - (i * 0.2f), player.dashRemaining < PLAYER_DASH_DIST ? HAIR_COLOR_DASHING : HAIR_COLOR_NORMAL);
            }

            //        DrawRectangleRec(playerRect, playerColor);
            if (player.speed.x != 0)
            {
                DrawTexturePro(walkSprites[player.currentFrame % 11], { 7, 18, 14.0f * (player.facingLeft ? -1.0f : 1.0f), 14 }, playerRect, { 0, 0 }, 0, WHITE);
            }
            else if (player.climbing)
            {
                DrawTexturePro(climbingSprites[player.currentFrame % 14], { 7, 18, 14.0f * (player.facingLeft ? -1.0f : 1.0f), 14 }, playerRect, { 0, 0 }, 0, WHITE);
            }
            else
            {
                DrawTexturePro(idleSprites[player.currentFrame % 9], { 7, 18, 14.0f * (player.facingLeft ? -1.0f : 1.0f), 14 }, playerRect, { 0, 0 }, 0, WHITE);
            }
        }
        EndMode2D();

        if (gameScene == LevelEditor)
        {
            int boxHeight = 15;
            for (int i = 0; i <= SCREEN_HEIGHT / boxHeight; i++)
            {
                DrawLine(0, i * boxHeight, SCREEN_WIDTH, i * boxHeight, BLACK);
            }
            int boxWidth = 15;
            for (int i = 0; i <= SCREEN_WIDTH / boxWidth; i++)
            {
                DrawLine(i * boxWidth, 0, i * boxWidth, SCREEN_HEIGHT, BLACK);
            }

            for (auto& [x, row] : levelEditor.items())
            {
                for (auto& [y, blockData] : row.items())
                {
                    int blockType = blockData["type"];
                    Rectangle boxRect = { std::stof(x) * boxWidth, std::stof(y) * boxHeight, static_cast<float>(boxWidth), static_cast<float>(boxHeight) };
                    if (blockType == 1)
                    {
                        int variant = (int)(std::stoi(x) +std::stoi(y)) % 4;
                        DrawTexturePro(snowTileset.get(blockData["orientation"].get<Orientation>(), variant), { 0, 0, 8, 8 }, boxRect, { 0, 0 }, 0, WHITE);
                    }
                    else if (blockType == 2)
                    {
                        DrawRectangleRec(boxRect, RED);
                    }
                    else if (blockType == 3)
                    {
                        DrawRectangleRec(boxRect, GREEN);
                    }
                    else if (blockType == 4)
                    {
                        DrawRectangleRec(boxRect, BLUE);
                    }
                    else if (blockType == 5 || blockType == 6)
                    {
                        int variant = (int)(std::stoi(x) +std::stoi(y)) % 4;
                        DrawTexturePro(snowTileset.get(blockData["orientation"].get<Orientation>(), variant), { 0, 0, 8, 8 }, boxRect, { 0, 0 }, 0, YELLOW);
                    }
                }
            }

            DrawRectangleRec(editorPreviewRectangle, RED);
            DrawText("Save and play", editorPreviewRectangle.x, editorPreviewRectangle.y, 15, BLACK);
            DrawRectangleRec(editorLevelnameRectangle, LIME);

            for (const auto& button : editorButtons)
            {
                button.render();
            }

            if (CheckCollisionPointRec(GetMousePosition(), editorLevelnameRectangle))
            {
                // Get char pressed (unicode character) on the queue
                int key = GetKeyPressed();

                // Check if more characters have been pressed on the same frame
                while (key > 0)
                {
                    // NOTE: Only allow keys in range [32..125]
                    if ((key >= 32) && (key <= 125) && (levelName.size() < 20))
                    {
                        levelName.push_back((char)key);
                    }

                    key = GetKeyPressed();  // Check next character in the queue
                }

                if (IsKeyPressed(KEY_BACKSPACE) && !levelName.empty())
                {
                    levelName.pop_back();
                }
            }

            char drawText[21] = "";

            for (int i = 0; i < levelName.size(); i++)
            {
                drawText[i] = levelName[i];
            }

            drawText[levelName.size()] = '\0';

            DrawText(drawText, editorLevelnameRectangle.x, editorLevelnameRectangle.y, 14, BLACK);
        }
        else if (gameScene == LevelSelect)
        {
            DrawRectangleRec({ 10, 10, 100, 30 }, RED);
            DrawText("Level editor", 10, 10, 24, BLACK);
            std::string path = SAVES_PATH;
            float row = 1.0f;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                DrawRectangleRec({ 10, 10 + (row * 50), 100, 30 }, RED);
                DrawRectangleRec({ 120, 10 + (row * 50), 60, 30 }, RED);
                DrawText(entry.path().stem().string().c_str(), 10, 10 + row * 50, 20, BLACK);
                DrawText("Edit", 120, 10 + row * 50, 20, BLACK);
                row++;
            }
        }
        else if (gameScene == LevelSelectEditor)
        {
            std::string path = SAVES_PATH;
            float row = 0.0f;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                DrawRectangleRec({ 10, 10 + (row * 50), 100, 30 }, RED);
                DrawText(entry.path().stem().string().c_str(), 10, 10 + row * 50, 20, BLACK);
                row++;
            }
        }
        else if (gameScene == Game)
        {
            DrawText(TextFormat("Stamina: %f", player.stamina), 20, SCREEN_HEIGHT - 40, 10, DARKGRAY);
            DrawRectangleRec({ 10, 10, 100, 30 }, RED);
            DrawText("Level select", 10, 10, 20, BLACK);
            for (const auto& p : particles)
            {
                DrawRectangleRec({ p.pos.x, p.pos.y, 3, 3 }, p.color);
            }
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
