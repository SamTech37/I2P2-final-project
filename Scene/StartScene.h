//
// Implemeting the start scene
//

#ifndef INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#define INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#include <allegro5/allegro_audio.h>

#include <memory>

#include "Engine/IScene.hpp"

// final means can't be inherited
class StartScene final : public Engine::IScene {
   public:
    explicit StartScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void SettingsOnClick(int stage);
};
#endif  // INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
