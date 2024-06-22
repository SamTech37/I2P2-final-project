#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include <allegro5/allegro_audio.h>

#include "Engine/IScene.hpp"

// the gameover scene
class WinScene final : public Engine::IScene {
   private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;
    int score = 0;

   public:
    explicit WinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    void setScore(int score);
    void writeScoreToFile();
    // add a text input
};

#endif  // WINSCENE_HPP
