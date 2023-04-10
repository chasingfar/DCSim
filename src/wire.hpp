
#ifndef DCSIM_WIRE_HPP
#define DCSIM_WIRE_HPP

#include "component.hpp"
#include "level.hpp"
#include <numeric>
#include "util/circular_list.hpp"

namespace DCSim {
	class Wire:public Util::CircularList<Wire>{
		using Base=Util::CircularList<Wire>;
		struct WireState{
			Level level=Level::Floating;
			std::unordered_map<Wire*,Level> sources{};
			Component::comp_set related{};
			WireState& operator +=(WireState& state){
				level+=state.level;
				related.merge(state.related);
				return *this;
			}
		};
		std::shared_ptr<WireState> state{new WireState};
	public:
		//Wire():state{new WireState}{}
		//Wire():out{val},in{new Level{val}}{}
		[[nodiscard]] Level get() const{
			return state->level;
		}
		void add_related(Component* comp) const{
			state->related.insert(comp);
		}
		[[nodiscard]] Component::comp_set get_related() const{
			return state->related;
		}
		void set(Level new_level){
			state->sources[this]=new_level;
			state->level=std::reduce(state->sources.begin(),state->sources.end(),
			                     Level::Floating,
			                     [](Level o,auto n){
				                     return o+n.second;
			                     });
		}
		Wire& operator=(Level new_level){
			set(new_level);
			return *this;
		}
		Wire& operator>>(Wire& wire){
			*state+=*wire.state;
			wire.each([&](Wire* w){
				w->state=state;
			});
			Base::link(&wire);
			return wire;
		}
	};

	inline std::expected<uint8_t,Level> read(const Wire& wire){
		return read(wire.get());
	}

} // DCSim

#endif //DCSIM_WIRE_HPP
