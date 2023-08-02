#pragma once
#include "common_archecore.h"
#include "common_entity.h"
#include "Character.h"

namespace e {
	
	/// Entity components and systems about units and character entities

	///----------[ Unit specific ]-------------------->

	struct Unit {
		uint32 Id = 0;
		uint8 guid[3] = { 0x3d, 0x05, 0x00 };
		std::string Name;
		uint8 Level = 1;
		uint32 ModelId = 0;
		ModelPostureType ModelPosture;
		// Level

		// ZoneId
		// FactionId
		// FactionName
		// ExpeditionId		// Like an ID for a party(group) I think

	};

	struct HealthMana {
		int32 Hp = 1;
		int32 Mp = 1;
		int32 MaxHp = 1;
		int32 MaxMp = 1;
	};

	/// maybe dont need to store on each char? .. yes we do, because pirate is a thing in AA
	struct Faction {
		uint32 FactionId;
	};

	///----------[ Many ]-------------------->

	struct ExpComponent {
		int32 Experience = 0;
		int32 RecoverableExp = 0;
	};

	
	// Level

	// Health Mana

	// What data is also needed on Unit, Mob ?

	// Experience

	// Position

	// Race, Gender

	// Faction, Family

	// Labor, Vocation Points, Honor points

	// Type flag, Bake into entity id ??

	// Model custom params

	// Quests

	// Money1,	// player cash
	// Money2	// bank cash

	// -------------------------------->

	// Member variables AAEmu
	
	// bool _inParty;
	// bool _isOnline;
	// 
	// bool _isUnderWater;
	// bool _isMounted;

	/*
	* 
	override UnitTypeFlag TypeFlag { get; } = UnitTypeFlag.Character;
	static Dictionary<uint, uint> _usedCharacterObjIds = new Dictionary<uint, uint>();

	Dictionary<ushort, string> _options;

	List<IDisposable> Subscribers { get; set; }

	uint Id { get; set; } // moved to BaseUnit

	uint AccountId { get; set; }
	Race Race { get; set; }
	Gender Gender { get; set; }
	short LaborPower { get; set; }
	DateTime LaborPowerModified { get; set; }
	int ConsumedLaborPower { get; set; }
	AbilityType Ability1 { get; set; }
	AbilityType Ability2 { get; set; }
	AbilityType Ability3 { get; set; }
	DateTime LastCombatActivity { get; set; }
	DateTime LastCast { get; set; }
	bool IsInCombat { get; set; } // there's already an isInBattle
	bool IsInPostCast { get; set; }
	bool IgnoreSkillCooldowns { get; set; }
	string FactionName { get; set; }
	uint Family { get; set; }
	short DeadCount { get; set; }
	DateTime DeadTime { get; set; }
	int RezWaitDuration { get; set; }
	DateTime RezTime { get; set; }
	int RezPenaltyDuration { get; set; }
	DateTime LeaveTime { get; set; }
	long Money { get; set; }
	long Money2 { get; set; }
	int HonorPoint { get; set; }
	int VocationPoint { get; set; }
	short CrimePoint { get; set; }
	int CrimeRecord { get; set; }
	DateTime DeleteRequestTime { get; set; }
	DateTime TransferRequestTime { get; set; }
	DateTime DeleteTime { get; set; }
	long BmPoint { get; set; }
	bool AutoUseAAPoint { get; set; }
	int PrevPoint { get; set; }
	int Point { get; set; }
	int Gift { get; set; }
	int Expirience { get; set; }
	int RecoverableExp { get; set; }
	DateTime Created { get; set; } // character creation time
	DateTime Updated { get; set; } // change time

	uint ReturnDictrictId { get; set; }
	uint ResurrectionDictrictId { get; set; }

	override UnitCustomModelParams ModelParams { get; set; }
	override float Scale => 1f;
	override byte RaceGender => (byte)(16 * (byte)Gender + (byte)Race);

	CharacterVisualOptions VisualOptions { get; set; }

	const int MaxActionSlots = 85;
	ActionSlot[] Slots { get; set; }
	Inventory Inventory { get; set; }
	byte NumInventorySlots { get; set; }
	short NumBankSlots { get; set; }

	Item[] BuyBack { get; set; }

	ItemContainer BuyBackItems { get; set; }
	BondDoodad Bonding { get; set; }
	CharacterQuests Quests { get; set; }
	CharacterMails Mails { get; set; }
	CharacterAppellations Appellations { get; set; }
	CharacterAbilities Abilities { get; set; }
	CharacterPortals Portals { get; set; }
	CharacterFriends Friends { get; set; }
	CharacterBlocked Blocked { get; set; }
	CharacterMates Mates { get; set; }

	byte ExpandedExpert { get; set; }
	CharacterActability Actability { get; set; }

	CharacterSkills Skills { get; set; }
	CharacterCraft Craft { get; set; }
	uint SubZoneId { get; set; } // needed to be stored to make points, Memory Tome (Recall)
	int AccessLevel { get; set; }
	WorldSpawnPosition LocalPingPosition { get; set; } // added as a GM command helper
	ConcurrentDictionary<uint, DateTime> _hostilePlayers { get; set; }
	
	*/
}
