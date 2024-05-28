#include "WaveTurret.hpp"

#include <allegro5/base.h>

#include <cmath>
#include <string>

#include "Bullet/WaveBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"

// Done: [CUSTOM-TOOL] You can imitate the 2 files: 'WaveTurret.hpp', 'WaveTurret.cpp' to create a new turret.
const int WaveTurret::Price = 100;
WaveTurret::WaveTurret(float x, float y) : Turret("play/tower-base.png", "play/turret-7.png", x, y, 200, Price, 2) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void WaveTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new WaveBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("shockwave.ogg");
}
