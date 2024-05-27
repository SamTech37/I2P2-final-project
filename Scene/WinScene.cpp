#include "WinScene.hpp"

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
    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("Score: " + std::to_string(score), "pirulen.ttf", 48, halfW, halfH / 4 + 50, 255, 255, 255, 255, 0.5, 0.5));

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

void WinScene::writeScoreToFile() {
    std::string filename = "Resource/scoreboard.txt";

    std::ifstream fin(filename);
    std::cout << "Current content of scoreboard.txt:\n";
    if (fin.is_open()) {
        std::string line;
        while (getline(fin, line)) {
            std::cout << line << "\n";
        }
        fin.close();
    }

    std::cout << "Now writing to scoreboard.txt:\n";
    std::ofstream fout(filename, std::ios::app);  // append mode
    if (!fout.is_open()) {
        return;
    }
    fout << "\n";                              // make sure the new score is on a new line
    fout << "foobar" << " " << score << "\n";  // default name: foobar
    fout.close();

    std::ifstream finModified(filename);
    std::cout << "Modified content of scoreboard.txt:\n";
    if (finModified.is_open()) {
        std::string line;
        while (getline(finModified, line)) {
            std::cout << line << "\n";
        }
        finModified.close();
    }
}