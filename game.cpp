#include <raylib.h>
#include "raymath.h"
#include <bits/stdc++.h>
#include "utils.h"
#include "game.h"

void Ranking::UpdatePoints() {
    lastRankChange = ((points - currP) / abs(points - currP)) * log_ab(abs(points - currP), 1.25);
    currP += lastRankChange;
}

void Ranking::UpdateRank() {
    if (currP > rrList[currR]) currR++;
    if (currP < rrList[currR]) currR--;
}

void Ranking::DrawRank(Vector2 pos, int upRank) {
    DrawTexturePro(rankk, (Rectangle){(currR/3)*32, (currR%3)*32, 32, 32}, (Rectangle){pos.x, pos.y, 100*__ratio, 100*__ratio}, {16,16}, 0, WHITE);
    string str = "Current rank: " + keyWordRank[currR/3] + " ";
    for(int i=-1; i<currR%3; i++) str += "I";
    DrawText(str.c_str(), pos.x + 125*__ratio, pos.y + 20*__ratio, 20*__ratio, BLACK );
    DrawText(TextFormat("%d / %d ",currP - rrList[currR] - lastRankChange + (int)(min(upRank, 120)*lastRankChange/120.0f), rrList[currR+1]-rrList[currR]  ), pos.x + 125*__ratio, pos.y + 45*__ratio, 20*__ratio, BLACK);
    string str2 = lastRankChange<0? "-" + to_string(abs(lastRankChange) - (int)(min(upRank, 120)*abs(lastRankChange)/120.0f)):"+"+to_string(lastRankChange - (int)(min(upRank, 120)*lastRankChange/120.0f));
    if(upRank<120) DrawText(str2.c_str(), pos.x + 125*__ratio, pos.y + 70*__ratio, 20*__ratio, BLACK);
}

void InitCamera(Camera2D* camera) {
    camera->target = {0, 0};
    camera->offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    camera->rotation = 0.0f;
    camera->zoom = 1.0f * __ratio * __ratio;
}

void InitializeEnemyTypes(EnemyType* enemyTypes, int numEnemyTypes) {
    auto SetEnemyTypeProperties = [](EnemyType* enemyType, int radius, Color color, float speed, int healthMax, float damage) {
        enemyType->radius = radius;
        enemyType->color = color;
        enemyType->speed = speed;
        enemyType->healthMax = healthMax;
        enemyType->health = healthMax;
        enemyType->damage = damage;
    };
    SetEnemyTypeProperties(&enemyTypes[0], 5, ORANGE, 3.0f, 10, 0.1f);
    SetEnemyTypeProperties(&enemyTypes[1], 10, RED, 2.0f, 25, 0.25f);
    SetEnemyTypeProperties(&enemyTypes[2], 15, BLUE, 2.5f, 100, 1.0f);
    SetEnemyTypeProperties(&enemyTypes[3], 20, PURPLE, 3.75f, 500, 2.5f);
    SetEnemyTypeProperties(&enemyTypes[4], 3, GREEN, 3.75f, 1500, 5.0f);
    SetEnemyTypeProperties(&enemyTypes[5], 50, PINK, 0.5f, 5000, 50.0f);
}

void ResetGun() {
    auto SetGunProperties = [](int id, string path, string shoot, string reload, int cooldownT, int type, int maxAmmo, int damage, int reloadT, int numBullet, int spreadV, int ammo) {
        gunTexture[id] = LoadTexture(path.c_str());
        for (int i = 0; i < MAX_SOUNDS; i++) gunSounds[id][i] = LoadSound(shoot.c_str());
        reloadSound[id] = LoadSound(reload.c_str());
        cooldown[id] = cooldownT;
        gunType[id] = type;
        gunMaxAmmo[id] = maxAmmo;
        gunDamage[id] = damage;
        gunReload[id] = reloadT;
        spread[id] = spreadV;
        shotgunBullet[id] = numBullet;
        ammotype[id] = ammo;
    };
    int id = 0;
    SetGunProperties(id++, "./Image/GPTL.png", "./Sound/GPTL.mp3", "./Reload/GPTL.mp3", 20, 0, 10, 12, 100, 0, 0, 0);
    SetGunProperties(id++, "./Image/C18.png", "./Sound/C18.mp3", "./Reload/C18.mp3", 12, 1, 30, 9, 130, 0, 0, 0);
    SetGunProperties(id++, "./Image/ST8.png", "./Sound/ST8.mp3", "./Reload/ST8.mp3", 10, 3, 30, 15, 120, 0, 0, 0);
    SetGunProperties(id++, "./Image/AK47.png", "./Sound/AK47.mp3", "./Reload/AK47.mp3", 10, 1, 30, 13, 150, 0, 0, 1);
    SetGunProperties(id++, "./Image/MP220.png", "./Sound/MP220.mp3", "./Reload/MP220.mp3", 6, 4, 2, 12, 175, 25, 9, 3);
    SetGunProperties(id++, "./Image/ZX400.png", "./Sound/ZX400.mp3", "./Reload/ZX400.mp3", 5, 1, 60, 8, 83, 0, 0, 1);
    SetGunProperties(id++, "./Image/S350.png", "./Sound/S350.mp3", "./Reload/S350.mp3", 50, 1, 6, 180, 150, 0, 0, 1);
}

