
#ifndef DCSIM_LEVEL_HPP
#define DCSIM_LEVEL_HPP

#include <cstdint>
#include <cmath>
#include <expected>
namespace DCSim{
	enum struct Level:int8_t{
		Low      = -2,
		PullDown = -1,
		Floating =  0,
		PullUp   =  1,
		High     =  2,
		Error    = INT8_MAX,
	};

	Level& operator+=(Level& o,Level n);

	inline Level operator+(const Level o,const Level n){
		Level t=o;
		return t+=n;
	}

	std::expected<uint8_t,Level> read(Level level);
}
#endif //DCSIM_LEVEL_HPP
