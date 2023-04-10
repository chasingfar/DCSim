
#ifndef DCSIM_COMPONENT_HPP
#define DCSIM_COMPONENT_HPP

#include "util/printable.hpp"
#include <unordered_set>
#include <iostream>

namespace DCSim {

	struct Component{
		std::string name{};
		explicit Component(std::string name=""):name(std::move(name)){}

		using comp_set=std::unordered_set<Component*>;
		virtual comp_set update()=0;
		virtual void run()=0;
		[[nodiscard]] virtual std::optional<std::string> is_floating() const=0;

		[[nodiscard]] virtual Util::Printable print() const{
			return [](std::ostream &os) {};
		}
		friend std::ostream& operator<<(std::ostream& os,const Component& comp){
			return os<<comp.name<<": "<<comp.print();
		}
	};
	inline Util::Printable print_name(const Component::comp_set& comps){
		return [=](std::ostream &os) {
			for(const auto& comp:comps){
				os<<comp->name<<" ";
			}
		};
	}
	inline std::ostream& operator<<(std::ostream& os,const Component::comp_set& comps){
		for(const auto& comp:comps){
			os<<*comp<<std::endl;
		}
		return os;
	}

} // DCSim

#endif //DCSIM_COMPONENT_HPP