void LoadGameSoundTexture() {
    waveSound = LoadSound("./Sound/Wave.mp3");
    skillButton = LoadTexture("./Gui/button.png");
    boom = LoadSound("./Sound/Boom.mp3");
    iconTexture = LoadTexture("./Gui/icon.png");
    objTypes[0].name = "f1__bluebox";
    objTypes[0].texture = LoadTexture("Object/Texture/f1__bluebox.png");
    objTypes[0].offsetHitbox = {0, 0, 32, 32};
}

void InitSkillButtons() {
    for (int i = 0; i < MAX_SKILLS; i++) {
        skillButtons[i] = {25.0f, 700.0f * __ratio + 35 * i * __ratio, 120.0f * __ratio, 26.0f * __ratio};
    }
}

void InitGrenades() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (grenades[i].state > 20 && i < numGrenade) InitGrenade(&grenades[i]);
    }
}

void Reset() {
    numCollectedCoins = numCollectedCoins / 10 * WaveCount;
    WaveCount = 0;
    numEnemies = 0;
    maxEnemies = 0;
    numCoins = 0;
    playerX = MAPSIZE * GRID_SIZE / 2;
    playerY = MAPSIZE * GRID_SIZE / 2;
    playerHealth = 100;
    playerMaxHealth = 100;
    gunID = 0;
    WaveSpawn();
    CoolDown = 0;
    playerHealth = playerMaxHealth;
    pointsLeft = 0;
    points = 0;
    bulletHits = 0;
    bulletShot = 0;
    for (int i = 0; i < MAX_SKILLS; i++) skills[i].level = 1;
}

void WaveSpawn() {
    waveTick += 1.0f / 600.0f;
    if (waveTick <= 1.0f) return;
    if (WaveCount > NUM_WAVES) {
        WaveCount = NUM_WAVES;
    } else {
        WaveCount++;
    }
    int wave = WaveCount;
    for (int i = 0; i < NUM_ENEMY_TYPES; i++) {
        int numE = waveData[wave][i];
        SpawnEnemy(numE, MAPSIZE * GRID_SIZE * 0.5, 500, i);
    }
    waveTick = 0.0f;
    PlaySound(waveSound);
    pointsLeft++;
}

void UpdateGameplay() {
    if (bulletShot > 0) accuracy = (float)bulletHits / (float)bulletShot;
    killCountImageCooldown = max(killCountImageCooldown - 1, 0);
    killCount = 0;
    Vector2 mousePosition = GetMousePosition();
    mouseInCombat = true;
    playerPointerAngle = atan2(mousePosition.y - screenHeight / 2, mousePosition.x - screenWidth / 2) * RAD2DEG - 45;

    if (IsKeyPressed(KEY_T)) ToggleFullscreen();
    if (IsKeyPressed(KEY_P)) pointsLeft++;
    InitSkillButtons();
    InitGrenades();
    UpdateGrenades();
    UpdateShop();
    if (IsKeyPressed(KEY_ESCAPE)) gameState = GameState::PAUSE;

    Rectangle hitbox = {playerX - playerRadius, playerY - playerRadius, 2 * playerRadius, 2 * playerRadius};
    UpdatePlayerMovement(MAPSIZE * GRID_SIZE, MAPSIZE * GRID_SIZE, &playerX, &playerY, playerSpeed, objs, hitbox);
    UpdateCamera(&camera);
    UpdatePlayer();
    UpdateBullets();
    UpdateEnemies();
    UpdateCoins();
}

void UpdateDamageText(DamageText* damageText) {
    damageText->alpha -= 2;
    damageText->position.y -= 2;
}

void SpawnEnemy(int numberEnemy, int radius, int radiusRandom, int enemyType) {
    for (int i = 0; i < numberEnemy; i++) {
        Vector2 position;
        do {
            float x = GetRandomValue(0, MapX);
            float y = GetRandomValue(0, MapY);
            position = { x, y };
        } while (!IsPositionFree(position, objs, enemyTypes[enemyType].radius));

        Enemy enemy;
        enemy.position = position;
        enemy.radius = enemyTypes[enemyType].radius;
        enemy.color = enemyTypes[enemyType].color;
        enemy.healthMax = enemyTypes[enemyType].healthMax;
        enemy.health = enemy.healthMax;
        enemy.direction = { 0, 0 };
        enemy.type = enemyType;
        enemy.speed = enemyTypes[enemyType].speed;
        enemy.damage = enemyTypes[enemyType].damage;

        enemies.push_back(enemy);
        numEnemies++;
    }
}

