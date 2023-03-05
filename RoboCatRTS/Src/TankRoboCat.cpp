#include "RoboCatPCH.h"

const float kMoveSpeed = 2.5f;
const float kAttackRangeSq = 1.5f * 1.5f;
const float kYarnCooldown = 1.0f;

TankRoboCat::TankRoboCat() :
	RoboCat()
{
	mHealth = 10;
	mState = RC_IDLE;
	mTargetNetId = 0;
	mTimeSinceLastAttack = kYarnCooldown;
	mType = 1;
	SetScale(1.0f);
	SetCollisionRadius(0.9f);
	mSpriteComponent = std::make_shared<SpriteComponent>(this);
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("cat"));
}