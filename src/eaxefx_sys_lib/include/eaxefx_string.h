/*

EAX OpenAL Extension

Copyright (c) 2020-2021 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

*/


#ifndef EAXEFX_STRING_INCLUDED
#define EAXEFX_STRING_INCLUDED


#include <cassert>

#include <cstddef>

#include <memory>
#include <string_view>


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename TChar
>
struct CharTraitsT
{
	static TChar* copy(
		TChar* dest,
		const TChar* src,
		std::size_t count)
	{
		assert(dest && src);

		for (auto i = std::size_t{}; i < count; ++i)
		{
			dest[i] = src[i];
		}

		return dest;
	}

	static std::size_t length(
		const TChar* s)
	{
		assert(s);

		auto count = std::size_t{};

		while (*s++ != '\0')
		{
			count += 1;
		}

		return count;
	}

	static int compare(
		const TChar* s1,
		const TChar* s2,
		std::size_t count)
	{
		assert(s1 && s2);

		for (auto i = std::size_t{}; i < count; ++i)
		{
			const auto diff = (*s1++) - (*s2++);

			if (diff != 0)
			{
				return diff;
			}
		}

		return 0;
	}
}; // CharTraitsT

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename TChar
>
class StringT
{
public:
	using traits_type = CharTraitsT<TChar>;
	using value_type = TChar;
	using size_type = std::size_t;


	StringT() noexcept
	{
		ctor_empty_string();
	}

	StringT(
		value_type ch)
	{
		ctor_c_string(&ch, 1);
	}

	StringT(
		const value_type* c_string)
	{
		ctor_c_string(c_string);
	}

	StringT(
		const value_type* c_string,
		size_type c_string_length)
	{
		ctor_c_string(c_string, c_string_length);
	}

	StringT(
		const StringT& rhs)
		:
		StringT{}
	{
		assign(rhs);
	}

	StringT(
		StringT&& rhs) noexcept
	{
		move(rhs);
	}

	StringT& operator=(
		const value_type* c_string)
	{
		return assign(c_string);
	}

	StringT& operator=(
		const StringT& rhs)
	{
		if (std::addressof(rhs) != this)
		{
			return assign(rhs);
		}

		return *this;
	}

	StringT& operator=(
		StringT&& rhs) noexcept
	{
		if (std::addressof(rhs) != this)
		{
			move(rhs);
		}

		return *this;
	}

	const value_type* data() const noexcept
	{
		return is_external_buffer_ ? external_buffer_.get() : internal_buffer_;
	}

	value_type* data() noexcept
	{
		return const_cast<value_type*>(const_cast<const StringT*>(this)->data());
	}

	const value_type* c_str() const noexcept
	{
		return data();
	}

	size_type size() const noexcept
	{
		return length_;
	}

	void reserve(
		size_type capacity)
	{
		if (capacity <= capacity_)
		{
			return;
		}

		if (is_external_buffer_)
		{
			auto external_buffer = std::make_unique<value_type[]>(capacity + 1);

			copy_c_string_with_terminator(external_buffer_.get(), length_, external_buffer.get());
			external_buffer_.swap(external_buffer);

			capacity_ = capacity;
		}
		else
		{
			if (capacity <= max_internal_buffer_length)
			{
				capacity_ = capacity;
			}
			else
			{
				external_buffer_ = std::make_unique<value_type[]>(capacity + 1);

				copy_c_string_with_terminator(internal_buffer_, length_, external_buffer_.get());

				is_external_buffer_ = true;
				capacity_ = capacity;
			}
		}
	}

	void resize(
		size_type length)
	{
		if (length > length_)
		{
			const auto old_length = length_;

			reserve(length);

			auto chars = data();

			for (auto i = old_length; i <= length; ++i)
			{
				chars[i] = '\0';
			}

			length_ = length;
		}
		else if (length < length_)
		{
			length_ = length;
			data()[length_] = '\0';
		}
	}

