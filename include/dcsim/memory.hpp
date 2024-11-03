
#ifndef DCSIM_MEMORY_HPP
#define DCSIM_MEMORY_HPP

#include "core.hpp"

namespace DCSim {

	// Basic D Flip-Flop
	template<size_t Size,bool UpTrigger=true>
	struct DFF:Chip{
		Clock clk;
		Port<Size> D,Q{0};
		val_t data{0};
		explicit DFF(std::string name=""):Chip(std::move(name)){
			add_inputs(clk, D);
			add_ports(Q);
		}
		void run() override {
			if(clk.value() == UpTrigger){
				save(data);
			}else{
				load(D.value());
			}
		}
		virtual void load(val_t val){
			data=val;
		}
		virtual void save(val_t val){
			Q=val;
		}
		void set(val_t val){
			load(val);
			save(val);
		}
		void reset() {
			set(0);
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os << D(s) << "=>" << data << "=>" << Q(s) << "(clk=" << clk(s) << ")";
			};
		}
	};

	// Basic D Flip-Flop with Q_
	template<size_t Size,bool UpTrigger=true>
	struct DFF_:DFF<Size,UpTrigger>{
		using Base=DFF<Size,UpTrigger>;
		Port<Size> Q_{-1};
		explicit DFF_(std::string name=""):Base(std::move(name)){
			Base::add_ports(Q_);
		}
		void save(val_t val) override{
			Base::save(val);
			Q_=~val;
		}
	};

	// Basic D Flip-Flop with Q_ ,Preset and Clear
	template<size_t Size,bool UpTrigger=true>
	struct DFF_PRCLR:DFF<Size,UpTrigger>{
		using Base=DFF<Size,UpTrigger>;
		Enable pr,clr;
		explicit DFF_PRCLR(std::string name=""):Base(std::move(name)){
			Base::add_inputs(pr,clr);
		}
		void run() override {
			if(clr.is_enable()){
				Base::reset();
			} else if(pr.is_enable()){
				Base::set(-1);
			}else{
				Base::run();
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<Base::print(s)<<"(pr="<<pr(s)<<",clr="<<clr(s)<<")";
			};
		}
	};

	// IC 74377
	template<size_t Size,bool UpTrigger=true>
	struct RegCE: DFF<Size,UpTrigger>{
		using Base=DFF<Size,UpTrigger>;
		Enable ce;
		explicit RegCE(std::string name=""): Base(std::move(name)){
			Base::add_inputs(ce);
		}
		void run() override {
			Base::run();
			if(!ce.is_enable()){
				Base::load(Base::Q.value());
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os << Base::print(s) << "(ce=" << ce(s) << ")";
			};
		}
	};

	// IC 74273
	template<size_t Size,bool UpTrigger=true>
	struct RegCLR:DFF<Size,UpTrigger>{
		using Base=DFF<Size,UpTrigger>;
		Enable clr;
		explicit RegCLR(std::string name=""):Base(std::move(name)){
			Base::add_inputs(clr);
		}
		void run() override {
			if(clr.is_enable()){
				Base::reset();
			} else {
				Base::run();
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<Base::print(s)<<"(clr="<<clr(s)<<")";
			};
		}
	};

	template<size_t Size,bool UpTrigger=true>
	struct RegCECLR:DFF<Size,UpTrigger>{
		using Base=DFF<Size,UpTrigger>;
		Enable ce1,ce2,clr;
		explicit RegCECLR(std::string name=""):Base(std::move(name)){
			Base::add_inputs(ce1,ce2,clr);
		}
		void run() override {
			if(clr.is_enable()){
				Base::reset();
			} else {
				Base::run();
				if(ce1.is_enable()&&ce2.is_enable()){
					return;
				}
				Base::data=Base::Q.value();
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<Base::print(s)
				<<"(ce1=" << ce1(s)
				<<",ce2=" << ce2(s)
				<<",clr=" << clr(s)
				<<")";
			};
		}
	};

	// Base on AS6C4008
	template<size_t ASize=19,size_t DSize=8,typename addr_t=size_t,typename data_t=val_t>
	struct RAM:Chip{
		static constexpr size_t data_size=1<<ASize;
		Enable ce,oe,we;
		Port<ASize> A;
		Port<DSize> D;
		std::array<data_t,data_size> data{};//[data_size]{0};

		explicit RAM(std::string name=""):Chip(std::move(name)){
			add_inputs(ce,oe,we,A);
			add_ports(D);
		}
		virtual void do_write(){
			data[A.value()]=D.value();
		}
		virtual void do_read(){
			D=data[A.value()];
		}
		void run() override {
			D=Level::Floating;
			if(ce.is_enable()){
				if(we.is_enable()){
					do_write();
				} else if(oe.is_enable()) {
					do_read();
				}
			}
		}
		[[nodiscard]] Util::Printable print(std::span<const Level> s) const override{
			return [=,this](std::ostream& os){
				os<<"data["<<A(s)<<"]="<<D(s)<<"(cow="<<ce(s)<<oe(s)<<we(s)<<")";
			};
		}
		void load(const std::vector<data_t>& new_data,addr_t start=0){
			std::copy_n(new_data.begin(), std::min(new_data.size(),data_size-start), &data[start]);
			run();
		}
		auto begin(){return data.begin();}
		auto   end(){return data.end();}
	};

	// Base on SST39SF040
	template<size_t ASize=19,size_t DSize=8,typename addr_t=size_t,typename data_t=val_t>
	struct ROM:RAM<ASize,DSize,addr_t,data_t>{
		using Base=RAM<ASize,DSize,addr_t,data_t>;
		explicit ROM(std::string name=""):Base(std::move(name)){}
		void do_write() override{
			std::cerr<<"[Warning]"<<this->name<<"Try write to ROM"<<std::endl;
		}
	};
} // DCSim
#endif //DCSIM_MEMORY_HPP
