#include "pch.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "Room.h"
#include "Player.h"
#include "AIEnemy.h"
#include "GameSessionManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	Protocol::S_LOGIN loginPkt;
	loginPkt.set_client_id(ObjectUtils::s_playeridGenerator.fetch_add(1));
	SendBufferRef sendbuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendbuffer);

	return true;
}

bool Handle_C_PLAYERSPAWNPOINT(PacketSessionRef& session, Protocol::C_PLAYERSPAWNPOINT& pkt)
{
	GRoom->DoAsync(&Room::SetSpawnSize, pkt.spawnpointsize());
	return true;
}

bool Handle_C_PLAYERCOUNT(PacketSessionRef& session, Protocol::C_PLAYERCOUNT& pkt)
{
	Protocol::S_PLAYERCOUNT playerCountPkt;
	playerCountPkt.set_playercount(ObjectUtils::s_playeridGenerator - 1);
	SendBufferRef sendbuffer = ServerPacketHandler::MakeSendBuffer(playerCountPkt);
	GSessionManager.Broadcast(sendbuffer);

	return true;
}

bool Handle_C_GAMESTART(PacketSessionRef& session, Protocol::C_GAMESTART& pkt)
{
	Protocol::S_GAMESTART startPkt;

	SendBufferRef sendbuffer = ServerPacketHandler::MakeSendBuffer(startPkt);
	GSessionManager.Broadcast(sendbuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));
	player->objectInfo->set_object_id(pkt.client_id());
	player->posInfo->set_object_id(pkt.client_id());
	GRoom->DoAsync(&Room::HandleEnterPlayer, player);
	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->HandleLeavePlayer(player);

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleMove, pkt);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;
	string temp = pkt.msg();
	temp += "A";
	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(temp);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_FIRE(PacketSessionRef& session, Protocol::C_FIRE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	GRoom->DoAsync(&Room::HandleAttack, pkt);

	return true;
}

bool Handle_C_HIT(PacketSessionRef& session, Protocol::C_HIT& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	GRoom->DoAsync(&Room::HandleHit, pkt);

	return true;
}

bool Handle_C_EXPUP(PacketSessionRef& session, Protocol::C_EXPUP& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	GRoom->DoAsync(&Room::HandleExpUP, pkt);

	return true;
}

bool Handle_C_LVUP(PacketSessionRef& session, Protocol::C_LVUP& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	GRoom->DoAsync(&Room::HandleLVUP, pkt);

	return true;
}

bool Handle_C_AISPAWN_RANDOM(PacketSessionRef& session, Protocol::C_AISPAWN_RANDOM& pkt)
{
	GRoom->DoAsync(&Room::SpawnEnemyRandom, pkt);

	return true;
}

bool Handle_C_AISPAWN_PATROL(PacketSessionRef& session, Protocol::C_AISPAWN_PATROL& pkt)
{
	GRoom->DoAsync(&Room::SpawnEnemyPatrol, pkt);

	return true;
}

bool Handle_C_AISPAWN_BOSS(PacketSessionRef& session, Protocol::C_AISPAWN_BOSS& pkt)
{
	GRoom->DoAsync(&Room::SpawnEnemyBoss, pkt);

	return true;
}

bool Handle_C_AIMOVE(PacketSessionRef& session, Protocol::C_AIMOVE& pkt)
{
	GRoom->DoAsync(&Room::AIMove, pkt);

	return true;
}

bool Handle_C_AIMOVESTOP(PacketSessionRef& session, Protocol::C_AIMOVESTOP& pkt)
{
	GRoom->DoAsync(&Room::AIMoveStop, pkt);
	return true;
}

bool Handle_C_AIATTACK(PacketSessionRef& session, Protocol::C_AIATTACK& pkt)
{
	GRoom->DoAsync(&Room::AIAttack, pkt);
	return true;
}

bool Handle_C_AIATTACK_BOSS2(PacketSessionRef& session, Protocol::C_AIATTACK_BOSS2& pkt)
{
	GRoom->DoAsync(&Room::AIAttackBoss2, pkt);
	return true;
}

bool Handle_C_AIROTATE(PacketSessionRef& session, Protocol::C_AIROTATE& pkt)
{
	GRoom->DoAsync(&Room::AIRotate, pkt);
	return true;
}

bool Handle_C_AIHIT(PacketSessionRef& session, Protocol::C_AIHIT& pkt)
{
	GRoom->DoAsync(&Room::AIHit, pkt);
	return true;
}

bool Handle_C_DAMAGED(PacketSessionRef& session, Protocol::C_DAMAGED& pkt)
{
	GRoom->DoAsync(&Room::HandleDamaged, pkt);
	return true;
}

bool Handle_C_AIDAMAGED(PacketSessionRef& session, Protocol::C_AIDAMAGED& pkt)
{
	GRoom->DoAsync(&Room::AIDmged, pkt);
	return true;
}

bool Handle_C_AIDEAD(PacketSessionRef& session, Protocol::C_AIDEAD& pkt)
{
	GRoom->DoAsync(&Room::AIDead, pkt);
	return true;
}

