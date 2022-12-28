#pragma once

#include <stdexcept>

#include "SpyAllocator.h"

namespace my
{
	#define SSO_BUFFER_SIZE 16
	#define CAPACITY_STEP 16

	template <typename T = char, class Allocator = SpyAllocator<T>>
	class basic_string
	{
	public:
		class const_iterator : public std::iterator<std::random_access_iterator_tag, T>
		{
		public:
			const_iterator();
			const_iterator(T* ptr);
			const_iterator(const const_iterator& other);
			const_iterator(const_iterator&& other) noexcept;
			~const_iterator() = default;

			const_iterator& operator=(const const_iterator& other);
			const_iterator& operator=(const_iterator&& other) noexcept;

			bool		operator==(const const_iterator& other) const;
			bool		operator!=(const const_iterator& other) const;

			const_iterator	operator+(size_t n) const;
			const_iterator	operator-(size_t n) const;

			const_iterator& operator+=(size_t n);
			const_iterator& operator-=(size_t n);

			const_iterator& operator++();
			const_iterator	operator++(int);

			const_iterator& operator--();
			const_iterator	operator--(int);

			T& operator*();
			T* operator->();

		private:
			T* m_ptr;
		};

		typedef const_iterator iterator;

		basic_string();
		basic_string(const T* str);
		basic_string(const basic_string& other);
		basic_string(basic_string&& other) noexcept;
		~basic_string();

		basic_string&	operator=(const T* str);
		basic_string&	operator=(const basic_string& other);
		basic_string&	operator=(basic_string&& other) noexcept;

		T				operator[](size_t index) const;
		T&				operator[](size_t index);

		basic_string	operator+(const basic_string& other) const;
		basic_string	operator+(const T* str) const;

		size_t			length() const;
		size_t			size() const;
		const T*		c_str() const;

		iterator		begin();
		iterator		end();

		const_iterator	begin() const;
		const_iterator	end() const;

		void			clear();

		static size_t	stringLength(const T* str);

	private:
		union
		{
			T*	m_str;
			T	m_small_str_buffer[SSO_BUFFER_SIZE];
		}; // Using a union to save memory since these members won't ever be used together
		size_t	m_size;
		size_t	m_capacity;

		void	setCapacity(size_t capacity);
		size_t	calculateCapacity(size_t size) const;
		bool	isSmallString() const;
	};

	using string = basic_string<char, SpyAllocator<char>>;

	template <typename T, class Allocator>
	basic_string<T, Allocator>::const_iterator::const_iterator() = default;

