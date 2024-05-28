#ifndef WAVETURRET_HPP
#define WAVETURRET_HPP
#include "Turret.hpp"

class WaveTurret : public Turret {
   public:
    static const int Price;
    WaveTurret(float x, float y);
    void CreateBullet() override;
};
#endif  // WAVETURRET_HPP
