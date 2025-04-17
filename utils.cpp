#include "utils.h"
#include <raylib.h>
#include "raymath.h"
#include <bits/stdc++.h>


#define logFile cout

float CoinSize(int coinType) {
    return (coinType+1.0f)*5.5f;
}
double log_ab(double a, double b) {
    return log2(a) / log2(b);
}
void UpdatePlayerMovement(int mapX, int mapY, float* playerX, float* playerY, float playerSpeed, vector<vector<vector<Object>>>& objs, Rectangle& hitbox) {
    float oldPlayerX = *playerX;
    float oldPlayerY = *playerY;
    *playerX += (1 + skills[2].level / 6) * playerSpeed * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    if (*playerX < 0) *playerX = 0;
    if (*playerX > mapX * 2) *playerX = mapX * 2;
    hitbox.x = *playerX - hitbox.width / 2;
    if (CheckCollisionWithAnyObject(objs, hitbox)) {
        *playerX = oldPlayerX;
        hitbox.x = *playerX - hitbox.width / 2;
    }
    *playerY -= (1 + skills[2].level / 6) * playerSpeed * (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    if (*playerY < 0) *playerY = 0;
    if (*playerY > mapY * 2) *playerY = mapY * 2;
    hitbox.y = *playerY - hitbox.height / 2;
    if (CheckCollisionWithAnyObject(objs, hitbox)) {
        *playerY = oldPlayerY;
        hitbox.y = *playerY - hitbox.height / 2;
    }
}
void UpdateCamera(Camera2D* camera) {
    camera->target.y = playerY;
    camera->target.x = playerX;
}
void UpdatePlayer() {
    playerHealth += 0.1f;
    if (playerHealth >= playerMaxHealth * skills[5].level) {
        playerHealth = playerMaxHealth * skills[5].level;
    }
    if (IsKeyDown(KEY_M)) numCollectedCoins += 99;
    if (playerHealth < 0) {
        gameState = GameState::GAMEOVER;
    }
}
void UpdateBullets() {
    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet& bullet = *it;
        bullet.position = Vector2Add(bullet.position, Vector2Scale(bullet.direction, bullet.speed));
        bullet.lifeTime += 1;
        bullet.speed /= 1.01f;
        if (bullet.lifeTime >= 250) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}
void ShootBullet(Vector2 position, float shootingAngleDegrees, int shootingError) {
    bulletShot++;
    Bullet bullet;
    bullet.position = position;
    float shootingAngleRadians = DEG2RAD * (shootingAngleDegrees - shootingError + rand() % (shootingError*2 + 1));
    Vector2 shootingDirection = {cosf(shootingAngleRadians), sinf(shootingAngleRadians)};
    bullet.direction = Vector2Scale(shootingDirection, 5.0f);
    bullet.damage = gunDamage[gunID];
    bullet.speed = 2.0f;
    bullet.lifeTime = 0;
    bullets.push_back(bullet);
}
void ShootSpread(Vector2 position, float shootingAngleDegrees, float spreadDegrees, int numBullets, int shootingError) {
    float shootingAngleRadians = DEG2RAD * shootingAngleDegrees;
    float spreadRadians = DEG2RAD * spreadDegrees;
    float halfSpread = spreadRadians / 2.0f;
    float interval = spreadRadians / (numBullets - 1.0f);
    for (int i = 0; i < numBullets; i++) {
        float currentSpread = -halfSpread + (interval * i);
        float currentAngleDegrees = (float)((shootingAngleRadians + currentSpread) * RAD2DEG);
        ShootBullet(position, currentAngleDegrees, shootingError);
    }
}
void SpawnCoin(Vector2 position, int type) {
    if (numCoins < MAX_COINS) {
        Coin coin;
        coin.position = position;
        coin.active = true;
        coin.type = type;
        coins.push_back(coin);
        numCoins++;
    }
}
void UpdateCoins() {
    for (auto coinIt = coins.begin(); coinIt != coins.end(); ) {
        Coin& coin = *coinIt;
        if (CheckCollisionCircles({playerX, playerY}, playerRadius * skills[0].level * 2, coin.position, CoinSize(coin.type))) {
            Vector2 distance = Vector2Subtract({playerX, playerY}, coin.position);
            float length = Vector2Length(distance);
            float overlap = (playerRadius + coin.type) - length;
            Vector2 normal = Vector2Scale(Vector2Normalize(distance), 2.0f / overlap);
            coin.position = Vector2Subtract(coin.position, normal);
        }
        if (CheckCollisionCircles({playerX, playerY}, playerRadius * skills[0].level, coin.position, CoinSize(coin.type))) {
            numCollectedCoins += 10 * pow(3, coin.type);
            points += 10 * pow(3, coin.type) * WaveCount * WaveCount;
            coinIt = coins.erase(coinIt);
        } else {
            coinIt++;
        }
    }

}
void UpdateShop() {
    Vector2 mousePosition = GetMousePosition();
    showShop = (1 - showShop) * IsKeyPressed(KEY_C) + showShop * !IsKeyPressed(KEY_C);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && showShop) {
        for (int i = 0; i < MAX_ITEMS; i++) {
            if (CheckCollisionPointRec(mousePosition, {(i % 5) * 110.0f * __ratio + 50 * __ratio, 50.0f * __ratio + (i / 5) * 110.0f * __ratio, 100.0f * __ratio, 100.0f * __ratio})) {
                mouseInCombat = false;
                selectedItem = i;
                break;
            }
        }
        if (selectedItem != -1 && !items[selectedItem].purchased) {
            BuyItem(&items[selectedItem], selectedItem);
            selectedItem = -1;
        }
    }
}
void BuyItem(Item* item, int id) {
    if (!item->purchased && numCollectedCoins >= item->price) {
        switch (m[item->type]) {
            case 0: item->purchased = true; numCollectedCoins -= item->price; break;
            case 1: bulletTypeCount[item->id - MAX_GUNS] += ammoBuyCount[item->id - MAX_GUNS]; numCollectedCoins -= item->price; break;
            case 2: grenadeLeft += 1; numCollectedCoins -= item->price; break;
        }
    }
}
void UpdateGrenades() {

    if (IsMouseButtonPressed(2) && grenadeLeft > 0) {
        grenadeLeft--;
        numGrenade++;
        Grenade grenade;
        grenade.position = {playerX, playerY};
        grenade.state = 0;
        grenade.radius = 10.0f;
        grenade.velocity = {sinf((playerPointerAngle + 135) * DEG2RAD) * 5, -cosf((playerPointerAngle + 135) * DEG2RAD) * 5};
        grenades.push_back(grenade);
    }

    for (auto grenadeIt = grenades.begin(); grenadeIt != grenades.end(); ) {
        Grenade& grenade = *grenadeIt;
        grenade.position.x += grenade.velocity.x;
        grenade.position.y += grenade.velocity.y;
        grenade.velocity.x *= 0.98f;
        grenade.velocity.y *= 0.98f;
        if (Vector2Length(grenade.velocity) < 0.5) {
            if (grenade.state == 10) {
                PlaySound(boom);
                grenade.color = {50, 0, 0, 100};
            }
            grenade.state++;
        }
        if (grenade.state > 10 && grenade.state < 20 && grenade.state % 2 == 0) {
            grenade.radius *= 1 + log2(grenade.radius) / 4;
        }
        if (CheckCollisionCircles({playerX, playerY}, playerRadius, grenade.position, grenade.radius) && grenade.state > 10) {
            playerHealth -= 100 * (1 + skills[1].level / 2);
        }
        if (grenade.state == 25) {
            numGrenade--;
            grenadeIt = grenades.erase(grenadeIt);
        } else {
            grenadeIt++;
        }
    }
}
void LoadFireUpdate() {
    CoolDown += 2 + skills[3].level / 3;
    if (!reloading && !IsKeyDown(KEY_Z)) {
        if (IsKeyPressed(KEY_RIGHT) || GetMouseWheelMove() > 0) {
            do {
                gunID++;
                if (gunID >= MAX_GUNS) gunID = 0;
            } while (!items[gunID].purchased);
        }
        if (IsKeyPressed(KEY_LEFT) || GetMouseWheelMove() < 0) {
            do {
                gunID--;
                if (gunID < 0) gunID = MAX_GUNS - 1;
            } while (!items[gunID].purchased);
        }
    }
    if (!reloading && IsKeyPressed(KEY_R) && ammoLeftOnGun[gunID] < gunMaxAmmo[gunID] * (1 + skills[4].level / 3) &&
        bulletTypeCount[ammotype[gunID]] >= gunMaxAmmo[gunID] * (1 + skills[4].level / 3) - ammoLeftOnGun[gunID]) {
        reloading = true;
        reload = 0;
        bulletTypeCount[ammotype[gunID]] -= gunMaxAmmo[gunID] * (1 + skills[4].level / 3) - ammoLeftOnGun[gunID];
        PlaySound(reloadSound[gunID]);
    }
    if (reloading) {
        reload += skills[3].level;
        if (reload > gunReload[gunID]) {
            reloading = false;
            ammoLeftOnGun[gunID] = gunMaxAmmo[gunID] * (1 + skills[4].level / 3);
            StopSound(reloadSound[gunID]);
        }
    } else if (ammoLeftOnGun[gunID] >= (float)gunMaxAmmo[gunID] * (1 + skills[4].level / 3) ||
               numCollectedCoins <= (float)gunMaxAmmo[gunID] * (1 + skills[4].level / 3) / 50.0f) {
        reloading = false;
    }

    switch (gunType[gunID]) {
        case 0:
        case 4: Fired = IsMouseButtonPressed(0); break;
        case 1:
        case 2:
        case 3: Fired = IsMouseButtonDown(0); break;
    }
    int ExtraCoolDownBurstT = (cooldown[gunID] * (((int)ammoLeftOnGun[gunID] % 3) == 0 * 7)) * (gunType[gunID] == 3);
    Vector2 playerPosition = (Vector2){playerX, playerY};
    if (mouseInCombat && Fired && CoolDown > cooldown[gunID] + ExtraCoolDownBurstT && (int)ammoLeftOnGun[gunID] >= 1.0f && !reloading) {
        if (gunType[gunID] != 4) {
            ShootBullet(playerPosition, playerPointerAngle + 45, 1);
            for (int i = 0; i < MAX_SOUNDS; i++) {
                if (!IsSoundPlaying(gunSounds[gunID][i])) {
                    PlaySound(gunSounds[gunID][i]);
                    break;
                }
            }
            CoolDown = 0;
            ammoLeftOnGun[gunID]--;
        } else {
            ShootSpread(playerPosition, playerPointerAngle + 45, spread[gunID], shotgunBullet[gunID], 1);
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
void NewWaveAnimation(Vector2* t) {
    t->x = 1920 * (4 * waveTick * waveTick * waveTick - 6 * waveTick * waveTick + 3 * waveTick);
    t->y = 20;
}
void AddObjectToChunk(int id, Vector2 position, vector<vector<vector<Object>>>& chunks) {
    Object obj;
    obj.id = id;
    obj.type = objTypes[id];
    obj.hasCollision = true;
    obj.position = position;
    obj.hitbox = obj.calculateHitbox();
    int chunkX = position.x / GRID_SIZE;
    int chunkY = position.y / GRID_SIZE;
    if (chunkX >= 0 && chunkX < MAPSIZE && chunkY >= 0 && chunkY < MAPSIZE) {
        chunks[chunkX][chunkY].push_back(obj);
    }
}
bool IsPositionFree(Vector2 position, vector<vector<vector<Object>>>& objs, float enemyRadius) {
    int chunkX = position.x / GRID_SIZE;
    int chunkY = position.y / GRID_SIZE;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int checkChunkX = chunkX + dx;
            int checkChunkY = chunkY + dy;
            if (checkChunkX > 0 && checkChunkX < objs.size() - 1 && checkChunkY > 0 && checkChunkY < objs[checkChunkX].size() - 1) {
                for (Object& obj : objs[checkChunkX][checkChunkY]) {
                    if (CheckCollisionCircles(position, enemyRadius, obj.position, obj.calculateHitbox().width / 2)) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
bool IsMouseHoverRectangle(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}
void UpgradeSkill(int index) {
    if (skills[index].level < MAX_LEVEL && pointsLeft > 0) {
        skills[index].level++;
        pointsLeft--;
    }
}
bool CheckCollisionWithAnyObject(vector<vector<vector<Object>>>& objs, Rectangle& hitbox) {
    int playerChunkX = hitbox.x / GRID_SIZE;
    int playerChunkY = hitbox.y / GRID_SIZE;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int chunkX = playerChunkX + dx;
            int chunkY = playerChunkY + dy;
            if (chunkX >= 0 && chunkX < MAPSIZE && chunkY >= 0 && chunkY < MAPSIZE) {
                for (Object& obj : objs[chunkX][chunkY]) {
                    if ((obj.position.x != 0 || obj.position.y != 0) && CheckCollisionRecs(hitbox, obj.calculateHitbox())) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
char* concat(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = (char*)malloc(len1 + len2 + 1);
    if (!result) return nullptr;
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}
