#ifndef type_hpp
#define type_hpp "Character Types"

#include "algo.hpp"
#include "fmt.hpp"
#include "it.hpp"
#include "utf.hpp"
#include <locale>

namespace fmt
{
	constexpr auto
		space  = std::ctype_base::space,
		print  = std::ctype_base::print,
		cntrl  = std::ctype_base::cntrl,
		upper  = std::ctype_base::upper,
		lower  = std::ctype_base::lower,
		alpha  = std::ctype_base::alpha,
		digit  = std::ctype_base::digit,
		punct  = std::ctype_base::punct,
		blank  = std::ctype_base::blank,
		alnum  = std::ctype_base::alnum,
		graph  = std::ctype_base::graph,
		xdigit = std::ctype_base::xdigit;

	template <class Char> struct type : std::ctype<Char>
	{
		using string     = basic_string<Char>;
		using basic_type = std::ctype<Char>;
		using mask       = typename basic_type::mask;
		using view       = typename string::view;
		using init       = typename view::init;
		using span       = typename view::span;
		using pair       = typename view::pair;
		using vector     = typename view::vector;
		using size_type  = typename view::size_type;
		using ctype      = typename view::ctype;
		using iterator   = typename view::iterator;
		using pointer    = typename view::cptr;
		using range      = fwd::range<iterator>;
		using mark       = fwd::vector<mask>;

		static type* get();
		static type* set(type*);

		static_assert(null == ~npos);

		template <class C> static string from(const C &);

		bool check(Char c, mask x = space) const;
		// Check whether code w is an x

		mark check(view u) const;
		// Classify all characters in a view

		iterator next(iterator it, iterator end, mask x = space) const;
		// Next iterator after $it but before $end which is an $x

		pointer next(pointer it, pointer end, mask x = space) const;
		// Next character after $it but before $end which is an $x

		iterator next(view u, mask x = space) const;
		// Index of first character in view $u which is an $x

		iterator skip(iterator it, iterator end, mask x = space) const;
		// Next iterator after $it but before $end which is not $x

		pointer skip(pointer it, pointer end, mask x = space) const;
		// Next character after $it but before $end which is not $x

		iterator skip(view u, mask x = space) const;
		// Index of first character in view $u which is not $x

		vector split(view u, mask x = space) const;
		// Split strings in $u delimited by $x

		auto widen(fwd::basic_string_view<char> u) const
		// Decode multibyte characters as wide type
		{
			struct iterator : utf
			{
				basic_type const* that;
				char const* pos;
				std::size_t size;

				bool operator!=(iterator const& it) const
				{
					return it.pos != pos;
				}

				auto operator*() const
				{
					Char c[1];
					(void) that->widen(pos, pos + size, c);
					return *c;
				}

				auto operator++()
				{
					pos += size;
					size = utf::len(pos);
					return *this;
				}

				iterator(basic_type const* owner, char const* it)
				: that(owner), pos(it)
				{
					if (nullptr != that)
					{
						size = utf::len(pos);
					}
				}
			};

			auto const begin = u.data();
			auto const end = begin + u.size();
			return fwd::range<iterator>
			(
				{ this, begin }, { nullptr, end }
			);
		}

		auto widen(fwd::basic_string_view<wchar_t> u) const
		{
			return u;
		}

		auto narrow(fwd::basic_string_view<Char> u) const
		// Encode wide characters as multibyte type
		{
			struct iterator
			{
				basic_type const* that;
				Char const* pos;

				bool operator!=(iterator const& it) const
				{
					return it.pos != pos;
				}

				auto operator*() const
				{
					std::string s(MB_CUR_MAX, 0);
					(void) that->narrow(pos, pos + 1, 0, s.data());
					return s;
				}

				auto operator++()
				{
					++ pos;
					return *this;
				}

				iterator(basic_type const* owner, Char const* it)
				: that(owner), pos(it)
				{ }
			};

			auto const begin = u.data();
			auto const end = begin + u.size();
			return fwd::range<iterator>
			(
				{ this, begin }, { nullptr, end }
			);
		}

		auto narrow(fmt::basic_string_view<char> u) const
		{
			return u;
		}

		iterator first(view u, mask x = space) const;
		// First iterator in view $u that is not $x

		iterator last(view u, mask x = space) const;
		// Last iterator in view $u that is not $x

		view trim(view u, mask x = space) const;
		// Trim $x off the front and back of $u

		bool all_of(view u, mask x = space) const;
		// All decoded characters in $u are $x

		bool any_of(view u, mask x = space) const;
		// Any decoded characters in $u are $x

		string to_upper(view u) const;
		// Recode characters in upper case

		string to_lower(view u) const;
		// Recode characters in lower case

		static pair to_pair(view u, view v);
		// Divide view $u by first occurance of $v

		static bool terminated(view u);
		// Check whether string is null terminated

		static auto count(view u, view v)
		// Count occurances in $u of a substring $v
		{
			auto n = null;
			auto const z = v.size();
			for (auto i = u.find(v); i != npos; i = u.find(v, i))
			{
				i += z;
				++ n;
			}
			return n;
		}

		template <class Iterator>
		static auto join(Iterator begin, Iterator end, view u)
		// Join strings in $t with $u inserted between
		{
			string s;
			for (auto it = begin; it != end; ++it)
			{
				if (begin != it)
				{
					s += u;
				}
				s += *it;
			}
			return s;
		}

		template <class Span> static auto split(Span t, view u)
		// Split strings in $p delimited by $v
		{
			fwd::vector<Span> s;
			auto p = t.data();
			size_t i = 0, j = 0;
			for (auto v : t)
			{
				if (u == v)
				{
					s.emplace_back(p + i, j - i);
					j = ++i;
				}
				else ++j;
			}
			return s;
		}

		static auto split(view u, view v)
		// Split strings in $u delimited by $v
		{
			vector t;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = uz < j ? uz : j;
				auto const w = u.substr(i, k - i);
				if (i <= k) t.emplace_back(w);
				i = k + vz;
			}
			return t;
		}

