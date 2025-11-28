#pragma once

#include "UIScreen.h"

class PauseMenu : public UIScreen
{
public:
    PauseMenu(class Game* game, const std::string& fontName);

    void HandleKeyPress(int key) override;
};