// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <fstream>
#include "Game.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Ship.h"
#include "Actors/Block.h"
#include "Camera.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/MainMenu.h"
#include "UI/Screens/PauseMenu.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mShip(nullptr)
        ,mCamera(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
        ,mNextBlock(0)
        ,mNextObstacle(0)
        ,mFadeState(FadeState::FadingIn)
        ,mFadeAlpha(1.0f)
{

}

bool Game::Initialize() {
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Init SDL Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    int mixFlags = MIX_INIT_OGG; // Ou MIX_INIT_MP3 se você usar MP3
    if (!(Mix_Init(mixFlags) & mixFlags)) {
        SDL_Log("Failed to initialize SDL_mixer codecs: %s", Mix_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP4: Star Fox", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mAudio = new AudioSystem();
    mAudio->CacheAllSounds();

    LoadScene(GameScene::MainMenu);

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::UnloadScene()
{
    // Use state so we can call this from withing an a actor update
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete obstacle patterns
    for (auto pattern : mObstaclePatterns) {
        for (auto obstacle : pattern) {
            delete obstacle;
        }
    }
    mObstaclePatterns.clear();

    mShip = nullptr;
    mCamera = nullptr;
    mHUD = nullptr;
}

void Game::SetScene(GameScene nextScene) {
    mNextScene = nextScene;
    mFadeState = FadeState::FadingOut;
}

void Game::LoadScene(GameScene nextScene) {
    UnloadScene();

    switch (nextScene) {
        case GameScene::MainMenu:
            new MainMenu(this, "../Assets/Fonts/Arial.ttf");
            break;
        case GameScene::Level1:
            LoadObstaclePatterns("../Assets/Blocks/", OBSTACLE_PATTERN_SIZE);
            mNextObstacle = 0;
            mMusicHandle = mAudio->PlaySound("Music.ogg", true);
            mShip = new Ship(this);
            mCamera = new Camera(this, Vector3(-300, 0, 0), Vector3(20, 0, 0), Vector3(0, 0, 1), 70.0, 10, 10000);
            mHUD = new HUD(this, "../Assets/Fonts/Arial.ttf");
    
            for(int i = 0; i < OBSTACLES_WAVES_IN_SCREEN; i++){
                SpawnObstacles(); 
            }
            
            for(mNextBlock = 0; mNextBlock < TUNNEL_DEPTH;) SpawnWalls();
            break;
        default:
            break;
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::LoadObstaclePatterns(const std::string& dirName, const int nBlockPatterns)
{
    for (int n = 0; n < nBlockPatterns; n++)
    {
        // Load pattern file
        std::string fileName = dirName + std::to_string(n + 1) + ".txt";
        std::ifstream file(fileName);

        if (!file.is_open())
        {
            SDL_Log("Failed to open block pattern file: %s", fileName.c_str());
            continue;
        }

        std::string line;

        // Create vector to store pattern
        std::vector<BlockObstacleItem *> pattern;

        int i = 0;
        while (std::getline(file, line))
        {
            int j = 0;
            std::vector<int> row;
            for (char c : line)
            {
                if (c == 'A')
                {
                    // Create BlockObstacleItem with texture type 3 and coordinates i,j
                    auto* blockPattern = new BlockObstacleItem({3, i, j});
                    pattern.emplace_back(blockPattern);
                }
                else if (c == 'B')
                {
                    // Create BlockObstacleItem with texture type 4 and coordinates i,j
                    auto* blockPattern = new BlockObstacleItem({4, i, j});
                    pattern.emplace_back(blockPattern);
                }

                j++;
            }
            i++;
        }

        // Add pattern to map
        mObstaclePatterns.emplace_back(pattern);

        file.close();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                { 
                    if (event.key.keysym.sym == SDLK_RETURN && mShip 
                        && mShip->GetState() == ActorState::Active) {
                        // Pausa o jogo
                        mShip->SetState(ActorState::Paused);
                        mAudio->PauseSound(mMusicHandle); 
                        new PauseMenu(this, "../Assets/Fonts/Arial.ttf");
                    } else if (!mUIStack.empty()) {
                        mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                    }
                    break;
                } 
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (mShip && mShip->GetState() == ActorState::Active) {
        for (auto actor : mActors) {
            actor->ProcessInput(state);
        }
    }
}

void Game::SpawnObstacles() {

    int patternIndex = 0;

    if(mNextObstacle < mObstaclePatterns.size()){
        patternIndex = mNextObstacle;
    } else  patternIndex = rand() % 20;
    
    float xPos = (mNextObstacle + 1) * OBSTACLE_PATTERN_GAP;

    const auto& pattern = mObstaclePatterns[patternIndex];
    for(const auto* item : pattern){
        Block* obstacle = new Block(this, true);
        obstacle->SetTexture(item->textureIndex);
        if(item->textureIndex == 4) obstacle->SetExploding(true);
        obstacle->SetScale(Vector3(OBSTACLE_SCALE, OBSTACLE_SCALE, OBSTACLE_SCALE));
        // obstacle->SetScale(Vector3(1.0f, 1.0f, 1.0f));

        float yOffset = -250.0f;
        float zOffset = -250.0f;

        Vector3 pos;
        pos.x = xPos;
        pos.y = (item->j * OBSTACLE_SCALE) + yOffset;
        pos.z = (item->i * OBSTACLE_SCALE) + zOffset; 
        
        obstacle->SetPosition(pos);
    }

    mNextObstacle++;
}

void Game::SpawnWalls() {

    Vector3 blockScale = Vector3(WALL_SCALE, WALL_SCALE, WALL_SCALE);
    Block* sky = new Block(this, false);
    sky->SetScale(blockScale);
    sky->SetPosition(Vector3(mNextBlock * 500, 0, 500));
    sky->SetTexture(6);
    Block* ground = new Block(this, false);
    ground->SetScale(blockScale);
    ground->SetPosition(Vector3(mNextBlock * 500, 0, -500));    
    ground->SetTexture(5);

    Block* right = new Block(this, false);
    right->SetScale(blockScale);
    right->SetPosition(Vector3(mNextBlock * 500, -500, 0));
    right->SetTexture(0);

    Block* left = new Block(this, false);
    left->SetScale(blockScale);
    left->SetPosition(Vector3(mNextBlock * 500, 500, 0));
    left->SetTexture(0);

    mNextBlock++;
}

void Game::UpdateGame(float deltaTime) {

    if (mFadeState == FadeState::FadingOut) {
        mFadeAlpha += 2.5f * deltaTime;
        if (mFadeAlpha >= 1.0f) {
            mFadeAlpha = 1.0f;
            LoadScene(mNextScene);
            mFadeState = FadeState::FadingIn;
        }
    } else if (mFadeState == FadeState::FadingIn) {
        mFadeAlpha -= 2.5f * deltaTime;
        if (mFadeAlpha <= 0.0f) {
            mFadeAlpha = 0.0f;
            mFadeState = FadeState::None;
        }
    }
    
    // Update all actors and pending actors
    if(mShip && mShip->GetState() == ActorState::Active){
        UpdateActors(deltaTime);
        if(mAudio) mAudio->Update(deltaTime);
        if(mCamera) mCamera->Update(deltaTime, mShip);
        if(mShip){
            Vector3 playerPos = mShip->GetPosition();
            float triggerDist = (mNextBlock - TUNNEL_DEPTH) * WALL_SCALE;
            if(playerPos.x > triggerDist) SpawnWalls();
            float obstacleDist = (mNextObstacle - OBSTACLES_WAVES_IN_SCREEN) * OBSTACLE_PATTERN_GAP;
            if(playerPos.x > obstacleDist) SpawnObstacles();

            if (mHUD) {
                // A pontuação é quantos "gaps" de 1000.0 o jogador cruzou
                int score = static_cast<int>(playerPos.x / OBSTACLE_PATTERN_GAP);
                mHUD->SetScore(score);
            }
        }
    }

    // Update UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UI that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddObstacle(Block* obstacle) {
    mObstacles.emplace_back(obstacle);
}

void Game::RemoveObstacle(Block* obstacle) {
    std::vector<class Block*>::iterator it;
    it = std::find(mObstacles.begin(), mObstacles.end(), obstacle);

    if (it != mObstacles.end()) mObstacles.erase(it);
}

void Game::DrawFade() {
    if (mFadeState != FadeState::None) {
        mRenderer->DrawFade(mFadeAlpha);
    }
}

void Game::GenerateOutput() {
    mRenderer->Clear();

    mRenderer->Draw();

    DrawFade();
    
    mRenderer->Present();
}

void Game::Shutdown()
{
    // Because ~Actor calls RemoveActor, have to use a different style loop
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete obstacle patterns
    for (auto pattern : mObstaclePatterns) {
        for (auto obstacle : pattern) {
            delete obstacle;
        }
    }
    mObstaclePatterns.clear();

    // Delete renderer
    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    delete mAudio;
    mAudio = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}