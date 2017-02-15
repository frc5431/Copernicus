//STANDARD INCLUDES
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdarg>

//OPENCV INCLUDES
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


#define PERCEPTION_BASE 					"/vision/"
#define PERCEPTION_LOGGER_PRINT 			"LOGGER"

#define SLOG_INIT() Logger::Init()
#define SLOG_PRINT(X) Logger::Log(X)
#define SLOG_FREE() Logger::Free()

///@brief Handle the PERCEPTION log tee (file and actual print output) operations
#ifndef PERCEPTION_LOG_TEE

//Default operation is to print both to file and console
#define PERCEPTION_LOG_TEE 					2

#endif //PERCEPTION_LOG_TEE


///@brief Handle the date settings of the logger
#ifndef PERCEPTION_LOG_DATE

//Default operation is to print the current logged date
#define PERCEPTION_LOG_DATE 				true

#endif

///@brief Handle the namespace settings of the logger
#if ! defined(PERCEPTION_LOG_NAMESPACE) || ! defined (PERCEPTION_LOG_DEFAULT_NAMESPACE)

//Default operations for namespacing of PERCEPTION log
#define PERCEPTION_LOG_NAMESPACE 			true
#define PERCEPTION_LOG_DEFAULT_NAMESPACE 	"GENERAL"

#endif

///@brief Handle the log message locations
#ifndef PERCEPTION_LOG_LOCATION

//Default log location will be in the log home folder
#define PERCEPTION_LOG_LOCATION 			PERCEPTION_BASE "/logs"
#define PERCEPTION_LOG_LATEST_LOCATION 		PERCEPTION_LOG_LOCATION "/latest.txt"

#endif


#ifndef PERCEPTION_LOG_H
#define PERCEPTION_LOG_H

///@brief To debug the perception windows (true show windows, false don't show)
#define PERCEPTION_LOG_WINDOWS				true
#define SC(X)								#X //Literal converstion
#define SW(X)								boost::lexical_cast<std::string> (X) //String wrap

/** @class Logger log.hpp
 * @brief Class to handle all data logs and streams for the PERCEPTION hub
 *
 * This will update logs that can be live saved to files check time stamps
 * and upload files to another host if needed to. Mainly used for error reporting
 * for the developers of PERCEPTION.
 *
 */
namespace Logger {
	/**
	 * @brief Initialize the logger by opening a new tee stream
	 *
	 */
	void Init();

	/**
	 * @brief
	 *
	 */
	template<typename T>
	void Log(T);
	void Log(const char *, const std::string &, bool error=false);

	template<typename T>
	void LogError(T);
	template<typename T>
	void LogError(const char *, const T &);

	void LogWindow(const std::string &, cv::Mat &);

	void Free();
}

#endif //PERCEPTION_LOG_H
