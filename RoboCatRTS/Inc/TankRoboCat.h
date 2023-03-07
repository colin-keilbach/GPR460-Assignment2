#pragma once

class TankRoboCat : public RoboCat {
public:
	CLASS_IDENTIFICATION('TCAT', GameObject)
	TankRoboCat();

	static	GameObjectPtr StaticCreate() { return NetworkManager::sInstance->RegisterAndReturn(new TankRoboCat()); }

	virtual	RoboCat* GetAsCat()	override { return this; }
};

typedef shared_ptr< TankRoboCat >	TankRoboCatPtr;