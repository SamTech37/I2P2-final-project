#ifndef TEXTINPUT_HPP
#define TEXTINPUT_HPP
#include <allegro5/bitmap.h>

#include <functional>
#include <memory>
#include <string>

#include "Engine/IControl.hpp"
#include "Label.hpp"

namespace Engine {
/// <summary>
/// An auto-focused text input, changes textValue when key pressed.
/// </summary>
class TextInput : public Label, public IControl {
   private:
    int textLimit = 10;

   public:
    explicit TextInput(const std::string& text, const std::string& font, int fontSize, float x, float y, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255, float anchorX = 0, float anchorY = 0);
    bool Enabled = true;
    void OnKeyUp(int keyCode) override;
    std::string getTextValue();
};
}  // namespace Engine
#endif  // TEXTINPUT_HPP
