#pragma once

class TankRoboCat : public RoboCat {
public:
	CLASS_IDENTIFICATION('TCAT', GameObject)
	TankRoboCat();

	static	GameObjectPtr StaticCreate() { return NetworkManager::sInstance->RegisterAndReturn(new TankRoboCat()); }
};

typedef shared_ptr< TankRoboCat >	TankRoboCatPtr;