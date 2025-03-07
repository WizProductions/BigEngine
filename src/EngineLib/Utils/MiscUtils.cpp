#include "pch.h"
#include "MiscUtils.h"

std::string MiscUtils::m_CompanyAppFolder = R"(\WizProductions\MultiProject\Client\)";

std::string MiscUtils::GetAppDataFolder() {

	PWSTR localAppDataPath = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath))) {
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, localAppDataPath, -1, nullptr, 0, nullptr, nullptr);
		char* localAppDataString = new char[bufferSize];
		WideCharToMultiByte(CP_UTF8, 0, localAppDataPath, -1, localAppDataString, bufferSize, nullptr, nullptr);
		CoTaskMemFree(localAppDataPath);
		
		std::string result(localAppDataString);
		delete[] localAppDataString;
		
		return result;
	}
	
	std::cerr << "MiscUtils(): Cannot find local AppData folder." << std::endl;
	return "null";
}

float MiscUtils::Lerp(const float a, const float b, float time) {
	time = clampNormalized(time);
	
	return a + (b - a) * time;
}

/*Vector2<float> MiscUtils::Lerp(const Vector2<float>& a, const Vector2<float>& b, const Vector2<float>& time) {

	const Vector2<float> clampedTime = { clampNormalized(time.x), clampNormalized(time.y) };
	
	return a + (b - a) * clampedTime;
}*/

double MiscUtils::SLerp(const float a, const float b, float time) {
	time = clampNormalized(time);
	
	time = sin(time * PI_f + DegreeToRadiant(270)); //TODO check if it's correct because normally need to try "if" => check pdf
	time = (time + 1.f) * 0.5f;
	return a + (b-a) * time;
}

double MiscUtils::DegreeToRadiant(const float degree) {
	return degree * (PI / 180.f);
}

double MiscUtils::SquaredDistance(DirectX::XMVECTOR vA, DirectX::XMVECTOR vB) const {
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(vA, vB);
	v = DirectX::XMVector3LengthSq(v);
	return DirectX::XMVectorGetX(v);
}

UINT64 MiscUtils::RandomUINT64() {
	static std::random_device rd;
	static std::mt19937_64 gen(rd());
	static std::uniform_int_distribution<UINT64> dis(0, UINT64(-1));

	return dis(gen);
}

std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& v) {
	os << std::setprecision(3) << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

SystemId MiscUtils::ConvertToSystemId(SystemType type) {
    
	uint32_t value = static_cast<uint32_t>(type);
	uint32_t index = 0;
	while (value > 1) {
		value >>= 1;
		index++;
	}

	return static_cast<SystemId>(index);
}