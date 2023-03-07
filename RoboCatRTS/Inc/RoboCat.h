class RoboCat : public GameObject
{
public:
	CLASS_IDENTIFICATION( 'RCAT', GameObject )

		enum RoboCatState
	{
		RC_IDLE,
		RC_MOVING,
		RC_ATTACK,
		RC_BUILD,
		RC_HEAL
	};

	static	GameObjectPtr StaticCreate() { return NetworkManager::sInstance->RegisterAndReturn( new RoboCat() ); }

	virtual	RoboCat* GetAsCat()	override { return this; }

	virtual void Update( float inDeltaTime )	override;
	void UpdateIdleState( float inDeltaTime );
	void UpdateMovingState( float inDeltaTime );
	void UpdateAttackState( float inDeltaTime );
	void UpdateBuildState( float inDeltaTime );
	void UpdateHealState( float inDeltaTime );
	// returns true if the move is done
	bool MoveToLocation( float inDeltaTime, const Vector3& inLocation );
	void UpdateRotation( const Vector3& inTarget );
	void EnterMovingState( const Vector3& inTarget );
	void EnterAttackState( uint32_t inTargetNetId );
	void EnterBuildState( uint32_t inType );
	void EnterHealState( uint32_t inTargetNetId );
	void TakeDamage( int inDmgAmount );
	bool IsFullHealth() { return mHealth >= mMaxHealth; }
	virtual void HandleDying() override;

	RoboCat();

	virtual void WriteForCRC( OutputMemoryBitStream& inStream ) override;
private:
	Vector3				mMoveLocation;

protected:
	uint32_t mType; // this is the type of RoboCat it is (normal, tank, etc)
	SpriteComponentPtr	mSpriteComponent;

	///move down here for padding reasons...
	int					mMaxHealth;
	int					mHealth;
	RoboCatState		mState;

	// attack information
	uint32_t			mTargetNetId;
	GameObjectPtr		mTargetCat;
	float				mTimeSinceLastAttack;

	// build information
	uint32_t			mBuildType;
	float			mBuildTime;
};

typedef shared_ptr< RoboCat >	RoboCatPtr;