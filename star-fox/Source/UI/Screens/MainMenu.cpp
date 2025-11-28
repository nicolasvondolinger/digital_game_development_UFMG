//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "MainMenu.h"
#include "../../Game.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        mGame = game;
        AddImage("../Assets/Textures/Logo.png", Vector2(0.0f, 150.0f), 0.5F);
        UIButton* startBtn = AddButton("Start Game", [this]() {
                this->Close();
                mGame->SetScene(GameScene::Level1); 
        }, Vector2(0.0f, -100.0f));

        startBtn->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
        startBtn->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));

        UIButton* exitBtn = AddButton("Exit", [this](){
                mGame->Quit();
        }, Vector2(0.0f, -200.0f));

        exitBtn->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
        exitBtn->SetBackgroundColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void MainMenu::HandleKeyPress(int key) {
        if (mButtons.empty()) return;
    
        if(key == SDLK_DOWN || key == SDLK_s){
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex++;
                mSelectedButtonIndex%=mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }else if(key == SDLK_UP || key == SDLK_w){
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex--;
                mSelectedButtonIndex%=mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key==SDLK_RETURN){
                mButtons[mSelectedButtonIndex]->OnClick();
        }

}