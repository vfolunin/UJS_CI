#ifndef UJS_CI_PARSER
#define UJS_CI_PARSER

#include <string>

class Parser {
	FILE *file;
	std::string fName;
	int row;
	int col;
	wchar_t curC;
	void skipChar();
	void skipSpaces();
	void skipBOM();
public:
	Parser(const std::string &filename);
	virtual ~Parser();
	wchar_t curChar();	
	wchar_t nextChar();
	std::wstring nextString(std::wstring breakers);
	bool testCurChar(std::wstring chars);
	bool testNextChar(std::wstring chars);
};

#endif