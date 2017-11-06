#include "Log.h"

Log::Log()
	: m_willWriteToConsole{ false }
{
}

void Log::setConsoleOut(bool willWriteToConsole)
{
	m_willWriteToConsole = willWriteToConsole;
}

void Log::setOutputFile(const std::string & filePath)
{

}
