#ifndef UJS_CI_PERMISSIONSET
#define UJS_CI_PERMISSIONSET

#include "Permission.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <set>

class PermissionSet {
	std::vector<Permission> permissions;
public:
	PermissionSet(const std::string &filename);
	virtual ~PermissionSet() { }
	typedef std::vector<Permission>::const_iterator const_iterator;
	const_iterator begin() const { return permissions.begin(); }
	const_iterator end() const { return permissions.end(); }
};

#endif