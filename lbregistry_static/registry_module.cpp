#include "pch.h"
#include "registry_module.h"

const std::string registry_subkey = "SOFTWARE\\LB_OS";

bool WriteUnsignedIntToRegistry(const std::string& key, uint32_t value)
{
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, registry_subkey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
    {
        if (RegSetValueExA(hKey, key.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value)) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return true;
        }
    }
    RegCloseKey(hKey);
    return false;
}

bool ReadUnsignedIntFromRegistry(const std::string& key, uint32_t& value)
{
    HKEY hKey;
    DWORD valueSize = sizeof(value);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, registry_subkey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueExA(hKey, key.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &valueSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);

            return true;
        }
    }
    RegCloseKey(hKey);
    return false; // Return -1 in case of failure
}

bool DeleteFromRegistry(const std::string& key)
{
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, registry_subkey.c_str(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        if (RegDeleteValueA(hKey, key.c_str()) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return true;
        }
    }
    RegCloseKey(hKey);
    return false;
}
