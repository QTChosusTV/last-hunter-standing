#include <bits/stdc++.h>
#include "game.h"
#include "utils.h"
#include "render.h"
#include "raylib.h"
#include "raygui.h"

using namespace std;

// Global game state
GameState gameState = GameState::MENU;
float __masterVolume = 1.0f;
Font font;
Camera2D camera = {0};
Ranking currentRank;
vector<ObjectType> objTypes(1);
vector<vector<vector<Object>>> objs(MAPSIZE, vector<vector<Object>>(MAPSIZE, vector<Object>(40)));
Sound waveSound;
Sound boom;
Sound killStreak[4];
Texture2D rankk;
Texture2D logotechno;
Texture2D skillButton;
Texture2D iconTexture;

// Game variables
float screenWidth = 1920 * __ratio;
float screenHeight = 1080 * __ratio;
int pointsUpAnimation = 0;
float waveTick = 0.0f;
int WaveCount = 0;
bool showShop = false;
int pointsLeft = 0;
long long int points = 0;
int rankP = 0;
int rankR = 0;
float numCollectedCoins = 0.0f;
int numEnemies = 0;
int maxEnemies = 0;
int lastRankChange = 0;
int numCoins = 0;
int bulletHits = 0;
int bulletShot = 0;
int currP = 0;
float accuracy = 1.0f;
int killCountImageCooldown = 0;
int lastKillCount = 0;
int killCount = 0;
int globalKillCount = 0;
int MapX = GRID_SIZE * MAPSIZE;
int MapY = GRID_SIZE * MAPSIZE;

// Player variables
float playerX = GRID_SIZE * MAPSIZE / 2;
float playerY = GRID_SIZE * MAPSIZE / 2;
float playerRadius = 15.0f;
float playerSpeed = 4.0f;
float playerHealth = 100.0f;
float playerMaxHealth = 100.0f;
float playerPointerAngle = 0.0f;

// Weapon and item variables
int gunID = 0;
bool Fired = false;
int CoolDown = 0;
bool reloading = false;
int reload = 0;
int selectedItem = -1;
bool mouseInCombat = true;
float ammoLeftOnGun[MAX_GUNS] = {0};
int bulletTypeCount[4] = {100, 100, 100, 2};
int grenadeLeft = 0;
int numGrenade = 0;
Grenade grenades[MAX_BULLETS];
Coin coins[MAX_COINS];
deque<Enemy> enemies;
deque<Bullet> bullets;
Item items[MAX_ITEMS] = {
    {"GPTL", "./Icons/GPTL.png", 0, true, 0, "gun"},
    {"C18", "./Icons/C18.png", 0, true, 1, "gun"},
    {"ST8", "./Icons/ST8.png", 200, false, 2, "gun"},
    {"AK47", "./Icons/AK47.png", 500, false, 3, "gun"},
    {"MP220", "./Icons/MP220.png", 1000, false, 4, "gun"},
    {"ZX400", "./Icons/ZX400.png", 2000, false, 5, "gun"},
    {"S350", "./Icons/S350.png", 4000, false, 6, "gun"},
    {"9mm", "./Icons/ammo0.png", 25, false, 7, "ammo"},
    {"7.62mm", "./Icons/ammo1.png", 50, false, 8, "ammo"},
    {"5.56mm", "./Icons/ammo2.png", 100, false, 9, "ammo"},
    {"12mm", "./Icons/ammo3.png", 500, false, 10, "ammo"},
    {"Grenade", "./Icons/Grenade.png", 1000, false, 11, "other"}
};
Skill skills[MAX_SKILLS] = {
    {1, MAX_LEVEL, "Magnet: Increase the item pick radius."},
    {1, MAX_LEVEL, "Damage: Increase the damage dealt to enemies."},
    {1, MAX_LEVEL, "Speed: Increase movement speed, x2 at level MAX."},
    {1, MAX_LEVEL, "Fire Rate: Increase shooting speed, +30% rate / level."},
    {1, MAX_LEVEL, "Max Ammo: Increase the maximum ammo capacity, x2 at level 3 and x3 at level MAX."},
    {1, MAX_LEVEL, "Max HP: Increase the Max health point."}
};
Rectangle skillButtons[MAX_SKILLS];
Texture2D GunIcon[MAX_ITEMS];
Texture2D gunTexture[MAX_GUNS];
Sound gunSounds[MAX_GUNS][MAX_SOUNDS];
Sound reloadSound[MAX_GUNS];
int gunType[MAX_GUNS];
int gunMaxAmmo[MAX_GUNS];
int gunDamage[MAX_GUNS];
int gunReload[MAX_GUNS];
int cooldown[MAX_GUNS];
int shotgunBullet[MAX_GUNS];
int spread[MAX_GUNS];
int ammotype[MAX_GUNS];
map<string, int> m = {{"gun", 0}, {"ammo", 1}, {"other", 2}};
Color ammoColor[4] = {GOLD, BLUE, GREEN, RED};
int ammoBuyCount[4] = {10, 10, 10, 10};

