
#include "include/dcsim/dcsim.hpp"
#include <iostream>

using namespace DCSim;
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

int main() {
	Accumulator<8> cnt;
	for(int i=0;i<10;i++){
		cnt.update();
		std::cout<<i/2<<" "<<cnt<<std::endl;
		cnt.clk.clock();
	}

	return 0;
}
