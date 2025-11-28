#include "PauseMenu.h"
#include "../../Game.h"
#include "../../Actors/Ship.h"

PauseMenu::PauseMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        mGame = game;
        Vector2 screenSize(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        UIRect* dimOverlay = AddRect(Vector2::Zero, screenSize, 1.0f, 0.0f, 50);
        dimOverlay->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.75f));

        UIButton* continueBtn = AddButton("Continue", [this]() {
                this->Close();
                if (mGame->GetPlayer()) {
                    mGame->GetPlayer()->SetState(ActorState::Active);
                }
                mGame->GetAudio()->ResumeSound(mGame->GetMusicHandle());
        }, Vector2(0.0f, -100.0f));

        continueBtn->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
        continueBtn->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));

        UIButton* exitBtn = AddButton("Exit", [this](){
                mGame->Quit();
        }, Vector2(0.0f, -200.0f));

        exitBtn->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
        exitBtn->SetBackgroundColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void PauseMenu::HandleKeyPress(int key) {
        if (mButtons.empty()) return;
    
        if(key == SDLK_DOWN || key == SDLK_s) {
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