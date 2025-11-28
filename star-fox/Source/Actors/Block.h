//
// Created by Lucas N. Ferreira on 06/11/25.
//

#pragma once

#include "Actor.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

class Block : public Actor
{
public:
    Block(class Game* game, bool isObstacle, const bool isExploding = false);
    ~Block();

    void SetExploding(const bool isExploding) { mIsExploding = isExploding; }
    bool IsExploding() const { return mIsExploding; }
    bool isObstable() const { return mIsObstacle; }

    void Explode();

    // Get collider
    AABBColliderComponent* GetCollider() const { return mColliderComponent; }

    void SetTexture(size_t textureId) { mMeshComponent->SetTextureIndex(textureId); }

    void OnUpdate(float deltaTime) override;

private:
    bool mIsObstacle, mIsExploding;
    MeshComponent *mMeshComponent;
    AABBColliderComponent *mColliderComponent;
};


