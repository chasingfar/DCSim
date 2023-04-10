
#ifndef DCSIM_CIRCULAR_LIST_HPP
#define DCSIM_CIRCULAR_LIST_HPP

#include <utility>
#include <iostream>
#include <functional>

namespace Util {
	template<typename T>
	struct CircularList {
		T *prev = self();
		T *next = self();

		CircularList(CircularList& list){
			link<false>(&list);
		}
		CircularList(CircularList&& list){
			link<false>(&list);
		}
		~CircularList() {
			remove();
		}

		auto self() const { return static_cast<const T *>(this); }

		auto self() { return static_cast<T *>(this); }

		auto each(auto &&fn) const {
			return loop(self(), std::forward<decltype(fn)>(fn));
		}

		auto each(auto &&fn) {
			return loop(self(), std::forward<decltype(fn)>(fn));
		}

		bool has(const CircularList *node) const {
			return each([node](const T *cur) {
				return cur == node;
			}) != nullptr;
		}

		[[nodiscard]] bool check() const {
			return each([](const T *cur) {
				return cur->next == nullptr;
			}) != nullptr;
		}

		template<bool check_if_exist=true>
		CircularList &link(CircularList *list) {
			if constexpr(check_if_exist){
				if (has(list)) {return *list;}
			}
			std::swap(next, list->next);
			std::swap(next->prev, list->next->prev);
			return *list;
		}

		T &operator>>(T &list) {
			return link(&list);
		}

		std::ostream &print_list(std::ostream &os) {
			each([&](const T *cur) {
				os << *cur << "->";
			});
			return os << *self();
		}
		void remove(){
			prev->next=next;
			next->prev=prev;
		}
	private:
		//CRTP guard from https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
		CircularList() = default;
		friend T;

		//loop over list with fn:(T*)->void
		template<typename Node, typename Fn>
		requires std::same_as<std::decay_t<Node>, T> && std::same_as<void, std::invoke_result_t<Fn, T *>>
		static void loop(Node *head, Fn &&fn) {
			Node *cur = head;
			do {
				fn(cur);
				cur = cur->next;
			} while (cur != head && cur != nullptr);
		}

		//loop list find node:T* make fn:(T*)->bool return true, or return nullptr if not found.
		template<typename Node, typename Fn>
		requires std::same_as<std::decay_t<Node>, T> && std::same_as<bool, std::invoke_result_t<Fn, T *>>
		static Node *loop(Node *head, Fn &&fn) {
			Node *cur = head;
			do {
				if (fn(cur)) {
					return cur;
				}
				cur = cur->next;
			} while (cur != head && cur != nullptr);

			return nullptr;
		}
	};
} // Util
#endif //DCSIM_CIRCULAR_LIST_HPP
