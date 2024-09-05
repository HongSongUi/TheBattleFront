#include "pch.h"
#include "Room.h"
#include "player.h"
#include "AIEnemy.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
}

Room::~Room()
{
}

void Room::Tick()
{
	DoTimer(100, &Room::Tick);
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

bool Room::SetSpawnSize(uint64 size)
{
	_spawnpoints.resize(size, true);
	return true;
}

// Room내부에서 일어나는 일
bool Room::EnterRoom(ObjectRef object, bool randPos)
{
	bool success = AddObject(object);

	if (randPos)
	{
		object->posInfo->set_x(Utils::GetRandom(500.f, 1000.0f));
		object->posInfo->set_y(Utils::GetRandom(500.f, 1000.0f));
		object->posInfo->set_z(100.0f);

		object->posInfo->set_yaw(Utils::GetRandom(0.f, 500.0f));
		object->posInfo->set_pitch(0.0f);
		object->posInfo->set_roll(0.0f);

		//object->posInfo->set_vx(0.0f);
		//object->posInfo->set_vy(0.0f);
		//object->posInfo->set_vz(0.0f);
		object->posInfo->set_speed(0.0f);
	}

	int64 idx;
	do
	{
		idx = Utils::GetRandom<int>(0, _spawnpoints.size() - 1);
	} while (_spawnpoints[idx] == false);

	_spawnpoints[idx] = false;
	object->objectInfo->set_spawnidx(idx);

	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_ENTER_GAME enterGamePkt;

		// 1)
		//Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		//playerInfo->CopyFrom(*object->objectInfo);
		//enterGamePkt.set_allocated_player(playerInfo);

		// 2)
		auto playerInfo = enterGamePkt.mutable_player();
		playerInfo->CopyFrom(*object->objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	{
		Protocol::S_SPAWN spawnPkt;
		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
		objectInfo->CopyFrom(*object->objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, object->objectInfo->object_id());
	}

	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_SPAWN spawnPkt;
		for (auto& item : _objects)
		{
			if (item.second->IsPlayer() == false)
				continue;

			Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(*item.second->objectInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	return success;
}

bool Room::LeaveRoom(ObjectRef object)
{
	if (object == nullptr)
		return false;

	const uint64 objectID = object->objectInfo->object_id();
	bool success = RemoveObject(objectID);

	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectID);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectID);

		if (auto player = dynamic_pointer_cast<Player>(object))
			if (auto session = player->session.lock())
				session->Send(sendBuffer);
	}

	if (objectID == 1)
		ProcessAllKill(L"GameServer.exe");

	return success;
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player, false);
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
	return LeaveRoom(player);
}

void Room::HandleMove(Protocol::C_MOVE pkt)
{
	const uint64 objectID = pkt.info().object_id();
	if (_objects.find(objectID) == _objects.end())
		return;

	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectID]);
	player->posInfo->CopyFrom(pkt.info());

	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer, objectID);
	}
}

void Room::HandleAttack(Protocol::C_FIRE pkt)
{
	const uint64 objectID = pkt.projectile().object_id();
	if (_objects.find(objectID) == _objects.end())
		return;

	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectID]);

	Protocol::S_FIRE firePkt;
	{
		Protocol::ProjectileInfo* info = firePkt.mutable_projectile();
		info->CopyFrom(pkt.projectile());
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(firePkt);
	Broadcast(sendBuffer, objectID);
}

void Room::HandleHit(Protocol::C_HIT pkt)
{
	const uint64 objectID = pkt.ownerid();
	if (_objects.find(objectID) == _objects.end())
		return;

	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectID]);

	Protocol::S_HIT hitPkt;
	{
		hitPkt.set_ownerid(pkt.ownerid());
		hitPkt.set_targetid(pkt.targetid());
		hitPkt.set_damage(pkt.damage());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(hitPkt);
	Broadcast(sendBuffer);
}

