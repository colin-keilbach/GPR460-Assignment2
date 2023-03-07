#include "RoboCatPCH.h"
#include <zlib.h>

const float kMoveSpeed = 2.5f;
const float kAttackRangeSq = 1.5f * 1.5f;
const float kBuildRangeSq = 0.5f * 0.5f;
const float kYarnCooldown = 1.0f;
const float kHealthShotCooldown = 2.0f;
const float kBuildTimer = 3.0f;

RoboCat::RoboCat() :
	GameObject(),
	mMaxHealth( 5 ),
	mHealth( 5 ),
	mState( RC_IDLE ),
	mTargetNetId( 0 ),
	mTimeSinceLastAttack( kYarnCooldown ),
	mType( 0 )
{
	SetScale( 0.75f );
	SetCollisionRadius( 0.6f );
	mSpriteComponent = std::make_shared<SpriteComponent>( this );
	mSpriteComponent->SetTexture( TextureManager::sInstance->GetTexture( "cat" ) );
}

void RoboCat::WriteForCRC( OutputMemoryBitStream& inStream )
{
	inStream.Write( mPlayerId );
	inStream.Write( mNetworkId );
	inStream.Write( mLocation );
	inStream.Write( mHealth );
	inStream.Write( mState );
	inStream.Write( mTargetNetId );
	inStream.Write( mType );
}

bool RoboCat::MoveToLocation( float inDeltaTime, const Vector3& inLocation )
{
	bool finishedMove = false;

	Vector3 toMoveVec = inLocation - GetLocation();
	float distToTarget = toMoveVec.Length();
	toMoveVec.Normalize2D();
	if (distToTarget > 0.1f)
	{
		if (distToTarget > ( kMoveSpeed * inDeltaTime ))
		{
			SetLocation( GetLocation() + toMoveVec * inDeltaTime * kMoveSpeed );
		}
		else
		{
			//we're basically almost there, so set it to move location
			SetLocation( inLocation );
			finishedMove = true;
		}
	}
	else
	{
		//since we're close, stop moving towards the target
		finishedMove = true;
	}

	return finishedMove;
}

void RoboCat::UpdateRotation( const Vector3& inTarget )
{
	Vector3 toMoveVec = inTarget - GetLocation();
	toMoveVec.Normalize2D();
	float angle = acosf( Dot2D( toMoveVec, Vector3::NegUnitY ) );
	Vector3 cross = Cross( Vector3::NegUnitY, toMoveVec );
	if (cross.mZ < 0.0f)
	{
		angle *= -1.0f;
	}
	SetRotation( angle );
}

#pragma region Enter States

void RoboCat::EnterMovingState( const Vector3& inTarget )
{
	mMoveLocation = inTarget;

	UpdateRotation( inTarget );

	mState = RC_MOVING;
}

void RoboCat::EnterAttackState( uint32_t inTargetNetId )
{
	mTargetNetId = inTargetNetId;

	//cache the target cat
	GameObjectPtr target = NetworkManager::sInstance->GetGameObject( mTargetNetId );
	//double check this attack target is valid
	if (target && ( target->GetClassId() == RoboCat::kClassId || target->GetClassId() == TankRoboCat::kClassId ) && target->GetPlayerId() != GetPlayerId())
	{
		mTargetCat = target;
		mState = RC_ATTACK;
	}
	else
	{
		mState = RC_IDLE;
	}
}

void RoboCat::EnterBuildState( uint32_t inType )
{
	mBuildType = inType;
	mBuildTime = 0;
	mState = RC_BUILD;
}

void RoboCat::EnterHealState( uint32_t inTargetNetId )
{
	mTargetNetId = inTargetNetId;

	//cache the target cat
	GameObjectPtr target = NetworkManager::sInstance->GetGameObject( mTargetNetId );
	//double check this attack target is valid
	if (target && ( target->GetClassId() == RoboCat::kClassId || target->GetClassId() == TankRoboCat::kClassId ) && target->GetPlayerId() == GetPlayerId())
	{
		mTargetCat = target;
		mState = RC_HEAL;
	}
	else
	{
		mState = RC_IDLE;
	}
}

#pragma endregion

void RoboCat::TakeDamage( int inDmgAmount )
{
	mHealth -= inDmgAmount;
	if (mHealth <= 0)
	{
		SetDoesWantToDie( true );
	}
	if (mHealth > mMaxHealth) mHealth = mMaxHealth;
}

