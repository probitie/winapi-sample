#pragma once
#include "pch.h"

bool WriteUnsignedIntToRegistry(const std::string& key, uint32_t value);
bool ReadUnsignedIntFromRegistry(const std::string& key, uint32_t& value);
bool DeleteFromRegistry(const std::string& key);
