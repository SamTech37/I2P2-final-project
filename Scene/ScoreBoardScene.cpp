#include "ScoreBoardScene.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>

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
    halfW = w / 2;
    halfH = h / 2;

    // initialize the scoreboard from file
    readRecordsFromFile();

    // sort the scoreboard by score or date
    std::sort(scoreRecords.begin(), scoreRecords.end(),
              // a lambda function to compare two ScoreBoardData
              [](const ScoreBoardData& a, const ScoreBoardData& b) {
                  return a._score > b._score;
              });

    // load the first batch
    currentIndex = -5;
    loadNextBatch();

    // title
    AddNewObject(new Engine::Label("SCORE BOARD", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));

    // go back button
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    // pagination button
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", 100, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::loadPrevBatch, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, 300, halfH * 7 / 4, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 300, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::loadNextBatch, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 48, halfW * 2 - 300, halfH * 7 / 4, 255, 255, 255, 255, 0.5, 0.5));

    bgmId = AudioHelper::PlayAudio("win.wav");
}
void ScoreBoardScene::Terminate() {
    nameLabels.clear();
    scoreLabels.clear();
    scoreRecords.clear();
    onScreenRecords.clear();
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
void ScoreBoardScene::loadNextBatch() {
    // do nothing if already at the end
    if (currentIndex + batchSize >= scoreRecords.size())
        return;

    currentIndex += batchSize;
    onScreenRecords.clear();
    for (int i = currentIndex; i < currentIndex + batchSize && i < scoreRecords.size(); i++) {
        onScreenRecords.push_back(scoreRecords[i]);
    }
    // redraw

    drawBatch(halfW, halfH);
}
void ScoreBoardScene::loadPrevBatch() {
    // do nothing if already at the beginning
    if (currentIndex - batchSize < 0)
        return;

    currentIndex -= batchSize;
    onScreenRecords.clear();
    for (int i = currentIndex; i < currentIndex + batchSize && i < scoreRecords.size(); i++) {
        onScreenRecords.push_back(scoreRecords[i]);
    }
    // redraw
    drawBatch(halfW, halfH);
}

void ScoreBoardScene::drawBatch(int halfW, int halfH) {
    for (auto label : nameLabels) {
        RemoveObject(label->GetObjectIterator());
    }
    nameLabels.clear();
    for (auto label : scoreLabels) {
        RemoveObject(label->GetObjectIterator());
    }
    scoreLabels.clear();

    for (int i = 0; i < onScreenRecords.size(); i++) {
        int recordY = halfH / 2 + 50 * i;
        int spacing = 50;
        nameLabels.push_back(new Engine::Label(onScreenRecords[i]._name, "pirulen.ttf", 36, halfW - 200, recordY, 255, 255, 255, 255, 0, 0.5));
        scoreLabels.push_back(new Engine::Label(std::to_string(onScreenRecords[i]._score), "pirulen.ttf", 36, halfW + 200, recordY, 255, 255, 255, 255, 0, 0.5));
        AddNewObject(nameLabels[i]);
        AddNewObject(scoreLabels[i]);
    }
}

// note:
// change in file doesn't instantly reflect in game
// solution?
void ScoreBoardScene::readRecordsFromFile() {
    std::string filename = "Resource/scoreboard.txt";
    std::string name;
    int score;
    scoreRecords.clear();
    std::ifstream fin(filename);
    while (fin >> name && fin >> score) {
        scoreRecords.push_back(ScoreBoardData(name, score));
        std::cout << "name = " << name << " score = " << score << "\n";
    }
    fin.close();
}