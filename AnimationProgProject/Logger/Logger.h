#pragma once
#include <cstdio>

class Logger {
	public:

		// Template for printing logs with specified log level.
		// If log level is greater current LogLevel then logs will be suppressed
		template<typename... Args>
		static void log(unsigned int logLevel, Args... args) {
			if (logLevel < mLogLevel) {
				std::printf(args ...);
			}
		}


		// Setters // 

		static void setLogLevel(unsigned int inLogLevel) {
			inLogLevel <= mMaxLogLevel ? mLogLevel = inLogLevel : mLogLevel = mMaxLogLevel;
		}

		static void setMaxLogLevel(unsigned int inMaxLogLevel) {
			mMaxLogLevel = inMaxLogLevel;
		}


	private:
		// Current Log Level
		static unsigned int mLogLevel;
		// Maximum Log Levels that can be set
		static unsigned int mMaxLogLevel;

};
