#pragma once

#include <xhash>

namespace Polarity
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t UUID);

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID; //? if needed increase to 128 bit
	};
}

namespace std
{
	template<>
	struct hash<Polarity::UUID> // adding uint64_t to std::hash
	{
		std::size_t operator()(const Polarity::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}