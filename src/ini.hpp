#ifndef ini_hpp
#define ini_hpp "Initial Options"

#include "fwd.hpp"
#include "fmt.hpp"

namespace doc
{
	struct ini
	{
		using fmt::string;
		using string::in;
		using string::out;
		using string::view;
		using view::pair;
		using view::span;
		using view::init;
		using view::vector;
		
		fmt::group<word> keys;
		fmt::graph<word> values;
		string::vector cache;

		static in::ref getline(in::ref, string::ref);
		friend in::ref operator>>(in::ref, ini::ref);
		friend out::ref operator<<(out::ref, ini::cref);

		static string join(span);
		static vector split(view);
		static string join(init);

		bool got(pair) const;
		view get(pair) const;
		bool set(pair, view);
		bool put(pair, view);
	};
}

#endif // file
