#pragma once
#include "pch.h"


/**
 * \brief these defines are registry keys for the first and the second number in the task
 *	      they are not related to registry itself, instead they are here to be available from
 *		  app1 and app2 that are using registry
 */
#define LB_REG_KEY1 "key1" // first number is stored under this key
#define LB_REG_KEY2 "key2" // second number is stored under this key


/**
 * \brief writes uint32_t to from registry
 * \param key string that represents a registry key
 * \param value uint32_t to write
 * \return true if success, false if error
 */
bool WriteUnsignedIntToRegistry(const std::string& key, uint32_t value);

/**
 * \brief reads uint32_t from registry
 * \param key string that represents a registry key in the app, not the full key
 *			  it can have any unique name
 * \param value uint32_t where to write the result
 * \return true if success, false if error
 */
bool ReadUnsignedIntFromRegistry(const std::string& key, uint32_t& value);

/**
 * \brief deletes registry key/value pair
 * \param key string that represents a registry key
 * \return true if success, false if error
 */
bool DeleteFromRegistry(const std::string& key);
