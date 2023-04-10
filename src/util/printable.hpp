
#ifndef DCSIM_PRINTABLE_HPP
#define DCSIM_PRINTABLE_HPP

#include <functional>

namespace Util {

	struct Printable{
		std::function<void(std::ostream&)> fn;
		Printable(auto fn):fn(std::move(fn)){}
		friend std::ostream& operator<<(std::ostream& os,const Printable& printer){
			printer.fn(os);
			return os;
		}
	};

} // Util

#endif //DCSIM_PRINTABLE_HPP
