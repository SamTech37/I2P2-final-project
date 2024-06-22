#include "WinScene.hpp"

#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    // AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
    int spacingY = 60;
    AddNewObject(new Engine::Label("Game Over", "pirulen.ttf", 48, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score: " + std::to_string(score), "pirulen.ttf", 48, halfW, halfH / 4 + spacingY, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Enter Name:", "pirulen.ttf", 48, halfW, halfH / 4 + 2 * spacingY, 255, 255, 255, 255, 0.5, 0.5));

    // TODO: add a text input
    // and a counter to indicate the limit of the input

    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");

    writeScoreToFile();
}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void WinScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void WinScene::setScore(int score) {
    this->score = score;
}

// note:
// the file being read/write is build/Resource/scoreboard.txt
// instead of Resource/scoreboard.txt
// because that's how Cmake builds this project
// and when build, scoreboard.txt will be overwritten,
// so it looks unchanged

std::string getCurrentDatetime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;
    int hour = ltm->tm_hour;
    int min = ltm->tm_min;
    std::string yearStr = std::to_string(year);
    std::string monthStr = ((month < 10) ? "0" : "") + std::to_string(month);
    std::string dayStr = ((day < 10) ? "0" : "") + std::to_string(day);
    std::string hourStr = ((hour < 10) ? "0" : "") + std::to_string(hour);
    std::string minStr = ((min < 10) ? "0" : "") + std::to_string(min);
    // format: yyyy-mm-dd@hh:mm
    return yearStr + "-" + monthStr + "-" + dayStr + "@" + hourStr + ":" + minStr;
}

void WinScene::writeScoreToFile() {
    std::string filename = "Resource/scoreboard.txt";

    // std::ifstream fin(filename);
    // std::cout << "Current content of scoreboard.txt:\n";
    // if (fin.is_open()) {
    //     std::string line;
    //     while (getline(fin, line)) {
    //         std::cout << line << "\n";
    //     }
    //     fin.close();
    // }

    std::cout << "Now writing to scoreboard.txt:\n";
    std::ofstream fout(filename, std::ios::app);  // append mode
    if (!fout.is_open()) {
        return;
    }
    fout << "\n";     // make sure the new score is on a new line
    fout << "foobar"  // default name: foobar
         << " " << score
         << " " << getCurrentDatetime()
         << "\n";
    fout.close();

    // std::ifstream finModified(filename);
    // std::cout << "Modified content of scoreboard.txt:\n";
    // if (finModified.is_open()) {
    //     std::string line;
    //     while (getline(finModified, line)) {
    //         std::cout << line << "\n";
    //     }
    //     finModified.close();
    // }
}