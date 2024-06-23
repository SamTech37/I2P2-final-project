
#ifndef UPGRADEBUTTON_HPP
#define UPGRADEBUTTON_HPP
#include <string>

#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"

class UpgradeButton : public Engine::ImageButton {
   protected:
    PlayScene* getPlayScene() {
        return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    }

   public:
    int cost;
    Engine::Sprite Icon;

    UpgradeButton(std::string img, std::string imgIn, Engine::Sprite Icon, float x, float y, int cost)
        : ImageButton(img, imgIn, x, y), cost(cost), Icon(Icon) {
    }
    void Update(float deltaTime) {
        ImageButton::Update(deltaTime);
        if (getPlayScene()->GetMoney() >= cost) {
            Enabled = true;
            Icon.Tint = al_map_rgba(255, 255, 255, 255);
        } else {
            Enabled = false;
            Icon.Tint = al_map_rgba(0, 0, 0, 160);
        }
    }
    void Draw() const {
        ImageButton::Draw();
        Icon.Draw();
    }
};

#endif  // UPGRADEBUTTON_HPP