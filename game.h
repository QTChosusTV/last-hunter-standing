#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <bits/stdc++.h>

using namespace std;

#define __ratio 1.5
#define GRID_SIZE 200
#define GRID_COLOR LIGHTGRAY
#define PLAYER_COLOR (Color){248, 197, 116, 255}
#define MAPSIZE 20
#define MAX_BULLETS 1000
#define MAX_COINS 1000
#define NUM_ENEMY_TYPES 6
#define NUM_WAVES 26
#define MAX_GUNS 7
#define MAX_ITEMS MAX_GUNS + 5
#define MAX_SKILLS 6
#define MAX_LEVEL 6
#define MAX_SOUNDS 10
#define MAX_GRENADES 100

enum class GameState { MENU, GAMEPLAY, GAMEOVER, PAUSE };

struct Enemy {
    Vector2 position;
    int radius;
    Color color;
    Vector2 direction;
    int health;
    int healthMax;
    int type;
    float speed;
    float damage;
};

struct EnemyType {
    int type;
    int radius;
    Color color;
    int health;
    int healthMax;
    float speed;
    float damage;
};

struct Bullet {
    Vector2 position;
    Vector2 direction;
    float damage;
    int lifeTime;
    float speed;
};

struct Coin {
    Vector2 position;
    bool active;
    int type;
};

struct Item {
    const char* name;
    const char* iconPath;
    int price;
    bool purchased;
    int id;
    string type;
};

struct Skill {
    int level;
    int maxLevel;
    const char* description;
};

struct DamageText {
    char text[20];
    Vector2 position;
    int alpha;
    Color color;
};

struct Grenade {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    int state;
};

struct ObjectType {
    string name;
    Texture2D texture;
    Rectangle offsetHitbox;
};

struct Object {
    ObjectType type;
    int id;
    Vector2 position;
    Rectangle hitbox;
    bool hasCollision;
    Rectangle calculateHitbox() {
        return {position.x, position.y, type.offsetHitbox.width, type.offsetHitbox.height};
    }
    int cnt;
};

struct Ranking {
    int currR;
    int currP;
    void UpdatePoints();
    void UpdateRank();
    void DrawRank(Vector2 pos, int upRank);
};

extern float screenWidth;
extern float screenHeight;
extern int lastRankChange;
extern GameState gameState;
extern Font font;
extern Camera2D camera;
extern Ranking currentRank;
extern vector<ObjectType> objTypes;
extern vector<vector<vector<Object>>> objs;
extern Sound waveSound;
extern Sound boom;
extern Texture2D rankk;
extern Texture2D logotechno;
extern Texture2D skillButton;
extern Texture2D iconTexture;
extern float waveTick;
extern int WaveCount;
extern bool showShop;
extern int pointsLeft;
extern long long int points;
extern float numCollectedCoins;
extern int numEnemies;
extern int maxEnemies;
extern int numCoins;
extern int bulletHits;
extern int bulletShot;
extern float accuracy;
extern float playerX;
extern float playerY;
extern float playerRadius;
extern float playerSpeed;
extern float playerHealth;
extern float playerMaxHealth;
extern float playerPointerAngle;
extern int gunID;
extern bool Fired;
extern int CoolDown;
extern bool reloading;
extern int reload;
extern int selectedItem;
extern bool mouseInCombat;
extern float ammoLeftOnGun[MAX_GUNS];
extern int bulletTypeCount[4];
extern int grenadeLeft;
extern int numGrenade;
extern deque<Bullet> bullets;
extern Grenade grenades[MAX_BULLETS];
extern Coin coins[MAX_COINS];
extern deque<Enemy> enemies;
extern Item items[MAX_ITEMS];
extern Skill skills[MAX_SKILLS];
extern Rectangle skillButtons[MAX_SKILLS];
extern Texture2D GunIcon[MAX_ITEMS];
extern Texture2D gunTexture[MAX_GUNS];
extern Sound gunSounds[MAX_GUNS][MAX_SOUNDS];
extern Sound reloadSound[MAX_GUNS];
extern int gunType[MAX_GUNS];
extern int gunMaxAmmo[MAX_GUNS];
extern int gunDamage[MAX_GUNS];
extern int gunReload[MAX_GUNS];
extern int cooldown[MAX_GUNS];
extern int shotgunBullet[MAX_GUNS];
extern int spread[MAX_GUNS];
extern int ammotype[MAX_GUNS];
extern map<string, int> m;
extern Color ammoColor[4];
extern int ammoBuyCount[4];
extern EnemyType enemyTypes[NUM_ENEMY_TYPES];
extern int waveData[NUM_WAVES][NUM_ENEMY_TYPES];
extern int numDamageTexts;
extern DamageText damageTexts[50];
extern vector<int> rrList;
extern vector<string> keyWordRank;
extern bool temp[1000];
extern Sound killStreak[4];
extern int killCountImageCooldown;
extern int lastKillCount;
extern int globalKillCount;
extern int killCount;
extern int pointsUpAnimation;
extern int MapX;
extern int MapY;
extern float __masterVolume;

void InitCamera(Camera2D* camera);
void InitializeEnemyTypes(EnemyType* enemyTypes, int numEnemyTypes);
void ResetGun();
void LoadGameSoundTexture();
void InitSkillButtons();
void InitGrenades();
void Reset();
void WaveSpawn();
void UpdateGameplay();
void SpawnEnemy(int numberEnemy, int radius, int radiusRandom, int enemyType);
void UpdateEnemies();
void UpdateDamageText(DamageText* damageText);

#endif
