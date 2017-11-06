#pragma once

#include <iostream>
#include <string>

class Log {
public:
	Log();

	void setConsoleOut(bool);
	void setOutputFile(const std::string& filePath);

	template <typename T>
	Log& operator<<(T);

private:
	bool m_willWriteToConsole;
};

template<typename T>
inline Log& Log::operator<<(T thingToWriteOut)
{
	if (m_willWriteToConsole) {
		std::cerr << thingToWriteOut << std::endl;
	} else {
		std::cerr << "ERROR: No log output set, either set console or log file" << std::endl;
	}

	return *this;
}
