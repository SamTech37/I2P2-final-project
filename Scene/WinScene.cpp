#include "WinScene.hpp"

#include <curl/curl.h>

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
#include "UI/Component/TextInput.hpp"
#include "json.hpp"

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
    // a text input
    AddNewControlObject(nameInput = new Engine::TextInput("foobar", "pirulen.ttf", 48, halfW, halfH / 4 + 3 * spacingY, 0, 255, 0, 255, 0.5, 0.5));

    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    // btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH, 400, 100);
    // btn->SetOnClickCallback(std::bind(&WinScene::writeScoreRecordToRemote, this));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("test", "pirulen.ttf", 48, halfW, halfH + 50, 0, 0, 0, 255, 0.5, 0.5));

    bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate() {
    // writeScoreRecordToFile();
    writeScoreRecordToRemote();
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

void WinScene::writeScoreRecordToFile() {
    std::string playerName = (nameInput->Text.length() >= 0) ? nameInput->Text : "foobar";  // default name: foobar

    std::string filename = "Resource/scoreboard.txt";

    std::cout << "Now writing to scoreboard.txt:\n";
    std::ofstream fout(filename, std::ios::app);  // append mode
    if (!fout.is_open()) {
        return;
    }
    fout << "\n";  // make sure the new score is on a new line
    fout << playerName
         << " " << score
         << " " << getCurrentDatetime()
         << "\n";
    fout.close();
}

// scoreRecord on remote:
//    playerName, score, datetime

using json = nlohmann::json;

void WinScene::writeScoreRecordToRemote() {
    std::string requestURL = "https://i2p2-server.vercel.app/api";
    CURL* curl;
    CURLcode res;

    std::string playerName = (nameInput->Text.length() >= 0) ? nameInput->Text : "foobar";  // default name: foobar
    json j;
    j["playerName"] = playerName;
    j["score"] = score;
    j["datetime"] = getCurrentDatetime();

    curl = curl_easy_init();
    // serialize the json
    std::string postData = j.dump();

    if (curl) {
        // set up the request
        curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // SSL & CA cert stuff
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 1);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "Resource/cacert.pem");
        curl_easy_setopt(curl, CURLOPT_CAPATH, "Resource/cacert.pem");

        // perform the request
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            std::cout << "Successfully wrote score record to remote.\n";
        } else {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        }

    } else {
        std::cerr << "curl_easy_init() failed\n";
    }
}