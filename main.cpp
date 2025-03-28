#include <raylib.h>
#include <bits/stdc++.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "raymath.h"

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
#define byte unsigned char
#define MAX_SOUNDS 10

using namespace std;

typedef struct Enemy{
    Vector2 position;
    int radius;
    Color color;
    Vector2 direction;
    int health;
    int healthMax;
    int type;
    float speed;
    float damage;
    bool active;
} Enemy;

typedef struct EnemyType {
    int type;
    int radius;
    Color color;
    int health;
    int healthMax;
    float speed;
    float damage;
} EnemyType;

typedef struct Bullet{
    Vector2 position;
    Vector2 direction;
    bool active;
    int lifeTime;
    float speed;
} Bullet;

typedef struct Coin{
    Vector2 position;
    bool active;
    int type;
} Coin;

typedef struct Item{
    const char* name;
    const char* iconPath;
    int price;
    bool purchased;
    int id;
    string type;
} Item;

typedef struct Skill{
    int level;
    int maxLevel;
    const char* description;
} Skill;

typedef struct DamageText{
    char text[20];
    Vector2 position;
    int alpha;
    Color color;
} DamageText;

typedef struct Grenade{
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    int state;
} Grenade;

enum GameState {
    MENU,
    GAMEPLAY,
    GAMEOVER,
    PAUSE
};

typedef struct ObjectType{
    string name;
    Texture2D texture;
    Rectangle offsetHitbox;
}ObjectType;


typedef struct Object{
    ObjectType type;
    int id;
    Vector2 position;
    Rectangle hitbox;
    bool hasCollision;
    Rectangle calculateHitbox(){
        return {position.x, position.y, type.offsetHitbox.width, type.offsetHitbox.height};
    }
    int cnt;
}Object;

vector<ObjectType> objTypes(1);

// System Variable
 map<string, int> m = {
 {"gun", 0},
 {"ammo", 1},
 {"other", 2}
 };
 enum GameState gameState = MENU;
 float __masterVolume = 1.0f;
 Font font;
 Sound waveSound;
 bool temp[1000];
 int gunReload[MAX_GUNS];
 int reload;
 int grenadeLeft = 0;
 float waveTick;
 float playerRadius = 15.0f;
 float playerX = GRID_SIZE * MAPSIZE/2;
 float playerY = GRID_SIZE * MAPSIZE/2;
 int Tick = 0;
 bool showShop;
 float playerSpeed = 4;
 float playerHealth = 100;
 float playerMaxHealth = 100;
 float playerPointerAngle;
 int MapX = GRID_SIZE * MAPSIZE;
 int MapY = GRID_SIZE * MAPSIZE;
 int gunID = 0;
 int bulletHits=0; int bulletShot=0; float accuracy;
 int pointsUpAnimation=0;
 Grenade grenade[MAX_BULLETS];

 int bulletTypeCount[4] = {100, 100, 100, 2};
 Bullet bullets[MAX_BULLETS];
 Sound gunSounds[MAX_GUNS][MAX_SOUNDS];
 float screenWidth = 1920 /*- 20*/;
 float screenHeight = 1080 /*- 60*/;


 bool Fired; int killCountImageCooldown = 0;
 int CoolDown; int killCount = 0; int lastKillCount = 0; int globalKillCount = 0;
 int cooldown[MAX_GUNS];
 int gunType[MAX_GUNS];
 Sound reloadSound[MAX_GUNS];
 int ammotype[MAX_GUNS];
 int gunMaxAmmo[MAX_GUNS];
 int gunDamage[MAX_GUNS];
 int bulletSize = 3;
 Texture2D GunIcon[MAX_ITEMS];
 float ammoLeftOnGun[MAX_GUNS];
 int numGrenade;
 int shotgunBullet[MAX_GUNS];
 int spread[MAX_GUNS];
 bool reloading;
 int selectedItem = -1;
 bool isGunIconLoaded = false;
 float ammoLeft;
 int numDamageTexts = 0;
 Sound boom;
 int enemyHitIdx;
 int pointsLeft; long long int points = 0;
 bool mouseInCombat;
 int ammoBuyCount[4] = {10, 10, 10, 10};
 Color ammoColor[4] = {
    GOLD,
    BLUE,
    GREEN,
    RED
 };
 DamageText damageTexts[50] = { 0 };
 Item items[MAX_ITEMS] = {
        {"GPTL", "./Icons/GPTL.png", 0, true, 0,"gun"},
        {"C18", "./Icons/C18.png", 0, true, 1,"gun"},
        {"ST8", "./Icons/ST8.png", 200, false,2, "gun"},
        {"AK47", "./Icons/AK47.png", 500, false, 3,"gun"},
        {"MP220", "./Icons/MP220.png", 1000, false,4, "gun"},
        {"ZX400", "./Icons/ZX400.png", 2000, false, 5,"gun"},
        {"S350", "./Icons/S350.png", 4000, false, 6,"gun"},
        {"9mm", "./Icons/ammo0.png", 25, false,7, "ammo"},
        {"7.62mm", "./Icons/ammo1.png", 50, false,8, "ammo"},
        {"5.56mm", "./Icons/ammo2.png", 100, false,9, "ammo"},
        {"12mm", "./Icons/ammo3.png", 500, false,10, "ammo"},
        {"Grenade", "./Icons/Grenade.png", 1000, false,11, "other"}
    };
 Skill skills[MAX_SKILLS] = {
    {1, MAX_LEVEL, "Magnet: Increase the item pick radius. "},
    {1, MAX_LEVEL, "Damage: Increase the damage dealt to enemies, "},
    {1, MAX_LEVEL, "Speed: Increase movement speed, x2 at level MAX. "},
    {1, MAX_LEVEL, "Fire Rate: Increase shooting speed, +30% rate / level."},
    {1, MAX_LEVEL, "Max Ammo: Increase the maximum ammo capacity, x2 at level 3 and x3 at level MAX."},
    {1, MAX_LEVEL, "Max HP: Increase the Max health point."}
    };
 Rectangle skillButtons[MAX_SKILLS];


//------------------

void SpawnEnemy(Enemy** enemies, int numberEnemy, int radius, int radiusRandom, int enemyType);
void UpdateEnemies(Enemy* enemies, int numEnemies);

