#pragma once

#include "aleContextFunctions.h"
#include "aleEngineConfig.h"

#include <iostream>
#include <stdexcept>

namespace almond {

	void initSDL3();
	bool processSDL3();
	void cleanupSDL3();

}