#ifndef PERCEPTION_TABLES_H
#define PERCEPTION_TABLES_H

//STANDARD INCLUDES
#include <iostream>

//PERCEPTION INCLUDES
#include "perception.hpp"
#include "processing.hpp"
#include "log.hpp"

namespace Table {
	void init();
	void updateTarget(processing::Target);


	template<typename T>
	void _Logger(T, bool = false);
}

#endif