void RoboCat::HandleDying()
{
	GameObject::HandleDying();
	ScoreBoardManager::sInstance->IncScore( mPlayerId, -1 );
}

#pragma region Update States

void RoboCat::Update( float inDeltaTime )
{
	switch (mState)
	{
	case RC_IDLE:
		UpdateIdleState( inDeltaTime );
		break;
	case RC_MOVING:
		UpdateMovingState( inDeltaTime );
		break;
	case RC_ATTACK:
		UpdateAttackState( inDeltaTime );
		break;
	case RC_BUILD:
		UpdateBuildState( inDeltaTime );
		break;
	case RC_HEAL:
		UpdateHealState( inDeltaTime );
		break;
	}
}

void RoboCat::UpdateIdleState( float inDeltaTime )
{
	//could do something cute here, like have the cat meow if idle for a while!
	mTimeSinceLastAttack += inDeltaTime;
}

void RoboCat::UpdateMovingState( float inDeltaTime )
{
	mTimeSinceLastAttack += inDeltaTime;
	if (MoveToLocation( inDeltaTime, mMoveLocation ))
	{
		//done with the move, so go idle
		mState = RC_IDLE;
	}
}

void RoboCat::UpdateAttackState( float inDeltaTime )
{
	mTimeSinceLastAttack += inDeltaTime;

	if (mTargetCat && !mTargetCat->DoesWantToDie())
	{
		//determine the distance to the target cat
		Vector3 diff = mTargetCat->GetLocation() - mLocation;
		float distSq = diff.LengthSq2D();

		//if we're in yarn cooldown, we aren't allowed to do anything
		if (mTimeSinceLastAttack >= kYarnCooldown)
		{
			UpdateRotation( mTargetCat->GetLocation() );
			if (distSq <= kAttackRangeSq)
			{
				//if we're in range, throw a ball of yarn
				mTimeSinceLastAttack = 0.0f;
				GameObjectPtr me = NetworkManager::sInstance->GetGameObject( mNetworkId );
				YarnPtr yarn = std::static_pointer_cast<Yarn>( GameObjectRegistry::sInstance->CreateGameObject( 'YARN' ) );
				yarn->InitFromShooter( me, mTargetCat );
			}
			else
			{
				MoveToLocation( inDeltaTime, mTargetCat->GetLocation() );
			}
		}
	}
	else
	{
		//target cat is dead
		mTargetCat.reset();
		mState = RC_IDLE;
	}
}

void RoboCat::UpdateBuildState( float inDeltaTime )
{
	mBuildTime += inDeltaTime;

	if (mBuildTime >= kBuildTimer)
	{
		NetworkManager::sInstance->SpawnCat( mPlayerId, mBuildType, mLocation );
		mState = RC_IDLE;
	}
}

void RoboCat::UpdateHealState( float inDeltaTime )
{
	mTimeSinceLastAttack += inDeltaTime;

	if (mTargetCat && !mTargetCat->DoesWantToDie() && !mTargetCat->GetAsCat()->IsFullHealth())
	{
		LOG( "Healing target is healable" );
		//determine the distance to the target cat
		Vector3 diff = mTargetCat->GetLocation() - mLocation;
		float distSq = diff.LengthSq2D();

		//if we're in yarn cooldown, we aren't allowed to do anything
		if (mTimeSinceLastAttack >= kHealthShotCooldown)
		{
			LOG( "Healing cooldown is up" );
			UpdateRotation( mTargetCat->GetLocation() );
			if (distSq <= kAttackRangeSq)
			{
				LOG( "Healing in range" );
				//if we're in range, throw a ball of yarn
				mTimeSinceLastAttack = 0.0f;
				GameObjectPtr me = NetworkManager::sInstance->GetGameObject( mNetworkId );
				LOG( "Healing" );
				HealthShotPtr healthshot = std::static_pointer_cast<HealthShot>( GameObjectRegistry::sInstance->CreateGameObject( 'HS' ) );
				LOG( "Healing created" );
				healthshot->InitFromShooter( me, mTargetCat );
				LOG( "Healing fired" );
			}
			else
			{
				LOG( "Healing out of range" );
				MoveToLocation( inDeltaTime, mTargetCat->GetLocation() );
			}
		}
	}
	else
	{
		//target cat is dead or at full health
		mTargetCat.reset();
		mState = RC_IDLE;
	}
}

#pragma endregion