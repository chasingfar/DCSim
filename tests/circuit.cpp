#include <doctest/doctest.h>
#include <array>
#include <ranges>
#include "../include/dcsim/dcsim.hpp"
using namespace DCSim;
using namespace std::ranges;
template<size_t Size>
struct Accumulator:Circuit{
	Clock clk{Level::PullDown};
	Port<1> clr{Level::PullUp};

	Adder<Size> adder{name+"[Adder]"};
	RegCLR<Size> reg{name+"[Reg]"};

	explicit Accumulator(std::string name=""):Circuit(std::move(name)){
		add_comps(adder,reg);

		clk>>reg.clk;
		clr>>reg.clr;
		adder.O>>reg.D;
		adder.A>>reg.Q;
		adder.B=1;
	}
	[[nodiscard]] Util::Printable print() const override{
		return [&](std::ostream& os){
			os<<"adder="<<adder<<"reg="<<reg;
		};
	}
};

TEST_CASE("CircuitAccumulator") {
	
	Accumulator<8> cnt;

	for (int Q : iota_view{0, 10}){
		cnt.update();
		CHECK(Q==cnt.reg.Q.value());
		CHECK(Q+1==cnt.adder.O.value());
		cnt.clk.clock();
		cnt.update();
		CHECK(Q+1==cnt.reg.Q.value());
		CHECK(Q+1==cnt.adder.O.value());
		cnt.clk.clock();
	}

}
