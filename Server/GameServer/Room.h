#pragma once
#include "JobQueue.h"
class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	void Tick();
	RoomRef GetRoomRef();

public:
	//Player
	bool SetSpawnSize(uint64 size);
	bool EnterRoom(ObjectRef object, bool randPos = true);
	bool LeaveRoom(ObjectRef object);

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::C_MOVE pkt);

	void HandleAttack(Protocol::C_FIRE pkt);
	void HandleHit(Protocol::C_HIT pkt);
	void HandleDamaged(Protocol::C_DAMAGED pkt);
	void HandleDead(Protocol::C_PLAYERDEAD pkt);
	void HandleExpUP(Protocol::C_EXPUP pkt);
	void HandleLVUP(Protocol::C_LVUP pkt);

	void HandleSkillBomb(Protocol::C_PLAYERSKILL_BOMB pkt);
	void HandleSkillChemical(Protocol::C_PLAYERSKILL_CHEMICAL pkt);
	void HandleSkillGranade(Protocol::C_PLAYERSKILL_GRANADE pkt);
	void HandleSkillGuard(Protocol::C_PLAYERSKILL_GUARD pkt);
	void HandleSkillHeal(Protocol::C_PLAYERSKILL_HEAL pkt);
	void HandleUPdateHeal(Protocol::C_PLAYERHEAL pkt);

	void HandleMakeDrone(Protocol::C_MAKEDRONE pkt);
	void HandleSearchDrone(Protocol::C_SEARCHDRONE pkt);
	void HandleMoveDrone(Protocol::C_MOVEDRONE pkt);
	void HandleReturnDrone(Protocol::C_RETURNDRONE pkt);
	void HandleAttackDrone(Protocol::C_ATTACKDRONE pkt);
	
	void HandleEatItemMaxHP(Protocol::C_EATITEM_MAXHPUP pkt);
	void HandleEatItemLVUP(Protocol::C_EATITEM_LVUP pkt);
	void HandleEatItemDmgUP(Protocol::C_EATITEM_DMGUP pkt);
	void HandleEatItemHealHP(Protocol::C_EATITEM_HEALHP pkt);


	//AI
	//bool SpawnEnemy(AIEnemyRef enermy);
	bool SpawnEnemyRandom(Protocol::C_AISPAWN_RANDOM pkt);
	bool SpawnEnemyPatrol(Protocol::C_AISPAWN_PATROL pkt);
	bool SpawnEnemyBoss(Protocol::C_AISPAWN_BOSS pkt);
	bool DespawnEnemy(uint64 objID);
	bool AIMove(Protocol::C_AIMOVE pkt);
	void AIMoveStop(Protocol::C_AIMOVESTOP pkt);
	void AIAttack(Protocol::C_AIATTACK pkt);
	void AIAttackBoss2(Protocol::C_AIATTACK_BOSS2 pkt);
	void AIRotate(Protocol::C_AIROTATE pkt);
	void AIHit(Protocol::C_AIHIT pkt);
	void AISpawnProjectile(Protocol::C_AIPROJSPAWN pkt);
	void AIDmged(Protocol::C_AIDAMAGED pkt);
	void AIDead(Protocol::C_AIDEAD pkt);
	void AIKnocksBack(Protocol::C_AI_KNOCKS_BACK pkt);

	//Map
	void MAP_Set_MagneticFild(Protocol::C_SET_MAGNETICFIELD pkt);
	void MAP_World_LvUP(Protocol::C_WORLD_LVUP pkt);
private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 ObjectID);

	bool AddEnermy(AIEnemyRef enermy);
	bool RemoveEnermy(uint64 objID);

private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptID = 0);

private:
	unordered_map<uint64, ObjectRef> _objects;
	//unordered_map<uint64, AIEnemyRef> _enemies;
	vector<bool> _spawnpoints;
};

extern RoomRef GRoom;