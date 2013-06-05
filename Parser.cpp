#include "Parser.h"

void Parser::skipChar() {
	if (curC == WEOF)
		return;	
	if (curC == L'\n') {
		col = 0;
		row++;
	} else {
		col++;
	}
	curC = fgetwc(file);
}

void Parser::skipSpaces() {
	while (iswspace(curC))
		skipChar();
}

void Parser::skipBOM() {
	if (curC == 0xEF)
		skipChar();
	if (curC == 0xBB)
		skipChar();
	if (curC == 0xBF)
		skipChar();
}

Parser::Parser(const std::string &filename) {
	fName = filename;
	file = fopen(fName.c_str(), "r");
	row = col = 0;
	curC = fgetwc(file);
	skipBOM();
	skipSpaces();
}

Parser::~Parser() {
	fclose(file);
}

wchar_t Parser::curChar() {
	return curC;
}

wchar_t Parser::nextChar() {
	skipChar();
	skipSpaces();
	return curC;
}

std::wstring Parser::nextString(std::wstring breakers) {
	skipChar();
	skipSpaces();
	std::wstring str;
	while (curC != WEOF && breakers.find(curC) == std::wstring::npos) {
		str += curC;
		skipChar();
	}
	return str;
}

bool Parser::testCurChar(std::wstring chars) {
	if (chars.find(curC) == std::wstring::npos) {
		fwprintf(stderr, L"Parse error (");
		fprintf(stderr, "%s",  fName.c_str());
		fwprintf(stderr, L": %d, %d): \"%ls\" expected, but '%ls' found.\n", 
			     row, col, chars.c_str(), curC == WEOF ? L"EOF" : std::wstring(1, curC).c_str());
		return false;
	}
	return true;
}

bool Parser::testNextChar(std::wstring chars) {
	nextChar();
	return testCurChar(chars);
}