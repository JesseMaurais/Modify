#ifndef wcs_hpp
#define wcs_hpp

#include <cwchar>
#include <cstring>
#include "str.hpp"

namespace fmt
{
	struct widen
	{
		struct iterator
		{
			friend struct widen;

			bool operator==(iterator const& it) const
			{
				return it.view.empty() ? view.empty() : it.view == view;
			}

			bool operator!=(iterator const& it) const
			{
				return not this->operator==(it);
			}

			wchar_t operator*() const
			{
				return wide;
			}

			iterator operator++()
			{
				convert();
				return *this;
			}

		private:

			iterator(fmt::string_view u = "")
			: view(u), size(0), wide(L'\0')
			{
				std::memset(&state, 0, sizeof state);
				convert();
			}

			void convert()
			{
				if (0 < size) view = view.substr(size);
				size = std::mbrtowc(&wide, view.data(), view.size(), &state); 
			}

			fmt::string_view view;
			std::mbstate_t state;
			std::ptrdiff_t size;
			wchar_t wide;
		};

		widen(fmt::string_view u)
		: view(u)
		{ }

		iterator begin() const
		{
			return iterator(view);
		}

		iterator end() const
		{
			return iterator();
		}

	private:

		fmt::string_view view;
	};

	struct narrow
	{
		struct iterator
		{
			friend struct narrow;

			bool operator==(iterator const& it) const
			{
				return it.view.empty() ? view.empty() : it.view == view;
			}

			bool operator!=(iterator const& it) const
			{
				return not this->operator==(it);
			}

			char operator*() const
			{
				return mbs[size - off];
			}

			iterator operator++()
			{
				if (--off <= 0) convert();
				return *this;
			}

		private:

			iterator(fmt::wstring_view w = L"")
			: view(w), size(0), mbs(MB_CUR_MAX, '\0')
			{ 
				std::memset(&state, 0, sizeof state);
				convert();
			}

			void convert()
			{
				if (0 < size) view = view.substr(1);
				off = size = std::wcrtomb(mbs.data(), view.front(), &state);
			}

			fmt::wstring_view view;
			std::mbstate_t state;
			std::ptrdiff_t size;
			std::ptrdiff_t off;
			std::string mbs;
		};

		narrow(fmt::wstring_view w)
		: view(w)
		{ }

		iterator begin() const
		{
			return iterator(view);
		}

		iterator end() const
		{
			return iterator();
		}

	private:

		fmt::wstring_view view;
	};
}

#endif // file