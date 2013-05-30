#include "CommandSet.h"
#include <string>
#include <cstring>

const int RESULT_OK = 0;
const int RESULT_BAD_USAGE = 1;
const int RESULT_NO_COMPILER = 2;

void printUsage() {
	fwprintf(stdout, 
		     L"USAGE:\n"
			 L"  ujs_ci  --cm <com_make> [--pm <per_make>] [--rc <res_compile>]               \n"
			 L"          [--rr <result_run>] [--rm <res_make>]                              \n\n"
			 L"  --cm <com_make>     Path to the JSON file containing server compilation and  \n"
			 L"                      run commands (like system_make.json).                    \n"
			 L"  --pm <per_make>     Path to the JSON file containing compilation permissions \n"
			 L"                      and options (like make.json or author_make.json).        \n"
			 L"  --rc <res_compile>  Path to the plain-text file to save one actual compile   \n"
			 L"                      command in.                                              \n"
			 L"  --rr <res_run>      Path to the plain-text file to save one actual run com-  \n"
			 L"                      mand in.                                                 \n"
			 L"  --rm <res_make>     Path to the JSON file to save actual compilation and run \n"
			 L"                      commands in.                                             \n");
}

bool exists(std::string filename) {
	FILE *f = fopen(filename.c_str(), "r");
	if (f) {
		fclose(f);
		return true;
	}
	return false;
}

int tryProcess(int argc, char **argv) {
	bool keyUsed[5] = { 0, 0, 0, 0, 0 };
	std::string keys[5] = { "--cm", "--pm", "--rc", "--rr", "--rm" };
	std::string file[5];	
	if (argc < 3 || argc > 11 || argc % 2 == 0) {
		fprintf(stderr, "Bad arguments count: %d\n", argc);
		return RESULT_BAD_USAGE;
	}
	for (int i = 1; i < argc; i += 2) {
		bool u = false;
		for (int j = 0; j < 5; j++) {
			if (keyUsed[j] || strcmp(argv[i], keys[j].c_str()))
				continue;
			u = true;
			keyUsed[j] = true;
			file[j] = argv[i + 1];
			break;
		}
		if (!u) {
			fprintf(stderr, "Illegal key \"%s\"\n", argv[i]);
			return RESULT_BAD_USAGE;
		}
	}
	if (!keyUsed[0] || !exists(file[0])) {
		fprintf(stderr, "File \"%s\" does not exist.\n", file[0].c_str());
		return RESULT_BAD_USAGE;
	}
	CommandSet commandSet(file[0]);	
	if (keyUsed[1]) {
		if (!exists(file[1])) {
			fprintf(stderr, "File \"%s\" does not exist.\n", file[1].c_str());
			return RESULT_BAD_USAGE;
		}
		commandSet.applyPermissions(PermissionSet(file[1]));
	}
	if (keyUsed[2]) {
		if (!commandSet.serializeCompileString(file[2]))
			return RESULT_NO_COMPILER;
	}
	if (keyUsed[3]) {
		if (!commandSet.serializeRunString(file[3]))
			return RESULT_NO_COMPILER;
	}
	if (keyUsed[4]) {
		if (!commandSet.serialize(file[4]))
			return RESULT_NO_COMPILER;
	}
	return RESULT_OK;
}

int main(int argc, char **argv) {
    int result = tryProcess(argc, argv);
	if (result == RESULT_BAD_USAGE)
		printUsage();
	return result;	
}