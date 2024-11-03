
#ifndef DCSIM_GATES_HPP
#define DCSIM_GATES_HPP

#include "core.hpp"

namespace DCSim {

	// IC 7400
	template<size_t Size>
	struct NAND:Chip{
		Port<Size> A,B,Y{0};
		explicit NAND(std::string name=""):Chip(std::move(name)){
			add_inputs(A,B);
			add_ports(Y);
		}
		void run() override{
			Y=~(A.value()&B.value());
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"A("<<A(s)<<") NAND B("<<B(s)<<") = "<<Y(s);
			};
		}
	};
	// IC 7404
	template<size_t Size>
	struct NOT:Chip{
		Port<Size> A,Y{0};
		explicit NOT(std::string name=""):Chip(std::move(name)){
			add_inputs(A);
			add_ports(Y);
		}
		void run() override{
			Y=~(A.value());
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"NOT A("<<A(s)<<") = "<<Y(s);
			};
		}
	};
	// IC 7408
	template<size_t Size>
	struct AND:Chip{
		Port<Size> A,B,Y{0};
		explicit AND(std::string name=""):Chip(std::move(name)){
			add_inputs(A,B);
			add_ports(Y);
		}
		void run() override{
			Y=(A.value()&B.value());
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"A("<<A(s)<<") AND B("<<B(s)<<") = "<<Y(s);
			};
		}
	};
	// IC 7432
	template<size_t Size>
	struct OR:Chip{
		Port<Size> A,B,Y{0};
		explicit OR(std::string name=""):Chip(std::move(name)){
			add_inputs(A,B);
			add_ports(Y);
		}
		void run() override{
			Y=(A.value()|B.value());
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"A("<<A(s)<<") OR B("<<B(s)<<") = "<<Y(s);
			};
		}
	};

	// Base on IC 74244
	template<size_t Size=8>
	struct Buffer:Chip{
		Enable oe;
		Port<Size> I,O;
		explicit Buffer(std::string name=""):Chip(std::move(name)){
			add_inputs(oe);
			add_ports(I,O);
		}
		void run() override {
			O=Level::Floating;
			if(oe.is_enable()){
				if(auto v=I.get();v){
					O=*v;
				}
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"I("<<I(s)<<")->O("<<O(s)<<") (oe="<<oe(s)<<")";
			};
		}
	};
	// Base on IC 74245
	template<size_t Size=8>
	struct Transceiver:Chip{
		Enable oe;
		Port<1> dir;
		Port<Size> A,B;
		enum Dir{BtoA=0,AtoB=1};
		explicit Transceiver(std::string name=""):Chip(std::move(name)){
			add_inputs(oe,dir);
			add_ports(A,B);
		}
		void run() override {
			A=Level::Floating;
			B=Level::Floating;
			if(oe.is_enable()){
				if(dir.value()==AtoB){
					if(auto v=A.get();v){
						B=*v;
					}
				}else{
					if(auto v=B.get();v){
						A=*v;
					}
				}
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"A("<<A(s)<<")"<<(dir(s).get().value_or(0)==1?"->":"<-")<<"B("<<B(s)<<") (oe="<<oe(s)<<")";
			};
		}
	};

} // DCSim
#endif //DCSIM_GATES_HPP
