#ifndef WAVEBULLET_HPP
#define WAVEBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class WaveBullet : public Bullet {
   protected:
    // this should match the shockwave image size
    const float shockWaveRadius = 180;

    // change the scalars instead of size
    // so that it works correctly
    const float minScale = 1.0f / 8;
    const float maxScale = 2;
    const float timeSpanShockwave = 1;
    int animationStage = 1;
    float timeTicks;
    float scale;

   public:
    explicit WaveBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
    void Update(float deltaTime) override;
};
#endif  // WAVEBULLET_HPP
