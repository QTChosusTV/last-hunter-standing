#ifndef RENDER_H
#define RENDER_H

#include <bits/stdc++.h>
using namespace std;

#include "game.h"

void MenuTick();
void PauseTick();
void GameOverTick();
void RenderGameplay();
void DrawMap();
void DrawCoins();
void DrawEnemies();
void DrawObjects();
void DrawBullets();
void DrawDamageTexts();
void DrawUI(Vector2 newWaveTextPosition);
void DrawCrosshair();
void DrawHealthBar(float x, float y, float width, float height, float healthPercent, float hmax, bool drawText);
void DrawItem(Item item, Vector2 position);
void DrawSkillButton(Texture2D t, const char* text, bool isEnabled, float div, Vector2 position);


#endif
