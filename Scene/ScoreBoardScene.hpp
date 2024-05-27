#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>

#include <ctime>
#include <string>
#include <vector>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

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
    int halfW, halfH;

    const int batchSize = 5;
    int currentIndex = 0;
    std::vector<Engine::Label*> nameLabels;
    std::vector<Engine::Label*> scoreLabels;

    //  dummy data
    std::vector<ScoreBoardData> scoreRecords =
        {
            {"Alice", 100},
            {"Bob", 90},
            {"Charlie", 80},
            {"David", 70},
            {"Eve", 60},
            {"Frank", 50},
            {"Grace", 40},
            {"Heidi", 30},
            {"foobar", 2500},
            {"Ivan", 20},
            {"Judy", 10},
            {"Kevin", 9},
            {"Lily", 8},
            {"Mason", 7},
            {"Nancy", 6},
            {"Oscar", 5},
            {"Peter", 4},
            {"Quincy", 3}};
    std::vector<ScoreBoardData> onScreenRecords;

   public:
    explicit ScoreBoardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    void loadNextBatch();
    void loadPrevBatch();
    void drawBatch(int halfW, int halfH);
};

#endif  // SCOREBOARDSCENE_HPP
