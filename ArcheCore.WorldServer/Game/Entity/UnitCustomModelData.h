#pragma once
#include "packet.h"

namespace e {

enum class UnitCustomModelType : uint8_t
{
	None = 0,
	Hair = 1,
	Skin = 2,
	Face = 3
};

// Not meant to be stand-alone component
struct FixedDecalAsset {
	
	void Read(packet& p)
	{
		p >> m_AssetId >> m_AssetWeight;
		//p >> m_AssetWeight;	// Maybe not read correctly?
	}

	void Write(packet& p)
	{
		p << m_AssetId << m_AssetWeight;	// Asset Weight not set like AAEmu
	}

	uint32_t m_AssetId = 0;
	float m_AssetWeight = 0;
};

// Not meant to be stand-alone component
struct FaceModel {
	FaceModel () {
		m_Modifier = std::vector<uint8_t>(128, 0);
		m_FixedDecalAsset = std::vector<FixedDecalAsset>(4, FixedDecalAsset{});
	}

	void Read(packet& p)
	{
		p >> m_MoveableDecalAssetId	>> m_MovableDecalWeight	>> m_MovableDecalScale	>> m_MovableDecalRotate
			>> m_MovableDecalMoveX	>> m_MovableDecalMoveY;

		for (auto elem : m_FixedDecalAsset) {
			elem.Read(p);
		}

		p >> m_DiffuseMapId >> m_NormalMapId >> m_EyelashMapId >> m_NormalMapWeight >> m_LipColor 
			>> m_LeftPupilColor >> m_RightPupilColor >> m_EyebrowColor >> m_DecoColor >> m_Modifier;
	}

	void Write(packet& p) const
	{
		p << m_MoveableDecalAssetId << m_MovableDecalWeight << m_MovableDecalScale << m_MovableDecalRotate
			<< m_MovableDecalMoveX << m_MovableDecalMoveY;

		for (auto elem : m_FixedDecalAsset) {
			elem.Write(p);
		}

		p << m_DiffuseMapId << m_NormalMapId << m_EyelashMapId << m_NormalMapWeight << m_LipColor
			<< m_LeftPupilColor << m_RightPupilColor << m_EyebrowColor << m_DecoColor << m_Modifier;

		//return p;
	}

	uint32_t m_MoveableDecalAssetId;
	float m_MovableDecalWeight;
	float m_MovableDecalScale;
	float m_MovableDecalRotate;
	uint16_t m_MovableDecalMoveX;
	uint16_t m_MovableDecalMoveY;
	//FixedDecalAsset m_FixedDecalAsset[4];
	std::vector<FixedDecalAsset> m_FixedDecalAsset;
	uint32_t m_DiffuseMapId;
	uint32_t m_NormalMapId;
	uint32_t m_EyelashMapId;
	float m_NormalMapWeight;
	uint32_t m_LipColor;
	uint32_t m_LeftPupilColor;
	uint32_t m_RightPupilColor;
	uint32_t m_EyebrowColor;
	uint32_t m_DecoColor;
	//uint8_t m_Modifier[MODIFIER_SIZE];
	std::vector<uint8_t> m_Modifier;
};

struct UnitCustomModelData {

	void Read(packet& p)
	{
		SetType((UnitCustomModelType)p.read<uint8_t>());

		if (Type == UnitCustomModelType::None)
			return;

		p >> HairColorId;

		if (Type == UnitCustomModelType::Hair)
			return;

		p >> SkinColorId
			>> ModelId;

		if (Type == UnitCustomModelType::Skin)
			return;

		FaceModel.Read(p);
	}

	void Write(packet& p) const
	{
		//std::cout << "Writing UnitCustomModelsParams..." << std::endl;
		p << (uint8_t)Type;

		if (Type == UnitCustomModelType::None)
			return;

		p << HairColorId;

		if (Type == UnitCustomModelType::Hair)
			return;

		p << SkinColorId
			<< ModelId;

		if (Type == UnitCustomModelType::Skin)
			return;

		FaceModel.Write(p);
	}

	uint32_t GetId() const { return Id; }
	uint32_t GetHairColorId() const { return HairColorId; }
	uint32_t GetSkinColorId() const { return SkinColorId; }
	uint32_t GetModelId() const { return ModelId; }
	const FaceModel& GetFace() const { return FaceModel; }

	UnitCustomModelData& SetId(uint8_t id)
	{
		Id = id;
		return *this;
	}

	UnitCustomModelData& SetType(UnitCustomModelType type)
	{
		Type = type;
		//if (Type == UnitCustomModelType::Face)
		//{
		//	FaceModel = FaceModel();
		//}
		return *this;
	}

	UnitCustomModelData& SetModelId(uint8_t modelId)
	{
		ModelId = modelId;
		return *this;
	}

	UnitCustomModelData& SetHairColorId(uint8_t hairColorId)
	{
		HairColorId = hairColorId;
		return *this;
	}

	UnitCustomModelData& SetSkinColorId(uint8_t skinColorId)
	{
		SkinColorId = skinColorId;
		return *this;
	}

	uint32_t Id = 0;
	uint32_t HairColorId = 0;
	uint32_t SkinColorId = 0;
	uint32_t ModelId = 0;
	FaceModel FaceModel;
	UnitCustomModelType Type = UnitCustomModelType::None;
};

}