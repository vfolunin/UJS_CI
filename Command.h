#ifndef UJS_CI_COMMAND
#define UJS_CI_COMMAND

#include <string>

class Command {
public:
	std::wstring languageId;
	std::wstring compilerId;
	std::wstring compileString;
	std::wstring runString;
	bool operator < (const Command &c) const { return compilerId < c.compilerId /*|| 
		                                              (compilerId == c.compilerId && languageId < c.languageId)*/; }
};

#endif