#pragma once
// used: directx math definitions
#include <directxmath.h>

// used: winapi, directx includes
#include "../common.h"
// used: vector, vector2d
#include "../sdk/datatypes/vector.h"
// used: angle
#include "../sdk/datatypes/qangle.h"
// used: matrix
#include "../sdk/datatypes/matrix.h"

extern "C" using RandomSeedFn = int(__cdecl*)(int iSeed);
extern "C" using RandomFloatFn = float(__cdecl*)(float flMinVal, float flMaxVal);
extern "C" using RandomFloatExpFn = float(__cdecl*)(float flMinVal, float flMaxVal, float flExponent);
extern "C" using RandomIntFn = int(__cdecl*)(int iMinVal, int iMaxVal);
extern "C" using RandomGaussianFloatFn = float(__cdecl*)(float flMean, float flStdDev);

#define M_HPI			DirectX::XM_PIDIV2	// half pi
#define M_QPI			DirectX::XM_PIDIV4	// quarter pi
#define M_PI			DirectX::XM_PI		// pi const
#define M_2PI			DirectX::XM_2PI		// pi double
#define M_GPI			1.6180339887498f	// golden ratio
#define M_RADPI			57.295779513082f	// pi in radians

#define M_METRE2INCH( x )	( x / 0.0254f )
#define M_INCH2METRE( x )	( x * 0.0254f )
#define M_METRE2FOOT( x )	( x * 3.28f )
#define M_FOOT2METRE( x )	( x / 3.28f )
#define M_RAD2DEG( x )		DirectX::XMConvertToDegrees( x )
#define M_DEG2RAD( x )		DirectX::XMConvertToRadians( x )

/*
 * MATHEMATICS
 */
namespace M
{
	// Get
	/* export specific functions addresses from libraries handles to get able call them */
	bool	Setup();
	/* convert vector to angles */
	void	VectorAngles(const Vector& vecForward, QAngle& angView);
	/* convert angles to x, y, z vectors */
	void	AngleVectors(const QAngle& angView, Vector* pForward, Vector* pRight = nullptr, Vector* pUp = nullptr);
	/* convert angles to matrix */
	void	AngleMatrix(const QAngle& angles, matrix3x4_t& matrix);
	/* convert angle to screen pixels by sensivity, pitch and yaw */
	// @note: can be useful at mouse event aimbot
	Vector2D AnglePixels(float flSensitivity, float flPitch, float flYaw, const QAngle& angBegin, const QAngle& angEnd);
	/* convert screen pixels to angles by sensivity, pitch and yaw */
	QAngle	PixelsAngle(float flSensitivity, float flPitch, float flYaw, const Vector2D& vecPixels);
	/* calculate angles by source and destination vectors */
	QAngle	CalcAngle(const Vector vecStart, Vector vecEnd);
	/* transform vector by matrix and return it */
	Vector	VectorTransform(const Vector& vecIn, const matrix3x4_t& matrix);
	/* calculate next tick position */
	Vector	ExtrapolateTick(Vector p0, Vector v0);

	// Exports
	inline RandomSeedFn				RandomSeed;
	inline RandomFloatFn			RandomFloat;
	inline RandomFloatExpFn			RandomFloatExp;
	inline RandomIntFn				RandomInt;
	inline RandomGaussianFloatFn	RandomGaussianFloat;
}
