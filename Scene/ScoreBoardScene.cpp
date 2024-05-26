#include "ScoreBoardScene.hpp"

#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void ScoreBoardScene::Initialize() {
    // scene property
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // initialize the scoreboard from file
    //  dummy data
    std::vector<ScoreBoardData> scoreRecords = {
        {"AAA", 100},
        {"BBB", 90},
        {"CCC", 80},
        {"DDD", 70},
        {"EEE", 60},
        {"foobar", 2500},
        {"GGG", 40},
        {"HHH", 30},
        {"III", 20},
        {"JJJ", 10},
    };
    // TODO: sort the scoreboard by score or date

    // title
    AddNewObject(new Engine::Label("SCORE BOARD", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    // the scoreboard
    for (int i = 0; i < scoreRecords.size(); i++) {
        int recordY = halfH / 2 + 50 * i;
        int spacing = 50;

        AddNewObject(new Engine::Label(scoreRecords[i]._name, "pirulen.ttf", 36, halfW - 200, recordY, 255, 255, 255, 255, 0, 0.5));
        AddNewObject(new Engine::Label(std::to_string(scoreRecords[i]._score), "pirulen.ttf", 36, halfW + 200, recordY, 255, 255, 255, 255, 1, 0.5));
    }

    // go back button
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    // pagination button
    // btn  = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 + 150, 400, 100);

    bgmId = AudioHelper::PlayAudio("win.wav");
}
void ScoreBoardScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void ScoreBoardScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void ScoreBoardScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
