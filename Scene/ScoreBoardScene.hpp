#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>

#include "Engine/IScene.hpp"

class ScoreBoardScene final : public Engine::IScene {
   private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;

   public:
    explicit ScoreBoardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
};

#endif  // SCOREBOARDSCENE_HPP
