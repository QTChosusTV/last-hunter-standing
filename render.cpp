#include "render.h"
#include "utils.h"
#include <raylib.h>
#include "raymath.h"
//#define logFile cout

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void MenuTick() {
    pointsUpAnimation++;
    if (GuiButton({screenWidth / 2 - 50 * __ratio, screenHeight / 2, 100 * __ratio * __ratio, 30 * __ratio * __ratio}, "Play Game")) {
        HideCursor();
        gameState = GameState::GAMEPLAY;
        SetMasterVolume(__masterVolume);
        //WaveSpawn();
    }
    GuiSliderBar({screenWidth / 2 - 300 * __ratio, screenHeight / 2 + 75 * __ratio, 700 * __ratio, 20 * __ratio},
                 "Master Volume", TextFormat("%.2f", __masterVolume), &__masterVolume, 0.0f, 1.0f);
    currentRank.UpdateRank();

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTextEx(font, concat("| Last Hunter Standing - Zombie Apocalypse | ", VERSION),
               {screenWidth / 2 - MeasureText(concat("| Last Hunter Standing - Zombie Apocalypse | ", VERSION), 40 * __ratio) / 2, screenHeight / 2 - 60 * __ratio},
               40 * __ratio, 3.0f * __ratio, DARKGRAY);
    currentRank.DrawRank((Vector2){50 * __ratio, 100 * __ratio}, pointsUpAnimation);
    DrawCrosshair();
    DrawTextureEx(logotechno, {screenWidth - 200 * __ratio, 75 * __ratio}, 0, 0.5f, WHITE);
    EndDrawing();
}

void PauseTick() {
    BeginDrawing();
    ShowCursor();
    DrawText("Game Pause", screenWidth / 2 - MeasureText("Game Pause", 40 * __ratio) / 2, screenHeight / 2 - 440 * __ratio, 40 * __ratio, DARKGRAY);
    if (GuiButton({screenWidth / 2 - 50 * __ratio, screenHeight / 2 - 250 * __ratio, 100 * __ratio, 30 * __ratio}, "Continue")) {
        gameState = GameState::GAMEPLAY;
    }
    DrawCrosshair();
    EndDrawing();
}

void GameOverTick() {
    if (IsKeyPressed(KEY_ENTER)) {
        gameState = GameState::GAMEPLAY;
        playerHealth = playerMaxHealth;
        Reset();
        WaveSpawn();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        gameState = GameState::MENU;
        pointsUpAnimation = 0;
        currentRank.UpdatePoints();
        for (int i = 0; i < 10; i++) currentRank.UpdateRank();
        Reset();
    }

    BeginDrawing();
    DrawRectangleRec({0, 0, screenWidth, screenHeight}, {255, 0, 0, 3});
    DrawText("You Lose!", screenWidth / 2 - MeasureText("You Lose!", 40 * __ratio) / 2, screenHeight / 2 - 40 * __ratio, 40 * __ratio, DARKGRAY);
    DrawText("Press Enter to play again", screenWidth / 2 - MeasureText("Press Enter to play again", 20 * __ratio) / 2, screenHeight / 2 + 10 * __ratio, 20 * __ratio, DARKGRAY);
    DrawCrosshair();
    EndDrawing();
}

void RenderGameplay() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    Vector2 newWaveTextPosition;
    NewWaveAnimation(&newWaveTextPosition);
    DrawFPS(10, 10);
    BeginMode2D(camera);
    DrawMap();
    DrawCoins();
    DrawEnemies();
    DrawDamageTexts();
    DrawObjects();
    DrawBullets();
    DrawCircleV({playerX, playerY}, playerRadius * skills[0].level * 2, Fade(GREEN, 0.15f));
    DrawCircleV({playerX, playerY}, playerRadius * skills[0].level, Fade(BLUE, 0.15f));
    DrawCircleV({playerX, playerY}, playerRadius, PLAYER_COLOR);
    DrawCircleLines(playerX, playerY, playerRadius, BLACK);
    DrawTextureEx(gunTexture[gunID], {playerX, playerY}, playerPointerAngle, 1, WHITE);
    EndMode2D();
    DrawHealthBar(screenWidth / 1.0f - 350 * __ratio, screenHeight / 1.0f - 70 * __ratio, 300 * __ratio, 30 * __ratio,
                  (playerHealth / (playerMaxHealth * skills[5].level)) * 100, playerMaxHealth * skills[5].level, 1);
    DrawUI(newWaveTextPosition);
    DrawCrosshair();
    LoadFireUpdate();
    EndDrawing();

    if (killCount >= 2 && killCount <= 5) {
        PlaySound(killStreak[killCount - 2]);
    } else if (killCount > 5) {
        PlaySound(killStreak[3]);
    }

    if (globalKillCount > 1 && killCount == 0) {
        lastKillCount = globalKillCount;
        killCountImageCooldown = 255;
    }
    if (killCount <= 0) globalKillCount = 0;
    else globalKillCount += killCount;
}

