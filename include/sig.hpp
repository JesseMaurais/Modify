#ifndef sig_hpp
#define sig_hpp "Signals and Sockets"

#include <map>
#include <csignal>
#include <algorithm>
#include <functional>
#include "ptr.hpp"
#include "fmt.hpp"

namespace sig
{
	template 
	<
		typename Slot, typename... Args
	> 
	class socket : fwd::unique
	{
	public:

		using signature = void(Args...);
		using observer = std::function<signature>;
		using container = std::map<Slot, observer>;
		using size_type = typename container::size_type;

		size_type const invalid = ~size_type(0);

		auto connect(Slot const& id, observer ob)
		{
			slots.emplace(id, ob);
			return slots.size();
		}

		auto disconnect(Slot const &id)
		{
			return slots.erase(id);
		}

		auto disconnect()
		{
			auto const sz = slots.size();
			slots.clear();
			return sz;
		}

		size_type find(Slot const &id) const
		{
			auto const it = slots.find(id);
			auto const begin = slots.begin();
			auto const end = slots.end();
			return end != it ? distance(begin, it) : invalid;
		}

		void raise(Args... args) const
		{
			raise([=](auto const &pair){ pair.second(args...); });
		}

		template <typename Filter>
		void raise(Filter &&filter) const
		{
			for_each(begin(slots), end(slots), filter);
		}

		template <typename Filter>
		void raise(Slot const &id, Filter &&filter) const
		{
			const auto pair = slots.equal_range(id);
			for_each(pair.first, pair.second, filter);
		}

	protected:

		container slots;
	};

	template 
	<
		typename... Args
	> 
	class slot
	{
	public:

		using address = slot*;
		using socket = ::sig::socket<address, Args...>;
		using signature = typename socket::signature;
		using observer = typename socket::observer;

		slot(socket *so, observer ob) : that(so)
		{
			that->connect(this, ob);
		}

		~slot()
		{
			that->disconnect(this);
		}

	private:

		socket *that;
	};
}

namespace sys::sig
{
	using slot = ::sig::slot<int>;
	using socket = slot::socket;
	using signature = slot::signature;
	using observer = slot::observer;

	fmt::string to_string(int);

	struct scope : fwd::unique, slot
	{
		scope(int no, observer ob) : slot(&event(no), ob)
		{
			old.ob = std::signal(no, raise);
			#ifdef alert
			alert(SIG_ERR == old.ob);
			#endif
		}

		~scope()
		{
			(void) std::signal(old.on, old.ob);
		}

	protected:

		struct {
			signature* ob;
			int on;
		} old;

	private:

		static socket& event(int);
		static void raise(int);
	};

	struct stack : scope
	{
		stack(int on, observer ob) : scope(on, next(ob))
		{ }

	protected:

		observer next(observer ob)
		{
			return [=](int on)
			{
				ob(on); old.ob(on);
			};
		}
	};
}

#endif // file