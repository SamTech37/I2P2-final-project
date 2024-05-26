#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>

#include <ctime>
#include <string>
#include <vector>

#include "Engine/IScene.hpp"

class ScoreBoardScene final : public Engine::IScene {
   private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;
    struct ScoreBoardData {
        std::string _name;
        int _score;
        // datetime

        ScoreBoardData(std::string name, int score) : _name(name), _score(score) {}
    };

   public:
    explicit ScoreBoardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
};

#endif  // SCOREBOARDSCENE_HPP
