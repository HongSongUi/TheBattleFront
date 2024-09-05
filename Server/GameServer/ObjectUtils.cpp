#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "AIEnemy.h"
#include "GameSession.h"
//#include "Projectile.h"

atomic<int64> ObjectUtils::s_playeridGenerator = 1;
atomic<int64> ObjectUtils::s_AIidGenerator = 100;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	//const int64 newId = s_playeridGenerator.fetch_add(1);

	PlayerRef player = make_shared<Player>();
	//player->objectInfo->set_object_id(newId);
	//player->posInfo->set_object_id(newId);

	player->session = session;
	session->player.store(player);

	return player;
}

AIEnemyRef ObjectUtils::CreateEnermy()
{
	const int64 newId = s_AIidGenerator.fetch_add(1);

	AIEnemyRef enermy = make_shared<AIEnemy>();
	enermy->objectInfo->set_object_id(newId);
	enermy->posInfo->set_object_id(newId);

	return enermy;
}
