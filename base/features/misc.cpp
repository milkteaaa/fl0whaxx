// used: random_device, mt19937, uniform_int_distribution
#include <random>

#include "misc.h"
// used: global variables
#include "../global.h"
// used: cheat variables
#include "../core/variables.h"
// used: convar interface
#include "../core/interfaces.h"
// used: angle-vector calculations
#include "../utilities/math.h"
#include "../utilities.h"
#include <memory.h>

void CMiscellaneous::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	// @credits: a8pure c:
	if (C::Get<bool>(Vars.bMiscNoCrouchCooldown))
		pCmd->iButtons |= IN_BULLRUSH;

	if (C::Get<bool>(Vars.bMiscBunnyHop))
		BunnyHop(pCmd, pLocal);

	if (C::Get<bool>(Vars.bMiscAutoStrafe))
		AutoStrafe(pCmd, pLocal);

	if (C::Get<bool>(Vars.bMiscRevealRanks) && pCmd->iButtons & IN_SCORE)
		I::Client->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0U, 0, nullptr);

	if (C::Get<bool>(Vars.bMiscClanTag))
		Clantag();
	if (C::Get<bool>(Vars.bMiscFakePrime))
		FakePrime();
	if (C::Get<bool>(Vars.bMiscDisableBlur))
		DisableBlur();
	if (C::Get<bool>(Vars.bMiscInverseGravity))
		InverseGravity();
	if (C::Get<bool>(Vars.bMiscPredictNade))
		PredictNade();
	if (C::Get<bool>(Vars.bMiscRightHandKNF))
		FlipKnife();
}

void CMiscellaneous::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::Engine->IsInGame())
		return;
}

void CMiscellaneous::MovementCorrection(CUserCmd* pCmd, CBaseEntity* pLocal, QAngle& angOldViewPoint)
{
	if (!pLocal->IsAlive())
		return;

	Vector vecForward, vecRight, vecUp, vecForwardOld, vecRightOld, vecUpOld;
	M::AngleVectors(angOldViewPoint, &vecForward, &vecRight, &vecUp);
	M::AngleVectors(pCmd->angViewPoint, &vecForwardOld, &vecRightOld, &vecUpOld);

	const float flFwdLenght = vecForward.Length2D();
	const float flRightLenght = vecRight.Length2D();
	const float flUpLenght = std::sqrtf(vecUp.z * vecUp.z);

	const Vector vecNormFwd = Vector(1.f / flFwdLenght * vecForward.x, 1.f / flFwdLenght * vecForward.y, 0.f) * pCmd->flForwardMove;
	const Vector vecNormRight = Vector(1.f / flRightLenght * vecRight.x, 1.f / flRightLenght * vecRight.y, 0.f) * pCmd->flSideMove;
	const Vector vecNormUp = Vector(0.f, 0.f, 1.f / flUpLenght * vecUp.z) * pCmd->flUpMove;

	const float flFwdOldLenght = vecForwardOld.Length2D();
	const float flRightOldLenght = vecRightOld.Length2D();
	const float flUpOldLenght = std::sqrtf(vecUpOld.z * vecUpOld.z);

	const Vector vecNormFwdOld(1.f / flFwdOldLenght * vecForwardOld.x, 1.f / flFwdOldLenght * vecForwardOld.y, 0.f);
	const Vector vecNormRightOld(1.f / flRightOldLenght * vecRightOld.x, 1.f / flRightOldLenght * vecRightOld.y, 0.f);
	const Vector vecNormUpOld(0.f, 0.f, 1.f / flUpOldLenght * vecUpOld.z);

	const float x = vecNormFwdOld.x * vecNormRight.x + vecNormFwdOld.y * vecNormRight.y + vecNormFwdOld.z * vecNormRight.z
		+ (vecNormFwdOld.x * vecNormFwd.x + vecNormFwdOld.y * vecNormFwd.y + vecNormFwdOld.z * vecNormFwd.z)
		+ (vecNormFwdOld.y * vecNormUp.x + vecNormFwdOld.x * vecNormUp.y + vecNormFwdOld.z * vecNormUp.z);

	const float y = vecNormRightOld.x * vecNormRight.x + vecNormRightOld.y * vecNormRight.y + vecNormRightOld.z * vecNormRight.z
		+ (vecNormRightOld.x * vecNormFwd.x + vecNormRightOld.y * vecNormFwd.y + vecNormRightOld.z * vecNormFwd.z)
		+ (vecNormRightOld.x * vecNormUp.y + vecNormRightOld.y * vecNormUp.x + vecNormRightOld.z * vecNormUp.z);

	const float z = vecNormUpOld.x * vecNormRight.y + vecNormUpOld.y * vecNormRight.x + vecNormUpOld.z * vecNormRight.z
		+ (vecNormUpOld.x * vecNormFwd.y + vecNormUpOld.y * vecNormFwd.x + vecNormUpOld.z * vecNormFwd.z)
		+ (vecNormUpOld.x * vecNormUp.x + vecNormUpOld.y * vecNormUp.y + vecNormUpOld.z * vecNormUp.z);

	// clamp and apply corrected movement
	pCmd->flForwardMove = std::clamp(x, -450.f, 450.f);
	pCmd->flSideMove = std::clamp(y, -450.f, 450.f);
	pCmd->flUpMove = std::clamp(z, -450.f, 450.f);
}

void CMiscellaneous::AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!pLocal->IsAlive())
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	// check for pistol and attack
	if (pWeaponData == nullptr || pWeaponData->bFullAuto || pWeaponData->nWeaponType != WEAPONTYPE_PISTOL || !(pCmd->iButtons & IN_ATTACK))
		return;

	if (pLocal->CanShoot(static_cast<CWeaponCSBase*>(pWeapon)))
		pCmd->iButtons |= IN_ATTACK;
	else
		pCmd->iButtons &= ~IN_ATTACK;
}

