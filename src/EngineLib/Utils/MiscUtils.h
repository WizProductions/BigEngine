#pragma once

//##############################################################################
//##--------------------------------- MACROS ---------------------------------##
//##############################################################################

#define INIT_NEW_PTR(ptr, type) \
do { \
ptr = new type; \
if (!ptr) { \
std::cerr << "Failed to allocate memory for " << #type << "at " << __FILE__ << ":" << __LINE__ << " in " << __func__ << std::endl; \
ptr = nullptr; \
return false; \
} \
} while (0)

#define INIT_PTR(ptr, value) \
do { \
ptr = value; \
if (!ptr) { \
std::cerr << "[INIT ERROR] Failed to assign pointer at " << __FILE__ << ":" << __LINE__ << " in " << __func__ << std::endl; \
ptr = nullptr; \
return false; \
} \
} while (0)

#define UNINIT_PTR(ptr) \
do { \
if (ptr != nullptr) { \
delete ptr; \
ptr = nullptr; \
} \
} while (0)

/** Inline check ptr, returns action if valid ptr otherwise returns -1 or CRASH :) */
#define IS_VALID(ptr, action) ((ptr) ? (ptr->action) : (std::cerr << "Invalid access to pointer at " << __FILE__ << ":" << __LINE__ << " in " << __func__ << std::endl, decltype(ptr->action)(-1)))

#ifdef _DEBUG
#define DESTRUCTOR_UNINIT(init_bool) \
do { \
this->UnInit(); \
if (init_bool) { \
std::cerr << "Object not properly uninitialized before destruction! Break at: " << __FILE__ << ":" << __LINE__ << " in " << __func__ << std::endl; \
} \
} while (0)
	
#else
#define DESTRUCTOR_UNINIT(init_bool) do { \
this->UnInit(); \
} while (0)
#endif

#define clamp(value, min, max) (value) < (min) ? (min) : (value) > (max) ? (max) : (value)
#define clampVector(value, min, max) (value) DirectX::XMVECTOR (min) ? (min) : (value) > (max) ? (max) : (value)
#define PI 3.14159265358979323846
#define PI_f 3.1415926535897932f
#define clampNormalized(value) (value) < 0 ? 0 : (value) > 1 ? 1 : (value)
#define gameDt GameManager::Get()->GetDeltaTime()
#define GETTER _forceinline [[nodiscard]]
#define SETTER _forceinline

/** eg: FOR_I(0, <=, lastIndex, ++) { ... } */
#define FOR_I(start, operator, end, nextOperator) for (int i(start); i operator end; i nextOperator)
/** eg: FOR_J(0, <=, lastIndex, ++) { ... } */
#define FOR_J(start, operator, end, nextOperator) for (int j(start); j operator end; j nextOperator)

#define ASSERT_MSG(expr, msg) \
	if (!expr) { \
	std::cerr << "ASSERT was triggered at: " << __FILE__ << ":" << __LINE__ << " in " << __func__ << std::endl; \
	assert(expr && msg); \
	} \

class MiscUtils {
	
//##############################################################################
//##---------------------------- STATIC ATTRIBUTES ---------------------------##
//##############################################################################

public:
	/* The folder is located at C:\Users\{username}\AppData\Local\WizProductions\ProjectBetise\ */
	/* But this variable store only the last part of the path (WizProductions\...) */
	static std::string m_CompanyAppFolder;
	

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

public:

/*----------< CONSTRUCTORS >----------*/


/*------------------------------------*/

/* GETTERS */

	static std::string GetAppDataFolder();

/* SETTERS */



/* OTHERS FUNCTIONS */

	/* Return the linear interpolation between a and b at time */
	static float Lerp(float a, float b, float time);
	//static Vector2<float> Lerp(const Vector2<float>& a, const Vector2<float>& b, const Vector2<float>& time);
	/* Return the spherical interpolation between a and b at time */
	static double SLerp(float a, float b, float time);

	/* Return the degree in radiant */
	static double DegreeToRadiant(float degree);

	/** Squared Distance between A and B */
	/** @param vA: Vector A */
	/** @param vB: Vector B */
	GETTER double SquaredDistance(DirectX::XMVECTOR vA, DirectX::XMVECTOR vB) const;

	static UINT64 RandomUINT64();

	static SystemId ConvertToSystemId(SystemType type);
};

std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& v);