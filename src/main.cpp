#include "raylib.h"
#include <iostream>
#include <string>
#include <array>
#include <vector>

constexpr auto SCREEN_WIDTH  = 800;
constexpr auto SCREEN_HEIGHT = 450;

#define G 1000
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define PLAYER_DASH_SPD 500.0f
#define PLAYER_DASH_SPD_DIAG 353.5f
#define PLAYER_DASH_DIST 100.0f

enum EnvItemType {
    Nonsolid,
    Solid,
    Hazard,
    Crystal
};

struct EnvItem {
    Rectangle rect;
    EnvItemType type;

    union
    {
        struct
        {
            Color color;
        } Nonsolid;

        struct
        {
            Color color;
        } Solid;

        struct
        {
            Color color;
        } Hazard;

        struct
        {
            bool respawning;
            float respawnTimer;
        } Crystal;
    };
};

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

typedef struct Player {
    Rectangle rect;
    Vector2 speed;
    Vector2 momentum;
    bool canMove;
    float moveTimer;
    bool canJump;
    bool dashing;
    bool climbing;
    int climbingOn;
    float dashRemaining;
    int currentFrame;
    int frameCounter;
    bool facingLeft;
    bool falling;
} Player;

void CollisionFinnaHappen(EnvItem& ei, int envIndex, Player* p, float delta)
{
    if (ei.type == EnvItemType::Nonsolid)
    {
        return;
    }

    Rectangle oldPos = p->rect;
    Rectangle newPosX = { oldPos.x + p->speed.x * delta, oldPos.y, oldPos.width, oldPos.height };
    Rectangle newPosY = { oldPos.x, oldPos.y + p->speed.y * delta, oldPos.width, oldPos.height };
    bool collidedX = CheckCollisionRecs(newPosX, ei.rect);
    bool collidedY = CheckCollisionRecs(newPosY, ei.rect);

    if ((collidedX || collidedY) && ei.type == EnvItemType::Crystal)
    {
        if (!ei.Crystal.respawning)
        {
            ei.Crystal.respawning = true;
            ei.Crystal.respawnTimer = 5.0f;
            p->dashRemaining = PLAYER_DASH_DIST;
        }

        return;
    }

    if ((collidedX || collidedY) && ei.type == EnvItemType::Hazard)
    {
        p->rect.x = 20;
        p->rect.y = 300;
        p->speed.x = 0;
        p->speed.y = 0;
        return;
    }

    if (collidedX)
    {
        Rectangle collisionRecX = GetCollisionRec(newPosX, ei.rect);
        if (oldPos.x < ei.rect.x)
        {
            p->rect.x -= collisionRecX.width - p->speed.x * delta;
        }
        else
        {
            p->rect.x += collisionRecX.width + p->speed.x * delta;
        }
        p->speed.x = 0;
        if (p->dashing)
        {
            p->dashRemaining = 0.0f;
        }
        p->dashing = false;
    }
    if (collidedY)
    {
        Rectangle collisionRecY = GetCollisionRec(newPosY, ei.rect);
        if (oldPos.y < ei.rect.y)
        {
            p->canJump = true;
            p->dashRemaining = PLAYER_DASH_DIST;
            p->rect.y -= collisionRecY.height - p->speed.y * delta;
            p->falling = false;
        }
        else
        {
            if (p->dashing)
            {
                p->dashRemaining = 0.0f;
            }
            p->rect.y += collisionRecY.height + p->speed.y * delta;
        }
        p->speed.y = 0;
        p->dashing = false;
    }

    if (collidedX)
    {
        if (p->rect.y + p->rect.height / 2 >= ei.rect.y)
        {
            if (IsKeyDown(KEY_Z))
            {
                p->climbing = true;
                p->climbingOn = envIndex;
                p->speed.y = 0;
                p->speed.x = 0;
            }
            else
            {
                p->climbing = false;
                p->climbingOn = -1;
            }
        }
    }
}

