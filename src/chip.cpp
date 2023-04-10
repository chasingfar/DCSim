
#include "chip.hpp"
#include <__ranges/zip_view.h>

namespace DCSim {

	Chip::state_t Chip::save() const {
		state_t state{pins.size()};
		std::ranges::transform(pins,state.begin(),[](const auto pin){
			return pin.first->get();
		});
		return state;
	}

	Chip::state_t Chip::is_need_update() const {
		state_t state;
		state.reserve(pins.size());
		for(auto [pin,is_input]:pins){
			auto v=pin->get();
			if(is_input&&!read(v)){
				return {};
			}
			state.push_back(v);
		}
		return state;
	}

	Component::comp_set Chip::get_affected(const Chip::state_t &before) {
		comp_set affected;
		
		for(auto [old,pin]:std::views::zip(before,pins|std::views::keys)){
			if(old!=pin->get()){
				affected.merge(pin->get_related());
			}
		}
		affected.erase(this);
		return affected;
	}

	Component::comp_set Chip::update() {
		auto before=is_need_update();
		if(before.empty()){
			input_floating=true;
			//if(log_state){ std::cout<<name<<"{"<<print(before)<<"}"<<std::endl; }
			return {};
		}
		try{
			//++run_count;
			run();
			input_floating=false;
		}catch(const std::bad_variant_access& e){
			//++err_count;
			input_floating=true;
			//if(log_state){ std::cout<<name<<"{"<<print(before)<<"}"<<std::endl; }
			//if(log_warning){ std::cerr<<"[Warning]"<<name<<"Read Floating"<<std::endl; }
			return {};
		}
		comp_set affected=get_affected(before);
		//if(log_change){
		//	std::cout<<name<<"{"<<print(before)<<"}";
		//	if(!affected.empty()){
		//		std::cout<<"=>{"<<print()<<"}";
		//	}
		//	std::cout<<std::endl;
		//}
		return affected;
	}
} // DCSim