void UpdateEnemies() {
    Vector2 playerPosition = { playerX, playerY };

    for (auto it = enemies.begin(); it != enemies.end();) {
        Enemy& enemy = *it;
        Vector2 oldPosition = enemy.position;

        float enemyRadius = (float)enemy.radius;
        // Collision with other enemies
        for (auto otherIt = enemies.begin(); otherIt != enemies.end(); ++otherIt) {
            if (it == otherIt) continue;
            if (CheckCollisionCircles(enemy.position, enemy.radius, otherIt->position, otherIt->radius)) {
                Vector2 distance = Vector2Subtract(enemy.position, otherIt->position);
                float length = Vector2Length(distance);
                if (length > 0.001f) {
                    float overlap = (enemy.radius + otherIt->radius) - length;
                    if (overlap > 0) {
                        Vector2 normal = Vector2Scale(Vector2Normalize(distance), 0.5f * overlap);
                        enemy.position = Vector2Add(enemy.position, normal);
                        otherIt->position = Vector2Subtract(otherIt->position, normal);
                    }
                }
            }
        }

        // Move toward player
        Vector2 enemyToPlayer = Vector2Subtract(playerPosition, enemy.position);
        float length = Vector2Length(enemyToPlayer);
        if (length > 0.001f) {
            enemy.direction = Vector2Normalize(enemyToPlayer);
        } else {
            enemy.direction = {0, 0};
        }
        enemy.position.x += Vector2Scale(enemy.direction, enemy.speed).x;
        Rectangle rec = {enemy.position.x - enemy.radius, enemy.position.y - enemy.radius, enemy.radius*2, enemy.radius*2};
        if (CheckCollisionWithAnyObject(objs, rec)) {
            enemy.position.x = oldPosition.x;
        }
        enemy.position.y += Vector2Scale(enemy.direction, enemy.speed).y;
        rec = {enemy.position.x - enemy.radius, enemy.position.y - enemy.radius, enemy.radius*2, enemy.radius*2};
        if (CheckCollisionWithAnyObject(objs, rec)) {
            enemy.position.y = oldPosition.y;
        }

        // Player collision
        if (CheckCollisionCircles(enemy.position, enemyRadius, playerPosition, playerRadius)) {
            playerHealth -= enemy.damage;
            Vector2 distance = Vector2Subtract(enemy.position, playerPosition);
            float length = Vector2Length(distance);
            if (length > 0.001f) {
                float overlap = (enemy.radius + playerRadius) - length;
                Vector2 normal = Vector2Scale(Vector2Normalize(distance), overlap);
                enemy.position = Vector2Add(enemy.position, normal);
            }
        }

        // Grenade collision
        for (int j = 0; j < MAX_GRENADES; j++) {
            if (CheckCollisionCircles(enemy.position, 10, grenades[j].position, grenades[j].radius) &&
                grenades[j].state < 25 && grenades[j].state > 10) {
                float damage = Vector2Distance(grenades[j].position, playerPosition) / 10.0f * (1 + skills[1].level / 2);
                enemy.health -= damage;
                if (numDamageTexts < 50) {
                    DamageText damageText;
                    strcpy(damageText.text, TextFormat("%.0f", damage));
                    damageText.position = enemy.position;
                    damageText.alpha = 255;
                    damageText.color = RED;
                    damageTexts[numDamageTexts++] = damageText;
                }
            }
        }

        // Bullet collision
        bool hit = false;
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            if (CheckCollisionCircles(enemy.position, enemy.radius, bulletIt->position, 3.0f /*bulletSize = 3.0f*/ * 2.0f)) {
                float damage = bulletIt->damage * (1 + skills[1].level / 2.0f);
                enemy.health -= damage;
                enemy.position = Vector2Add(enemy.position, Vector2Scale(bulletIt->direction, bulletIt->speed * 1.0f));
                bulletHits++;
                if (numDamageTexts < 50) {
                    DamageText damageText;
                    strcpy(damageText.text, TextFormat("%.1f", damage));
                    damageText.position = enemy.position;
                    damageText.alpha = 255;
                    damageText.color = RED;
                    damageTexts[numDamageTexts] = damageText;
                    numDamageTexts++;
                }
                bulletIt = bullets.erase(bulletIt);
                hit = true;
                break;
            } else {
                ++bulletIt;
            }
        }

        // Consolidated death check
        if (enemy.health <= 0) {
            killCount++;
            SpawnCoin(enemy.position, enemy.type);
            it = enemies.erase(it);
            numEnemies--;
        } else {
            ++it;
        }
    }
}
