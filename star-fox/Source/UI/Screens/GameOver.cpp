//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "GameOver.h"
#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        AddText("Game Over", Vector2(0.0f, 100.0f));
        AddButton("Press Enter", [this]() {
                mGame->GetAudio()->StopAllSounds();
                this->Close(); 
                mGame->SetScene(GameScene::MainMenu); 
        }, Vector2(0.0f, -50.0f));
}

void GameOver::HandleKeyPress(int key) {
        UIScreen::HandleKeyPress(key);
        if(key == SDLK_RETURN && !mButtons.empty() && mSelectedButtonIndex != -1){
                mButtons[mSelectedButtonIndex]->OnClick();
        }
}