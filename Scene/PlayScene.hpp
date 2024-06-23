#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>

#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

class Turret;
namespace Engine {
class Group;
class Image;
class Label;
class Sprite;
}  // namespace Engine

class PlayScene final : public Engine::IScene {
   private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;

   protected:
    int lives;
    int money;
    int SpeedMult;

   public:
    struct TurretUpgrades {
        int atkLevel;
        int cdLevel;
        int rangeLevel;
        const int cost = 50;
        const float atkMul = 0.2;
        void Reset() {
            atkLevel = 0;
            cdLevel = 0;
            rangeLevel = 0;
        }
        inline float GetAtk() const {
            return 1.0f + atkMul * atkLevel;
        }
    } turretUpgrades;
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks;
    float deathCountDown;

    int killCount = 0;
    int waveCount = 0;
    // Map tiles.
    Group* TileMapGroup;
    Group* GroundEffectGroup;
    Group* DebugIndicatorGroup;
    Group* BulletGroup;
    Group* TowerGroup;
    Group* EnemyGroup;
    Group* EffectGroup;
    Group* UIGroup;
    Engine::Label* UIKills;
    Engine::Label* UIWaves;
    Engine::Label* UIMoney;
    Engine::Label* UILives;
    Engine::Image* imgTarget;
    Engine::Sprite* dangerIndicator;
    // upgrade btns for turrets: atk,cd,range
    Group* UpgradeGroup;
    Engine::Label* UIAtkLv;
    Engine::Label* UICdLv;
    Engine::Label* UIRangeLv;

    Turret* preview;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    // randomly generate enemy wave data, 10 waves each time
    void GenerateEnemyWaveData();
    void ConstructUI();
    void UIBtnClicked(int id);
    void HandleUpgrade(int id);
    bool CheckSpaceValid(int x, int y);
    std::vector<std::vector<int>> CalculateBFSDistance();
    void killCountAdd(int val);
    void waveCountAdd(int val);
    void gameover();
};
#endif  // PLAYSCENE_HPP
