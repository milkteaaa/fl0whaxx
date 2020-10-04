#pragma once
// used: winapi includes
#include "../common.h"

// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: listener event function
#include "../sdk/interfaces/igameeventmanager.h"
// used: baseentity
#include "../sdk/entity.h"
class CMiscellaneous : public CSingleton<CMiscellaneous>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);
	void Event(IGameEvent* pEvent, const FNV1A_t uNameHash);
	/* correct movement while anti-aiming */
	void MovementCorrection(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle& angOldViewPoint);

	// Extra
	/* automatic shoot when pressed attack key */
	void AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal);
	/* dont send packets for a certain number of ticks */
	void FakeLag(CBaseEntity* pLocal, bool& bSendPacket);
	void ChangeName(const char* Name);
	void NameChanger();
	void Clantag();
	void FakePrime();
	void PredictNade();
	void DisableBlur();
	void InverseGravity();
	void FlipKnife();
	
	int changes = -1;


private:
	
	std::string clantag = "fl0whaxx ";
	void BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocal);
	const char* setStrRight(std::string Txt, unsigned int value);
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
	
};