#include <doctest/doctest.h>
#include <array>
#include <ranges>
#include "../src/wire.hpp"
using namespace DCSim;
using namespace std::ranges;

TEST_CASE("WirePlus") {
	Level L = Level::Low      ,H = Level::High,
	      D = Level::PullDown ,U = Level::PullUp,
	      Z = Level::Floating ,E = Level::Error;
	std::array lvl{L,H,D,U,Z,E};
	
	Wire a,b;
	a>>b;

	for (size_t o : iota_view{0uz, lvl.size()}){
		a=lvl[o];
		for (size_t n : iota_view{0uz, lvl.size()}){
			b=lvl[n];
			CHECK(lvl[o]+lvl[n]==b.get());
		}
	}

}
TEST_CASE("WireLinkage") {
	Wire a,b,c;

	a=Level::High;
	c=Level::PullDown;

	CHECK(a.get()!=b.get());
	a>>b;
	CHECK(a.get()==b.get());

	{
		Wire d;
		c>>d;
		CHECK(c.get()==d.get());
		CHECK(a.get()!=c.get());
		b>>d;
		a=Level::Low;
		CHECK(a.get()==c.get());
	}
	a=Level::PullUp;
	CHECK(a.get()==c.get());
}
