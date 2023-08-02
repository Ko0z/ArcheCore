#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include "Character/CharacterStructs.h"

using json = nlohmann::json;

inline void to_json(json& j, const WorldSpawnPos& p) {
	j = json{ {"X", p.X}, {"Y", p.Y}, {"Z", p.Z}, {"Roll", p.Roll}, {"Pitch", p.Pitch}, {"Yaw", p.Yaw} };
}

inline void from_json(const json& j, WorldSpawnPos& p) {
	j.at("X").get_to(p.X);
	j.at("Y").get_to(p.Y);
	j.at("Z").get_to(p.Z);
	j.at("Roll").get_to(p.Roll);
	j.at("Pitch").get_to(p.Pitch);
	j.at("Yaw").get_to(p.Yaw);
}

inline void to_json(json& j, const CharacterTemplateConfig& p) {
	j = json{
		{"Id", p.Id},
		{"Name", p.Name},
		{"Pos", p.WorldSpawnPos},
		{"NumInventorySlot", p.NumInventorySlot},
		{"NumBankSlot", p.NumBankSlot}
	};
}

inline void from_json(const json& j, CharacterTemplateConfig& p) {
	j.at("Id").get_to(p.Id);
	j.at("Name").get_to(p.Name);
	//p.WorldSpawnPos = j.get<WorldSpawnPos>();
	j.at("Pos").get_to(p.WorldSpawnPos);

	j.at("NumInventorySlot").get_to(p.NumInventorySlot);
	j.at("NumBankSlot").get_to(p.NumBankSlot);
}