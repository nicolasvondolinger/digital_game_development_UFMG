//
// Created by Lucas N. Ferreira on 02/11/25.
//

#include "Ship.h"
#include "../Actors/Block.h"
#include "../Game.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../UI/Screens/HUD.h"
#include "../UI/Screens/GameOver.h"

Ship::Ship(class Game* game)
        : Actor(game)
        , mMeshComponent(nullptr)
        , mCollisionComponent(nullptr)
        , mRigidBody(nullptr)
        , mLaserCooldown(0.f)
        , mInvincibleCooldown(0.f)
        , mHealth(3)
        , mIsInvincible(false)
        , mTurnDirection(0.0f)
        ,mTurnAcceleration(0.0f)
        ,mTurnVelocity(0.0f)
        ,mSpeed(400.0f)
        ,mTargetRoll(0.0f)
        ,mTargetPitch(0.0f)
        ,mCurrentRoll(0.0f)
        ,mCurrentPitch(0.0f)
        ,mLaserWeapon(nullptr)
{
    mMeshComponent = new MeshComponent(this);
    Mesh* mesh = mGame->GetRenderer()->GetMesh("../Assets/Arwing.gpmesh");
    mMeshComponent->SetMesh(mesh);

    mCollisionComponent = new AABBColliderComponent(this, 15.0f, 40.0f, 25.0f, Vector3::Zero, ColliderLayer::Player);

    mRigidBody = new RigidBodyComponent(this, 1.0f, 2.0f);
    
    mRigidBody->SetVelocity(Vector3(400.0f, 0.0f, 0.0f));

    mLaserWeapon = new ParticleSystemComponent<Bullet>(this, 50);

    mShipSound = mGame->GetAudio()->PlaySound("ShipLoop.ogg", true);

    SetScale(Vector3(2.0f, 2.0f, 2.0f));
}

Ship::~Ship()
{
}

void Ship::DealDamage(const int damage) {

    if(mIsInvincible) return;
    mHealth-=damage;
    mIsInvincible = true; mInvincibleCooldown = 2.0f;
    mGame->GetHUD()->SetHealth(mHealth);
    mGame->GetAudio()->PlaySound("ShipHit.wav");
    if(mHealth <= 0){
        SetState(ActorState::Paused);
        new GameOver(mGame, "../Assets/Fonts/Arial.ttf");
        mGame->GetAudio()->PlaySound("ShipDie.wav");
        
        mGame->GetAudio()->StopSound(mShipSound);
        mGame->GetAudio()->StopSound(mAlertSound);
    } 

    if (mHealth == 1) {
        mAlertSound = mGame->GetAudio()->PlaySound("DamageAlert.ogg", true);
    }
}

void Ship::SetSpeed(const float speed)
{
    mSpeed = speed;
    mRigidBody->SetVelocity(Vector3(mSpeed, 0.0f, 0.0f));
}

void Ship::OnUpdate(float deltaTime) {

    if (mIsInvincible) {
        mInvincibleCooldown -= deltaTime;
        if (mInvincibleCooldown <= 0.0f) {
            mIsInvincible = false;
        }
    }

    if (mLaserCooldown > 0.0f) {
        mLaserCooldown -= deltaTime;
    }

    Vector3 pos = GetPosition();

    if(pos.y > 180.0f) pos.y = 180.0f;
    else if(pos.y < -180.0f) pos.y = -180.0f;

    if(pos.z > 225.0f) pos.z = 225.0f;
    else if(pos.z < -225.0f) pos.z = -225.0f;

    SetPosition(pos);

    const auto& obstacles = mGame->GetObstacles();
    for(auto* obt: obstacles){
        if (obt->GetState() == ActorState::Destroy) continue;
        
        if(mCollisionComponent->Intersect(obt->GetCollider())){
            obt->Explode();
            DealDamage(1);
            // cout << "aqui" << endl;
        }
    }

    float interpSpeed = 7.5f;
    mCurrentRoll = Math::Lerp(mCurrentRoll, mTargetRoll, interpSpeed * deltaTime);
    mCurrentPitch = Math::Lerp(mCurrentPitch, mTargetPitch, interpSpeed * deltaTime);

    Vector3 eulerRotation = Vector3(mCurrentRoll, mCurrentPitch, 0.0f);

    SetRotation(eulerRotation);
}

void Ship::OnProcessInput(const Uint8 *keyState) {

    float targetRoll = 0.0f;
    float targetPitch = 0.0f;

    if (keyState[SDL_SCANCODE_W]) {
        mRigidBody->ApplyForce(Vector3(0.0f, 0.0f, 1000.0f));
        targetPitch = Math::ToRadians(-20.0f);
    }
    
    if (keyState[SDL_SCANCODE_S]) {
        mRigidBody->ApplyForce(Vector3(0.0f, 0.0f, -1000.0f));
        targetPitch = Math::ToRadians(20.0f);
    }

    if (keyState[SDL_SCANCODE_A]) {
        mRigidBody->ApplyForce(Vector3(0.0f, -1000.0f, 0.0f));
        targetRoll = Math::ToRadians(20.0f);
    }

    if (keyState[SDL_SCANCODE_D]) {
        mRigidBody->ApplyForce(Vector3(0.0f, 1000.0f, 0.0f));
        targetRoll = Math::ToRadians(-20.0f);
    }

    mTargetRoll = targetRoll;
    mTargetPitch = targetPitch;

    if(keyState[SDL_SCANCODE_SPACE]){
        if(mLaserCooldown > 0.0f) return;

        mGame->GetAudio()->PlaySound("Shoot.wav");

        mLaserCooldown = 0.25f;

        Vector3 offset = GetForward() * 15.0f;
            
        float speed = mSpeed * 2.0f;        
        
        float lifetime = 1.0f;

        mLaserWeapon->EmitParticle(lifetime, speed, offset);
    }
}