	template <typename T, class Allocator>
	basic_string<T, Allocator>::const_iterator::const_iterator(T* ptr)
	{
		m_ptr = ptr;
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::const_iterator::const_iterator(const const_iterator& other) : m_ptr(other.m_ptr)
	{
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::const_iterator::const_iterator(const_iterator&& other) noexcept : m_ptr(other.m_ptr)
	{
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator& basic_string<T, Allocator>::const_iterator::operator=(
		const const_iterator& other)
	{
		if (this == &other)
			return *this;

		this->m_ptr = other.m_ptr;

		return *this;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator& basic_string<T, Allocator>::const_iterator::operator=(
		const_iterator&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->m_ptr = other.m_ptr;

		return *this;
	}

	template <typename T, class Allocator>
	bool basic_string<T, Allocator>::const_iterator::operator==(const const_iterator& other) const
	{
		return other.m_ptr == m_ptr;
	}

	template <typename T, class Allocator>
	bool basic_string<T, Allocator>::const_iterator::operator!=(const const_iterator& other) const
	{
		return other.m_ptr != m_ptr;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::const_iterator::operator+(size_t n) const
	{
		return const_iterator(this->m_ptr + n);
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::const_iterator::operator-(size_t n) const
	{
		return const_iterator(this->m_ptr - n);
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator& basic_string<T, Allocator>::const_iterator::operator+=(size_t n)
	{
		m_ptr += n;
		return *this;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator& basic_string<T, Allocator>::const_iterator::operator-=(size_t n)
	{
		m_ptr -= n;
		return *this;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator& basic_string<T, Allocator>::const_iterator::operator++()
	{
		++m_ptr;
		return *this;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::const_iterator::operator++(int)
	{
		const_iterator tmp = *this;
		++m_ptr;
		return tmp;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator& basic_string<T, Allocator>::const_iterator::operator--()
	{
		--m_ptr;
		return *this;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::const_iterator::operator--(int)
	{
		const_iterator tmp = *this;
		--m_ptr;
		return tmp;
	}

	template <typename T, class Allocator>
	T& basic_string<T, Allocator>::const_iterator::operator*()
	{
		return *m_ptr;
	}

	template <typename T, class Allocator>
	T* basic_string<T, Allocator>::const_iterator::operator->()
	{
		return m_ptr;
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::basic_string()
		: m_str(nullptr), m_size(0), m_capacity(0)
	{
		// Set every element of the small string buffer to it's equivalent of 0
		for (T& i : m_small_str_buffer)
			i = static_cast<T>(0);
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::basic_string(const T* str)
	{
		m_size = stringLength(str);
		m_capacity = 0;
		m_str = nullptr;

		if (!isSmallString())
		{
			setCapacity(calculateCapacity(m_size));

			for (size_t i = 0; i < m_size; i++)
				new (&m_str[i]) T(str[i]);
		}
		else
		{
			memcpy_s(m_small_str_buffer, m_size * sizeof(T),
				str, (m_size) * sizeof(T));

			for (size_t i = m_size; i < SSO_BUFFER_SIZE; i++)
				m_small_str_buffer[i] = static_cast<T>(0);
		}
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::basic_string(const basic_string& other)
	{
		m_size = other.m_size;
		m_capacity = 0;
		m_str = nullptr;

		if (isSmallString())
		{
			memcpy_s(m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T),
			         other.m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T));
		}
		else
		{
			setCapacity(calculateCapacity(m_size));

			for (size_t i = 0; i < m_size; i++)
				new (&m_str[i]) T(other.m_str[i]);
		}
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::basic_string(basic_string&& other) noexcept
	{
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_str = other.m_str;

		if (isSmallString())
			memcpy_s(m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T),
				other.m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T));

		other.m_size = 0;
		other.m_str = nullptr;
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>::~basic_string()
	{
		setCapacity(0);
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::operator=(const T* str)
	{
		if (str == m_str)
			return *this;

		clear();

		m_size = stringLength(str);

		if (isSmallString())
		{
			for (size_t i = 0; i < SSO_BUFFER_SIZE; i++)
				new (&m_small_str_buffer[i]) T(i < m_size ? str[i] 
					: static_cast<T>(0));
		}
		else
		{
			setCapacity(calculateCapacity(m_size));

			for (size_t i = 0; i < m_size; i++)
				new (&m_str[i]) T(str[i]);
		}

		return *this;
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::operator=(const basic_string& other)
	{
		if (this == &other)
			return *this;

		clear();

		m_size = other.m_size;

		if (isSmallString())
		{
			memcpy_s(m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T),
			         other.m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T));
		}
		else
		{
			setCapacity(calculateCapacity(m_size));

			for (size_t i = 0; i < m_size; i++)
				new (&m_str[i]) T(other.m_str[i]);
		}

		return *this;
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::operator=(basic_string&& other) noexcept
	{
		if (this == &other)
			return *this;

		clear();

		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_str = other.m_str;

		if (isSmallString())
			memcpy_s(m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T),
				other.m_small_str_buffer, SSO_BUFFER_SIZE * sizeof(T));

		other.m_size = 0;
		other.m_str = nullptr;

		return *this;
	}

	template <typename T, class Allocator>
	T basic_string<T, Allocator>::operator[](size_t index) const
	{
		if (index >= m_size)
			throw std::out_of_range("Index out of range");

		if (isSmallString())
			return m_small_str_buffer[index];

		return m_str[index];
	}

	template <typename T, class Allocator>
	T& basic_string<T, Allocator>::operator[](size_t index)
	{
		if (index >= m_size)
			throw std::out_of_range("Index out of range");

		if (isSmallString())
			return m_small_str_buffer[index];

		return m_str[index];
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator> basic_string<T, Allocator>::operator+(const basic_string& other) const
	{
		const size_t totalSize = m_size + other.m_size;

		basic_string result;

		result.setCapacity(result.calculateCapacity(totalSize));
		result.m_size = totalSize;

		for (size_t i = 0; i < result.m_size; i++)
			result[i] = i < m_size ? (*this)[i] : other[i - m_size];

		return result;
	}

	template <typename T, class Allocator>
	basic_string<T, Allocator> basic_string<T, Allocator>::operator+(const T* str) const
	{
		const size_t strLen = stringLength(str);  
		const size_t totalSize = m_size + strLen;

		basic_string result;

		result.setCapacity(calculateCapacity(totalSize));
		result.m_size = totalSize;

		for (size_t i = 0; i < totalSize; i++)
			result[i] = i < m_size ? (*this)[i] : str[i - m_size];

		return result;
	}

	template <typename T, class Allocator>
	size_t basic_string<T, Allocator>::length() const
	{
		return m_size;
	}

	template <typename T, class Allocator>
	size_t basic_string<T, Allocator>::size() const
	{
		return m_size;
	}

	template <typename T, class Allocator>
	const T* basic_string<T, Allocator>::c_str() const
	{
		return m_str;
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::begin()
	{
		if (isSmallString())
			return iterator(&m_small_str_buffer[0]);

		return iterator(m_str);
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::end()
	{
		if (isSmallString())
			return iterator(&m_small_str_buffer[m_size]);

		return iterator(m_str + m_size);
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::begin() const
	{
		if (isSmallString())
			return const_iterator(&m_small_str_buffer[0]);

		return const_iterator(m_str);
	}

	template <typename T, class Allocator>
	typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::end() const
	{
		if (isSmallString())
			return const_iterator(&m_small_str_buffer[m_size]);
		
		return const_iterator(m_str + m_size);
	}

	template <typename T, class Allocator>
	void basic_string<T, Allocator>::clear()
	{
		if (!isSmallString())
			for (size_t i = 0; i < m_size; i++)
				m_str[i].~T();

		m_size = 0;
	}

	template <typename T, class Allocator>
	size_t basic_string<T, Allocator>::stringLength(const T* str)
	{
		size_t size = 0;

		if (str != nullptr)
			for (size_t i = 0; str[i] != static_cast<T>(0); i++)
				size++;

		return size;
	}

	template <typename T, class Allocator>
	void basic_string<T, Allocator>::setCapacity(const size_t capacity)
	{
		if (capacity == m_capacity)
			return;

		Allocator	allocator;

		if (capacity < m_capacity)
		{
			if (isSmallString())
				for (size_t i = capacity; i < m_size; i++)
					m_small_str_buffer[i].~T();
			else
				for (size_t i = capacity; i < m_size; i++)
					m_str[i].~T();

			if (capacity < m_size)
				m_size -= m_size - capacity;

			allocator.deallocate(m_str + capacity, m_capacity - capacity);

			if (capacity == 0)
				m_str = nullptr;
		}
		else if (capacity > SSO_BUFFER_SIZE)
		{
			T* str = allocator.allocate(capacity);

			if (m_str != nullptr)
			{
				for (size_t i = 0; i < m_size; i++)
				{
					new (&str[i]) T(std::move(m_str[i]));
					m_str[i].~T();
				}

				allocator.deallocate(m_str, m_capacity);
			}

			for (size_t i = m_size; i < capacity; i++)
				str[i] = static_cast<T>(0);

			m_str = str;
		}

		m_capacity = capacity;
	}

	template <typename T, class Allocator>
	size_t basic_string<T, Allocator>::calculateCapacity(size_t size) const
	{
		size++; // Add 1 to the size for the \0

		if (m_capacity >= size)
			return m_capacity;

		if (size <= SSO_BUFFER_SIZE)
			return 0;

		// To get the capacity, round the size to the next multiple of CAPACITY_STEP
		const size_t remainder = size % CAPACITY_STEP;

		if (remainder == 0)
			return size;

		return size + CAPACITY_STEP - remainder;
	}

	template <typename T, class Allocator>
	bool basic_string<T, Allocator>::isSmallString() const
	{
		return m_size + 1 <= SSO_BUFFER_SIZE;
	}
}