void Room::HandleDamaged(Protocol::C_DAMAGED pkt)
{
	const uint64 objectID = pkt.object_id();
	if (_objects.find(objectID) == _objects.end())
		return;
	Protocol::S_DAMAGED dmgPkt;
	{
		dmgPkt.set_object_id(pkt.object_id());
		dmgPkt.set_hp(pkt.hp());
		dmgPkt.set_guardpoint(pkt.guardpoint());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(dmgPkt);
	Broadcast(sendBuffer, 1);
}

void Room::HandleDead(Protocol::C_PLAYERDEAD pkt)
{
	const uint64 objectID = pkt.object_id();
	if (_objects.find(objectID) == _objects.end())
		return;
	Protocol::S_PLAYERDEAD PDeadPkt;
	{
		PDeadPkt.set_object_id(pkt.object_id());
		PDeadPkt.set_dead(pkt.dead());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(PDeadPkt);
	Broadcast(sendBuffer, 1);
}

void Room::HandleExpUP(Protocol::C_EXPUP pkt)
{
	const uint64 objectID = pkt.ownerid();
	if (_objects.find(objectID) == _objects.end())
		return;
	auto player = dynamic_pointer_cast<Player>(_objects.find(objectID)->second);
	Protocol::S_EXPUP expPkt;
	{
		expPkt.set_exp(pkt.exp());
		expPkt.set_ownerid(pkt.ownerid());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(expPkt);
	if (auto session = player->session.lock())
		session->Send(sendBuffer);
}

void Room::HandleLVUP(Protocol::C_LVUP pkt)
{
	const uint64 objectID = pkt.ownerid();
	if (_objects.find(objectID) == _objects.end())
		return;
	Protocol::S_LVUP lvUpPkt;
	{
		lvUpPkt.set_currexp(pkt.currexp());
		lvUpPkt.set_level(pkt.level());
		lvUpPkt.set_ownerid(pkt.ownerid());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(lvUpPkt);
	Broadcast(sendBuffer, 1);
}


void Room::HandleSkillBomb(Protocol::C_PLAYERSKILL_BOMB pkt)
{
	Protocol::S_PLAYERSKILL_BOMB bombPkt;
	{
		bombPkt.set_object_id(pkt.object_id());
		bombPkt.set_abilityarrayidx(pkt.abilityarrayidx());
		bombPkt.set_x(pkt.x());
		bombPkt.set_y(pkt.y());
		bombPkt.set_z(pkt.z());
		bombPkt.set_dmg(pkt.dmg());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(bombPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleSkillChemical(Protocol::C_PLAYERSKILL_CHEMICAL pkt)
{
	Protocol::S_PLAYERSKILL_CHEMICAL chemicalPkt;
	{
		chemicalPkt.set_object_id(pkt.object_id());
		chemicalPkt.set_abilityarrayidx(pkt.abilityarrayidx());
		chemicalPkt.set_x(pkt.x());
		chemicalPkt.set_y(pkt.y());
		chemicalPkt.set_z(pkt.z());
		chemicalPkt.set_dmg(pkt.dmg());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(chemicalPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleSkillGranade(Protocol::C_PLAYERSKILL_GRANADE pkt)
{
	Protocol::S_PLAYERSKILL_GRANADE granPkt;
	{
		granPkt.set_object_id(pkt.object_id());
		granPkt.set_abilityarrayidx(pkt.abilityarrayidx());
		granPkt.set_x(pkt.x());
		granPkt.set_y(pkt.y());
		granPkt.set_z(pkt.z());
		granPkt.set_rx(pkt.rx());
		granPkt.set_ry(pkt.ry());
		granPkt.set_rz(pkt.rz());
		granPkt.set_dmg(pkt.dmg());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(granPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleSkillGuard(Protocol::C_PLAYERSKILL_GUARD pkt)
{
	Protocol::S_PLAYERSKILL_GUARD guardPkt;
	{
		guardPkt.set_object_id(pkt.object_id());
		guardPkt.set_guardpoint(pkt.guardpoint());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(guardPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleSkillHeal(Protocol::C_PLAYERSKILL_HEAL pkt)
{
	Protocol::S_PLAYERSKILL_HEAL healPkt;
	{
		healPkt.set_object_id(pkt.object_id());
		healPkt.set_abilityarrayidx(pkt.abilityarrayidx());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(healPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleUPdateHeal(Protocol::C_PLAYERHEAL pkt)
{
	Protocol::S_PLAYERHEAL updatedHealPkt;
	{
		updatedHealPkt.set_object_id(pkt.object_id());
		updatedHealPkt.set_updeatedhp(pkt.updeatedhp());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(updatedHealPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleMakeDrone(Protocol::C_MAKEDRONE pkt)
{
	Protocol::S_MAKEDRONE makeDrnPkt;
	{
		makeDrnPkt.set_object_id(pkt.object_id());
		makeDrnPkt.set_abilityarrayidx(pkt.abilityarrayidx());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(makeDrnPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleSearchDrone(Protocol::C_SEARCHDRONE pkt)
{
	Protocol::S_SEARCHDRONE searchDronPkt;
	{
		searchDronPkt.set_object_id(pkt.object_id());
		searchDronPkt.set_x(pkt.x());
		searchDronPkt.set_y(pkt.y());
		searchDronPkt.set_z(pkt.z());
		searchDronPkt.set_yaw(pkt.yaw());
		searchDronPkt.set_pitch(pkt.pitch());
		searchDronPkt.set_roll(pkt.roll());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(searchDronPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleMoveDrone(Protocol::C_MOVEDRONE pkt)
{
	Protocol::S_MOVEDRONE moveDrnPkt;
	{
		moveDrnPkt.set_object_id(pkt.object_id());
		moveDrnPkt.set_x(pkt.x());
		moveDrnPkt.set_y(pkt.y());
		moveDrnPkt.set_z(pkt.z());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(moveDrnPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleReturnDrone(Protocol::C_RETURNDRONE pkt)
{
	Protocol::S_RETURNDRONE returnDrnPkt;
	{
		returnDrnPkt.set_object_id(pkt.object_id());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(returnDrnPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleAttackDrone(Protocol::C_ATTACKDRONE pkt)
{
	Protocol::S_ATTACKDRONE atkDrnPkt;
	{
		atkDrnPkt.set_object_id(pkt.object_id());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(atkDrnPkt);
	Broadcast(sendBuffer, pkt.object_id());
}

void Room::HandleEatItemMaxHP(Protocol::C_EATITEM_MAXHPUP pkt)
{
	const uint64 objectID = pkt.object_id();
	if (_objects.find(objectID) == _objects.end())
		return;

	Protocol::S_EATITEM_MAXHPUP maxHpPkt;
	{
		maxHpPkt.set_object_id(pkt.object_id());
		maxHpPkt.set_maxhp(pkt.maxhp());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(maxHpPkt);
	Broadcast(sendBuffer, 1);
}

void Room::HandleEatItemLVUP(Protocol::C_EATITEM_LVUP pkt)
{
	const uint64 objectID = pkt.object_id();
	if (_objects.find(objectID) == _objects.end())
		return;

	Protocol::S_EATITEM_LVUP lvUpPkt;
	{
		lvUpPkt.set_object_id(pkt.object_id());
		lvUpPkt.set_curlv(pkt.curlv());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(lvUpPkt);
	Broadcast(sendBuffer, 1);
}

void Room::HandleEatItemDmgUP(Protocol::C_EATITEM_DMGUP pkt)
{
	const uint64 objectID = pkt.object_id();
	if (_objects.find(objectID) == _objects.end())
		return;

	Protocol::S_EATITEM_DMGUP dmgUpPkt;
	{
		dmgUpPkt.set_object_id(pkt.object_id());
		dmgUpPkt.set_attackdmg(pkt.attackdmg());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(dmgUpPkt);
	Broadcast(sendBuffer, 1);
}

void Room::HandleEatItemHealHP(Protocol::C_EATITEM_HEALHP pkt)
{
	const uint64 objectID = pkt.object_id();
	if (_objects.find(objectID) == _objects.end())
		return;

	Protocol::S_EATITEM_HEALHP HealedUpPkt;
	{
		HealedUpPkt.set_object_id(pkt.object_id());
		HealedUpPkt.set_curhp(pkt.curhp());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(HealedUpPkt);
	Broadcast(sendBuffer, 1);
}

//bool Room::SpawnEnemy(AIEnemyRef enermy)
//{
//	bool success = AddEnermy(enermy);sb
//	Protocol::S_AISPAWNRANDOM aipkts;
//	auto temp = aipkts.add_enemies();
//	temp->CopyFrom(*enermy->objectInfo);
//
//	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(aipkts);
//	Broadcast(sendBuffer, 1);
//	
//	return true;
//}

bool Room::SpawnEnemyRandom(Protocol::C_AISPAWN_RANDOM pkt)
{
	Protocol::S_AISPAWN_RANDOM AISpawnPkt;
	{
		AISpawnPkt.set_object_id(pkt.object_id());
		AISpawnPkt.set_container_idx(pkt.container_idx());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(AISpawnPkt);
		Broadcast(sendBuffer, 1);
	}
	return true;
}

bool Room::SpawnEnemyPatrol(Protocol::C_AISPAWN_PATROL pkt)
{
	Protocol::S_AISPAWN_PATROL AISpawnPkt;
	{
		AISpawnPkt.set_object_id(pkt.object_id());
		AISpawnPkt.set_container_idx(pkt.container_idx());
		AISpawnPkt.set_pointidx(pkt.pointidx());
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(AISpawnPkt);
		Broadcast(sendBuffer, 1);
	}
	return true;
}

bool Room::SpawnEnemyBoss(Protocol::C_AISPAWN_BOSS pkt)
{
	Protocol::S_AISPAWN_BOSS AISpawnPkt;
	{
		AISpawnPkt.set_object_id(pkt.object_id());
		AISpawnPkt.set_container_idx(pkt.container_idx());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(AISpawnPkt);
		Broadcast(sendBuffer, 1);
	}
	return true;
}

bool Room::AIMove(Protocol::C_AIMOVE pkt)
{
	const uint64 objectID = pkt.info().object_id();
	//if (_enemies.find(objectID) == _enemies.end())
	//	return false;

	//AIEnemyRef enemy = _enemies[objectID];
	//enemy->posInfo->CopyFrom(pkt.info());

	{
		Protocol::S_AIMOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer, 1);
	}

	return true;
}

void Room::AIMoveStop(Protocol::C_AIMOVESTOP pkt)
{
	const uint64 objectID = pkt.object_id();
	//if (_enemies.find(objectID) == _enemies.end())
	//	return;

	//AIEnemyRef enemy = _enemies[objectID];
	//enemy->posInfo->set_speed(pkt.speed());

	Protocol::S_AIMOVESTOP moveStopPkt;
	{
		moveStopPkt.set_object_id(pkt.object_id());
		moveStopPkt.set_speed(pkt.speed());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(moveStopPkt);
	Broadcast(sendBuffer, 1);
}

void Room::AIAttack(Protocol::C_AIATTACK pkt)
{
	const uint64 objectID = pkt.object_id();
	//if (_enemies.find(objectID) == _enemies.end())
	//	return;

	Protocol::S_AIATTACK aiAttackPkt;
	{
		aiAttackPkt.set_object_id(pkt.object_id());
		aiAttackPkt.set_attack_idx(pkt.attack_idx());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(aiAttackPkt);
	Broadcast(sendBuffer, 1);
}

void Room::AIAttackBoss2(Protocol::C_AIATTACK_BOSS2 pkt)
{
	Protocol::S_AIATTACK_BOSS2 bossAttack2;
	{
		bossAttack2.set_object_id(pkt.object_id());
		for (auto target : pkt.target_id())
			bossAttack2.add_target_id(target);
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(bossAttack2);
	Broadcast(sendBuffer, 1);
}

void Room::AIRotate(Protocol::C_AIROTATE pkt)
{
	const uint64 objectID = pkt.object_id();
	//if (_enemies.find(objectID) == _enemies.end())
	//	return;

	Protocol::S_AIROTATE airotPkt;
	{
		airotPkt.set_object_id(objectID);
		airotPkt.set_yaw(pkt.yaw());
		airotPkt.set_pitch(pkt.pitch());
		airotPkt.set_roll(pkt.roll());
		airotPkt.set_x(pkt.x());
		airotPkt.set_y(pkt.y());
		airotPkt.set_z(pkt.z());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(airotPkt);
	Broadcast(sendBuffer, 1);
}

void Room::AIHit(Protocol::C_AIHIT pkt)
{
	const uint64 objectID = pkt.object_id();
	//if (_enemies.find(objectID) == _enemies.end())
	//	return;

	Protocol::S_AIHIT aihitpkt;
	{
		aihitpkt.set_object_id(pkt.object_id());
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(aihitpkt);
	Broadcast(sendBuffer, 1);
}

void Room::AISpawnProjectile(Protocol::C_AIPROJSPAWN pkt)
{
	const uint64 objectID = pkt.object_id();
	//if (_enemies.find(objectID) == _enemies.end())
	//	return;

	Protocol::S_AIPROJSPAWN AiProjPkt;
	{
		AiProjPkt.set_object_id(pkt.object_id());
		AiProjPkt.set_projidx(pkt.projidx());
		AiProjPkt.set_x(pkt.x());
		AiProjPkt.set_y(pkt.y());
		AiProjPkt.set_z(pkt.z());
		AiProjPkt.set_yaw(pkt.yaw());
		AiProjPkt.set_pitch(pkt.pitch());
		AiProjPkt.set_roll(pkt.roll());
		AiProjPkt.set_speed(pkt.speed());
		AiProjPkt.set_attackdmg(pkt.attackdmg());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(AiProjPkt);
	Broadcast(sendBuffer, 1);
}

void Room::AIDmged(Protocol::C_AIDAMAGED pkt)
{
	Protocol::S_AIDAMAGED AIDmgedPkt;
	{
		AIDmgedPkt.set_object_id(pkt.object_id());
		AIDmgedPkt.set_hp(pkt.hp());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(AIDmgedPkt);
	Broadcast(sendBuffer, 1);
}

void Room::AIDead(Protocol::C_AIDEAD pkt)
{
	//if (_enemies.find(pkt.object_id()) != _enemies.end())
	//{
	//	_enemies.erase(pkt.object_id());
	Protocol::S_AIDEAD AiDeadPkt;
	{
		AiDeadPkt.set_object_id(pkt.object_id());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(AiDeadPkt);
	Broadcast(sendBuffer, 1);
	//}
}

void Room::AIKnocksBack(Protocol::C_AI_KNOCKS_BACK pkt)
{
	Protocol::S_AI_KNOCKS_BACK knocksBackPkt;
	{
		knocksBackPkt.set_object_id(pkt.object_id());
		knocksBackPkt.set_target_id(pkt.target_id());
		knocksBackPkt.set_vx(pkt.vx());
		knocksBackPkt.set_vy(pkt.vy());
		knocksBackPkt.set_vz(pkt.vz());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(knocksBackPkt);
	Broadcast(sendBuffer, 1);
}

void Room::MAP_Set_MagneticFild(Protocol::C_SET_MAGNETICFIELD pkt)
{
	Protocol::S_SET_MAGNETICFIELD magneticPkt;
	{
		magneticPkt.set_a(pkt.a());
		magneticPkt.set_b(pkt.b());
		magneticPkt.set_g(pkt.g());
		magneticPkt.set_r(pkt.r());
		magneticPkt.set_radius(pkt.radius());
		magneticPkt.set_time(pkt.time());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(magneticPkt);
	Broadcast(sendBuffer, 1);
}

void Room::MAP_World_LvUP(Protocol::C_WORLD_LVUP pkt)
{
	Protocol::S_WORLD_LVUP wlvpkt;
	{
		wlvpkt.set_worldlevel(pkt.worldlevel());
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(wlvpkt);
	Broadcast(sendBuffer, 1);
}

bool Room::AddObject(ObjectRef object)
{
	if (_objects.find(object->objectInfo->object_id()) != _objects.end())
		return false;

	_objects.insert(make_pair(object->objectInfo->object_id(), object));

	object->room.store(GetRoomRef());

	return true;
}

bool Room::RemoveObject(uint64 ObjectID)
{
	if (_objects.find(ObjectID) == _objects.end())
		return false;

	ObjectRef object = _objects[ObjectID];
	PlayerRef player = dynamic_pointer_cast<Player>(object);
	if (player)
		player->room.store(weak_ptr<Room>());

	_objects.erase(ObjectID);

	return true;
}

bool Room::AddEnermy(AIEnemyRef enermy)
{
	//if (_enemies.find(enermy->objectInfo->object_id()) != _enemies.end())
	//	return false;

	//_enemies.insert(make_pair(enermy->objectInfo->object_id(), enermy));

	return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptID)
{
	for (auto& item : _objects)
	{
		PlayerRef player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			continue;
		if (player->objectInfo->object_id() == exceptID)
			continue;

		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}
}