	StringT& append(
		value_type c)
	{
		reserve(length_ + 1);

		auto chars = data() + length_;
		*chars++ = c;
		*chars = '\0';

		length_ += 1;

		return *this;
	}

	StringT& append(
		const value_type* c_string,
		size_type count)
	{
		assert(c_string);

		reserve(length_ + count);

		copy_c_string_with_terminator(c_string, count, data() + length_);

		length_ += count;

		return *this;
	}

	StringT& append(
		const value_type* c_string)
	{
		assert(c_string);

		const auto length = traits_type::length(c_string);

		return append(c_string, length);
	}

	StringT& append(
		const StringT& rhs)
	{
		return append(rhs.data(), rhs.size());
	}

	StringT& assign(
		const value_type* c_string,
		size_type count)
	{
		resize(count);

		traits_type::copy(data(), c_string, count);

		return *this;
	}

	StringT& assign(
		const value_type* c_string)
	{
		const auto length = traits_type::length(c_string);

		return assign(c_string, length);
	}

	StringT& assign(
		const StringT& rhs)
	{
		return assign(rhs.data(), rhs.size());
	}

	void clear()
	{
		length_ = 0;
	}

	bool empty() const noexcept
	{
		return length_ == 0;
	}

	operator std::basic_string_view<value_type>() const noexcept
	{
		return std::basic_string_view<value_type>(data(), size());
	}

	int compare(
		const value_type* rhs) const
	{
		const auto rhs_length = traits_type::length(rhs);

		if (length_ == rhs_length)
		{
			return traits_type::compare(data(), rhs, rhs_length);
		}
		else if (length_ < rhs_length)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}

	int compare(
		const StringT& rhs) const
	{
		if (length_ == rhs.length_)
		{
			return traits_type::compare(data(), rhs.data(), rhs.length_);
		}
		else if (length_ < rhs.length_)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}


	value_type* begin() noexcept
	{
		return data();
	}

	value_type* end() noexcept
	{
		return begin() + size();
	}


	const value_type* begin() const noexcept
	{
		return data();
	}

	const value_type* end() const noexcept
	{
		return begin() + size();
	}


	value_type back() const
	{
		assert(!empty());
		return data()[size() - 1];
	}


private:
	static constexpr auto max_internal_buffer_length = 15;

	using InternalBuffer = value_type[max_internal_buffer_length];
	using ExternalBuffer = std::unique_ptr<value_type[]>;


	bool is_external_buffer_;
	size_type capacity_;
	size_type length_;
	InternalBuffer internal_buffer_;
	ExternalBuffer external_buffer_;


	static void copy_c_string_with_terminator(
		const value_type* src_c_string,
		size_type src_length,
		value_type* dst_c_string)
	{
		traits_type::copy(dst_c_string, src_c_string, src_length);

		dst_c_string[src_length] = '\0';
	}

	void ctor_empty_string()
	{
		is_external_buffer_ = false;
		capacity_ = 0;
		length_ = 0;
		internal_buffer_[0] = '\0';
	}

	void ctor_c_string_internal_buffer(
		const value_type* c_string,
		size_type length) noexcept
	{
		is_external_buffer_ = false;
		capacity_ = max_internal_buffer_length;
		length_ = length;

		copy_c_string_with_terminator(c_string, length, internal_buffer_);
	}

	void ctor_c_string_external_buffer(
		const value_type* c_string,
		size_type length)
	{
		is_external_buffer_ = true;
		capacity_ = length;
		length_ = length;
		external_buffer_ = std::make_unique<value_type[]>(length + 1);

		copy_c_string_with_terminator(c_string, length, external_buffer_.get());
	}

	void ctor_c_string(
		const value_type* c_string,
		size_type c_string_length)
	{
		if (c_string_length <= max_internal_buffer_length)
		{
			ctor_c_string_internal_buffer(c_string, c_string_length);
		}
		else
		{
			ctor_c_string_external_buffer(c_string, c_string_length);
		}
	}

