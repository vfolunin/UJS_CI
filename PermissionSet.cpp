#include "PermissionSet.h"

PermissionSet::PermissionSet(const std::string &filename) {	
	Parser parser(filename);
	if (!parser.testCurChar(L"{"))
		return;
	std::set<Permission> tempPermissionSet;
	Permission permission;
	for (int permissionIndex = 1; true; permissionIndex++) {
		if (!parser.testNextChar(L"\""))
			return;
		permission.id = parser.nextString(L"\"");
		if (!parser.testNextChar(L":") || !parser.testNextChar(L"\"")) 
			return;
		permission.options = parser.nextString(L"\"");
		if (!parser.testNextChar(L",}"))
			return;
		if (!permission.id.length()) {
			fwprintf(stderr, L"Warning: permission %d has no ID. Permission will be skipped.\n", permissionIndex);
		} else if (!tempPermissionSet.insert(permission).second) {
			fwprintf(stderr, L"Warning: permission %d has ID \"%ls\" which is already used. Permission will be skipped.\n", permissionIndex, permission.id.c_str());
		}
		if (parser.curChar() == L'}')
			break;
	}
	for (std::set<Permission>::iterator i = tempPermissionSet.begin(); i != tempPermissionSet.end(); i++)
		permissions.push_back(*i);
}