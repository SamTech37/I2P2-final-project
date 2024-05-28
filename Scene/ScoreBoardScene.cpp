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
    currentIndex = -batchSize;
    loadNextBatch();
    if (onScreenRecords.size() == 0)
        drawEmptyNote(halfW, halfH);

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
    for (auto label : datetimeLabels) {
        RemoveObject(label->GetObjectIterator());
    }
    datetimeLabels.clear();

    for (int i = 0; i < onScreenRecords.size(); i++) {
        int recordY = halfH / 2 + 60 * i;
        int recordX = halfW - 600;
        int spacingX = 400;
        nameLabels.push_back(new Engine::Label(onScreenRecords[i]._name, "pirulen.ttf", 36, recordX, recordY, 255, 255, 255, 255, 0, 0.5));
        scoreLabels.push_back(new Engine::Label(std::to_string(onScreenRecords[i]._score), "pirulen.ttf", 36, recordX + spacingX, recordY, 255, 255, 255, 255, 0, 0.5));
        datetimeLabels.push_back(new Engine::Label(onScreenRecords[i].datetime, "pirulen.ttf", 36, recordX + 2 * spacingX, recordY, 255, 255, 255, 255, 0, 0.5));
        AddNewObject(nameLabels[i]);
        AddNewObject(scoreLabels[i]);
        AddNewObject(datetimeLabels[i]);
    }
}

// note:
// the file being read/write is build/Resource/scoreboard.txt
// instead of Resource/scoreboard.txt
// because that's how Cmake builds this project
// and whenever we build, the build folder is cleaned
void ScoreBoardScene::readRecordsFromFile() {
    std::string filename = "Resource/scoreboard.txt";
    std::string name;
    std::string datetime;
    int score;
    scoreRecords.clear();
    std::ifstream fin(filename);

    std::cout << "reading from scoreboard.txt:\n";
    // TODO refactor this to getline
    //  and deal with formatting issues
    while (fin >> name && fin >> score && fin >> datetime) {
        scoreRecords.push_back(ScoreBoardData(name, score, datetime));
        std::cout << "name = " << name
                  << " score = " << score
                  << " datetime = " << datetime
                  << "\n";
    }
    fin.close();
}
void ScoreBoardScene::drawEmptyNote(int halfW, int halfH) {
    nameLabels.push_back(
        new Engine::Label("No records yet.", "pirulen.ttf", 48, halfW, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));

    AddNewObject(nameLabels[0]);
    return;
}