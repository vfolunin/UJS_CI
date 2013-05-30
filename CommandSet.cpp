#include "CommandSet.h"

bool CommandSet::addCommand(const Command &command) {
	std::pair<std::map<std::wstring, Command>::iterator, bool> insertResult = commands.insert(std::make_pair(command.compilerId, command));
	if (!insertResult.second)
		return false;
	Command *insertedCommandPtr = &(insertResult.first->second);
	std::map<std::wstring, std::vector<Command *> >::iterator lgIt = languageGroups.find(command.languageId);
	if (lgIt == languageGroups.end()) {
		std::vector<Command *> languageGroup(1, insertedCommandPtr);
		languageGroups.insert(std::make_pair(command.languageId, languageGroup));
	} else {
		lgIt->second.push_back(insertedCommandPtr);
	}
	return true;
}

CommandSet::CommandSet(const std::string &filename) {	
	Parser parser(filename);
	if (!parser.testCurChar(L"{"))
		return;
	Command command;
	for (int languageIndex = 1; true; languageIndex++) {
		if (!parser.testNextChar(L"\"")) {
			commands.clear();
			languageGroups.clear();
			return;
		}
		command.languageId = parser.nextString(L"\"");
		if (!parser.testNextChar(L":") || !parser.testNextChar(L"{"))
			return;
		for (int commandIndex = 1; true; commandIndex++) {
			if (!parser.testNextChar(L"\"")){
				commands.clear();
				languageGroups.clear();
				return;
			}
			command.compilerId = parser.nextString(L"\"");
			if (!parser.testNextChar(L":") || !parser.testNextChar(L"[") || !parser.testNextChar(L"\"")) {
				commands.clear();
				languageGroups.clear();
				return;
			}
			command.compileString = parser.nextString(L"\"");
			if (!parser.testNextChar(L",") || !parser.testNextChar(L"\"")) {
				commands.clear();
				languageGroups.clear();
				return;
			}
			command.runString = parser.nextString(L"\"");
			if (!parser.testNextChar(L"]") || !parser.testNextChar(L",}")) {
				commands.clear();
				languageGroups.clear();
				return;
			}
			if (!command.languageId.length()) {
				fwprintf(stderr, L"Warning: command %d in language group %d has no language ID. Command will be skipped.\n", commandIndex, languageIndex);
			} else if (!command.compilerId.length()) {
				fwprintf(stderr, L"Warning: command %d in language group %d has no compiler ID. Command will be skipped.\n", commandIndex, languageIndex);
			} else if (!command.compileString.length()) {
				fwprintf(stderr, L"Warning: command %d in language group %d has no compile string. Command will be skipped.\n", commandIndex, languageIndex);
			} else if (!command.runString.length()) {
				fwprintf(stderr, L"Warning: command %d in language group %d has no run string. Command will be skipped.\n", commandIndex, languageIndex);
			} else if (!addCommand(command)) {
				fwprintf(stderr, L"Warning: command %d in language group %d has compiler ID \"%ls\" and language ID \"%ls\" which are already used. Command will be skipped.\n", 
					     commandIndex, languageIndex, command.compilerId.c_str(), command.languageId.c_str());
			}
			if (parser.curChar() == L'}')
				break;
		}
		if (!parser.testNextChar(L",}")) {
			commands.clear();
			languageGroups.clear();
			return;
		}
		if (parser.curChar() == L'}')
			break;
	}
}

std::wstring setOptions(const std::wstring &compileString, const std::wstring &optionsString) {
	static const std::wstring OPT_MARKER = L"#OPT#";
	static const size_t OPT_MARKER_SIZE = OPT_MARKER.length();
	std::wstring result = compileString;
	size_t optPos = result.find(OPT_MARKER);
	if (optPos != std::wstring::npos)
		result.replace(optPos, OPT_MARKER_SIZE, optionsString);
	return result;
}

void CommandSet::applyPermissions(const PermissionSet &permissions) {
	std::set<Command> tempCommandSet;
	for (PermissionSet::const_iterator i = permissions.begin(); i != permissions.end(); i++) {
		std::map<std::wstring, std::vector<Command *> >::iterator lgIt = languageGroups.find(i->id);
		if (lgIt != languageGroups.end()) {
			std::vector<Command *> languageGroup = lgIt->second;
			for (int j = 0; j < languageGroup.size(); j++) {
				tempCommandSet.insert(*languageGroup[j]);
			}
		}
		std::map<std::wstring, Command>::iterator cIt = commands.find(i->id);
		if (cIt != commands.end()) {
			Command command = cIt->second;
			command.compileString = setOptions(command.compileString, i->options);
			tempCommandSet.insert(command);
		}
	}
	commands.clear();
	languageGroups.clear();
	for (std::set<Command>::iterator i = tempCommandSet.begin(); i != tempCommandSet.end(); i++)
		addCommand(*i);
}

bool CommandSet::serialize(const std::string &filename) {
	if (commands.empty())
		return false;
	setlocale(LC_ALL, "en_US.utf8");
	FILE *file = fopen(filename.c_str(), "w");
	fwprintf(file, L"{\n");
	for (std::map<std::wstring, std::vector<Command *> >::iterator i = languageGroups.begin(); i != languageGroups.end(); i++) {
		if (i != languageGroups.begin())
			fwprintf(file, L",\n");
		fwprintf(file, L"  \"%ls\" : {\n", i->first.c_str());
		for (std::vector<Command *>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			if (j != i->second.begin())
				fwprintf(file, L",\n");			
			fwprintf(file, L"    \"%ls\" : [\"%ls\", \"%ls\"]", (*j)->compilerId.c_str(), setOptions((*j)->compileString, L"").c_str(), (*j)->runString.c_str());
		}
		fwprintf(file, L"\n  }");
	}
	fwprintf(file, L"\n}");
	fclose(file);
	return true;
}

bool CommandSet::serializeCompileString(const std::string &filename) {
	if (commands.empty())
		return false;
	FILE *file = fopen(filename.c_str(), "w");
	fwprintf(file, L"%ls", setOptions(commands.begin()->second.compileString, L"").c_str());
	fclose(file);
	return true;
}

bool CommandSet::serializeRunString(const std::string &filename) {
	if (commands.empty())
		return false;
	FILE *file = fopen(filename.c_str(), "w");
	fwprintf(file, L"%ls", commands.begin()->second.runString.c_str());
	fclose(file);
	return true;
}