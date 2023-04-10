
#ifndef DCSIM_CIRCUIT_HPP
#define DCSIM_CIRCUIT_HPP

#include "chip.hpp"
#include <numeric>

namespace DCSim {

	struct Circuit:Component{
		inline static bool debug=false;
		std::vector<Component*> comps;
		explicit Circuit(std::string name=""):Component(std::move(name)){}

		comp_set update() override;
		std::vector<Chip*> get_chips();
		void run() override;
		[[nodiscard]] std::optional<std::string> is_floating() const override;

		template<typename ...Ts>
		requires ((std::is_base_of_v<Component,Ts>)&&...)
		void add_comps(Ts&... c){
			(comps.push_back(&c),...);
		}
	};

} // DCSim

#endif //DCSIM_CIRCUIT_HPP
