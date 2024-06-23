#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
   protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    int money;
    // static since it's shared by all enemy onjects.
    static float waveUpgradeFactor;
    PlayScene* getPlayScene();
    virtual void OnExplode();

   public:
    float reachEndTime;
    std::list<Turret*> lockedTurrets;
    std::list<Bullet*> lockedBullets;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
    void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
    void Update(float deltaTime) override;
    void Draw() const override;
    static void getUpgradeMultiplier(int currentWave);
};
#endif  // ENEMY_HPP
