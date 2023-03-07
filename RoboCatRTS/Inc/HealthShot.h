class HealthShot : public GameObject
{
public:

	CLASS_IDENTIFICATION( 'HS', GameObject )

		static	GameObjectPtr StaticCreate() { return NetworkManager::sInstance->RegisterAndReturn( new HealthShot() ); }

	void			SetVelocity( const Vector3& inVelocity ) { mVelocity = inVelocity; }
	const Vector3& GetVelocity() const { return mVelocity; }

	void		InitFromShooter( GameObjectPtr inShooter, GameObjectPtr inTarget );

	virtual void Update( float inDeltaTime ) override;

	HealthShot();

protected:
	SpriteComponentPtr	mSpriteComponent;
	Vector3		mVelocity;
	GameObjectPtr mShooterCat;
	GameObjectPtr mTargetCat;

	float		mLifeSpan;
};

typedef shared_ptr< HealthShot >	HealthShotPtr;