#pragma once
class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);
	static AIEnemyRef CreateEnermy();
public:
	static atomic<int64> s_playeridGenerator;
	static atomic<int64> s_AIidGenerator;
};
