#pragma once
#include <cstdint>
#include <bitset>
#include <queue>
#include <array>

namespace Polarity::ECS
{
	using Entity = uint32_t;
	const Entity MAX_ENTITIES = 50000;
	const Entity INVALID_ENTITY = -1;

	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32;

	using Signature = std::bitset<MAX_COMPONENTS>;

	static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

	struct EntityMeta {
		bool enabled;
		uint32_t layerMask;
		uint8_t mobility;   // Static/Dynamic/Kinematic
		uint8_t flags;      // bitmask (not in use atm)
	};
}