void CMiscellaneous::FakeLag(CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	INetChannel* pNetChannel = reinterpret_cast<INetChannel*>(I::ClientState->pNetChannel);

	if (pNetChannel == nullptr)
		return;

	static CConVar* sv_maxusrcmdprocessticks = I::ConVar->FindVar(XorStr("sv_maxusrcmdprocessticks"));

	if (sv_maxusrcmdprocessticks == nullptr)
		return;

	/*
	 * @note: get max available ticks to choke
	 * 2 ticks reserved for server info else player can be stacked
	 * while antiaiming and fakelag is disabled choke only 1 tick
	 */
	const int iMaxCmdProcessTicks = C::Get<bool>(Vars.bMiscFakeLag) ? sv_maxusrcmdprocessticks->GetInt() - 2 :
		C::Get<bool>(Vars.bAntiAim) ? 1 : 0;

	// choke
	bSendPacket = I::ClientState->nChokedCommands >= iMaxCmdProcessTicks;
}

void CMiscellaneous::BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static CConVar* sv_autobunnyhopping = I::ConVar->FindVar(XorStr("sv_autobunnyhopping"));

	if (sv_autobunnyhopping->GetBool())
		return;

	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_OBSERVER)
		return;

	std::random_device randomDevice;
	std::mt19937 generate(randomDevice());
	std::uniform_int_distribution<> chance(0, 100);

	if (chance(generate) > C::Get<int>(Vars.iMiscBunnyHopChance))
		return;

	static bool bLastJumped = false, bShouldFake = false;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		pCmd->iButtons |= IN_JUMP;
	}
	else if (pCmd->iButtons & IN_JUMP)
	{
		if (pLocal->GetFlags() & FL_ONGROUND || pLocal->GetFlags() & FL_PARTIALGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			pCmd->iButtons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void CMiscellaneous::AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	if (pLocal->GetFlags() & FL_ONGROUND)
		return;

	pCmd->flSideMove = pCmd->sMouseDeltaX < 0.f ? -450.f : 450.f;
}

void CMiscellaneous::ChangeName(const char* name)
{
	CConVar* cv = I::ConVar->FindVar("name");
	*(int*)((DWORD)&cv->fnChangeCallbacks + 0xC) = 0;
	cv->SetValue(name);
}


void CMiscellaneous::NameChanger()
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected())
		return;

	if (changes == -1)
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 150)
		return;

	timestamp = curTime;
	++changes;

	if (changes >= 5)
	{
		std::string name = "\n";
		char chars[3] = { '\n', '\0', '\t' };

		for (int i = 0; i < 127; i++)
			name += chars[rand() % 2];

		CMiscellaneous::ChangeName(name.c_str());

		changes = -1;

		return;
	}
	CMiscellaneous::ChangeName(setStrRight("fl0whaxx", strlen("fl0whaxx") + changes));
}

const char* CMiscellaneous::setStrRight(std::string txt, unsigned int value)
{
	txt.insert(txt.length(), value - txt.length(), ' ');

	return txt.c_str();
}

void CMiscellaneous::Clantag()
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected())
		return;
	static size_t lastTime = 0;

	if (GetTickCount() > lastTime)
	{
		clantag += clantag.at(0);
		clantag.erase(0, 1);

		U::SendClanTag(clantag.c_str(), clantag.c_str());

		lastTime = GetTickCount() + 650;
	}
}

void CMiscellaneous::InverseGravity()
{
	auto ragdollGrav = I::ConVar->FindVar("cl_ragdoll_gravity");
	ragdollGrav->SetValue((int)Vars.iMiscRagdollGravity);
}

void CMiscellaneous::DisableBlur()
{
	auto blur = I::ConVar->FindVar("@panorama_disable_blur");
	blur->SetValue((int)Vars.bMiscDisableBlur);
}

void CMiscellaneous::PredictNade()
{
	static auto nadeVar{ I::ConVar->FindVar("cl_grenadepreview") };

	nadeVar->fnChangeCallbacks.Size() = 0;
	nadeVar->SetValue((int)Vars.bMiscPredictNade);
}

void CMiscellaneous::FakePrime()
{
	uint8_t* prime = reinterpret_cast<uint8_t*>(MEM::FindPattern(CLIENT_DLL, "\x17\xF6\x40\x14\x10"));
	DWORD oldProtect;
	static bool lastPrimeState = false;
	if (Vars.bMiscFakePrime != lastPrimeState)
	{
		lastPrimeState = Vars.bMiscFakePrime;
		if (DWORD oldProtect; VirtualProtect(prime, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			constexpr uint8_t patch[]{ 0x74, 0xEB };
			*prime = patch[Vars.bMiscFakePrime];
			VirtualProtect(prime, 1, oldProtect, nullptr);
		}
	}
}


void CMiscellaneous::FlipKnife()
{
	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	if (!Vars.bMiscRightHandKNF)
		return;

	if (!pLocal)
		return;

	static const auto cl_righthand = I::ConVar->FindVar("cl_righthand");
	static bool laststate;
	
	const auto activeWeapon = pLocal->GetWeapon();
	if (const auto classId = activeWeapon->GetClientClass()->nClassID; classId == EClassIndex::CKnife || classId == EClassIndex::CKnifeGG)
	{
		I::Engine->ExecuteClientCmd("cl_righthand 0");
	}
	else
	{
		I::Engine->ExecuteClientCmd("cl_righthand 1");
	}

}