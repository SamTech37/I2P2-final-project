#include "WaveBullet.hpp"

#include <cmath>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"

// damage = 0.15
// it's kinda like a DamageOverTime
// the numerical balancing is a bit tricky
WaveBullet::WaveBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent)
    : Bullet("play/shockwave.png", 0, 0.15, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {}
void WaveBullet::Update(float deltaTime) {
    // mimic the plane's shockwave
    PlayScene* scene = getPlayScene();

    float scaleExp;
    switch (animationStage) {
        case 1:
            timeTicks += deltaTime;
            if (timeTicks >= timeSpanShockwave) {
                timeTicks = 0;
                animationStage++;
                break;
            }
            scaleExp = ((timeSpanShockwave - timeTicks) * log2(minScale) + timeTicks * log2(maxScale)) / (timeSpanShockwave);
            scale = pow(2, scaleExp);
            Size.x = GetBitmapWidth() * scale;
            Size.y = GetBitmapHeight() * scale;
            CollisionRadius = shockWaveRadius * scale;
            // loop through all the enemy to check if overlap.
            for (auto& it : scene->EnemyGroup->GetObjects()) {
                Enemy* enemy = dynamic_cast<Enemy*>(it);
                if (Engine::Collider::IsCircleOverlap(this->Position, CollisionRadius, enemy->Position, enemy->CollisionRadius))
                    enemy->Hit(damage);
            }
            break;
        case 2:
            // delete the shockwave after the animation is done
            scene->EffectGroup->RemoveObject(objectIterator);
            return;
    }
    Sprite::Update(deltaTime);
}