bool Handle_C_PLAYERDEAD(PacketSessionRef& session, Protocol::C_PLAYERDEAD& pkt)
{
	GRoom->DoAsync(&Room::HandleDead, pkt);
	return true;
}

bool Handle_C_AIPROJSPAWN(PacketSessionRef& session, Protocol::C_AIPROJSPAWN& pkt)
{
	GRoom->DoAsync(&Room::AISpawnProjectile, pkt);
	return true;
}

bool Handle_C_AI_KNOCKS_BACK(PacketSessionRef& session, Protocol::C_AI_KNOCKS_BACK& pkt)
{
	GRoom->DoAsync(&Room::AIKnocksBack, pkt);
	return true;
}

bool Handle_C_PLAYERSKILL_BOMB(PacketSessionRef& session, Protocol::C_PLAYERSKILL_BOMB& pkt)
{
	GRoom->DoAsync(&Room::HandleSkillBomb, pkt);
	return true;
}

bool Handle_C_PLAYERSKILL_GRANADE(PacketSessionRef& session, Protocol::C_PLAYERSKILL_GRANADE& pkt)
{
	GRoom->DoAsync(&Room::HandleSkillGranade, pkt);
	return true;
}

bool Handle_C_PLAYERSKILL_CHEMICAL(PacketSessionRef& session, Protocol::C_PLAYERSKILL_CHEMICAL& pkt)
{
	GRoom->DoAsync(&Room::HandleSkillChemical, pkt);
	return true;
}

bool Handle_C_PLAYERSKILL_GUARD(PacketSessionRef& session, Protocol::C_PLAYERSKILL_GUARD& pkt)
{
	GRoom->DoAsync(&Room::HandleSkillGuard, pkt);
	return true;
}

bool Handle_C_PLAYERSKILL_HEAL(PacketSessionRef& session, Protocol::C_PLAYERSKILL_HEAL& pkt)
{
	GRoom->DoAsync(&Room::HandleSkillHeal, pkt);
	return true;
}

bool Handle_C_PLAYERHEAL(PacketSessionRef& session, Protocol::C_PLAYERHEAL& pkt)
{
	GRoom->DoAsync(&Room::HandleUPdateHeal, pkt);
	return true;
}

bool Handle_C_MAKEDRONE(PacketSessionRef& session, Protocol::C_MAKEDRONE& pkt)
{
	GRoom->DoAsync(&Room::HandleMakeDrone, pkt);
	return true;
}

bool Handle_C_MOVEDRONE(PacketSessionRef& session, Protocol::C_MOVEDRONE& pkt)
{
	GRoom->DoAsync(&Room::HandleMoveDrone, pkt);
	return false;
}

bool Handle_C_SEARCHDRONE(PacketSessionRef& session, Protocol::C_SEARCHDRONE& pkt)
{
	GRoom->DoAsync(&Room::HandleSearchDrone, pkt);
	return true;
}

bool Handle_C_RETURNDRONE(PacketSessionRef& session, Protocol::C_RETURNDRONE& pkt)
{
	GRoom->DoAsync(&Room::HandleReturnDrone, pkt);
	return true;
}

bool Handle_C_ATTACKDRONE(PacketSessionRef& session, Protocol::C_ATTACKDRONE& pkt)
{
	GRoom->DoAsync(&Room::HandleAttackDrone, pkt);
	return true;
}

bool Handle_C_EATITEM_MAXHPUP(PacketSessionRef& session, Protocol::C_EATITEM_MAXHPUP& pkt)
{
	GRoom->DoAsync(&Room::HandleEatItemMaxHP, pkt);
	return true;
}

bool Handle_C_EATITEM_LVUP(PacketSessionRef& session, Protocol::C_EATITEM_LVUP& pkt)
{
	GRoom->DoAsync(&Room::HandleEatItemLVUP, pkt);
	return true;
}

bool Handle_C_EATITEM_DMGUP(PacketSessionRef& session, Protocol::C_EATITEM_DMGUP& pkt)
{
	GRoom->DoAsync(&Room::HandleEatItemDmgUP, pkt);
	return true;
}

bool Handle_C_EATITEM_HEALHP(PacketSessionRef& session, Protocol::C_EATITEM_HEALHP& pkt)
{
	GRoom->DoAsync(&Room::HandleEatItemHealHP, pkt);
	return true;
}

bool Handle_C_SET_MAGNETICFIELD(PacketSessionRef& session, Protocol::C_SET_MAGNETICFIELD& pkt)
{
	GRoom->DoAsync(&Room::MAP_Set_MagneticFild, pkt);
	return true;
}

bool Handle_C_WORLD_LVUP(PacketSessionRef& session, Protocol::C_WORLD_LVUP& pkt)
{
	GRoom->DoAsync(&Room::MAP_World_LvUP, pkt);
	return true;
}

bool Handle_C_GAMERESULT(PacketSessionRef& session, Protocol::C_GAMERESULT& pkt)
{
	Protocol::S_GAMERESULT resultPkt;
	resultPkt.set_object_id(pkt.object_id());

	SendBufferRef sendbuffer = ServerPacketHandler::MakeSendBuffer(resultPkt);
	GSessionManager.Broadcast(sendbuffer);
	return true;
}