		static auto replace(view u, view v, view w)
		// Replace in u all occurrances of v with w
		{
			string s;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = std::min(j, uz);
				s = u.substr(i, k - i);
				if (j < uz) s += w;
				i = k + vz;
			}
			return s;
		}

		static auto embrace(view u, view v)
		// Position in u with matching braces front and back in v
		{
			auto i = u.find_first_of(v.front()), j = i;
			if (i < npos)
			{
				int n = 1;
				do
				{
					j = u.find_first_of(v, j + 1);
					if (npos == j)
					{
						break; // missing
					}
					else
					if (v.back() == u[j])
					{
						--n; // close
					}
					else
					if (v.front() == u[j])
					{
						++n; // open
					}
					else
					{
						break; // other
					}
				}
				while (0 < n);
			}
			return std::pair { i, j };
		}
	};

	// Common characters

	extern template struct type<char>;
	using ctype = type<char>;

	extern template struct type<wchar_t>;
	using wtype = type<wchar_t>;

	// Multibyte shims

	template <typename iterator>
	inline auto next(iterator it, iterator end)
	{
		return type<char>::get()->next(it, end);
	}

	inline auto next(string::view u)
	{
		return type<char>::get()->next(u);
	}

	template <typename iterator>
	inline auto skip(iterator it, iterator end)
	{
		return type<char>::get()->skip(it, end);
	}

	inline auto skip(string::view u)
	{
		return type<char>::get()->skip(u);
	}

	inline auto widen(string::view u)
	{
		return type<char>::get()->widen(u);
	}

	inline auto widen(char c)
	{
		return widen(string(1, c));
	}

	inline auto narrow(wstring::view u)
	{
		return type<wchar_t>::get()->narrow(u);
	}

	inline auto narrow(wchar_t c)
	{
		return narrow(wstring(1, c));
	}

	inline auto first(string::view u)
	{
		return type<char>::get()->first(u);
	}

	inline auto last(string::view u)
	{
		return type<char>::get()->last(u);
	}

	inline auto trim(string::view u)
	{
		return type<char>::get()->trim(u);
	}

	inline auto all_of(string::view u, type<char>::mask m)
	{
		return type<char>::get()->all_of(u, m);
	}

	inline auto any_of(string::view u, type<char>::mask m)
	{
		return type<char>::get()->any_of(u, m);
	}

	inline auto to_upper(string::view u)
	{
		return type<char>::get()->to_upper(u);
	}

	inline auto to_lower(string::view u)
	{
		return type<char>::get()->to_lower(u);
	}

	inline bool terminated(string::view u)
	{
		return type<char>::get()->terminated(u);
	}

	inline auto count(string::view u, string::view v)
	{
		return type<char>::get()->count(u, v);
	}

	inline auto join(string::view::span t, string::view u = empty)
	{
		return type<char>::get()->join(t.begin(), t.end(), u);
	}

	inline auto join(string::span t, string::view u = empty)
	{
		return type<char>::get()->join(t.begin(), t.end(), u);
	}

	inline auto join(string::view::init t, string::view u = empty)
	{
		return type<char>::get()->join(t.begin(), t.end(), u);
	}

	inline auto join(string::init t, string::view u = empty)
	{
		return type<char>::get()->join(t.begin(), t.end(), u);
	}

	inline auto split(string::view u)
	{
		return type<char>::get()->split(u);
	}

	inline auto split(string::view u, string::view v)
	{
		return type<char>::get()->split(u, v);
	}

	inline auto split(string::view::span p, string::view v)
	{
		return type<char>::get()->split(p, v);
	}

	inline auto split(string::span p, string::view v)
	{
		return type<char>::get()->split(p, v);
	}

	inline auto replace(string::view u, string::view v, string::view w)
	{
		return type<char>::get()->replace(u, v, w);
	}

	inline auto embrace(string::view u, string::view v)
	{
		return type<char>::get()->embrace(u, v);
	}

	inline auto to_pair(string::view u, string::view v = assign)
	{
		return type<char>::get()->to_pair(u, v);
	}

	inline auto to_pair(string::view::pair p, string::view u = assign)
	{
		string::view::vector x { p.first, p.second };
		return fmt::join(x, u);
	}

	inline bool same(string::view u, string::view v)
	{
		return u.empty() ? v.empty() :
			u.data() == v.data() and u.size() == v.size();
	}

	//
	// Generic string converters
	//

	template <typename T>
	inline string to_string(T const& x)
	{
		return std::to_string(x);
	}

	template <typename T>
	inline wstring to_wstring(T const& x)
	{
		return std::to_wstring(x);
	}

	template <>
	inline string to_string(string const& s)
	{
		return s;
	}

	template <>
	inline wstring to_wstring(wstring const& w)
	{
		return w;
	}

	template <>
	inline string to_string(string::view const& s)
	{
		return fmt::string(s.data(), s.size());
	}

	template <>
	inline wstring to_wstring(wstring::view const& w)
	{
		return wstring(w.data(), w.size());
	}

	template <>
	inline string to_string(char const*const& s)
	{
		return s ? string(s) : string();
	}

	template <>
	inline wstring to_wstring(wchar_t const*const& w)
	{
		return w ? wstring(w) : wstring();
	}

	template <>
	inline string to_string(char *const& s)
	{
		return s ? string(s) : string();
	}

	template <>
	inline wstring to_wstring(wchar_t *const& w)
	{
		return w ? wstring(w) : wstring();
	}

	template <>
	inline string to_string(char const& c)
	{
		return string(1, c);
	}

	template <>
	inline wstring to_wstring(wchar_t const& c)
	{
		return wstring(1, c);
	}

	//
	// Wide & narrow conversions
	//

	template <>
	inline string to_string(wstring::view const& w)
	{
		string s;
		for (auto const c : narrow(w)) s += c;
		return s;
	}

	template <>
	inline wstring to_wstring(string::view const& s)
	{
		wstring w;
		for (auto const c : widen(s)) w += c;
		return w;
	}

	template <>
	inline string to_string(wstring const& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(string const& s)
	{
		return to_wstring(string::view(s));
	}

	template <>
	inline string to_string(wchar_t const*const& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(char const*const& s)
	{
		return to_wstring(string::view(s));
	}

	template <>
	inline string to_string(wchar_t *const& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(char *const& s)
	{
		return to_wstring(string::view(s));
	}

	template <>
	inline string to_string(wchar_t const& c)
	{
		return to_string(wstring(1, c));
	}

	template <>
	inline wstring to_wstring(char const& c)
	{
		return to_wstring(string(1, c));
	}

	// Converters for character class

	template <> template <class T> string type<char>::from(T const& s)
	{
		return ::fmt::to_string(s);
	}

	template <> template <class T> wstring type<wchar_t>::from(T const& s)
	{
		return ::fmt::to_wstring(s);
	}
}

#endif // file
