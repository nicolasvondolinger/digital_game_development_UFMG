//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{   
    Vector2 healthBarPos = Vector2(-200, 350);     
    Vector2 scoreLabelPos = Vector2(350.0f, 350.0f);
    Vector2 scoreValuePos = Vector2(450.0f, 350.0f);

    // Camada Base (Vermelha)
    mHealth1 = AddImage("../Assets/HUD/ShieldRed.png", healthBarPos, 0.75f);
    
    // Camada do Meio (Laranja)
    mHealth2 = AddImage("../Assets/HUD/ShieldOrange.png", healthBarPos, 0.75f);

    // Camada do Topo (Azul)
    mHealth3 = AddImage("../Assets/HUD/ShieldBlue.png", healthBarPos, 0.75f);
    
    // Moldura (ShieldBar) 
    AddImage("../Assets/HUD/ShieldBar.png", healthBarPos, 0.75f);
    
    AddText("Score: ", scoreLabelPos);
    mScore = AddText("0", scoreValuePos);
}

void HUD::SetHealth(int health) {
    if (health == 3) {
        mHealth1->SetIsVisible(true);
        mHealth2->SetIsVisible(true);
        mHealth3->SetIsVisible(true);
    } else if (health == 2) {
        mHealth1->SetIsVisible(true);
        mHealth2->SetIsVisible(true);
        mHealth3->SetIsVisible(false);
    } else if (health == 1) {
        mHealth1->SetIsVisible(true);
        mHealth2->SetIsVisible(false);
        mHealth3->SetIsVisible(false);
    } else {
        mHealth1->SetIsVisible(false);
        mHealth2->SetIsVisible(false);
        mHealth3->SetIsVisible(false);
    }
}

void HUD::SetScore(int score) {
    mScore->SetText(to_string(score));
}
