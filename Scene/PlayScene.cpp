#include "PlayScene.hpp"

#include <allegro5/allegro.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "Enemy/Enemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/Resources.hpp"
#include "Scene/WinScene.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "Turret/WaveTurret.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1)};
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
// the Konami Command cheat code feature
const int CheatEarn = 10000;
const std::vector<int> PlayScene::code = {ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
                                          ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
                                          ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER};
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    killCount = 0;
    waveCount = 0;
    SpeedMult = 1;
    srand(time(nullptr));
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    AddNewControlObject(UpgradeGroup = new Group());
    ReadMap();
    GenerateEnemyWaveData();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    /* a buggy death count down feature

    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper
    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
    // Calculate danger zone.
    std::vector<float> reachEndTimes;
    for (auto& it : EnemyGroup->GetObjects()) {
        reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto& it : reachEndTimes) {
        if (it <= DangerTime) {
            danger--;
            if (danger <= 0) {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown) {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
                }
                float alpha = pos / DangerTime;
                alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0) {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;
    */

    // spawn enemies
    for (int i = 0; i < SpeedMult; i++) {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        if (enemyWaveData.empty()) {
            GenerateEnemyWaveData();
            waveCountAdd(10);
            Enemy::getUpgradeMultiplier(waveCount);
            continue;
        }
        auto current = enemyWaveData.front();
        if (ticks < current.second)  // keep waiting
            continue;
        ticks -= current.second;
        enemyWaveData.pop_front();

        const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
        Enemy* enemy;
        switch (current.first) {
            case 1:
                EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 2:
                EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 3:
                EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
                // TODO: [CUSTOM-ENEMY]: You need to modify 'Resource/enemy1.txt', or 'Resource/enemy2.txt' to spawn the 4th enemy.
                //         The format is "[EnemyId] [TimeDelay] [Repeat]".
                // TODO: [CUSTOM-ENEMY]: Enable the creation of the enemy.
            default:
                continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
}
void PlayScene::Draw() const {
    IScene::Draw();
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && preview) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1) {
        if (mapState[y][x] != TILE_OCCUPIED) {
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y)) {
                // std::cout << "current space (" << x << "," << y << ") is invalid\n";
                Engine::Sprite* sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            TowerGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);
            // Remove Preview.
            preview = nullptr;

            mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    } else {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();
        if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size()) {
            auto it = keyStrokes.begin();
            for (int c : code) {
                if (!((*it == c) ||
                      (c == ALLEGRO_KEYMOD_SHIFT &&
                       (*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
                    return;
                ++it;
            }
            // key input matches the cheat code
            EffectGroup->AddNewObject(new Plane());
            EarnMoney(CheatEarn);
        }
    }
    if (keyCode == ALLEGRO_KEY_Q) {
        // Hotkey for MachineGunTurret.
        UIBtnClicked(0);
    } else if (keyCode == ALLEGRO_KEY_W) {
        // Hotkey for LaserTurret.
        UIBtnClicked(1);
    } else if (keyCode == ALLEGRO_KEY_E) {
        // Hotkey for MissileTurret.
        UIBtnClicked(2);
    }
    // TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
}
void PlayScene::Hit() {
    lives--;
    UILives->Text = std::string("Life ") + std::to_string(lives);
    if (lives <= 0) {
        // Engine::GameEngine::GetInstance().ChangeScene("lose");
        gameover();
    }
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::killCountAdd(int val) {
    killCount += val;
    UIKills->Text = std::string("kills ") + std::to_string(killCount);
}
void PlayScene::waveCountAdd(int val) {
    waveCount += val;
    UIWaves->Text = std::string("Wave ") + std::to_string(waveCount);
}
void PlayScene::ReadMap() {
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c) {
        switch (c) {
            case '0':
                mapData.push_back(false);
                break;
            case '1':
                mapData.push_back(true);
                break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0)
                    throw std::ios_base::failure("Map data is corrupted.");
                break;
            default:
                throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data is corrupted.");
    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void PlayScene::GenerateEnemyWaveData() {
    const int enemyTypes = 3;  // infantry, plane, tank
    const int maxWait = 2;
    int maxRepeat, minRepeat;
    int type, wait, repeat;
    // std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // enemyWaveData.clear();
    // std::ifstream fin(filename);
    // while (fin >> type && fin >> wait && fin >> repeat) {
    //     for (int i = 0; i < repeat; i++)
    //         enemyWaveData.emplace_back(type, wait);
    // }
    // fin.close();
    for (int i = 0; i < 10; i++) {
        type = rand() % enemyTypes + 1;  // 1~3
        wait = rand() % maxWait;         // 0~2
        if (type == 1)
            maxRepeat = 20, minRepeat = 5;
        else if (type == 2)
            maxRepeat = 10, minRepeat = 3;
        else if (type == 3)
            maxRepeat = 5, minRepeat = 1;

        repeat = rand() % (maxRepeat - minRepeat + 1) + minRepeat;  // 1~20
        for (int j = 0; j < repeat; j++)
            enemyWaveData.emplace_back(type, wait);
    }
}
void PlayScene::ConstructUI() {
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    const int textUIHeight = 30;
    UIGroup->AddNewObject(UIKills = new Engine::Label(std::string("kills ") + std::to_string(killCount), "pirulen.ttf", 24, 1294, textUIHeight * 1));
    UIGroup->AddNewObject(UIWaves = new Engine::Label(std::string("waves ") + std::to_string(waveCount), "pirulen.ttf", 24, 1294, textUIHeight * 2));
    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, textUIHeight * 3));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, textUIHeight * 4));
    TurretButton* btn;
    // Button 1
    const int TurretBtnX[4] = {1294, 1370, 1446, 1446 + 76};
    const int TurretBtnY = 200;
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", TurretBtnX[0], TurretBtnY, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", TurretBtnX[0], TurretBtnY - 8, 0, 0, 0, 0), TurretBtnX[0], TurretBtnY, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    // Button 2
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", TurretBtnX[1], TurretBtnY, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", TurretBtnX[1], TurretBtnY - 8, 0, 0, 0, 0), TurretBtnX[1], TurretBtnY, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    // Button 3
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", TurretBtnX[2], TurretBtnY, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-3.png", TurretBtnX[2], TurretBtnY, 0, 0, 0, 0), TurretBtnX[2], TurretBtnY, MissileTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    // TODO: [CUSTOM-TURRET]: Create a button to support constructing the turret.

    // Button 4, my turret
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", TurretBtnX[3], TurretBtnY, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-7.png", TurretBtnX[3], TurretBtnY, 0, 0, 0, 0), TurretBtnX[3], TurretBtnY, WaveTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);

    Engine::ImageButton* GameOverBtn;
    GameOverBtn = new Engine::ImageButton("play/floor.png", "play/dirt.png", 1350, h - 100, 200, 50, 0, 0);
    GameOverBtn->SetOnClickCallback([this]() {
        gameover();
    });
    AddNewControlObject(GameOverBtn);
    AddNewObject(
        new Engine::Label(
            "Quit", "pirulen.ttf", 20, 1450, h - 80, 0, 0, 0, 255, 0.5, 0.5));
}