// Enemy Variable
EnemyType enemyTypes[NUM_ENEMY_TYPES];
Enemy* enemies = NULL;
int numEnemies = 0;
int maxEnemies = 0;
int lastRankChange = 0;
int numCoins = 0;
Coin coins[MAX_COINS];
float numCollectedCoins = 0.0f;
int WaveCount = 0;
int waveData[NUM_WAVES][NUM_ENEMY_TYPES] = {
        {5, 0, 0, 0, 0, 0},
        {7, 1, 0, 0, 0, 0},
        {10, 3, 0, 0, 0, 0},
        {20, 5, 1, 0, 0, 0} ,
        {17, 6, 3, 0, 0, 0} ,
        {10, 10, 5, 0, 0, 0} ,
        {5, 15, 7, 0, 0, 0},
        {10, 10, 10, 1, 0, 0},
        {7, 8, 9, 3, 0, 0},
        {4, 6, 8, 5, 0, 0},
        {1, 4, 7, 10, 0, 0},
        {0, 2, 6, 15, 0, 0},
        {0, 0, 5, 15, 1, 0},
        {0, 0, 4, 13, 2, 0},
        {0, 0, 3, 10, 3, 0},
        {0, 0, 2, 6, 5, 0},
        {0, 0, 1, 1, 8, 0},
        {0, 0, 0, 0, 13, 0},
        {0, 0, 0, 0, 21, 0},
        {0, 0, 0, 0, 20, 1},
        {0, 0, 0, 0, 18, 2},
        {0, 0, 0, 0, 15, 3},
        {0, 0, 0, 0, 11, 5},
        {0, 0, 0, 0, 6, 8},
        {0, 0, 0, 0, 0, 15},
        {0, 0, 0, 0, 0, 100}


    };


//------------------

//Graphic Variable
Texture2D gunTexture[MAX_GUNS];
Texture2D skillButton;
Texture2D iconTexture;
//------------------
double log_ab(double a, double b){
    return log2(a) / log2(b);
}
vector<int> rrList = {0, 25, 75, 160, 275, 440, 645, 905, 1215, 1580, 2005, 2490, 3045, 3665, 4335, 5120, 5955, 6870, 7865, 8940, 10000, 999999, 999999, 999999};
vector<string> keyWordRank = {"Copper", "Iron", "Gold", "Emerald", "Diamond", "Epic", "Legendary", "???"};
Texture2D rankk;
typedef struct Ranking{
    int currP;
    int currR;
    void UpdatePoints(){
        currP += ((points - currP)/abs(points - currP))*log_ab(abs(points - currP), 1.25);
        lastRankChange = ((points - currP)/abs(points - currP))*log_ab(abs(points - currP), 1.25);
    }
    void UpdateRank(){
        if(currP > rrList[currR])currR++;
        if(currP < rrList[currR])currR--;
    }
    void DrawRank(Vector2 pos, int upRank){
        DrawTexturePro(rankk, {(currR/3)*32, (currR%3)*32, 32, 32}, {pos.x, pos.y, 100, 100}, {16,16}, 0, WHITE);
        string str = "Current rank: " + keyWordRank[currR/3] + " "; for(int i=-1; i<currR%3; i++)str += "I";
        DrawText(str.c_str(), pos.x + 125, pos.y + 20, 20, BLACK );
        DrawText(TextFormat("%d / %d ",currP - rrList[currR] - lastRankChange + (int)(min(upRank, 120)*lastRankChange/120.0f), rrList[currR+1]-rrList[currR]  ), pos.x + 125, pos.y + 45, 20, BLACK);
        string str2 = lastRankChange<0? "-" + to_string(abs(lastRankChange) - (int)(min(upRank, 120)*abs(lastRankChange)/120.0f)):"+"+to_string(lastRankChange - (int)(min(upRank, 120)*lastRankChange/120.0f));
        if(upRank<120)DrawText(str2.c_str(), pos.x + 125, pos.y + 70, 20, BLACK);
    }

}Ranking;

Ranking currentRank;

vector<vector<vector<Object>>> objs(GRID_SIZE, vector<vector<Object>>(GRID_SIZE, vector<Object>(40)));


