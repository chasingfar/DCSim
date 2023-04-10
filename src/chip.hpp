
#ifndef DCSIM_CHIP_HPP
#define DCSIM_CHIP_HPP

#include "port.hpp"
#include <algorithm>
#include <ranges>

namespace DCSim {
	struct Chip:Component{
		using state_t=std::vector<Level>;
		std::vector<std::pair<Wire*,bool>> pins;
		bool input_floating=false;

		explicit Chip(std::string name=""):Component(std::move(name)){}

		[[nodiscard]] std::optional<std::string> is_floating() const override{
			if(input_floating){
				return name;
			}
			return {};
		}
		template<bool is_input=false,size_t ...Sizes>
		void add_ports(Port<Sizes>&... ports){
			pins.reserve((pins.size()+...+Sizes));
			([&](auto& port){
				port.offset=pins.size();
				for(auto& w:port.pins){
					w.add_related(this);
					pins.emplace_back(&w,is_input);
				}
			}(ports),...);
		}
		template<size_t ...Sizes>
		void add_inputs(Port<Sizes>&... ports){add_ports<true>(ports...);}

		[[nodiscard]] state_t save() const;
		[[nodiscard]] state_t is_need_update() const;
		comp_set get_affected(const state_t& before);
		comp_set update() override;

		[[nodiscard]] virtual Util::Printable print(std::span<const Level> state) const{
			return [](std::ostream& os){};
		}
		[[nodiscard]] Util::Printable print() const override{
			return [&](std::ostream& os){
				os<<print(save());
			};
		}
	};
	/*struct Chip:Component{
		inline static bool log_warning=false;
		inline static bool log_state=false;
		inline static bool log_change=false;
		inline static size_t run_count=0;
		inline static size_t err_count=0;
		static Util::Printer print_count(){ return [](std::ostream& os){os<<"run:"<<run_count<<",err:"<<err_count;};}

		using state_t=std::vector<Level>;
		std::vector<std::pair<Mode,Wire*>> pins;
		std::vector<std::pair<std::string,size_t>> port_names;
		bool input_floating=false;

		explicit Chip(std::string name=""):Component(std::move(name)){}

		virtual Util::Printer print(std::span<const Level> state) const{
			return [](std::ostream& os){};
		}
		Util::Printer print() const override{
			return [&](std::ostream& os){
				os<<print(save());
			};
		}
		void print_port_names(std::ostream& os1,std::ostream& os2){
			std::stringstream ss;
			for(auto [name,size]:port_names){
				ss<<std::setw(std::max(name.size(),size))<<name<<"|";
			}
			auto str=ss.str();
			os1<<std::setw(std::max(str.size(),name.size()+1))<<name+"|";
			os2<<std::setw(std::max(str.size(),name.size()+1))<<str;
		}
		void print_port_status(std::ostream& os){
			auto it=pins.begin();
			std::stringstream ss;
			for(auto [name,size]:port_names){
				std::string str="";
				for(size_t i=0;i<size;++i){
					Level level=it->second->get();
					if(level==Level::Floating){
						str="F"+str;
					}else if(level==Level::Error){
						str="E"+str;
					}else  if(static_cast<int8_t>(level)>0){
						str="1"+str;
					}else {
						str="0"+str;
					}
					++it;
				}
				ss<<std::setw(std::max(name.size(),size))<<str<<"|";
			}
			auto str=ss.str();
			os<<std::setw(std::max(str.size(),name.size()+1))<<str;
		}
	};*/

} // DCSim

#endif //DCSIM_CHIP_HPP
