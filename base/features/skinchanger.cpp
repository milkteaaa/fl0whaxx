#include "skinchanger.h"

// used: create entity
#include "../utilities.h"
// used: client, engine interfaces
#include "../core/interfaces.h"
// it will help you here my sweetest candy <3
// https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Skin_Changer
void CSkinChanger::Run()
{
	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	if (pLocal == nullptr || !pLocal->IsAlive())
		return;

	PlayerInfo_t pInfo;
	if (!I::Engine->GetPlayerInfo(I::Engine->GetLocalPlayer(), &pInfo))
		return;

	
}

void CSkinChanger::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::Engine->IsInGame())
		return;
}
