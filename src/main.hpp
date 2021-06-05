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
    LevelEntry,
    LevelEntrySpawn,
    Crystal,
};

enum Direction
{
    Up,
    Right,
    Down,
    Left
};

enum class Orientation
{
    Top,
    Bottom,
    Left,
    Right,
    HorizMiddle,
    VertMiddle,
    VertTop,
    VertBottom,
    HorizLeft,
    HorizRight,
    Block,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    IGBR,
    IGUR,
    IGBL,
    IGUL,
    IGRight,
    IGTop,
    IGLeft,
    IGBottom,
    IGSBL,
    IGSBR,
    IGSUR,
    IGSUL,
    IGCircle,
    IGDiagDR,
    IGDiagUR,
    Inground1,
    Inground2,
    Inground3,
    Inground4,
    Inground5,
    Inground6,
    Inground7,
    Inground8,
    Inground9,
    Inground10,
    Inground11,
    Inground12,
    Inground13,
    Inground14,
    Inground15
};

struct EnvItemJson
{
    EnvItemType type;
    std::string switchName;
    std::string enteringFrom;
};

struct EnvItem {
    Rectangle rect;
    EnvItemType type;
    Color color;
    Direction direction;
    bool respawning;
    float respawnTimer;
    std::string levelName;
    std::string enteringFrom;
    Orientation orientation;
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
    Vector2 animationMovement;
    int animationFramesRemaining;
} Player;

enum class CollisionResult
{
    None,
    DashRecharge,
    Death,
    SwitchLevel
};

enum class LevelSelectResultType
{
    LoadLevel,
    EditLevel,
    None
};

struct LevelSelectResult
{
    LevelSelectResultType type;
    std::string filename;
};

struct CityParticle
{
    Vector2 pos;
    Color color;
    float yDir;
};

void OnDeath(Player& p);
CollisionResult CollisionFinnaHappen(EnvItem& ei, int envIndex, float delta);
void UpdatePlayer(float delta);
void LoadEditorLevel(Camera2D& camera);
void LoadLevelFile(const std::string& file);
void UpdateLevelCamera(Camera2D& camera);
LevelSelectResult UpdateLevelSelect();
void AnimatePlayerEnteringLevel();

#endif //RAYLIB_TEMPLATE_MAIN_HPP
