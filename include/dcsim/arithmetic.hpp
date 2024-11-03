
#ifndef DCSIM_ARITHMETIC_HPP
#define DCSIM_ARITHMETIC_HPP

#include "core.hpp"

namespace DCSim {

	// Base on IC 74682
	template<size_t Size=8>
	struct Cmp:Chip{
		Port<Size> P,Q;
		Port<1> PgtQ{1},PeqQ{1};

		explicit Cmp(std::string name=""):Chip(std::move(name)){
			add_inputs(P,Q);
			add_ports(PgtQ,PeqQ);
		}
		void run() override{
			PgtQ = !( P.value() >  Q.value() );
			PeqQ = !( P.value() == Q.value() );
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"Cmp"<<P(s)<<"<=>"<<Q(s)<<"(P>Q:"<<PgtQ(s)<<",P==Q:"<<PeqQ(s)<<")";
			};
		}
	};

	// Base on IC 74688
	template<size_t Size=8>
	struct Eq:Chip{
		Port<Size> P,Q;
		Enable oe;
		Port<1> PeqQ;

		explicit Eq(std::string name=""):Chip(std::move(name)){
			add_inputs(P,Q,oe);
			add_ports(PeqQ);
		}
		void run() override{
			PeqQ=!(oe.is_enable()&&P.value()==Q.value());
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"Eq"<<P(s)<<(PeqQ(s).get().value_or(0)==1?"!=":"==")<<Q(s)<<"(oe:"<<oe(s)<<")";
			};
		}
	};

	template<size_t Size>
	struct Adder:Chip{
		Port<Size> A,B,O{0};
		explicit Adder(std::string name=""):Chip(std::move(name)){
			add_inputs(A,B);
			add_ports(O);
		}
		void run() override{
			O=A.value()+B.value();
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<A(s)<<"+"<<B(s)<<"="<<O(s);
			};
		}
	};
} // DCSim
#endif //DCSIM_ARITHMETIC_HPP
