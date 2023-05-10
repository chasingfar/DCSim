# Digital Circuit Simulator

A simple simulator for digital locgic circuit.
Developed for verificate and auto-test circuits in SOCP.
Support basic three-state logic and nest circuit.

# Example

```cpp
#include <dcsim/dcsim.hpp>
#include <iostream>

using namespace DCSim;

//define a new chip
template<size_t Size>
struct Adder:Chip{
	//define ports
	Port<Size> A,B,O{0};
	explicit Adder(std::string name=""):Chip(std::move(name)){
        //add ports to chip
		add_inputs(A,B);//for input only ports.
		                //run() won't be called if any input is floating
		add_ports(O);   //for output or bidirectional ports
	}
	void run() override{
		//Port.get() return std::expected<unsigned long long,Level>
		//Port.value() is shortcut for Port.get().value()
		O=A.value()+B.value();
	}
};
//define a new circuit
template<size_t Size>
struct Accumulator:Circuit{
	//Level have 5 states: High,Low,Floating,PullUp,PullDown
	Clock clk{Level::PullDown};
	Port<1> clr{Level::PullUp};
	//define used chips or circuits
	Adder<Size> adder{name+"[Adder]"};
	RegCLR<Size> reg{name+"[Reg]"};

	explicit Accumulator(std::string name=""):Circuit(std::move(name)){
		//add chips or circuits to Circuit
		add_comps(adder,reg);
		//use `>>` to link ports together,
		clk>>reg.clk;
		clr>>reg.clr;
		adder.O>>reg.D;
		adder.A>>reg.Q;
		adder.B=1;
	}
};

int main() {
	Accumulator<8> cnt;
	for(int i=0;i<10;i++){
		cnt.update();//`update()` will call `run()` repeatedly until no state change
		std::cout<<i<<" "<<cnt.reg.Q.value()<<std::endl;
		cnt.clk.clock();// toggle clock value
	}

	return 0;
}
```
