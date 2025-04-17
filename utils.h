#ifndef UTILS_H
#define UTILS_H

#define VERSION "1.9.2"

#include "game.h"

float CoinSize(int coinType);
double log_ab(double a, double b);
void UpdatePlayerMovement(int mapX, int mapY, float* playerX, float* playerY, float playerSpeed, vector<vector<vector<Object>>>& objs, Rectangle& hitbox);
void UpdateCamera(Camera2D* camera);
void UpdatePlayer();
void UpdateBullets();
void ShootBullet(Vector2 position, float shootingAngleDegrees, int shootingError);
void ShootSpread(Vector2 position, float shootingAngleDegrees, float spreadDegrees, int numBullets, int shootingError);
void SpawnCoin(Vector2 position, int type);
void UpdateCoins();
void UpdateShop();
void BuyItem(Item* item, int id);
void UpdateGrenades();
void LoadFireUpdate();
void NewWaveAnimation(Vector2* t);
void AddObjectToChunk(int id, Vector2 position, vector<vector<vector<Object>>>& chunks);
bool IsPositionFree(Vector2 position, vector<vector<vector<Object>>>& objs, float enemyRadius);
bool IsMouseHoverRectangle(Rectangle rect);
void UpgradeSkill(int index);
bool CheckCollisionWithAnyObject(vector<vector<vector<Object>>>& objs, Rectangle& hitbox);
char* concat(const char* str1, const char* str2);

#endif
