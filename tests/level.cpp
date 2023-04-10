#include <doctest/doctest.h>
#include <array>
#include <ranges>
#include "../src/level.hpp"
using namespace DCSim;
using namespace std::ranges;

TEST_CASE("LevelPlus") {
	Level L = Level::Low      ,H = Level::High,
	      D = Level::PullDown ,U = Level::PullUp,
	      Z = Level::Floating ,E = Level::Error;
	std::array lvl{L,H,D,U,Z,E};
	Level res[][lvl.size()]={
		//    L,H,D,U,Z,E
		/*L*/{L,E,L,L,L,E},
		/*H*/{E,H,H,H,H,E},
		/*D*/{L,H,D,E,D,E},
		/*U*/{L,H,E,U,U,E},
		/*Z*/{L,H,D,U,Z,E},
		/*E*/{E,E,E,E,E,E},
	};
	for (auto o : iota_view{0uz, lvl.size()}){
		for (auto n : iota_view{0uz, lvl.size()}){
			CHECK(lvl[o]+lvl[n]==res[o][n]);
		}
	}
}