void UpdatePlayer(Player *player, std::vector<EnvItem>& envItems, float delta)
{
    player->frameCounter++;

    if (player->frameCounter >= (60/8))
    {
        player->frameCounter = 0;
        player->currentFrame++;

        if (player->currentFrame > 15) player->currentFrame = 0;
    }

    if (player->speed.x < 0)
    {
        player->facingLeft = true;
    }
    else if (player->speed.x > 0)
    {
        player->facingLeft = false;
    }

    if (player->canMove && !player->dashing)
    {
        if (IsKeyDown(KEY_LEFT)) player->speed.x = -200;
        if (IsKeyDown(KEY_RIGHT)) player->speed.x = 200;
        if (IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_RIGHT)) player->speed.x = 0;
    }
    if (IsKeyPressed(KEY_SPACE) && player->canJump)
    {
        player->speed.y = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }
    if (IsKeyPressed(KEY_X) && player->dashRemaining > 0.0f)
    {
        player->dashing = true;
        if (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_UP))
        {
            player->speed.x = -PLAYER_DASH_SPD_DIAG;
            player->speed.y = -PLAYER_DASH_SPD_DIAG;
            player->dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_UP))
        {
            player->speed.x = PLAYER_DASH_SPD_DIAG;
            player->speed.y = -PLAYER_DASH_SPD_DIAG;
            player->dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_DOWN))
        {
            player->speed.x = PLAYER_DASH_SPD_DIAG;
            player->speed.y = PLAYER_DASH_SPD_DIAG;
            player->dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_DOWN))
        {
            player->speed.x = -PLAYER_DASH_SPD_DIAG;
            player->speed.y = PLAYER_DASH_SPD_DIAG;
            player->dashRemaining -= PLAYER_DASH_SPD * delta * 1.414f;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            player->speed.x = -PLAYER_DASH_SPD;
            player->speed.y = 0;
            player->dashRemaining -= PLAYER_DASH_SPD * delta;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            player->speed.x = PLAYER_DASH_SPD;
            player->speed.y = 0;
            player->dashRemaining -= PLAYER_DASH_SPD * delta;
        }
        else if (IsKeyDown(KEY_UP))
        {
            player->speed.x = 0;
            player->speed.y = -PLAYER_DASH_SPD;
            player->dashRemaining -= PLAYER_DASH_SPD * delta;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            player->speed.x = 0;
            player->speed.y = PLAYER_DASH_SPD;
            player->dashRemaining -= PLAYER_DASH_SPD * delta;
        }
    }

    if (player->dashing && player->dashRemaining <= 0.0f)
    {
        player->dashing = false;
        player->speed.x = 0;
        player->speed.y = 0;
    }

    if (player->climbing)
    {
        if (IsKeyDown(KEY_UP) && player->rect.y > envItems[player->climbingOn].rect.y)
        {
            player->speed.y = -100;
        }
        else if (IsKeyDown(KEY_DOWN)
                 && player->rect.y < envItems[player->climbingOn].rect.y + envItems[player->climbingOn].rect.height - player->rect.height)
        {
            player->speed.y = 100;
        }
        else
        {
            player->speed.y = 0;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            bool isLeft = player->rect.x < envItems[player->climbingOn].rect.x;
            player->speed.y = -PLAYER_JUMP_SPD;
            player->speed.x = isLeft ? -200.0f : 200.0f;
            player->climbing = false;
            player->climbingOn = -1;
            player->canMove = false;
            player->moveTimer = 0.1f;
        }
    }

    player->canJump = false;
    for (int i = 0; i < envItems.size(); i++)
    {
        auto& ei = envItems[i];
        CollisionFinnaHappen(ei, i, player, delta);
        if (ei.type == EnvItemType::Crystal && ei.Crystal.respawning)
        {
            ei.Crystal.respawnTimer -= delta;
            if (ei.Crystal.respawnTimer <= 0.0f)
            {
                ei.Crystal.respawning = false;
            }
        }
    }

    if (!player->climbing)
    {
        player->rect.x += player->speed.x * delta;
    }
    else
    {
        if (IsKeyReleased(KEY_Z))
        {
            player->climbing = false;
            player->climbingOn = -1;
            player->speed.y = 0;
            player->speed.x = 0;
        }
    }

    player->rect.y += player->speed.y * delta;

    if (player->dashing)
    {
        player->dashRemaining -= PLAYER_DASH_SPD_DIAG * delta;
    }
    else if (!player->climbing)
    {
        player->speed.y += G * delta;
    }

    if (!player->canMove)
    {
        player->moveTimer -= delta;
        if (player->moveTimer <= 0.0f)
        {
            player->canMove = true;
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

    std::array<Texture2D, 12> walkSprites;

    for (int i = 0; i < walkPaths.size(); i++)
    {
        walkSprites[i] = LoadTexture(walkPaths[i].c_str());
    }

    std::array<Texture2D, 9> idleSprites;

    for (int i = 0; i < idleSprites.size(); i++)
    {
        idleSprites[i] = LoadTexture((ASSETS_PATH"sprites/player/idle0" + std::to_string(i) + ".png").c_str());
    }

    std::array<Texture2D, 15> climbingSprites;

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

//    const Texture2D rightSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_right00.png");
//    const Texture2D upSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_up00.png");
//    const Texture2D leftSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_left00.png");
//    const Texture2D downSpikes = LoadTexture(ASSETS_PATH"sprites/spikes/outline_down00.png");

    Player player = { 0 };
    player.rect = {0, 300, 40, 40 };
    player.speed.x = 0;
    player.speed.y = 0;
    player.canJump = false;
    player.canMove = true;
    player.currentFrame = 0;
    player.frameCounter = 0;

    std::vector<EnvItem> envItems = {
       {{ 290, 440, 20, 20 }, EnvItemType::Crystal },
       {{ 0, 0, 1000, 400 }, EnvItemType::Nonsolid, { LIGHTGRAY } },
       {{ 0, 400, 200, 200 }, EnvItemType::Solid, { GRAY } },
       {{ 400, 400, 200, 200 }, EnvItemType::Solid, { GRAY } },
       {{ 800, 400, 200, 200 }, EnvItemType::Solid, { GRAY } },
       {{ 250, 300, 100, 10 }, EnvItemType::Solid, { GRAY } },
       {{ 650, 300, 100, 10 }, EnvItemType::Hazard, { RED } },
       {{ 200, 580, 200, 20 }, EnvItemType::Hazard, { RED } },
       {{ 600, 580, 200, 20 }, EnvItemType::Hazard, { RED } },
       {{ -1000, 1000, 3000, 20 }, EnvItemType::Hazard, { RED } }
    };

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

        UpdatePlayer(&player, envItems, deltaTime);
        camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        camera.target = { player.rect.x, player.rect.y };

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        for (auto ei : envItems) {
            switch (ei.type)
            {
                case EnvItemType::Crystal:
                    DrawRectangleRec(ei.rect, ei.Crystal.respawning ? LIME : GREEN);
                    break;
                case EnvItemType::Nonsolid:
                    DrawRectangleRec(ei.rect, ei.Nonsolid.color);
                    break;
                case EnvItemType::Solid:
                    DrawRectangleRec(ei.rect, ei.Solid.color);
                    break;
                case EnvItemType::Hazard:
                    DrawRectangleRec(ei.rect, ei.Hazard.color);
                    break;
            }
        }

        Rectangle playerRect = { player.rect.x, player.rect.y, 40, 40 };

        Color playerColor;

        if (player.dashing) playerColor = ORANGE;
        else if (player.dashRemaining <= 0) playerColor = BLUE;
        else playerColor = RED;

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

        EndMode2D();

        DrawText(TextFormat("Climbing: %i", player.climbing), 20, 20, 10, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
