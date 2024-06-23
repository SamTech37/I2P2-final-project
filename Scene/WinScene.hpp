#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include <allegro5/allegro_audio.h>

#include "Engine/IScene.hpp"
#include "UI/Component/TextInput.hpp"

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
    void writeScoreRecordToFile();
    void writeScoreRecordToRemote();
    Engine::TextInput* nameInput;

    void test();
};

#endif  // WINSCENE_HPP