float CoinSize(int coinType){
    return (coins[coinType].type+1)*5.5f;


}
void InitCamera(Camera2D *camera){
    camera->target = (Vector2){ 0 };
    camera->offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = 0.5f;
}
void UpgradeSkill(int index) {
    if (skills[index].level < MAX_LEVEL && pointsLeft > 0) {
        skills[index].level++;
        pointsLeft--;

    }
}
void DrawSkillButton(Texture2D t, const char* text, bool isEnabled, float div, Vector2 position) {

  const int buttonWidth = 20;
  const int buttonHeight = 26;
  const int buttonParts = 6;

  for (int i = 0; i < buttonParts; i++) {
    // Calculate the source and destination rectangles
    Rectangle srcRec = {i * (float)buttonWidth, 0, buttonWidth, buttonHeight};
    Rectangle dstRec = {position.x + i * buttonWidth, position.y, buttonWidth, buttonHeight};


    if (i<div) {

      srcRec.y = 0;
    } else {

      srcRec.y = buttonHeight;
    }

    Color col = isEnabled? WHITE : Fade(GRAY, 0.5f);
    DrawTexturePro(t, srcRec, dstRec, {0, 0}, 0.0f, col);

  }


}
bool IsMouseHoverRectangle(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}
void DrawHealthBar(float x, float y, float width, float height, float healthPercent, float hmax, bool drawText){
    float currentWidth = width * (healthPercent/100);
    Color barColor = (Color){ (byte)(255 * (100 - healthPercent)/100), (byte)(255 * healthPercent/100), 0, 255 };
    barColor = ColorContrast(barColor, 0.9f);
    Rectangle rect = (Rectangle){x, y, width, height};
    DrawRectangleRec(rect, GRAY);
    DrawRectangle(x, y, currentWidth, height, barColor);
    DrawRectangleLines(x, y, width, height, BLACK);
    int nodes = hmax/50;
    float HPperPX = width/hmax;
    for(int i=0; i<nodes; i++){
        DrawLine(x + i*50*HPperPX, y, x + i*50*HPperPX, y +  height, BLACK );

    }
    const char* text = TextFormat("%.0f / %.0f", healthPercent/100.0f*hmax, hmax);
    int textSize = 5;
    if(drawText)DrawText(text, x+width*0.5f - MeasureText(text, textSize)/2, y+height*0.5f - textSize + 1, textSize, BLACK);

}
void InitBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}
void UpdateBullets(Enemy* enemies) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position = Vector2Add(bullets[i].position, Vector2Scale(bullets[i].direction, bullets[i].speed));
            if (bullets[i].lifeTime >= 250 || bullets[i].speed < 5.0f) {

                bullets[i].active = false;
            }
            bullets[i].lifeTime +=1;
            bullets[i].speed /= 1.01f;


            for(int j = 0; j < numEnemies; j++){
                    float enemyRadius = (float)enemies[i].radius;
                if(CheckCollisionCircles(bullets[i].position, bulletSize*2.0f, enemies[j].position, enemyRadius)){
                    if(enemies[j].active){
                            enemies[j].health -= gunDamage[gunID]*(1 + (skills[1].level)/2);
                            bulletHits+=1;
                    }
                    enemies[j].position = Vector2Add(enemies[j].position, Vector2Scale(bullets[i].direction, bullets[i].speed * 0.5f));
                    bullets[i].active = false;


                    if (numDamageTexts < 50) {
                    DamageText damageText;
                    strcpy(damageText.text, TextFormat("%d", gunDamage[gunID]*(1 + (skills[1].level)/2)));
                    damageText.position = enemies[j].position;
                    damageText.alpha = 255;
                    damageText.color = RED;
                    damageTexts[numDamageTexts++] = damageText;
                    }
                }
            }

        }
    }
}
void ShootBullet(float shootingAngleDegrees) {
    float shootingAngleRadians = DEG2RAD * (shootingAngleDegrees - 2 + rand()%4);

    Vector2 shootingDirection;
    shootingDirection.x = cosf(shootingAngleRadians);
    shootingDirection.y = sinf(shootingAngleRadians);

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {

            bulletShot+=1;
            bullets[i].active = true;
            bullets[i].position = Vector2Add((Vector2){ playerX , playerY }, Vector2Scale(shootingDirection, 5.0f));
            bullets[i].direction = shootingDirection;
            bullets[i].lifeTime = 0;
            bullets[i].speed = 15.0f;
            break;
        }
    }
}
void ShootSpread(float shootingAngleDegrees, float spreadDegrees, int numBullets) {
    float shootingAngleRadians = DEG2RAD * shootingAngleDegrees;

    float spreadRadians = DEG2RAD * spreadDegrees;

    float halfSpread = spreadRadians / 2.0f;
    float interval = spreadRadians / (numBullets - 1);

    for (int i = 0; i < numBullets; i++) {
        float currentSpread = -halfSpread + (interval * i);
        float currentAngleRadians = shootingAngleRadians + currentSpread;

        ShootBullet(RAD2DEG * currentAngleRadians);
    }
}
void SpawnCoin(Vector2 position, int type) {
    if (numCoins < MAX_COINS) {
        coins[numCoins].position = position;
        coins[numCoins].active = true;
        coins[numCoins].type = type;
        numCoins++;
    }
}
void DrawCrosshair(){
    Vector2 mousePosition = GetMousePosition();
    DrawTexturePro(iconTexture, (Rectangle){64, 0, 128, 64}, (Rectangle){mousePosition.x - 16.0f, mousePosition.y - 16.0f, 64.0f, 32.0f}, (Vector2){ 0.0f, 0.0f }, 0, WHITE);

}
void UpdateCoins() {
    for (int i = 0; i < numCoins; i++) {
        if (coins[i].active) {

            if (CheckCollisionCircles((Vector2){playerX, playerY}, playerRadius*skills[0].level*2, coins[i].position, CoinSize(coins[i].type))) {

                Vector2 distance = Vector2Subtract((Vector2){playerX, playerY}, coins[i].position);
                float length = Vector2Length(distance);
                float overlap = (playerRadius + coins[i].type) - length;
                    Vector2 normal = Vector2Scale(Vector2Normalize(distance), 2.0f/overlap);

                    coins[i].position = Vector2Subtract(coins[i].position, normal);

            }
            if (CheckCollisionCircles((Vector2){playerX, playerY}, playerRadius*skills[0].level, coins[i].position, CoinSize(coins[i].type))) {
                numCollectedCoins+=10*pow(3, coins[i].type);
                points+=10*pow(3, coins[i].type)*WaveCount*WaveCount;
                coins[i].active = false;
            }
        }
    }
}
void DrawCoins() {
    for (int i = 0; i < numCoins; i++) {
        if (coins[i].active) {
            float coinRadius = 5.5f*(coins[i].type+1);
            Color coinColor;
            switch (coins[i].type){
                case 0:  coinColor = (Color){205, 127, 50, 200}; break;
                case 1:  coinColor = (Color){192, 192, 192, 200}; break;
                case 2:  coinColor = (Color){255, 125, 0, 200}; break;
                case 3: coinColor = (Color){91, 185, 226, 200}; break;
                case 4: coinColor = (Color){210, 199, 217, 200}; break;
                case 5: coinColor = (Color){224, 17, 95, 200}; break;
                default: coinColor = DARKGRAY;

            }
            DrawCircleLines(coins[i].position.x, coins[i].position.y, coinRadius, BLACK);
            DrawCircleV(coins[i].position, coinRadius, coinColor);

        }
    }
}
void DrawGameBorder(int mapX, int mapY) {
    DrawRectangleLinesEx((Rectangle){(float)0, (float)0, 2.0f * mapX, 2.0f * mapY}, 50,  RED);
}
bool CheckCollisionWithAnyObject(vector<vector<vector<Object>>> &objs, Rectangle &hitbox) {

    int playerChunkX = hitbox.x / GRID_SIZE;
    int playerChunkY = hitbox.y / GRID_SIZE;

    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++) {
            int chunkX = playerChunkX + dx;
            int chunkY = playerChunkY + dy;

            if (chunkX >= 0 && chunkX < MAPSIZE && chunkY >= 0 && chunkY < MAPSIZE)
                for (Object &obj : objs[chunkX][chunkY])
                    if(obj.position.x!=0 || obj.position.y != 0)if (CheckCollisionRecs(hitbox, obj.calculateHitbox())){
                            DrawRectangleRec(obj.calculateHitbox(), GREEN);
                            return true;
                    }

        }


    return false;
}
void UpdatePlayerMovement(int mapX, int mapY, float* playerX, float* playerY, float playerSpeed, vector<vector<vector<Object>>> &objs, Rectangle &hitbox) {
    float oldPlayerX = *playerX;
    float oldPlayerY = *playerY;

    // Move and check collision on the x-axis
    *playerX += (1+skills[2].level/6)*playerSpeed * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    if (*playerX < 0)
        *playerX = 0;
    if (*playerX > mapX*2)
        *playerX = mapX*2;

    // Update hitbox position
    hitbox.x = *playerX - hitbox.width / 2;


    if (CheckCollisionWithAnyObject(objs, hitbox)) {
        *playerX = oldPlayerX;
        // Update hitbox position again after reverting player's position
        hitbox.x = *playerX - hitbox.width / 2 ;
    }

    // Move and check collision on the y-axis
    *playerY -= (1+skills[2].level/6)* playerSpeed * (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    if (*playerY < 0)
        *playerY = 0;
    if (*playerY > mapY*2)
        *playerY = mapY*2;

    // Update hitbox position
    hitbox.y = *playerY - hitbox.height / 2;

    if (CheckCollisionWithAnyObject(objs, hitbox)) {
        *playerY = oldPlayerY;
        // Update hitbox position again after reverting player's position
       hitbox.y = *playerY - hitbox.height / 2;
    }
}
void SetEnemyTypeProperties(EnemyType* enemyType, int radius, Color color, float speed, int healthMax, float damage) {
    enemyType->radius = radius;
    enemyType->color = color;
    enemyType->speed = speed;
    enemyType->healthMax = healthMax;
    enemyType->damage = damage;
}
void InitializeEnemyTypes(EnemyType* enemyTypes, int numEnemyTypes) {
    SetEnemyTypeProperties(&enemyTypes[0], 5, ORANGE, 3.0f, 10, 0.1f);
    SetEnemyTypeProperties(&enemyTypes[1], 10, RED, 2.0f, 25, 0.25f);
    SetEnemyTypeProperties(&enemyTypes[2], 15, BLUE, 2.5f, 100, 1.0f);
    SetEnemyTypeProperties(&enemyTypes[3], 20, PURPLE, 3.75f, 500, 2.5f);
    SetEnemyTypeProperties(&enemyTypes[4], 3, GREEN, 3.75f, 1500, 5.0f);
    SetEnemyTypeProperties(&enemyTypes[5], 50, PINK, 0.5f, 5000, 50.0f);
    //void SetEnemyTypeProperties(EnemyType* enemyType, int radius, Color color, float speed, int healthMax, float damage) {
}
void WaveSpawn(int wave) {
    waveTick+=1.0f/600.0f;
    if(waveTick<=1.0f)return;
    if(wave>NUM_WAVES){
        wave = NUM_WAVES;
    }
    else WaveCount++;
    wave = WaveCount;
    for(int i=0; i<NUM_ENEMY_TYPES; i++){
    int numE = waveData[wave][i];
    SpawnEnemy(&enemies, numE, MapX*0.5, 500, i);
    }
    waveTick = 0.0f;
    PlaySound(waveSound);
    pointsLeft++;

}
void Reset(){
    numCollectedCoins = numCollectedCoins / 10 * WaveCount;
    WaveCount = 0;
    numEnemies = 0;
    maxEnemies = 0;
    numCoins = 0;
    playerX = MAPSIZE*GRID_SIZE/2;
    playerY = MAPSIZE*GRID_SIZE/2;
    playerHealth = 100;
    playerMaxHealth = 101;
    WaveCount = 0;
    gunID = 0;
    WaveSpawn(0);
    CoolDown = 0;
    playerHealth = playerMaxHealth;
    pointsLeft = 0;
    points = 0;
    bulletHits = 0;
    bulletShot = 0;
    for(int i=0; i<MAX_SKILLS; i++)skills[i].level = 1;

}
void SetGunProperties(int* id, string path, string shoot, string reload,  int cooldownT, int type, int maxAmmo, int damage, int reloadT, int numBullet, int spreadV, int ammo){
    gunTexture[*id] = LoadTexture(path.c_str());
    for(int i=0; i<MAX_SOUNDS; i++) gunSounds[*id][i] = LoadSound(shoot.c_str());
    reloadSound[*id] = LoadSound(reload.c_str());
    cooldown[*id] = cooldownT;
    gunType[*id] = type;
    gunMaxAmmo[*id] = maxAmmo;
    gunDamage[*id] = damage;
    gunReload[*id] = reloadT;
    spread[*id] = spreadV;
    shotgunBullet[*id] = numBullet;
    ammotype[*id] = ammo;
}
void ResetGun(){
    //SetGunProperties(int* id, string path, string shoot, string reload,  int cooldownT, int type, int maxAmmo, int damage, int reloadT, int numBullet, int spreadV)
    int id;
    id = 0;
    SetGunProperties(&id, "./Image/GPTL.png", "./Sound/GPTL.mp3", "./Reload/GPTL.mp3", 20, 0, 10, 12, 100, 0, 0, 0);
    id++;
    SetGunProperties(&id, "./Image/C18.png", "./Sound/C18.mp3", "./Reload/C18.mp3", 12, 1, 30, 9, 130, 0, 0, 0);
    id++;
    SetGunProperties(&id, "./Image/ST8.png", "./Sound/ST8.mp3", "./Reload/ST8.mp3", 10, 3, 30, 15, 120, 0, 0, 0);
    id++;
    SetGunProperties(&id, "./Image/AK47.png", "./Sound/AK47.mp3", "./Reload/AK47.mp3", 10, 1, 30, 13, 150, 0, 0, 1);
    id++;
    SetGunProperties(&id, "./Image/MP220.png", "./Sound/MP220.mp3", "./Reload/MP220.mp3", 6, 4, 2, 12, 175, 25, 9, 3);
    id++;
    SetGunProperties(&id, "./Image/ZX400.png", "./Sound/ZX400.mp3", "./Reload/ZX400.mp3", 5, 1, 60, 8, 83, 0, 0, 1);
    id++;
    SetGunProperties(&id, "./Image/S350.png", "./Sound/S350.mp3", "./Reload/S350.mp3", 50, 1, 6, 180, 150, 0, 0, 1);

}
void DrawItem(Item item, Vector2 position){


    DrawRectangleLines(position.x, position.y, 100, 100, BLACK);






    Texture2D icon = GunIcon[item.id];
    DrawTextureEx(icon, (Vector2) { position.x+10, position.y }, 0, 0.7, WHITE);
    DrawText(item.name, position.x + 90 - MeasureText(item.name, 20), position.y + 65, 20, BLACK);
    Color col = (item.purchased || item.price>numCollectedCoins)? RED : DARKGREEN;
    DrawText(TextFormat("$%d", item.price), position.x + 10, position.y + 83, 15, col);

}
void BuyItem(Item* item, int id) {
    if (!item->purchased && numCollectedCoins >= item->price) {
        switch(m[item->type]){
        case 0:
        item->purchased = true;
        numCollectedCoins -= item->price;
        break;
        case 1:
        bulletTypeCount[item->id - MAX_GUNS] += ammoBuyCount[item->id - MAX_GUNS];
        numCollectedCoins -= item->price;
        break;
        case 2:
        grenadeLeft += 1;
        numCollectedCoins -= item->price;
        }


    }
}
void DrawDamageText(const DamageText* damageText) {
    DrawText(damageText->text, damageText->position.x - 5, damageText->position.y, 10, Fade(damageText->color, damageText->alpha));
}
void UpdateDamageText(DamageText* damageText) {
    damageText->alpha -= 2;
    damageText->position.y -= 2;
}
void InitGrenade(Grenade *grenade){

        grenade->position = (Vector2){9999999.0f, 9999999.0f};
        grenade->velocity = (Vector2){0.0f, 0.0f};
        grenade->radius = 1.0f;
        grenade->color = BLACK;
        grenade->state = 26;

}
void UpdateGrenade(Grenade *grenade){
    grenade->position.x += grenade->velocity.x;
    grenade->position.y += grenade->velocity.y;
    grenade->velocity.x *= 0.98f;
    grenade->velocity.y *= 0.98f;

    if(Vector2Length(grenade->velocity)<0.5){
        if(grenade->state == 10){
        PlaySound(boom);

        grenade->color = (Color){50,0,0, 100};

        }
        grenade->state ++;

    }
    if(grenade->state > 10 && grenade->state < 20 && grenade->state%2==0){
        grenade->radius *= 1+log2(grenade->radius)/4;
    }
    if(grenade->state==25){ grenade->radius = 0.01f;

    numGrenade--;
    InitGrenade(grenade);
    }
}
void PauseTick(){
    BeginDrawing();
    ShowCursor();
    DrawText("Game Pause", screenWidth / 2 - MeasureText("Game Pause", 40) / 2, screenHeight / 2 - 440, 40, DARKGRAY);
    if( GuiButton({screenWidth / 2 - 50,  screenHeight / 2 - 250, 100, 30},"Continue")){

        gameState = GAMEPLAY;
    }
    DrawCrosshair();
    EndDrawing();
}
void MenuTick(){
    pointsUpAnimation++;
    if(GuiButton({screenWidth/2 - 50, screenHeight/2 - 15, 100, 30}, "Play Game")){
        HideCursor();
        gameState = GAMEPLAY;
        SetMasterVolume(__masterVolume);
        WaveSpawn(0);
    }
    GuiSliderBar({screenWidth/2 - 150, screenHeight/2 + 25, 300, 20}, "Master Volume", TextFormat("%.2f", __masterVolume), &__masterVolume, 0.0f, 1.0f);
    currentRank.UpdateRank();

    BeginDrawing();
    ClearBackground(RAYWHITE);


    DrawTextEx(font, "Last Hunter Standing - Zombie Apocalypse v1.9.0",
               {screenWidth / 2 - MeasureText("Last Hunter Standing - Zombie Apocalypse v1.9.0", 20) / 2, screenHeight / 2 - 60}
               , 20, 1.5f, DARKGRAY);
    //DrawText("Press Enter to Start", screenWidth / 2 - MeasureText("Press Enter to Start", 20) / 2, screenHeight / 2 + 10, 20, DARKGRAY);
    currentRank.DrawRank({25, 50}, pointsUpAnimation);
    DrawCrosshair();
    EndDrawing();
}
void GameOverTick(){
    if (IsKeyPressed(KEY_ENTER)) {
        gameState = GAMEPLAY;
        playerHealth = playerMaxHealth;
        Reset();
        WaveSpawn(0);
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        gameState = MENU;
        pointsUpAnimation = 0;
        currentRank.UpdatePoints();
        for(int i=0; i<10; i++)currentRank.UpdateRank();
        Reset();
        freopen("rank.txt", "w", stdout);
        cout << currentRank.currR << " " << currentRank.currP;
        fclose(stdout);
    }

    BeginDrawing();
    DrawRectangleRec((Rectangle){0, 0, screenWidth, screenHeight}, (Color){255, 0, 0, 3});

    DrawText("You Lose!", screenWidth / 2 - MeasureText("You Lose!", 40) / 2, screenHeight / 2 - 40, 40, DARKGRAY);
    DrawText("Press Enter to play again", screenWidth / 2 - MeasureText("Press Enter to play again", 20) / 2, screenHeight / 2 + 10, 20, DARKGRAY);
    DrawText("Pts: %d", screenWidth / 2 - MeasureText("Press Enter to play again", 20) / 2, screenHeight / 2 + 10, 20, DARKGRAY);
    DrawCrosshair();
    EndDrawing();




}
void InitSkillButtons(){
    for (int i = 0; i < MAX_SKILLS; i++) {
    skillButtons[i] = (Rectangle){25.0f, 700.0f + 35*i, 120.0f, 26.0f}; // 20x26 for 1 seg
    }
}
void InitGrenades(){
    for(int i=0; i<MAX_BULLETS; i++){
        if(grenade[i].state>20 && i<numGrenade)InitGrenade(&grenade[i]);
    }
}
void LoadGameSoundTexture(){
    waveSound = LoadSound("./Sound/Wave.mp3"); skillButton = LoadTexture("./Gui/button.png");
    boom = LoadSound("./Sound/Boom.mp3"); iconTexture = LoadTexture("./Gui/icon.png");
    ExportImageAsCode(LoadImageFromTexture(iconTexture), "icon.raw");

    objTypes[0].name = "f1__bluebox";
    objTypes[0].texture = LoadTexture("Object/Texture/f1__bluebox.png");
    objTypes[0].offsetHitbox = {0,0,32,32};
}
void DrawMap(){
                for (int x = 0; x <= MapX*2; x += GRID_SIZE)
                DrawLine(x,  0, x, MapX*2, GRID_COLOR);
                for (int y = 0; y <= MapY*2; y += GRID_SIZE)
                DrawLine(0, y, MapY*2, y, GRID_COLOR);
                DrawGameBorder(MapX, MapY);
}
void UpdateGrenades(){
if (IsMouseButtonPressed(2) && grenadeLeft > 0){
            grenadeLeft --;
            numGrenade++;


            grenade[numGrenade].position.x = playerX;
            grenade[numGrenade].position.y = playerY;
            grenade[numGrenade].state = 0;
            grenade[numGrenade].radius = 10.0f;
            grenade[numGrenade].velocity.x = sinf((playerPointerAngle + 135)*DEG2RAD) * 5;
            grenade[numGrenade].velocity.y = -cosf((playerPointerAngle + 135)*DEG2RAD) * 5;
            }
            for(int i=0; i<MAX_BULLETS; i++){
            UpdateGrenade(&grenade[i]);
            if (CheckCollisionCircles((Vector2){playerX, playerY}, playerRadius, grenade[i].position ,grenade[i].radius) && grenade[i].state>10){
                playerHealth -= 100*(1 + (skills[1].level)/2);
            }
            }






}
void UpdateShop(){
        //--
        Vector2 mousePosition = GetMousePosition();
        showShop = (1-showShop)*IsKeyPressed(KEY_C) + showShop*!IsKeyPressed(KEY_C);
        //Shop --
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && showShop) {

                for (int i = 0; i < MAX_ITEMS; i++) {
                    if (CheckCollisionPointRec(mousePosition, (Rectangle) { (i%5) * 110.0f + 50, 50.0f + (i/5)*110.0f, 100.0f, 100.0f })) {
                        mouseInCombat = 0;
                        selectedItem = i;
                        break;
                    }
                }

                if (selectedItem != -1 && !items[selectedItem].purchased) {
                    BuyItem(&items[selectedItem], selectedItem);
                    selectedItem = -1;
                }

            //--
        }
}
void UpdateCamera(Camera2D *camera){
            camera->target.y = playerY;
            camera->target.x = playerX;

}
void UpdatePlayer(){
            playerHealth += 0.1f;
            if (playerHealth >= playerMaxHealth * skills[5].level){
                playerHealth = playerMaxHealth * skills[5].level;
            }
            if(IsKeyDown(KEY_M))numCollectedCoins+=99;
            if (playerHealth<0){
                gameState = GAMEOVER;
            }
}
void LoadFireUpdate(){
            CoolDown+=2+(skills[3].level)/3;

                if(!reloading && !IsKeyDown(KEY_Z)){
                    if(IsKeyPressed(KEY_RIGHT) || GetMouseWheelMove()>0) {
                        do{
                            gunID++;
                            if(gunID>MAX_GUNS) gunID = 0;
                        } while(!items[gunID].purchased);

                    }
                    if(IsKeyPressed(KEY_LEFT)|| GetMouseWheelMove()<0 ) {
                        do{
                            gunID--;
                            if(gunID<0) gunID = MAX_GUNS - 1;
                        } while(!items[gunID].purchased) ;


                    }
                }
                ammoLeft = ammoLeftOnGun[gunID];
            if(!reloading && IsKeyPressed(KEY_R) && ammoLeftOnGun[gunID]< gunMaxAmmo[gunID]*(1 + (skills[4].level)/3) && bulletTypeCount[ammotype[gunID]]>=gunMaxAmmo[gunID]*(1 + (skills[4].level)/3) - ammoLeftOnGun[gunID]){
                reloading = true;
                reload = 0;
                bulletTypeCount[ammotype[gunID]] -= gunMaxAmmo[gunID]*(1 + (skills[4].level)/3) - ammoLeftOnGun[gunID];
                PlaySound(reloadSound[gunID]);
            }
            if (reloading){
                reload+= skills[3].level;
                if(reload>gunReload[gunID]){
                    reloading = false;
                    ammoLeftOnGun[gunID] = gunMaxAmmo[gunID]*(1 + (skills[4].level)/3);
                    StopSound(reloadSound[gunID]);
                }
            }


            else if (ammoLeftOnGun[gunID] >= (float)gunMaxAmmo[gunID]*(1 + (skills[4].level)/3) || numCollectedCoins<=(float)gunMaxAmmo[gunID]*(1 + (skills[4].level)/3)/50.0f) reloading = false;

            switch(gunType[gunID]){
                case 0:
                case 4: Fired = IsMouseButtonPressed(0); break;
                case 1:
                case 2:
                case 3: Fired = IsMouseButtonDown(0); break;
            }
            int ExtraCoolDownBurstT = (cooldown[gunID]*( ((int)ammoLeftOnGun[gunID]%3)==0 * 7) )*(gunType[gunID]==3) ;
            if(mouseInCombat && Fired && CoolDown>cooldown[gunID]+ExtraCoolDownBurstT&& (int)ammoLeftOnGun[gunID] >= 1.0f && !reloading){
                if(gunType[gunID]!=4){
                    ShootBullet(playerPointerAngle +45);
                    // Play the first available sound
                    for (int i = 0; i < MAX_SOUNDS; i++) {
                        if (!IsSoundPlaying(gunSounds[gunID][i])) {
                            PlaySound(gunSounds[gunID][i]);
                            break;
                        }
                    }
                    CoolDown = 0;
                    ammoLeftOnGun[gunID] --;
                }
                else{
                    ShootSpread(playerPointerAngle + 45, spread[gunID], shotgunBullet[gunID]);
                    // Play the first available sound
                    for (int i = 0; i < MAX_SOUNDS; i++) {
                        if (!IsSoundPlaying(gunSounds[gunID][i])) {
                            PlaySound(gunSounds[gunID][i]);
                            break;
                        }
                    }
                    CoolDown = 0;
                    ammoLeftOnGun[gunID]--;
                }
            }






}
void NewWaveAnimation(Vector2 *t){
        t->x = 1920*(4*waveTick*waveTick*waveTick-6*waveTick*waveTick+3*waveTick);
        t->y = 20;



}
void DrawEnemies(){

                for (int i = 0; i < numEnemies; i++){
                    if(enemies[i].active){
                        DrawCircleV(enemies[i].position, enemies[i].radius, enemies[i].color);
                        if (enemies[i].health < enemies[i].healthMax)
                        DrawHealthBar(enemies[i].position.x - 17.5f, enemies[i].position.y - 10, 35, 5, (enemies[i].health/(float)(enemies[i].healthMax))*100.0f, (float)enemies[i].healthMax, 0);
                    }

                }

                bool allEnemiesDefeated = false;
                int sumEnemiesDied = 0;
                for (int i = 0; i < numEnemies; i++)
                    if (enemies[i].health <= 0)sumEnemiesDied++;
                if(sumEnemiesDied >= numEnemies){
                    allEnemiesDefeated = true;

                    if(!temp[0]){waveTick = 0.0f;}
                    temp[0]=1;

                }
                if (allEnemiesDefeated) {
                    WaveSpawn(WaveCount);

                }



}
void DrawBullets(){
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        DrawCircleV(bullets[i].position, bulletSize, BLACK);
                    }
                    DrawCircleV(grenade[i].position, grenade[i].radius/2, grenade[i].color);
                }



}
void DrawDamageTexts(){

                for (int i = 0; i < numDamageTexts; i++) {
                    UpdateDamageText(&damageTexts[i]);
                    if (damageTexts[i].alpha <= 0) {
                    damageTexts[i] = damageTexts[--numDamageTexts];
                    i--;
                    }
                }
                for (int i = 0; i < numDamageTexts; i++) {
                DrawDamageText(&damageTexts[i]);
                }





}
void DrawUI(Vector2 newWaveTextPosition){

            DrawText(TextFormat("$%.0f", numCollectedCoins),  screenWidth - 345, screenHeight - 105, 20, BLACK);
            DrawText(TextFormat("%d / %d", (int)ammoLeft, gunMaxAmmo[gunID]*(1 + (skills[4].level)/3)), screenWidth - 345, screenHeight - 130, 20, BLACK);
            DrawText(TextFormat("%d Kills", lastKillCount),  screenWidth -  345, screenHeight - 155, 20, {0,0,0, killCountImageCooldown});
            DrawText(TextFormat("Accuracy %.2f%", accuracy*100.0f),  screenWidth -  345, screenHeight - 180, 20, BLACK);
            if(showShop)
            for (int i = 0; i < MAX_ITEMS; i++) DrawItem(items[i], (Vector2) { (i%5) * 110.0f + 50, 50.0f + (i/5)*110.0f });
            else
            DrawTexturePro(iconTexture, (Rectangle){0, 0, 64, 64}, (Rectangle){50.0f, 50.0f, 100.0f, 100.0f}, (Vector2){ 0 }, 0, WHITE);
            if(reloading) DrawText(TextFormat("Reloading (%d%)", (int)100*reload/gunReload[gunID]), screenWidth/2 - 200, screenHeight/2 - 150, 50, BLACK);
            if(waveTick>0.01f)DrawText(TextFormat("Coming: Wave %d", WaveCount), newWaveTextPosition.x, newWaveTextPosition.y, 20.0f, BLACK);
            for(int k=0; k<4; k++){
                DrawRectangle(20 + k*100, screenHeight - 100, 50, 50, ammoColor[k]);
                DrawText(TextFormat("%d", bulletTypeCount[k]), 30 + k*100, screenHeight - 83, 15, BLACK);
                DrawRectangleLines(20 + k*100, screenHeight - 100, 50, 50, BLACK);
            }


            for (int i = 0; i < MAX_SKILLS; i++) {
                if (IsMouseHoverRectangle(skillButtons[i]) && pointsLeft > 0) {
                        mouseInCombat = 0;
                    DrawText(skills[i].description, GetMouseX() + 50, GetMouseY(), 20, BLACK);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        UpgradeSkill(i);
                    }
                }
            }
            string str;
            for (int i = 0; i < MAX_SKILLS; i++) {
                if(skills[i].level==skills[i].maxLevel) str = "MAX";
                else str = TextFormat("%d/%d",skills[i].level, skills[i].maxLevel);
                DrawSkillButton(skillButton, str.c_str(), pointsLeft > 0 && skills[i].level != skills[i].maxLevel, skills[i].level, (Vector2){skillButtons[i].x, skillButtons[i].y});


            }


            DrawText(TextFormat("P-Skill left: %d", pointsLeft), 25, screenHeight - 40, 20, BLACK);
            DrawText(TextFormat("Grenade: %d, on world %d", grenadeLeft, numGrenade), 250, screenHeight - 40, 20, BLACK);

}
void DrawObjects(){
    for(int i=0; i<MAPSIZE; i++){
        for(int j=0; j<MAPSIZE; j++){

            vector<Object> objss = objs[i][j];
            if(objss.size() == 0)continue;
             for(Object obj : objss){
                    /*if(obj.position.x!=0 || obj.position.y != 0)*/DrawTextureV(objTypes[obj.id].texture, obj.position/*Vector2Subtract(obj.position, {(GRID_SIZE*MAPSIZE)/2, (GRID_SIZE*MAPSIZE)/2})*/, WHITE);
                    //DrawRectangleRec(obj.calculateHitbox(), GREEN);
             }

        }
    }


}
void AddObjectToChunk(int id, Vector2 position, vector<vector<vector<Object>>> &chunks) {
    // Calculate the chunk coordinates
    Object obj;
    obj.id = id;
    obj.type = objTypes[obj.id];
    obj.hasCollision = true;
    obj.position = position;


    obj.hitbox = obj.calculateHitbox();
    int chunkX = (position.x) / GRID_SIZE;
    int chunkY = (position.y) / GRID_SIZE;


    if (chunkX >= 0 && chunkX < MAPSIZE && chunkY >= 0 && chunkY < MAPSIZE) {
        chunks[chunkX][chunkY].push_back(obj);

    }
}
bool IsPositionFree(Vector2 position, vector<vector<vector<Object>>> &objs, float enemyRadius) {
    int chunkX = position.x / GRID_SIZE;
    int chunkY = position.y / GRID_SIZE;

    // Check the player's chunk and the surrounding chunks
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int checkChunkX = chunkX + dx;
            int checkChunkY = chunkY + dy;

            // Make sure the chunk is within the grid
            if (checkChunkX > 0 && checkChunkX < objs.size()-1 && checkChunkY > 0 && checkChunkY < objs[checkChunkX].size()-1) {
                // Check all objects in the chunk
                for (Object &obj : objs[checkChunkX][checkChunkY]) {
                    if (CheckCollisionCircles(position, enemyRadius, obj.position, obj.calculateHitbox().width/2)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

int main(){
    InitializeEnemyTypes(enemyTypes, NUM_ENEMY_TYPES);
    InitWindow(screenWidth, screenHeight, "| Last Hunter Standing - Zombie Apocalypse | 1.9.0");
    freopen("rank.txt", "r", stdin);
    cin >> currentRank.currR >> currentRank.currP;
    fclose(stdin);
    //Setup Components
    for(int i=0; i<MAX_GUNS; i++){
        GunIcon[i] = LoadTexture(items[i].iconPath);
        }
    for(int i=MAX_GUNS; i<MAX_ITEMS; i++){
        GunIcon[i] = LoadTexture(items[i].iconPath);
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        InitGrenade(&grenade[i]);
    }

    SetWindowIcon(LoadImage("./Image/Icon.png"));
    rankk = LoadTexture("./Gui/rank.png");
    InitAudioDevice();
    SetMasterVolume(__masterVolume);
    ResetGun();
    font = LoadFontEx("pixelplay.png", 20, 0, 0);

    Camera2D camera = { 0 };
    InitCamera(&camera);
    SetTargetFPS(60);
    LoadGameSoundTexture();

    //AddObjectToChunk(0, {20,20}, objs, GRID_SIZE);
    //vector<string> map = {""}

    Image worldMap = LoadImage("./Generate/f1.png");
    for(int i=0; i<(GRID_SIZE*MAPSIZE)/32; i++){
        for(int j=0; j<(GRID_SIZE*MAPSIZE)/32; j++){
            int index = (j * worldMap.width) + i;
            Color pixel = GetImageColor(worldMap, i, j);
            //if(pixel.a)AddObjectToChunk(0, {i*32, j*32}, objs);
        }
    }
    //vector<vector<int>> grid(125, vector<int>(125))
    Sound killStreak[4] = {
        LoadSound("./SoundEffects/2.mp3"),
        LoadSound("./SoundEffects/3.mp3"),
        LoadSound("./SoundEffects/4.mp3"),
        LoadSound("./SoundEffects/5.mp3")
    };



    while (!WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)){

        switch (gameState) {
        case PAUSE:
            PauseTick(); break;
        case MENU:
            MenuTick(); break;
        case GAMEOVER:
            GameOverTick(); break;
        case GAMEPLAY:
            if(bulletShot > 0)accuracy = (float)bulletHits/(float)bulletShot;
            else accuracy = 1.0f;
            killCount = 0; killCountImageCooldown--; killCountImageCooldown = max(killCountImageCooldown, 0);
            //Mouse
            Vector2 mousePosition = GetMousePosition(); mouseInCombat = 1;
            playerPointerAngle = atan2(mousePosition.y - screenHeight/2, mousePosition.x - screenWidth/2) * RAD2DEG - 45 ;

            if(IsKeyPressed(KEY_T))ToggleFullscreen();
            //Skills
            InitSkillButtons();

            InitGrenades();

            UpdateGrenades();

            UpdateShop();


            //Pause
            if (IsKeyPressed(KEY_ESCAPE))gameState = PAUSE;


            Rectangle hitbox = {playerX - playerRadius, playerY - playerRadius, 2*playerRadius, 2*playerRadius};

            UpdatePlayerMovement(MapX, MapY, &playerX, &playerY, playerSpeed, objs, hitbox);
            UpdateCamera(&camera);

            UpdatePlayer();



            UpdateBullets(enemies);

            UpdateEnemies(enemies, numEnemies);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            Vector2 newWaveTextPosition;
            NewWaveAnimation(&newWaveTextPosition);


            BeginMode2D(camera);

                DrawMap();

                UpdateCoins();
                DrawCoins();

                DrawEnemies();
                DrawObjects();
                //DrawRectangleRec(hitbox, GREEN);

                DrawBullets();


                DrawCircleV((Vector2){playerX, playerY}, playerRadius*skills[0].level*2, Fade(GREEN, 0.15f));
                DrawCircleV((Vector2){playerX, playerY}, playerRadius*skills[0].level, Fade(BLUE, 0.15f));
                DrawCircleV((Vector2){playerX, playerY}, playerRadius, PLAYER_COLOR);
                DrawCircleLines(playerX, playerY, playerRadius, BLACK);

                DrawTextureEx(gunTexture[gunID], (Vector2){playerX, playerY}, playerPointerAngle , 1, WHITE);



                DrawDamageTexts();

            EndMode2D();

            DrawHealthBar(screenWidth/1.0f - 350, screenHeight/1.0f - 70, 300, 30, (playerHealth/(playerMaxHealth*skills[5].level))*100, playerMaxHealth*skills[5].level, 1);




            DrawUI(newWaveTextPosition);


            DrawCrosshair();

            LoadFireUpdate();

            EndDrawing();
            if (killCount >= 2 && killCount <= 5) {
                PlaySound(killStreak[killCount - 2]);
            } else if (killCount > 5) {
                PlaySound(killStreak[3]); // Play the last sound
            }

            if(globalKillCount>1 && killCount == 0){


                lastKillCount = globalKillCount;
                killCountImageCooldown = 255;
            }
            if(killCount<=0)globalKillCount = 0;
            else globalKillCount += killCount;

            Tick ++;
        }
    }
    freopen("rank.txt", "w", stdout);
    cout << currentRank.currR << " " << currentRank.currP;
    fclose(stdout);
    CloseWindow();

    return 0;
}

void SpawnEnemy(Enemy** enemies, int numberEnemy, int radius, int radiusRandom, int enemyType) {
    *enemies = (Enemy*)realloc(*enemies, (maxEnemies + numberEnemy) * sizeof(Enemy));
    if (*enemies != NULL) {
        for (int i = maxEnemies; i < maxEnemies + numberEnemy; i++) {
            Vector2 position;
            do {
                //float angle = GetRandomValue(0, 360) * DEG2RAD;
                float x = GetRandomValue(0, MapX); /* cos(angle) + MapX/2;*/
                float y = GetRandomValue(0, MapY); /* cos(angle) + MapY/2;*/
                position = (Vector2){ x, y };
            } while (!IsPositionFree(position, objs, GRID_SIZE));

            (*enemies)[i].position = position;
            (*enemies)[i].radius = enemyTypes[enemyType].radius;
            (*enemies)[i].color = enemyTypes[enemyType].color;
            (*enemies)[i].healthMax = enemyTypes[enemyType].healthMax;
            (*enemies)[i].health = (*enemies)[i].healthMax;

            (*enemies)[i].direction = (Vector2){ 0 };
            (*enemies)[i].type = enemyType;
            (*enemies)[i].speed = enemyTypes[enemyType].speed;
            (*enemies)[i].damage = enemyTypes[enemyType].damage;
            (*enemies)[i].active = 1;
        }

        maxEnemies += numberEnemy;
        numEnemies += numberEnemy;
    }
}
void UpdateEnemies(Enemy* enemies, int numEnemies){
    Vector2 playerPosition = (Vector2){ playerX, playerY };

    for (int i = 0; i < numEnemies; i++)
    {
        if (enemies[i].active)
        {
            Vector2 oldPosition = enemies[i].position;

            float enemyRadius = (float)enemies[i].radius;
            for(int j = 0; j < numEnemies; j++){
                if(i == j)continue;
                if (CheckCollisionCircles(enemies[i].position, enemies[i].radius,enemies[j].position, enemies[j].radius)){
                    Vector2 distance = Vector2Subtract(enemies[i].position, enemies[j].position);
                    float length = Vector2Length(distance);
                    float overlap = (enemies[i].radius + enemies[j].radius) - length;
                    Vector2 normal = Vector2Scale(Vector2Normalize(distance), 0.5f * overlap);
                    enemies[i].position = Vector2Add(enemies[i].position, normal);
                    enemies[j].position = Vector2Subtract(enemies[j].position, normal);
                }



            }

            Vector2 enemyToPlayer = Vector2Subtract(playerPosition, enemies[i].position);
            enemies[i].direction = Vector2Normalize(enemyToPlayer);
            enemies[i].position.x += Vector2Scale(enemies[i].direction, (float)enemies[i].speed).x;
            Rectangle rec = {enemies[i].position.x - enemies[i].radius, enemies[i].position.y - enemies[i].radius, enemies[i].radius*2, enemies[i].radius*2};
            if (CheckCollisionWithAnyObject(objs, rec)) {
                enemies[i].position.x = oldPosition.x;
            }
            enemies[i].position.y += Vector2Scale(enemies[i].direction, (float)enemies[i].speed).y;
            rec = {enemies[i].position.x - enemies[i].radius, enemies[i].position.y - enemies[i].radius, enemies[i].radius*2, enemies[i].radius*2};
            if (CheckCollisionWithAnyObject(objs, rec)) {
                enemies[i].position.y = oldPosition.y;
            }



            if (CheckCollisionCircles(enemies[i].position, enemyRadius, playerPosition, playerRadius))
            {
                playerHealth -= enemies[i].damage;
                Vector2 distance = Vector2Subtract(enemies[i].position, playerPosition);
                    float length = Vector2Length(distance);
                    float overlap = (enemies[i].radius + playerRadius) - length;
                    Vector2 normal = Vector2Scale(Vector2Normalize(distance), overlap);
                    enemies[i].position = Vector2Add(enemies[i].position, normal);
            }

            for (int j = 0; j < MAX_BULLETS; j++)
            {
                if(CheckCollisionCircles(enemies[i].position, 10, grenade[j].position, grenade[j].radius) && grenade[j].state < 25 &&grenade[j].state>10){
                enemies[i].health -= 100*(1 + (skills[1].level)/2);
                if (numDamageTexts < 50) {
                    DamageText damageText;
                    strcpy(damageText.text, TextFormat("%d", enemies[i].health -= 100*(1 + (skills[1].level)/2)));
                    damageText.position = enemies[i].position;
                    damageText.alpha = 255;
                    damageText.color = RED;
                    damageTexts[numDamageTexts++] = damageText;
                }
                }
                if(enemies[i].health<=0){
                    bullets[j].active = false;
                    killCount++;
                    SpawnCoin(enemies[i].position, enemies[i].type);
                    enemies[i].active = 0;
                    enemies[i].position = Vector2{999999, 999999};

                    break;
                }

            }



        }
    }
}
