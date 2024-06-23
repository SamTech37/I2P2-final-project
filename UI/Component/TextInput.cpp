
#include "TextInput.hpp"

#include <allegro5/allegro_font.h>

#include <memory>
#include <string>

#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Label.hpp"

namespace Engine {

TextInput::
    TextInput(const std::string& text, const std::string& font, int fontSize, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float anchorX, float anchorY)
    : Label(text, font, fontSize, x, y, r, g, b, a, anchorX, anchorY) {
}

std::string TextInput::getTextValue() {
    return this->Text;
}
void TextInput::OnKeyUp(int keyCode) {
    if (this->Enabled) {
        if (keyCode == ALLEGRO_KEY_BACKSPACE && this->Text.length() > 0) {
            this->Text.pop_back();
        } else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
            if (this->Text.length() < this->textLimit)
                this->Text.push_back('a' + keyCode - ALLEGRO_KEY_A);
        }
    }
}

}  // namespace Engine