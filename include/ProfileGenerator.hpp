#ifndef __PROFILE_MANAGER_H__
#define __PROFILE_MANAGER_H__

#include "ProfileDefinition.hpp"
#include <ArduinoJson.h>

#include <stdexcept>
#include <string>

class ProfileGenerator
{
public:
    Profile profile;
    ProfileGenerator(const char* json);
    size_t memoryUsed;
};

#endif  // __PROFILE_MANAGER_H__