// Enemy variables
EnemyType enemyTypes[NUM_ENEMY_TYPES];
int waveData[NUM_WAVES][NUM_ENEMY_TYPES] = {
    {5, 0, 0, 0, 0, 0}, {7, 1, 0, 0, 0, 0}, {10, 3, 0, 0, 0, 0}, {20, 5, 1, 0, 0, 0},
    {17, 6, 3, 0, 0, 0}, {10, 10, 5, 0, 0, 0}, {5, 15, 7, 0, 0, 0}, {10, 10, 10, 1, 0, 0},
    {7, 8, 9, 3, 0, 0}, {4, 6, 8, 5, 0, 0}, {1, 4, 7, 10, 0, 0}, {0, 2, 6, 15, 0, 0},
    {0, 0, 5, 15, 1, 0}, {0, 0, 4, 13, 2, 0}, {0, 0, 3, 10, 3, 0}, {0, 0, 2, 6, 5, 0},
    {0, 0, 1, 1, 8, 0}, {0, 0, 0, 0, 13, 0}, {0, 0, 0, 0, 21, 0}, {0, 0, 0, 0, 20, 1},
    {0, 0, 0, 0, 18, 2}, {0, 0, 0, 0, 15, 3}, {0, 0, 0, 0, 11, 5}, {0, 0, 0, 0, 6, 8},
    {0, 0, 0, 0, 0, 15}, {0, 0, 0, 0, 0, 100}
};

// Damage text variables
int numDamageTexts = 0;
DamageText damageTexts[50] = {0};

// Ranking variables
vector<int> rrList = {0, 25, 75, 160, 275, 440, 645, 905, 1215, 1580, 2005, 2490, 3045, 3665, 4335, 5120, 5955, 6870, 7865, 8940, 10000, 999999, 999999, 999999};
vector<string> keyWordRank = {"Copper", "Iron", "Gold", "Emerald", "Diamond", "Epic", "Legendary", "???"};

bool temp[1000] = {false};

int main() {
    // Initialize window
    InitWindow(screenWidth, screenHeight, concat("| Last Hunter Standing - Zombie Apocalypse | ", VERSION));
    SetWindowIcon(LoadImage("./Image/Icon.png"));
    InitAudioDevice();
    SetMasterVolume(__masterVolume);
    SetTargetFPS(60);
    // Load resources
    font = LoadFontEx("pixelplay.png", 20, 0, 0);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20 * __ratio);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 2 * __ratio);
    rankk = LoadTexture("./Gui/rank.png");
    logotechno = LoadTexture("./Image/TNGlogo.png");
    LoadGameSoundTexture();
    InitCamera(&camera);
    InitializeEnemyTypes(enemyTypes, NUM_ENEMY_TYPES);
    ResetGun();

    // Load rank data
    ifstream rankFile("rank.txt");
    if (rankFile.is_open()) {
        rankFile >> currentRank.currR >> currentRank.currP;
        rankFile.close();
    }

    // Load icons
    for (int i = 0; i < MAX_ITEMS; i++) {
        GunIcon[i] = LoadTexture(items[i].iconPath);
    }

    // Initialize bullets and grenades
    for (int i = 0; i < MAX_BULLETS; i++) {
        InitGrenade(&grenades[i]);
    }

    // Load kill streak sounds
    killStreak[0] = LoadSound("./SoundEffects/2.mp3");
    killStreak[1] = LoadSound("./SoundEffects/3.mp3");
    killStreak[2] = LoadSound("./SoundEffects/4.mp3");
    killStreak[3] = LoadSound("./SoundEffects/5.mp3");

    // Main game loop
    while (!WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
        switch (gameState) {
            case GameState::MENU:
                MenuTick();
                break;
            case GameState::PAUSE:
                PauseTick();
                break;
            case GameState::GAMEOVER:
                GameOverTick();
                break;
            case GameState::GAMEPLAY:
                UpdateGameplay();
                RenderGameplay();
                break;
        }
    }

    // Save rank data
    ofstream outFile("rank.txt");
    if (outFile.is_open()) {
        outFile << currentRank.currR << " " << currentRank.currP;
        outFile.close();
    }

    // Cleanup
    for (int i = 0; i < MAX_GUNS; i++) {
        UnloadTexture(gunTexture[i]);
        for (int j = 0; j < MAX_SOUNDS; j++) {
            UnloadSound(gunSounds[i][j]);
        }
        UnloadSound(reloadSound[i]);
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        UnloadTexture(GunIcon[i]);
    }
    UnloadSound(waveSound);
    UnloadSound(boom);
    for (int i = 0; i < 4; i++) {
        UnloadSound(killStreak[i]);
    }
    UnloadTexture(rankk);
    UnloadTexture(logotechno);
    UnloadTexture(skillButton);
    UnloadTexture(iconTexture);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
