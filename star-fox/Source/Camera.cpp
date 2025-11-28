//
// Created by Lucas N. Ferreira on 03/11/25.
//

#include "Camera.h"
#include "Game.h"
#include "Actors/Actor.h"

Camera::Camera(class Game* game, const Vector3 &eye, const Vector3 &target, const Vector3 &up,
        float foy, float near, float far)
      :mGame(game)
      ,mEye(eye)
      ,mTarget(target)
      ,mUp(up)
      ,mFovY(foy)
      ,mNear(near)
      ,mFar(far)
      ,mHDistance(300.0f)
      ,mVDistance(0.0f)
      ,mTDistance(20.0f)
{
  mViewMatrix= Matrix4::CreateLookAt(mEye, mTarget, mUp);
  mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
  mPerspProjMatrix = Matrix4::CreatePerspectiveFOV(mFovY, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, mNear, mFar);
  mGame->GetRenderer()->SetProjectionMatrix(mPerspProjMatrix);
}

void Camera::Update(float deltaTime, Actor *targetActor) {
  Vector3 target = targetActor->GetPosition() + targetActor->GetForward() * mTDistance;
  Vector3 eye = targetActor->GetPosition() - targetActor->GetForward() * mHDistance;
  eye += mUp * mVDistance;
  eye.z = 0.0f;
  mViewMatrix = Matrix4::CreateLookAt(eye, target, mUp);
  mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
}