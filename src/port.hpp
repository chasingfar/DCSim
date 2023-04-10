
#ifndef DCSIM_PORT_HPP
#define DCSIM_PORT_HPP

#include "wire.hpp"
#include <array>
#include <span>
#include <ranges>

namespace DCSim {

	using val_t=unsigned long long;

	template<typename T>
	concept LinkablePort=std::is_convertible_v<std::remove_cvref_t<decltype(std::declval<T>().pins[0])>,Wire>|| T::size==0;
	template<size_t S,typename T>
	concept SizedPort=LinkablePort<T>&&(std::remove_cvref_t<T>::size==S);

	template<size_t Size,typename Pins=std::array<Wire,Size> >
	struct Port{
		static constexpr size_t size=Size;
		Pins pins{};
		size_t offset=0;

		Port()=default;
		explicit Port(Pins&& pins,size_t offset=0):pins{pins},offset{offset}{}
		explicit Port(val_t val){set(val);}
		explicit Port(Level level){set(0,level);}

		void set(val_t val,Level zero=Level::Low,Level one=Level::High){
			for(auto& p:pins){
				p=(val&1u)==1u?one:zero;
				val>>=1;
			}
		}
		template<typename T=val_t>
		[[nodiscard]] std::expected<T,Level> get() const{
			val_t val=0;
			for(auto& p:pins){
				if(auto v=read(p);v){
					val|=*v;
					val=(val >> 1u) | ((val&1u) << (pins.size() - 1u));
				}else{
					return std::unexpected{v.error()};
				}
			}
			return static_cast<T>(val);
		}
		template<typename T=val_t>
		[[nodiscard]] T value() const{
			return get<T>().value();
		}
		auto& operator =(val_t val){
			set(val);
			return *this;
		}
		auto& operator =(Level level){
			set(0,level);
			return *this;
		}

		[[nodiscard]] Util::Printable print_ptr() const{
			return [&](std::ostream& os){
				for(const auto& w:pins){
					os<<&w<<" ";
				}
			};
		}
		[[nodiscard]] Util::Printable print_bit() const{
			return [&](std::ostream& os){
				for (auto it = pins.rbegin(); it != pins.rend(); ++it) {
					if(auto v=read(*it);v) {
						os << (*v?"1":"0");
					}else{
						if(v.error()==Level::Error){
							
						}
						os << (v.error()==Level::Floating?"F":"E");
					}
				}
			};
		}
		[[nodiscard]] Util::Printable print_val() const{
			return [&](std::ostream& os){
				if(auto v=get();v) {
					os << *v;
				}else{
					os << print_bit();
				}
				return os;
			};
		}
		friend std::ostream& operator<<(std::ostream& os,const Port<Size,Pins>& port){
			return os<<port.print_bit();
		}

		auto operator()(std::span<const Level> state) const{
			return Port<Size,std::span<const Level>>{state.subspan(offset,Size)};
		}

		template<size_t NewSize=Size,size_t Start=0>
		[[nodiscard]] auto sub(size_t start=Start) {
			using SPAN=std::span<std::remove_reference_t<decltype(pins[0])>,NewSize>;
			return Port<NewSize,SPAN>{SPAN{&pins[start],NewSize},offset+start};
		}
		template<size_t NewSize=Size,size_t Start=0>
		[[nodiscard]] auto sub(size_t start=Start) const{
			using SPAN=std::span<std::remove_reference_t<decltype(pins[0])>,NewSize>;
			return Port<NewSize,SPAN>{SPAN{&pins[start],NewSize},offset+start};
		}
		template<LinkablePort T> requires (std::remove_cvref_t<T>::size>=Size)
		auto&& operator >>(T&& port) {
			[&]<size_t ...I>(std::index_sequence<I...>){
				((pins[I]>>port.pins[I]),...);
			}(std::make_index_sequence<Size>{});
			return port;
		}

		auto link(LinkablePort auto&&... ports) requires (sizeof...(ports)>0) {
			return (*this>>...>>ports);
		}

		template<LinkablePort R>
		auto operator |(R&& rhs){
			constexpr size_t Rs=std::remove_cvref_t<R>::size;
			Port<Size+Rs> res;
			rhs>>res.template sub<Rs>(0);
			*this>>res.template sub<Size>(Rs);
			return res;
		}
		auto reverse(){
			Port<Size> res;
			for(auto [o,n]:std::views::zip(pins|std::views::reverse,res.pins)){
				o>>n;
			}
			return res;
		}
	};
	struct Enable:Port<1>{
		using Port<1>::Port;
		using Port<1>::operator=;
		enum En{Yes=0,No=1};
		bool is_enable(){
			return value()==Yes;
		}
		void  enable(){set(Yes);}
		void disable(){set(No);}
	};
	struct Clock:Port<1>{
		using Port<1>::Port;
		using Port<1>::operator=;
		void clock(){
			set(~value());
		}
	};

} // DCSim

#endif //DCSIM_PORT_HPP
