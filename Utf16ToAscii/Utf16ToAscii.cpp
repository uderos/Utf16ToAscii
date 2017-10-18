// Utf16ToAscii.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Config.h"
#include "FileConverter.h"

enum eReturnCode
{
	RC_OKAY = 0,
	RC_ERROR = 1
};

int main(const int argc, const char *argv[])
{
	int rc = RC_OKAY;

	try
	{
		g_CONFIG.ProcessCmdLine(argc, argv);

		FileConverter fc;
		fc.Convert(g_CONFIG.GetInputFileName(), g_CONFIG.GetOutputFileName());
		std::cout << "Created " << g_CONFIG.GetOutputFileName() << std::endl;
	}
	catch (std::exception & exh)
	{
		std::cout << "\nEXCEPTION: " << exh.what() << std::endl;
		rc = RC_ERROR;
	}

    return 0;
}

