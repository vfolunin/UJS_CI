#ifndef UJS_CI_PERMISSION
#define UJS_CI_PERMISSION

#include <string>

class Permission {
public:
	std::wstring id;
	std::wstring options;
	bool operator < (const Permission &p) const { return id < p.id; }
};

#endif