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
        std::string datetime;  // Add a semicolon here

        ScoreBoardData(std::string name, int score, std::string datetime) : _name(name), _score(score), datetime(datetime){};
    };
    int halfW, halfH;

    const int batchSize = 5;
    int currentIndex = 0;
    std::vector<Engine::Label*> nameLabels;
    std::vector<Engine::Label*> scoreLabels;
    std::vector<Engine::Label*> datetimeLabels;

    //  dummy data
    std::vector<ScoreBoardData> scoreRecords;
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
    void drawEmptyNote(int halfW, int halfH);
    void readRecordsFromFile();
};

#endif  // SCOREBOARDSCENE_HPP
