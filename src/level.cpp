#include "level.hpp"

namespace DCSim {

	Level &operator+=(Level &o, const Level n) {
		/*
o\n	L	H	D	U	Z	E
L	L1	E3	L4	L4	L4	E2
H	E3	H1	H4	H4	H4	E2
D	L2	H2	D1	E3	D4	E2
U	L2	H2	E3	U1	U4	E2
Z	L2	H2	D2	U2	Z1	E2
E	E4	E4	E4	E4	E4	E1
		*/
		if(n==o){return o;}//1
		int cmp=std::abs(static_cast<int8_t>(n))-std::abs(static_cast<int8_t>(o));
		if(cmp>0){//2
			return o=n;
		}else if(cmp==0){//3
			return o=Level::Error;
		}
		return o;//4
	}

	std::expected<uint8_t, Level> read(const Level level) {
		if(level==Level::Floating || level==Level::Error){
			return std::unexpected{level};
		}
		return static_cast<int8_t>(level) > 0 ? 1 : 0;
	}

} // DCSim