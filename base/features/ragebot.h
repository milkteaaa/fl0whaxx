#pragma once
// used: winapi includes, singleton
#include "../common.h"
// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: angle
#include "../sdk/datatypes/qangle.h"
// used: baseentity
#include "../sdk/entity.h"

#include "../sdk/datatypes/vector.h"

// @note: FYI - https://www.unknowncheats.me/forum/general-programming-and-reversing/173986-math-hack-2-predicting-future-3d-kinematics.html

struct FireBulletData_r
{
	Vector			vecPosition = { };
	Vector			vecDirection = { };
	Trace_t			enterTrace = { };
	float			flCurrentDamage = 0.0f;
	int				iPenetrateCount = 0;
};

class CRageBot : public CSingleton<CRageBot>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);

	// Global Values
	CBaseEntity* pBestEntity = nullptr;
private:
	// Main
	/* get entities, choose best target, aim */
	void TargetSpecificEnt(CBaseEntity* ent);
	void TargetEntities();
	void BestHitPoint(CBaseEntity* player, int prioritized, float minDmg, mstudiohitboxset_t* hitset, matrix3x4_t matrix[], Vector& vecOut);
		// Other
		/* 3-rd party functions */
	Vector CalculateBestPoint(CBaseEntity* pLocal, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[]);
	bool HitChance(QAngle angles, CBaseEntity* ent, float chance);

	void AutoStop();
	void AutoCrouch();
	
	bool SimulateFireBullet(CBaseCombatWeapon* weap, FireBulletData_r& data, CBaseEntity* player, int hitbox);
	bool HandleBulletPenetration(CCSWeaponData* wpn_data, FireBulletData_r& data);
	bool TraceToExit(Vector& end, CGameTrace* enter_trace, Vector start, Vector dir, CGameTrace* exit_trace);
	bool IsBreakableEntity(CBaseEntity* ent);
	void ClipTraceToPlayers(CBaseEntity* player, const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr);
	bool IsArmored(CBaseEntity* player, int armorVal, int hitgroup);

	void traceIt(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ign, CGameTrace* tr);
	// Check
	/* is entity valid */
	bool CheckTarget(int i);

	// Values
	/* hitbox position other info */
	CUserCmd* usercmd = nullptr;
	CBaseCombatWeapon* local_weapon = nullptr;
	int prev_aimtarget = NULL;
	bool can_fire_weapon = false;
	float cur_time = 0.f;
};
