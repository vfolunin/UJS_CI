#ifndef UJS_CI_COMMANDSET
#define UJS_CI_COMMANDSET

#include "Command.h"
#include "PermissionSet.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <set>
#include <map>

class CommandSet {
	std::map<std::wstring, Command> commands;
	std::map<std::wstring, std::vector<Command *> > languageGroups;
	bool addCommand(const Command &command);
public:
	CommandSet(const std::string &filename);
	virtual ~CommandSet() { }
	void applyPermissions(const PermissionSet &permissions);
	bool serialize(const std::string &filename);
	bool serializeCompileString(const std::string &filename);
	bool serializeRunString(const std::string &filename);
	typedef std::map<std::wstring, Command>::const_iterator const_iterator;
	const_iterator begin() const { return commands.begin(); }
	const_iterator end() const { return commands.end(); }
};

#endif