	void ctor_c_string(
		const value_type* c_string)
	{
		const auto length = traits_type::length(c_string);

		ctor_c_string(c_string, length);
	}

	void move(
		StringT& rhs)
	{
		is_external_buffer_ = rhs.is_external_buffer_;
		capacity_ = rhs.capacity_;
		length_ = rhs.length_;

		if (!is_external_buffer_)
		{
			copy_c_string_with_terminator(rhs.internal_buffer_, length_, internal_buffer_);
		}

		external_buffer_ = std::move(rhs.external_buffer_);
	}
}; // StringT

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename TChar
>
inline StringT<TChar> operator+(
	const TChar* lhs,
	const StringT<TChar>& rhs)
{
	using Traits = typename StringT<TChar>::traits_type;

	const auto lhs_length = Traits::length(lhs);

	auto result = StringT<TChar>{};
	result.reserve(lhs_length + rhs.size());
	result.append(lhs, lhs_length);
	result.append(rhs);
	return result;
}

template<
	typename TChar
>
inline StringT<TChar> operator+(
	const StringT<TChar>& lhs,
	TChar rhs)
{
	auto result = StringT<TChar>{};
	result.reserve(lhs.size() + 1);
	result.append(lhs);
	result.append(rhs);
	return result;
}

template<
	typename TChar
>
inline StringT<TChar> operator+(
	const StringT<TChar>& lhs,
	const TChar* rhs)
{
	using Traits = typename StringT<TChar>::traits_type;

	const auto rhs_length = Traits::length(rhs);

	auto result = StringT<TChar>{};
	result.reserve(lhs.size() + rhs_length);
	result.append(lhs);
	result.append(rhs, rhs_length);
	return result;
}

template<
	typename TChar
>
inline StringT<TChar> operator+(
	const StringT<TChar>& lhs,
	const StringT<TChar>& rhs)
{
	auto result = StringT<TChar>{};
	result.reserve(lhs.size() + rhs.size());
	result.append(lhs);
	result.append(rhs);
	return result;
}

template<
	typename TChar
>
inline StringT<TChar>& operator+=(
	StringT<TChar>& lhs,
	TChar rhs)
{
	return lhs.append(rhs);
}

template<
	typename TChar
>
inline StringT<TChar>& operator+=(
	StringT<TChar>& lhs,
	const TChar* rhs)
{
	return lhs.append(rhs);
}

template<
	typename TChar,
	typename TStringViewCharTraits
>
inline StringT<TChar>& operator+=(
	StringT<TChar>& lhs,
	const std::basic_string_view<TChar, TStringViewCharTraits>& rhs)
{
	return lhs.append(rhs.data(), rhs.size());
}

template<
	typename TChar
>
inline StringT<TChar>& operator+=(
	StringT<TChar>& lhs,
	const StringT<TChar>& rhs)
{
	return lhs.append(rhs);
}

template<
	typename TChar
>
inline bool operator==(
	const StringT<TChar>& lhs,
	const TChar* rhs)
{
	return lhs.compare(rhs) == 0;
}

template<
	typename TChar
>
inline bool operator==(
	const StringT<TChar>& lhs,
	const StringT<TChar>& rhs)
{
	return lhs.compare(rhs) == 0;
}

template<
	typename TChar
>
inline bool operator!=(
	StringT<TChar>& lhs,
	const TChar* rhs)
{
	return !(lhs == rhs);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using String = StringT<char>;
using U16String = StringT<char16_t>;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void to_string(
	int value,
	String& string);

void to_string(
	unsigned int value,
	String& string);

void to_string(
	float value,
	String& string);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

String to_string(
	int value);

String to_string(
	unsigned int value);

String to_string(
	float value);

String to_string_hex(
	const void* value);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int stoi(
	String string);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_STRING_INCLUDED