void PlayScene::UIBtnClicked(int id) {
    if (preview)
        UIGroup->RemoveObject(preview->GetObjectIterator());
    // TODO: [CUSTOM-TURRET]: On callback, create the turret.
    if (id == 0 && money >= MachineGunTurret::Price)
        preview = new MachineGunTurret(0, 0);
    else if (id == 1 && money >= LaserTurret::Price)
        preview = new LaserTurret(0, 0);
    else if (id == 2 && money >= MissileTurret::Price)
        preview = new MissileTurret(0, 0);
    else if (id == 3 && money >= WaveTurret::Price)
        preview = new WaveTurret(0, 0);
    if (!preview)
        return;
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    // invalid coordinates
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        std::cout << "case1\n";
        return false;
    }

    // placing turrets should not block all enemy paths
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1) {
        std::cout << "case2\n";  // blocking right-bottom corner
        return false;
    }
    for (auto& it : EnemyGroup->GetObjects()) {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0) pnt.x = 0;
        if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
        if (pnt.y < 0) pnt.y = 0;
        if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
        if (map[pnt.y][pnt.x] == -1) {
            std::cout << "case3\n";  // blocking all enemy paths
            return false;
        }
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto& it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.

    // NOTE: map[y][x] and queue of point(x,y)
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;

    // Push end point (the right-bottom corner of map)
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();

        // left
        Engine::Point NextPoint(p.x - 1, p.y);
        if (0 <= NextPoint.y && NextPoint.y <= MapHeight - 1 &&  // valid y
            0 <= NextPoint.x && NextPoint.x <= MapWidth - 1      // valid x
            && mapState[NextPoint.y][NextPoint.x] == TILE_DIRT   // empty tile
            && map[NextPoint.y][NextPoint.x] == -1               // not visited
        ) {
            // assign distance & push to queue
            map[NextPoint.y][NextPoint.x] = map[p.y][p.x] + 1;
            que.push(NextPoint);
        }
        // right
        NextPoint.x = p.x + 1;
        if (0 <= NextPoint.y && NextPoint.y <= MapHeight - 1 &&  // valid y
            0 <= NextPoint.x && NextPoint.x <= MapWidth - 1      // valid x
            && mapState[NextPoint.y][NextPoint.x] == TILE_DIRT   // empty tile
            && map[NextPoint.y][NextPoint.x] == -1               // not visited
        ) {
            // assign distance
            map[NextPoint.y][NextPoint.x] = map[p.y][p.x] + 1;
            que.push(NextPoint);
        }
        // up
        NextPoint.x = p.x;
        NextPoint.y = p.y - 1;
        if (0 <= NextPoint.y && NextPoint.y <= MapHeight - 1 &&  // valid y
            0 <= NextPoint.x && NextPoint.x <= MapWidth - 1      // valid x
            && mapState[NextPoint.y][NextPoint.x] == TILE_DIRT   // empty tile
            && map[NextPoint.y][NextPoint.x] == -1               // not visited
        ) {
            // assign distance
            map[NextPoint.y][NextPoint.x] = map[p.y][p.x] + 1;
            que.push(NextPoint);
        }
        // bottom
        NextPoint.x = p.x;
        NextPoint.y = p.y + 1;
        if (0 <= NextPoint.y && NextPoint.y <= MapHeight - 1 &&  // valid y
            0 <= NextPoint.x && NextPoint.x <= MapWidth - 1      // valid x
            && mapState[NextPoint.y][NextPoint.x] == TILE_DIRT   // empty tile
            && map[NextPoint.y][NextPoint.x] == -1               // not visited
        ) {
            // assign distance
            map[NextPoint.y][NextPoint.x] = map[p.y][p.x] + 1;
            que.push(NextPoint);
        }

        // Done: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
        //               For each step you should assign the corresponding distance to the most right-bottom block.
        //               mapState[y][x] is TILE_DIRT if it is empty.
    }

    // testing
    // std::cout << "current distances:\n";
    // for (auto row : map) {
    //     for (auto it : row) {
    //         std::cout << it << " ";
    //     }
    //     std::cout << "\n";
    // }

    return map;
}

void PlayScene::gameover() {
    // calculate score
    int finalScore = money + lives * 100;

    // pass the score to win scene
    // it's certain that this returns a Winscene
    // so we do a static cast
    WinScene* winScene = static_cast<WinScene*>(Engine::GameEngine::GetInstance().GetScene("win"));
    winScene->setScore(finalScore);
    Engine::GameEngine::GetInstance().ChangeScene("win");
}