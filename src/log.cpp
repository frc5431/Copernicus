#include "../include/log.hpp"

//STANDARD INCLUDES
#include <iostream>
#include <fstream>
#include <sstream>

//BOOST INCLUDES
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>


//Terminal color definitions
#define NO_COLOR 				"\033[0m"
#define RED_COLOR 				"\033[0;31m"
#define BLUE_COLOR 				"\033[0;34m"

//Boost datetime namespaces
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;


/**
 * @brief Logger handler
 * Details located in header file. The reason why we use printf is because
 * it's lower than cout and takes less processing, so it's faster and just/almost as
 * easy to use as cout
 */
namespace Logger {

	std::ofstream logFile;

	void Init() {
		#if PERCEPTION_LOG_TEE == 2 || PERCEPTION_LOG_TEE == 0
			if(logFile.is_open()) {
				Log("Logger already opened");
				return;
			}

			//Check for the log folder so that the new file can be created
			path testLog(SW(PERCEPTION_LOG_LOCATION));

			try {
				if(!is_directory(status(testLog))) {
					Log("PERCEPTION log folder doesn't exist! Creating...");

					if(create_directory(testLog)) {
						Log("Done creating folder!");
					} else {
						LogError(PERCEPTION_LOGGER_PRINT, "Failed creating log folder!");
					}

				}
			} catch (filesystem_error &e) {
				LogError(PERCEPTION_LOGGER_PRINT, std::string("Log folder error: ") + e.what());
			}

			//Open the defined PERCEPTION log location
			logFile.open(PERCEPTION_LOG_LATEST_LOCATION, std::ifstream::out);

			if(!logFile.is_open()) {
				LogError(PERCEPTION_LOGGER_PRINT, "Failed loading Logger!");
				return;
			}
		#endif

		Log("Started PERCEPTION Logger!");
	}

	template<typename T>
	void Log(T toLog) {
		Log(PERCEPTION_LOG_DEFAULT_NAMESPACE, toLog); //Use the standard namespace
	}

	template<typename T>
	void LogError(T toLog) {
		LoggError(PERCEPTION_LOG_DEFAULT_NAMESPACE, toLog);
	}

	template<typename T>
	void LogError(const char *space, const T &toPrint) {
		Log(space, toPrint, true);
	}

	std::string __formatTime(ptime now) {
	  time_facet* facet = new time_facet("%m/%d/%Y/ %H:%M:%S:%f");
	  std::stringstream wss;
	  wss.imbue(std::locale(wss.getloc(), facet));
	  wss << microsec_clock::universal_time();
	  return (wss.str());
	}

	std::string __formatLog(const std::string &name_space,
			const std::string &to_print, bool print_error=false,
			bool color=false) {
		std::stringstream toret("");
		#if PERCEPTION_LOG_DATE == true
			//Get current UTC date
			ptime now = second_clock::universal_time();

			toret << "[" << __formatTime(now) << "]";
		#endif

		#if PERCEPTION_LOG_NAMESPACE == true
			toret << "|" << ((color) ? BLUE_COLOR : "") <<
					name_space << ((color) ? NO_COLOR : "") << "|:";
		#endif

		//Set the print color to red and show error inprint
		if(print_error) {
			toret << ((color) ? RED_COLOR : "") <<
					"(ERROR)-> ";
		}

		//Print the main message
		toret << to_print;

		//Reset the color back to normal
		if(print_error) {
			toret << ((color) ? NO_COLOR : "");
		}

		return (toret.str());
	}

	template<typename T>
	void __printLog(const char *space, T toPrint, bool error=false) {
		std::cout << __formatLog(std::string(space),
						std::string(toPrint), error, true) << std::endl;
	}

	template<typename T>
	void __saveLog(const char *space, T toPrint, bool error=false) {
		if(!logFile.is_open()) return;
		logFile << __formatLog(std::string(space),
						std::string(toPrint), error) << std::endl;
	}

	void Log(const char * space, const std::string &toPrint, bool error) {
	#if PERCEPTION_LOG_TEE == 0
		__saveLog<T>(space, toPrint, error);
	#else
		#if PERCEPTION_LOG_TEE == 1
			__printLog<T>(space, toPrint, error);
		#else
			#if PERCEPTION_LOG_TEE == 2
				__printLog(space, toPrint, error);
				__saveLog(space, toPrint, error);
			#endif
		#endif
	#endif
	}

	void LogWindow(const std::string &window_name, cv::Mat &toLog) {
	#if PERCEPTION_LOG_WINDOWS == true
		if(toLog.empty()) return; //Skip the window logging if it's blank
		cv::imshow(window_name, toLog);
	#endif
	}

	void Free() {
		#if PERCEPTION_LOG_TEE == 2 || PERCEPTION_LOG_TEE == 0
			if(logFile.is_open()) {
				Log("Closed the Logger!");
			} else {
				LogError(PERCEPTION_LOGGER_PRINT, "Failed closing Logger!");
			}
		#endif
	}

}
