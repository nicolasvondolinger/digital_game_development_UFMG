//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "Block.h"
#include "../Game.h"
#include "../Actors/Ship.h"

Block::Block(class Game* game, bool isObstacle, const bool isExploding)
        :Actor(game),
        mIsObstacle(isObstacle),
        mIsExploding(isExploding)
{
        mMeshComponent = new MeshComponent(this);
        // cout << "AQUI" << endl;
        // Mesh* mesh = mGame->GetRenderer()->GetMesh("../Assets/Arwing.gpmesh");
        Mesh* mesh = mGame->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
        // cout << "AQUI 2" << endl;
        mMeshComponent->SetMesh(mesh);

        if(mIsObstacle){
                mColliderComponent = new AABBColliderComponent(this, 1.0f, 1.0f, 1.0f, Vector3::Zero, ColliderLayer::Block, true);
                mGame->AddObstacle(this);
        }
        else mColliderComponent = new AABBColliderComponent(this, 1.0f, 1.0f, 1.0f, Vector3::Zero, ColliderLayer::Block);
}

Block::~Block() {
        if (mIsObstacle) mGame->RemoveObstacle(this);
}

void Block::OnUpdate(float deltaTime) {

        Vector3 playerPos = mGame->GetPlayer()->GetPosition();
        const float despawnDistance = 300.0f;
        if(GetPosition().x * 2 < (playerPos.x - despawnDistance)) {
                SetState(ActorState::Destroy);
        }
}

void Block::Explode(){
        if(GetState() == ActorState::Destroy) return;

        queue<Block*> toDestroy;
        toDestroy.push(this);
        while(!toDestroy.empty()){
                Block* temp = toDestroy.front();
                toDestroy.pop();
                temp->SetState(ActorState::Destroy);
                if (!temp->IsExploding()) continue;
                Vector3 currentPos = temp->GetPosition();
                const float radius = 50.0f;
                for(auto obstacle: mGame->GetObstacles()){
                        if(obstacle->GetState() == ActorState::Destroy) continue;
                        Vector3 dir = obstacle->GetPosition() - currentPos;
                        if (dir.Length() <= radius) {
                                
                                obstacle->SetState(ActorState::Destroy);

                                if (obstacle->IsExploding()) toDestroy.push(obstacle);
                                
                        }
                }
        }
}