void DrawMap() {
    for (int x = 0; x <= MAPSIZE * GRID_SIZE * 2; x += GRID_SIZE)
        DrawLine(x, 0, x, MAPSIZE * GRID_SIZE * 2, GRID_COLOR);
    for (int y = 0; y <= MAPSIZE * GRID_SIZE * 2; y += GRID_SIZE)
        DrawLine(0, y, MAPSIZE * GRID_SIZE * 2, y, GRID_COLOR);
    DrawRectangleLinesEx({0, 0, 2.0f * MAPSIZE * GRID_SIZE, 2.0f * MAPSIZE * GRID_SIZE}, 50, RED);
}

void DrawCoins() {
    for (auto coinIt = coins.begin(); coinIt != coins.end(); coinIt++) {
        Coin coin = *coinIt;
        if (coin.active) {
            float coinRadius = (coin.type+1.0f)*5.5f;
            Color coinColor;
            switch (coin.type) {
                case 0: coinColor = {205, 127, 50, 200}; break;
                case 1: coinColor = {192, 192, 192, 200}; break;
                case 2: coinColor = {255, 125, 0, 200}; break;
                case 3: coinColor = {91, 185, 226, 200}; break;
                case 4: coinColor = {210, 199, 217, 200}; break;
                case 5: coinColor = {224, 17, 95, 200}; break;
                default: coinColor = DARKGRAY;
            }
            DrawCircleLines(coin.position.x, coin.position.y, coinRadius, BLACK);
            DrawCircleV(coin.position, coinRadius, coinColor);
        }
    }
}

void DrawEnemies() {
    logFile << "-> DrawEnemies()\n";
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); enemyIt++) {
        Enemy enemy = *enemyIt;
        DrawCircleV(enemy.position, enemy.radius, enemy.color);
        //DrawText(TextFormat("%d/%d", enemy.health, enemy.healthMax), enemy.position.x, enemy.position.y, 20, RED);
        if (enemy.health < enemy.healthMax) {
            DrawHealthBar(enemy.position.x - 17.5f, enemy.position.y - 10.0f, 35, 5,
                          ((float)enemy.health / (float)enemy.healthMax) * 100.0f, (float)enemy.healthMax, 0);
        }
    }


    if (enemies.empty()) {
        if (!temp[0]) waveTick = 0.0f;
        temp[0] = true;
        logFile << "DrawEnemies() -> Called WaveSpawn()\n";
        WaveSpawn();
        logFile << "DrawEnemies() -> enemies.size(): " << enemies.size() << "\n";
    }
    logFile << "<- DrawEnemies()\n";
}

void DrawObjects() {
    for (int i = 0; i < MAPSIZE; i++) {
        for (int j = 0; j < MAPSIZE; j++) {
            vector<Object> objss = objs[i][j];
            for (Object obj : objss) {
                if (obj.position.x != 0 || obj.position.y != 0)
                    DrawTextureV(objTypes[obj.id].texture, obj.position, WHITE);
            }
        }
    }
}

void DrawBullets() {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ++bulletIt) {
        Bullet& bullet = *bulletIt;
        DrawCircleV(bullet.position, 3.0f * bullet.speed / 2.0f, (Color){0, 0, 0, 255.0f * bullet.speed / 2.0f});
    }
}

void DrawDamageTexts() {
    for (auto damageTextIt = damageTexts.begin(); damageTextIt != damageTexts.end(); ++damageTextIt) {
        DamageText& damageText = *damageTextIt;
        DrawText(damageText.text, damageText.position.x /* __ratio*/ - 5,
                 damageText.position.y /* __ratio*/, 10, (Color){200, 0, 0, damageText.alpha});
    }
}

