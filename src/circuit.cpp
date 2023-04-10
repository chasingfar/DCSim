
#include "circuit.hpp"

namespace DCSim {

	static Component::comp_set update_comps(const auto& comps){
		using comp_set = Component::comp_set;
		return std::reduce(comps.begin(),comps.end(),
			comp_set{},
			[](comp_set res,Component* c){
				res.merge(c->update());
				if(Circuit::debug) {
					std::cout<<*c<<std::endl;
				}
				return res;
			});
	}
	Component::comp_set Circuit::update() {
		return update_comps(get_chips());
	}

	std::vector<Chip *> Circuit::get_chips() {
		std::vector<Chip*> chips;
		for(auto* comp:comps){
			if(auto* chip=dynamic_cast<Chip*>(comp);chip){
				chips.emplace_back(chip);
			}else if(auto* circuit=dynamic_cast<Circuit*>(comp);circuit){
				auto vec=circuit->get_chips();
				chips.insert(chips.end(),vec.begin(),vec.end());
			}
		}
		return chips;
	}

	void Circuit::run() {
		if(Circuit::debug){
			std::cout<<"======"<<std::endl;
		}
		comp_set affected=update();
		while(!affected.empty()){
			if(Circuit::debug){
				std::cout<<"------"<<std::endl;
			}
			affected=update_comps(affected);
			/*affected=std::reduce(affected.begin(),affected.end(),
			                     comp_set{},
			                     [](comp_set res,Component* c){
				                     res.merge(c->update());
				                     return res;
			                     });*/
		}
	}

	std::optional<std::string> Circuit::is_floating() const {
		using res_t=std::optional<std::string>;
		return std::reduce(comps.begin(),comps.end(),res_t{},[](auto res,auto c){
			return c->is_floating().and_then([&](auto name)->res_t{
				return name+res.value_or("");
			}).or_else([&](){
				return res;
			});
		});
	}
} // DCSim