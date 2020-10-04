// used: shgetknownfolderpath
#include <shlobj.h>

#include "config.h"
// used: cheat variables
#include "variables.h"
// used: log config result state
#include "../utilities/logging.h"
// used: json parser implementation
#include "../../dependencies/json/json.hpp"

bool C::Setup(std::string_view szDefaultFileName)
{
	// create directory "settings" in "%userprofile%\documents\.qo0" if it incorrect or doesnt exists
	if (!std::filesystem::is_directory(fsPath))
	{
		std::filesystem::remove(fsPath);
		if (!std::filesystem::create_directories(fsPath))
			return false;
	}

	// create default config
	if (!Save(szDefaultFileName))
		return false;

	// load default config
	if (!Load(szDefaultFileName))
		return false;

	// refresh configs list
	Refresh();

	return true;
}

bool C::Save(std::string_view szFileName)
{
	// check for extension if it is not our replace it
	std::filesystem::path fsFilePath(szFileName);
	if (fsFilePath.extension() != XorStr(".fl0w"))
		fsFilePath.replace_extension(XorStr(".fl0w"));

	// get utf-8 full path to config
	const std::string szFile = std::filesystem::path(fsPath / fsFilePath).u8string();
	nlohmann::json config;

	try
	{
		for (auto& variable : vecVariables)
		{
			nlohmann::json entry;

			// save hashes to compare it later
			entry[XorStr("name-id")] = variable.uNameHash;
			entry[XorStr("type-id")] = variable.uTypeHash;

			// get current variable
			switch (variable.uTypeHash)
			{
			case FNV1A::HashConst("int"):
			{
				entry[XorStr("value")] = variable.Get<int>();
				break;
			}
			case FNV1A::HashConst("float"):
			{
				entry[XorStr("value")] = variable.Get<float>();
				break;
			}
			case FNV1A::HashConst("bool"):
			{
				entry[XorStr("value")] = variable.Get<bool>();
				break;
			}
			case FNV1A::HashConst("std::string"):
			{
				entry[XorStr("value")] = variable.Get<std::string>();
				break;
			}
			case FNV1A::HashConst("Color"):
			{
				auto colVariable = variable.Get<Color>();

				// store RGBA as sub-node
				nlohmann::json sub;

				// fill node with all color values
				sub.push_back(colVariable.r());
				sub.push_back(colVariable.g());
				sub.push_back(colVariable.b());
				sub.push_back(colVariable.a());

				entry[XorStr("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<bool>"):
			{
				auto vecBools = variable.Get<std::vector<bool>>();

				// store vector values as sub-node
				nlohmann::json sub;

				// fill node with all vector values
				for (const auto& bValue : vecBools)
					sub.push_back(static_cast<bool>(bValue));

				entry[XorStr("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<int>"):
			{
				auto vecInts = variable.Get<std::vector<int>>();

				// store vector values as sub-node
				nlohmann::json sub;

				// fill node with all vector values
				for (auto& iValue : vecInts)
					sub.push_back(iValue);

				entry[XorStr("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<float>"):
			{
				auto vecFloats = variable.Get<std::vector<float>>();

				// store vector values as sub-node
				nlohmann::json sub;

				// fill node with all vector values
				for (auto& flValue : vecFloats)
					sub.push_back(flValue);

				entry[XorStr("value")] = sub.dump();
				break;
			}
			default:
				break;
			}

			// add current variable to config
			config.push_back(entry);
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		L::PushConsoleColor(FOREGROUND_RED);
		L::Print(fmt::format(XorStr("[error] json save failed: {}"), ex.what()));
		L::PopConsoleColor();
		return false;
	}

	// open output config file
	std::ofstream ofsOutFile(szFile, std::ios::out | std::ios::trunc);

	if (!ofsOutFile.good())
		return false;

	try
	{
		// write stored variables
		ofsOutFile << config.dump(4);
		ofsOutFile.close();
	}
	catch (std::ofstream::failure& ex)
	{
		L::PushConsoleColor(FOREGROUND_RED);
		L::Print(fmt::format(XorStr("[error] failed to save configuration: {}"), ex.what()));
		L::PopConsoleColor();
		return false;
	}

	L::Print(fmt::format(XorStr("saved configuration at: {}"), szFile));
	return true;
}

bool C::Load(std::string_view szFileName)
{
	// get utf-8 full path to config
	const std::string szFile = std::filesystem::path(fsPath / szFileName).u8string();
	nlohmann::json config;

	// open input config file
	std::ifstream ifsInputFile(szFile, std::ios::in);

	if (!ifsInputFile.good())
		return false;

	try
	{
		// parse saved variables
		ifsInputFile >> config;
		ifsInputFile.close();
	}
	catch (std::ifstream::failure& ex)
	{
		L::PushConsoleColor(FOREGROUND_RED);
		L::Print(fmt::format(XorStr("[error] failed to load configuration: {}"), ex.what()));
		L::PopConsoleColor();
		return false;
	}

	try
	{
		for (auto& variable : config)
		{
			int nIndex = GetVariableIndex(variable[XorStr("name-id")].get<FNV1A_t>());

			// check is variable exist
			if (nIndex == C_INVALID_VARIABLE)
				continue;

			// get variable
			auto& entry = vecVariables.at(nIndex);

			switch (variable[XorStr("type-id")].get<FNV1A_t>())
			{
			case FNV1A::HashConst("bool"):
			{
				entry.Set<bool>(variable[XorStr("value")].get<bool>());
				break;
			}
			case FNV1A::HashConst("float"):
			{
				entry.Set<float>(variable[XorStr("value")].get<float>());
				break;
			}
			case FNV1A::HashConst("int"):
			{
				entry.Set<int>(variable[XorStr("value")].get<int>());
				break;
			}
			case FNV1A::HashConst("std::string"):
			{
				entry.Set<std::string>(variable[XorStr("value")].get<std::string>());
				break;
			}
			case FNV1A::HashConst("Color"):
			{
				auto color = nlohmann::json::parse(variable[XorStr("value")].get<std::string>());

				entry.Set<Color>(Color(
					color.at(0).get<std::uint8_t>(),
					color.at(1).get<std::uint8_t>(),
					color.at(2).get<std::uint8_t>(),
					color.at(3).get<std::uint8_t>()
				));

				break;
			}
			case FNV1A::HashConst("std::vector<bool>"):
			{
				auto vector = nlohmann::json::parse(variable[XorStr("value")].get<std::string>());
				auto& vecBools = entry.Get<std::vector<bool>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{
					// check is item out of bounds
					if (i < vecBools.size())
						vecBools.at(i) = vector.at(i).get<bool>();
				}

				break;
			}
			case FNV1A::HashConst("std::vector<int>"):
			{
				auto vector = nlohmann::json::parse(variable[XorStr("value")].get<std::string>());
				auto& vecInts = entry.Get<std::vector<int>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{
					// check is item out of bounds
					if (i < vecInts.size())
						vecInts.at(i) = vector.at(i).get<int>();
				}

				break;
			}
			case FNV1A::HashConst("std::vector<float>"):
			{
				auto vector = nlohmann::json::parse(variable[XorStr("value")].get<std::string>());
				auto& vecFloats = entry.Get<std::vector<float>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{
					// check is item out of bounds
					if (i < vecFloats.size())
						vecFloats.at(i) = vector.at(i).get<float>();
				}

				break;
			}
			default:
				break;
			}
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		L::PushConsoleColor(FOREGROUND_RED);
		L::Print(fmt::format(XorStr("[error] json load failed: {}"), ex.what()));
		L::PopConsoleColor();
		return false;
	}

	L::Print(fmt::format(XorStr("loaded configuration at: {}"), szFile));
	return true;
}

void C::Remove(std::string_view szFileName)
{
	// unable delete default config
	if (!szFileName.compare(XorStr("default.fl0w")))
		return;

	// get utf-8 full path to config
	const std::string szFile = std::filesystem::path(fsPath / szFileName).u8string();

	if (std::filesystem::remove(szFile))
		L::Print(fmt::format(XorStr("removed configuration at: {}"), szFile));
}

void C::Refresh()
{
	vecFileNames.clear();

	for (const auto& it : std::filesystem::directory_iterator(fsPath))
    {
		if (it.path().filename().extension() == XorStr(".fl0w"))
		{
			L::Print(fmt::format(XorStr("found configuration file: {}"), it.path().filename().u8string()));
			vecFileNames.push_back(it.path().filename().u8string());
		}
    }
}

std::size_t C::GetVariableIndex(const FNV1A_t uNameHash)
{
	for (std::size_t i = 0U; i < vecVariables.size(); i++)
	{
		if (vecVariables.at(i).uNameHash == uNameHash)
			return i;
	}

	return C_INVALID_VARIABLE;
}

std::filesystem::path C::GetWorkingPath()
{
	std::filesystem::path fsWorkingPath;

	// get path to user documents
	if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
	{
		fsWorkingPath.assign(pszPathToDocuments);
		fsWorkingPath.append(XorStr(".fl0w"));
		CoTaskMemFree(pszPathToDocuments);
	}
	
	return fsWorkingPath;
}
