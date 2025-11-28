//
// Created by Lucas N. Ferreira on 04/11/25.
//

#include "Bullet.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../Drawing/MeshComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"
#include "../../Actors/Block.h"

Bullet::Bullet(class Game* game)
        :Particle(game)
{
    mMeshComponent = new MeshComponent(this);
    Mesh* mesh = mGame->GetRenderer()->GetMesh("../Assets/Laser.gpmesh");
    mMeshComponent->SetMesh(mesh);

    SetScale(Vector3(5, 5, 5));
    mRigidBody = new RigidBodyComponent(this, 1, 0);
    mRigidBody->SetVelocity(Vector3(0, 0, 0));
    mCollisionComponent = new AABBColliderComponent(this, 10, 10, 10, Vector3::Zero, ColliderLayer::Bullet);
}

Bullet::~Bullet()
{
}

void Bullet::Kill() {
    Particle::Kill();

    mMeshComponent->SetVisible(false);
    mCollisionComponent->SetEnabled(false);
    mRigidBody->SetVelocity(Vector3::Zero);
}

void Bullet::Awake(const Vector3 &position, const Vector3 &rotation, float lifetime) {
    Particle::Awake(position, rotation, lifetime);

    mCollisionComponent->SetEnabled(true);
    mMeshComponent->SetVisible(true);
}

void Bullet::Emit(const Vector3 &direction, float speed) {
    mRigidBody->SetVelocity(direction * speed);
}

void Bullet::OnUpdate(float deltaTime) {
    Particle::OnUpdate(deltaTime);

    auto & obstacles = mGame->GetObstacles();

    for(auto *obt: obstacles){
        if(obt->GetCollider()->Intersect(mCollisionComponent)){
            if(obt->IsExploding()) {
                mGame->GetAudio()->PlaySound("BlockExplode.wav");
                obt->Explode();
            }
            Kill();
            return;
        }
    }

}