void DrawUI(Vector2 newWaveTextPosition) {
    DrawText(TextFormat("$%.0f", numCollectedCoins), screenWidth - 345 * __ratio, screenHeight - 105 * __ratio, 20 * __ratio, BLACK);
    DrawText(TextFormat("%d / %d", (int)ammoLeftOnGun[gunID], gunMaxAmmo[gunID] * (1 + skills[4].level / 3)),
             screenWidth - 345 * __ratio, screenHeight - 130 * __ratio, 20 * __ratio, BLACK);
    DrawText(TextFormat("%d Kills", lastKillCount), screenWidth - 345 * __ratio, screenHeight - 155 * __ratio, 20 * __ratio, (Color){0, 0, 0, (unsigned char)killCountImageCooldown});
    DrawText(TextFormat("Accuracy %.2f%", accuracy * 100.0f), screenWidth - 345 * __ratio, screenHeight - 180 * __ratio, 20 * __ratio, BLACK);
    if (showShop) {
        for (int i = 0; i < MAX_ITEMS; i++)
            DrawItem(items[i], {(i % 5) * 110.0f * __ratio + 50 * __ratio, 50.0f * __ratio + (i / 5) * 110.0f * __ratio});
    } else {
        DrawTexturePro(iconTexture, {0, 0, 64, 64}, {50.0f * __ratio, 50.0f * __ratio, 100.0f * __ratio, 100.0f * __ratio}, {0}, 0, WHITE);
    }
    if (reloading)
        DrawText(TextFormat("Reloading (%d%)", (int)(100 * reload / gunReload[gunID])), screenWidth / 2 - 200 * __ratio,
                 screenHeight / 2 - 150 * __ratio, 50 * __ratio, BLACK);
    if (waveTick > 0.01f)
        DrawText(TextFormat("Coming: Wave %d", WaveCount), newWaveTextPosition.x, newWaveTextPosition.y, 20.0f * __ratio, BLACK);
    for (int k = 0; k < 4; k++) {
        DrawRectangle(20 * __ratio + k * 100 * __ratio, screenHeight - 100 * __ratio, 50 * __ratio, 50 * __ratio, ammoColor[k]);
        DrawText(TextFormat("%d", bulletTypeCount[k]), 30 * __ratio + k * 100 * __ratio, screenHeight - 83 * __ratio, 15 * __ratio, BLACK);
        DrawRectangleLines(20 * __ratio + k * 100 * __ratio, screenHeight - 100 * __ratio, 50 * __ratio, 50 * __ratio, BLACK);
    }

    for (int i = 0; i < MAX_SKILLS; i++) {
        if (IsMouseHoverRectangle(skillButtons[i]) && pointsLeft > 0) {
            mouseInCombat = false;
            DrawText(skills[i].description, GetMouseX() + 50, GetMouseY(), 20 * __ratio, BLACK);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                UpgradeSkill(i);
            }
        }
    }
    string str;
    for (int i = 0; i < MAX_SKILLS; i++) {
        str = (skills[i].level == skills[i].maxLevel) ? "MAX" : TextFormat("%d/%d", skills[i].level, skills[i].maxLevel);
        DrawSkillButton(skillButton, str.c_str(), pointsLeft > 0 && skills[i].level != skills[i].maxLevel, skills[i].level,
                        {skillButtons[i].x, skillButtons[i].y});
    }

    DrawText(TextFormat("P-Skill left: %d", pointsLeft), 25 * __ratio, screenHeight - 40 * __ratio, 20 * __ratio, BLACK);
    DrawText(TextFormat("Grenade: %d, on world %d", grenadeLeft, numGrenade), 250 * __ratio, screenHeight - 40 * __ratio, 20 * __ratio, BLACK);
}

void DrawCrosshair() {
    Vector2 mousePosition = GetMousePosition();
    DrawTexturePro(iconTexture, {64, 0, 128, 64}, {mousePosition.x - 16.0f, mousePosition.y - 16.0f, 64.0f, 32.0f}, {0, 0}, 0, WHITE);
}

void DrawHealthBar(float x, float y, float width, float height, float healthPercent, float hmax, bool drawText) {

    float currentWidth = width * (healthPercent / 100.0f);
    Color barColor = {(unsigned char)(255 * (100 - healthPercent) / 100), (unsigned char)(255 * healthPercent / 100), 0, 255};
    barColor = ColorContrast(barColor, 0.9f);
    Rectangle rect = {x, y, width, height};
    DrawRectangleRec(rect, GRAY);
    DrawRectangle(x, y, currentWidth, height, barColor);
    DrawRectangleLines(x, y, width, height, BLACK);
    int nodes = hmax / 50;
    float HPperPX = width / hmax;
    for (int i = 0; i < nodes; i++) {
        DrawLine(x + i * 50 * HPperPX, y, x + i * 50 * HPperPX, y + height, BLACK);
    }
    const char* text = TextFormat("%.0f / %.0f", healthPercent / 100.0f * hmax, hmax);
    int textSize = 16 * __ratio;
    if (drawText)
        DrawText(text, x + width * 0.5f - MeasureText(text, textSize) / 2, y + height / 2 - textSize / 2 + 1, textSize, BLACK);
}

void DrawItem(Item item, Vector2 position) {
    DrawRectangleLines(position.x, position.y, 100 * __ratio, 100 * __ratio, BLACK);
    Texture2D icon = GunIcon[item.id];
    DrawTextureEx(icon, {position.x + 10 * __ratio, position.y}, 0, 0.7 * __ratio, WHITE);
    DrawText(item.name, (position.x + 90 * __ratio - MeasureText(item.name, 20 * __ratio)), (position.y + 65 * __ratio), 20 * __ratio, BLACK);
    Color col = (item.purchased || item.price > numCollectedCoins) ? RED : DARKGREEN;
    DrawText(TextFormat("$%d", item.price), (position.x + 10 * __ratio), (position.y + 83 * __ratio), 15 * __ratio, col);
}

void DrawSkillButton(Texture2D t, const char* text, bool isEnabled, float div, Vector2 position) {
    const int buttonWidth = 20;
    const int buttonHeight = 26;
    const int buttonParts = 6;
    for (int i = 0; i < buttonParts; i++) {
        Rectangle srcRec = {(float)i * buttonWidth, 0, (float)buttonWidth, (float)buttonHeight};
        Rectangle dstRec = {position.x + i * buttonWidth * __ratio, position.y, buttonWidth * __ratio, buttonHeight * __ratio};
        srcRec.y = (i < div) ? 0 : buttonHeight;
        Color col = isEnabled ? WHITE : Fade(GRAY, 0.5f);
        DrawTexturePro(t, srcRec, dstRec, {0, 0}, 0.0f, col);
    }
}
