#include <doctest/doctest.h>
#include "../src/port.hpp"
using namespace DCSim;

TEST_CASE("PortLinkage") {
	Port<2> a,b;
	{
		Port<2> tmp;
		a>>tmp;
		tmp>>b;
	}
	a=2;
	
	CHECK(b.value()== 2);
}
TEST_CASE("PortJoin") {
	Port<16> a{0x1234};
	Port<8> b,c;//,d{0x56};
	a>>(b|c);
	CHECK(b.value()==0x12);
	CHECK(c.value()==0x34);
}
TEST_CASE("PortReverse") {
	Port<4> a{0b1101},b;
	a.reverse()>>b;
	
	CHECK(b.value()==0b1011);
}
