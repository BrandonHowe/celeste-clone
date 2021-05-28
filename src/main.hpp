//
// Created by Brandon 2 on 5/27/2021.
//

#ifndef RAYLIB_TEMPLATE_MAIN_HPP
#define RAYLIB_TEMPLATE_MAIN_HPP

enum Scene
{
    Game,
    LevelEditor,
    LevelSelect,
    LevelSelectEditor
};

enum EnvItemType {
    Nonsolid,
    Solid,
    Hazard,
    RespawnPoint,
    SwitchLevel,
    Crystal,
};

enum Direction
{
    Up,
    Right,
    Down,
    Left
};

struct EnvItemJson
{
    EnvItemType type;
    std::string switchName;
};

struct EnvItem {
    Rectangle rect;
    EnvItemType type;
    Color color;
    Direction direction;
    bool respawning;
    float respawnTimer;
    std::string levelName;
};

typedef struct Player {
    Rectangle rect;
    Vector2 speed;
    Vector2 momentum;
    Vector2 respawnPoint;
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
    float stamina;
    std::array<Vector2, 5> hair;
} Player;

enum class CollisionResult
{
    None,
    DashRecharge,
    Death,
    SwitchLevel
};

void OnDeath(Player& p);
CollisionResult CollisionFinnaHappen(EnvItem& ei, int envIndex, float delta);
void UpdatePlayer(float delta);
void LoadEditorLevel(Camera2D& camera);
void LoadLevelFile(const std::string& file);
void UpdateLevelCamera(Camera2D& camera);
std::string UpdateLevelSelect();


#endif //RAYLIB_TEMPLATE_MAIN_HPP
