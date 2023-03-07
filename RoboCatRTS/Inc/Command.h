class Command
{
public:
	enum ECommandType
	{
		CM_INVALID,
		CM_ATTACK,
		CM_MOVE,
		CM_CREATE,
		CM_HEAL,
		CM_MEOW
	};

	Command() :
	mCommandType( CM_INVALID ),
	mNetworkId( 0 ),
	mPlayerId( 0 )
	{}

	//given a buffer, will construct the appropriate command subclass
	static shared_ptr< Command > StaticReadAndCreate( InputMemoryBitStream& inInputStream );

	void SetNetworkId( uint32_t inId ) { mNetworkId = inId; }
	int GetNetworkId() const { return mNetworkId; }

	void SetPlayerId( uint32_t inId ) { mPlayerId = inId; }
	int GetPlayerId() const { return mPlayerId; }

	virtual void Write( OutputMemoryBitStream& inOutputStream );
	virtual void ProcessCommand() = 0;
protected:
	virtual void Read( InputMemoryBitStream& inInputStream ) = 0;

	ECommandType mCommandType;
	uint32_t mNetworkId;
	uint32_t mPlayerId;
};

typedef shared_ptr< Command >	CommandPtr;

#pragma region Attack Command

class AttackCommand : public Command
{
public:
	AttackCommand():
	mTargetNetId(0)
	{
		mCommandType = CM_ATTACK;
	}

	static shared_ptr< AttackCommand > StaticCreate( uint32_t inMyNetId, uint32_t inTargetNetId );

	virtual void Write( OutputMemoryBitStream& inOutputStream ) override;

	virtual void ProcessCommand() override;

protected:
	virtual void Read( InputMemoryBitStream& inInputStream ) override;

	uint32_t mTargetNetId;
};

typedef shared_ptr< AttackCommand > AttackCommandPtr;

#pragma endregion

#pragma region Move Command

class MoveCommand : public Command
{
public:
	MoveCommand()
	{
		mCommandType = CM_MOVE;
	}

	static shared_ptr< MoveCommand > StaticCreate( uint32_t inNetworkId, const Vector3& inTarget );

	virtual void Write( OutputMemoryBitStream& inOutputStream ) override;

	virtual void ProcessCommand() override;

protected:
	virtual void Read( InputMemoryBitStream& inInputStream ) override;

	Vector3 mTarget;
};

typedef shared_ptr< MoveCommand > MoveCommandPtr;

#pragma endregion

#pragma region Create Command

class CreateCommand : public Command
{
public:
	CreateCommand()
	{
		mCommandType = CM_CREATE;
	}

	static shared_ptr< CreateCommand > StaticCreate(uint32_t inNetworkId, uint32_t inType);

	virtual void Write(OutputMemoryBitStream& inOutputStream) override;

	virtual void ProcessCommand() override;

protected:
	virtual void Read(InputMemoryBitStream& inInputStream) override;

	uint32_t mType;
};

typedef shared_ptr< CreateCommand > CreateCommandPtr;

#pragma endregion

#pragma region Heal Command

class HealCommand : public Command
{
public:
	HealCommand() :
		mTargetNetId( 0 )
	{
		mCommandType = CM_HEAL;
	}

	static shared_ptr< HealCommand > StaticCreate( uint32_t inMyNetId, uint32_t inTargetNetId );

	virtual void Write( OutputMemoryBitStream& inOutputStream ) override;

	virtual void ProcessCommand() override;

protected:
	virtual void Read( InputMemoryBitStream& inInputStream ) override;

	uint32_t mTargetNetId;
};

typedef shared_ptr< HealCommand > HealCommandPtr;

#pragma endregion

#pragma region Meow Command

class MeowCommand : public Command
{
public:
	MeowCommand()
	{
		mCommandType = CM_MEOW;
	}

	static shared_ptr< MeowCommand > StaticCreate( uint32_t inMyNetId );

	virtual void Write( OutputMemoryBitStream& inOutputStream ) override;

	virtual void ProcessCommand() override;

protected:
	virtual void Read( InputMemoryBitStream& inInputStream ) override;
};

typedef shared_ptr< MeowCommand > MeowCommandPtr;

#pragma endregion