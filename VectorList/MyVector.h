#pragma once

#include "SpyAllocator.h"

namespace my
{
	template <typename T, class Allocator = SpyAllocator<T>>
	class vector
	{
	public:
		class iterator : public std::iterator<std::random_access_iterator_tag, T>
		{
		public:
			iterator();
			iterator(T* ptr);
			iterator(const iterator& other);
			iterator(iterator&& other) noexcept;
			~iterator() = default;

			iterator& operator=(const iterator& other);
			iterator& operator=(iterator&& other) noexcept;

			bool		operator==(const iterator& other) const;
			bool		operator!=(const iterator& other) const;

			iterator	operator+(size_t n) const;
			iterator	operator-(size_t n) const;

			iterator&	operator+=(size_t n);
			iterator&	operator-=(size_t n);

			iterator&	operator++();
			iterator	operator++(int);

			iterator&	operator--();
			iterator	operator--(int);

			T&			operator*();
			T*			operator->();

		private:
			T*	m_ptr;
		};

		vector();
		vector(const vector& other);
		vector(vector&& other) noexcept;
		~vector();

		vector&		operator=(const vector& other);
		vector&		operator=(vector&& other) noexcept;

		T			operator[](size_t i) const;
		T&			operator[](size_t i);

		void		push_back(const T& item);
		void		push_back(T&& item);

		template	<typename... Args>
		void		emplace_back(Args... args);

		size_t		capacity() const;
		size_t		size() const;

		iterator	begin() const;
		iterator	end() const;

		void		reserve(size_t capacity);
		void		resize(size_t size);
		void		shrink_to_fit();
		void		clear();

	private:
		size_t		m_capacity;
		size_t		m_size;
		T*			m_items;

		size_t		calculateCapacity(size_t size) const;
		void		setCapacity(size_t capacity);
	};

	template <typename T, class Allocator>
	vector<T, Allocator>::iterator::iterator() = default;

	template <typename T, class Allocator>
	vector<T, Allocator>::iterator::iterator(T* ptr)
	{
		m_ptr = ptr;
	}

	template <typename T, class Allocator>
	vector<T, Allocator>::iterator::iterator(const iterator& other) : m_ptr(other.m_ptr)
	{
	}

	template <typename T, class Allocator>
	vector<T, Allocator>::iterator::iterator(iterator&& other) noexcept : m_ptr(other.m_ptr)
	{
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator& vector<T, Allocator>::iterator::operator=(
		const iterator& other)
	{
		if (this == &other)
			return *this;

		this->m_ptr = other.m_ptr;

		return *this;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator& vector<T, Allocator>::iterator::operator=(
		iterator&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->m_ptr = other.m_ptr;

		return *this;
	}

	template <typename T, class Allocator>
	bool vector<T, Allocator>::iterator::operator==(const iterator& other) const
	{
		return other.m_ptr == m_ptr;
	}

	template <typename T, class Allocator>
	bool vector<T, Allocator>::iterator::operator!=(const iterator& other) const
	{
		return other.m_ptr != m_ptr;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator+(size_t n) const
	{
		return iterator(this->m_ptr + n);
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator-(size_t n) const
	{
		return iterator(this->m_ptr - n);
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator& vector<T, Allocator>::iterator::operator+=(size_t n)
	{
		m_ptr += n;
		return *this;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator& vector<T, Allocator>::iterator::operator-=(size_t n)
	{
		m_ptr -= n;
		return *this;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator& vector<T, Allocator>::iterator::operator++()
	{
		++m_ptr;
		return *this;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator++(int)
	{
		iterator tmp = *this;
		++m_ptr;
		return tmp;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator& vector<T, Allocator>::iterator::operator--()
	{
		--m_ptr;
		return *this;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator--(int)
	{
		iterator tmp = *this;
		--m_ptr;
		return tmp;
	}

	template <typename T, class Allocator>
	T& vector<T, Allocator>::iterator::operator*()
	{
		return *m_ptr;
	}

	template <typename T, class Allocator>
	T* vector<T, Allocator>::iterator::operator->()
	{
		return m_ptr;
	}

	template <typename T, class Allocator>
	vector<T, Allocator>::vector(): m_capacity(0), m_size(0), m_items(nullptr)
	{
	}

	template <typename T, class Allocator>
	vector<T, Allocator>::vector(const vector& other)
	{
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_items = Allocator().allocate(m_capacity);

		for (size_t i = 0; i < m_size; i++)
			new (&m_items[i]) T(other[i]);
	}

	template <typename T, class Allocator>
	vector<T, Allocator>::vector(vector&& other) noexcept
	{
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_items = other.m_items;

		other.m_size = other.m_capacity = 0;
		other.m_items = nullptr;
	}

	template <typename T, class Allocator>
	vector<T, Allocator>::~vector()
	{
		if (m_capacity > 0)
		{
			for (size_t i = 0; i < m_size; i++)
				m_items[i].~T();

			Allocator().deallocate(m_items, m_capacity);
		}
	}

	template <typename T, class Allocator>
	vector<T, Allocator>& vector<T, Allocator>::operator=(const vector& other)
	{
		if (this == &other)
			return *this;

		this->clear();
		this->setCapacity(other.m_capacity);

		m_size = other.m_size;

		for (size_t i = 0; i < m_size; i++)
			new (&m_items[i]) T(other[i]);

		return *this;
	}

	template <typename T, class Allocator>
	vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->setCapacity(0); // Clear and deallocate the previous data

		// Steal the data from the other vector
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_items = other.m_items;

		// Clean up the other vector
		other.m_size = other.m_capacity = 0;
		other.m_items = nullptr;

		return *this;
	}

	template <typename T, class Allocator>
	T vector<T, Allocator>::operator[](const size_t i) const
	{
		return this->m_items[i];
	}

	template <typename T, class Allocator>
	T& vector<T, Allocator>::operator[](const size_t i)
	{
		return this->m_items[i];
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::push_back(const T& item)
	{
		// The commented implementation would give the correct result
		// but it wouldn't match the expected output
		/*setCapacity(calculateCapacity(m_size + 1));
			
			new (&m_items[m_size]) T(item);
			m_size++;*/

		if (m_size == m_capacity)
		{
			Allocator	allocator;

			size_t	capacity = calculateCapacity(m_size + 1);
			T*		items = allocator.allocate(capacity);

			// Could do this right before m_size++ and remove the else
			// but wouldn't match the expected output
			new (&items[m_size]) T(item);

			if (m_items != nullptr)
			{
				for (size_t i = 0; i < m_size; i++)
					new (&items[i]) T(std::move(m_items[i]));

				// Wouldn't match the expected output if done in the previous for
				for (size_t i = 0; i < m_size; i++)
					m_items[i].~T();

				allocator.deallocate(m_items, m_capacity);
			}

			m_items = items;
			m_capacity = capacity;
		}
		else
		{
			new (&m_items[m_size]) T(item);
		}

		m_size++;
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::push_back(T&& item)
	{
		// The commented implementation would give the correct result
		// but it wouldn't match the expected output
		/*setCapacity(calculateCapacity(m_size + 1));

			new (&m_items[m_size]) T(std::move(item));
			m_size++;*/

		if (m_size == m_capacity)
		{
			Allocator	allocator;

			size_t	capacity = calculateCapacity(m_size + 1);
			T*		items = allocator.allocate(capacity);

			// Could do this right before m_size++ and remove the else but wouldn't match the expected output
			new (&items[m_size]) T(std::move(item));

			if (m_items != nullptr)
			{
				for (size_t i = 0; i < m_size; i++)
					new (&items[i]) T(std::move(m_items[i]));

				// Wouldn't match the expected output if done in the previous for
				for (size_t i = 0; i < m_size; i++)
					m_items[i].~T();

				allocator.deallocate(m_items, m_capacity);
			}

			m_items = items;
			m_capacity = capacity;
		}
		else
		{
			new (&m_items[m_size]) T(std::move(item));
		}

		m_size++;
	}

	template <typename T, class Allocator>
	template <typename ... Args>
	void vector<T, Allocator>::emplace_back(Args... args)
	{
		// The commented implementation would give the correct result
		// but it wouldn't match the expected output
		/*setCapacity(calculateCapacity(m_size + 1));

			new (&m_items[m_size]) T(args...);
			m_size++;*/

		if (m_size == m_capacity)
		{
			Allocator	allocator;

			size_t	capacity = calculateCapacity(m_size + 1);
			T*		items = allocator.allocate(capacity);

			// Could do this right before m_size++ and remove the else but wouldn't match the expected output
			new (&items[m_size]) T(args...);

			if (m_items != nullptr)
			{
				for (size_t i = 0; i < m_size; i++)
					new (&items[i]) T(std::move(m_items[i]));

				// Could be done in the previous for but wouldn't match the expected output
				for (size_t i = 0; i < m_size; i++)
					m_items[i].~T();

				allocator.deallocate(m_items, m_capacity);
			}

			m_items = items;
			m_capacity = capacity;
		}
		else
		{
			new (&m_items[m_size]) T(args...);
		}

		m_size++;
	}

	template <typename T, class Allocator>
	size_t vector<T, Allocator>::capacity() const
	{
		return this->m_capacity;
	}

	template <typename T, class Allocator>
	size_t vector<T, Allocator>::size() const
	{
		return this->m_size;
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator vector<T, Allocator>::begin() const
	{
		return iterator(m_items);
	}

	template <typename T, class Allocator>
	typename vector<T, Allocator>::iterator vector<T, Allocator>::end() const
	{
		return iterator(m_items + m_size);
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::reserve(size_t capacity)
	{
		if (capacity <= m_capacity)
			return;

		setCapacity(capacity);
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::resize(const size_t size)
	{
		if (size == m_size)
			return;
			
		if (size < m_size)
		{
			for (size_t i = size; i < m_size; i++)
				m_items[i].~T();
		}
		else if (size > m_size)
		{
			setCapacity(calculateCapacity(size));

			for (size_t i = m_size; i < size; i++)
				new (&m_items[i]) T();
		}

		m_size = size;
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::shrink_to_fit()
	{
		setCapacity(m_size);
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::clear()
	{
		resize(0);
	}

	template <typename T, class Allocator>
	size_t vector<T, Allocator>::calculateCapacity(const size_t size) const
	{
		if (m_capacity >= size)
			return m_capacity;

		if (m_capacity > 2)
			return std::max(m_capacity + m_capacity / 2, size);
			
		return std::max(m_capacity + 1, size);
	}

	template <typename T, class Allocator>
	void vector<T, Allocator>::setCapacity(const size_t capacity)
	{
		if (capacity == m_capacity)
			return;

		Allocator	allocator;

		if (capacity < m_capacity)
		{
			for (size_t i = capacity; i < m_size; i++)
				m_items[i].~T();

			if (capacity < m_size)
				m_size = capacity;

			allocator.deallocate(m_items + capacity, m_capacity - capacity);

			if (capacity == 0)
				m_items = nullptr;
		}
		else
		{
			T* items = allocator.allocate(capacity);

			if (m_items != nullptr)
			{
				for (size_t i = 0; i < m_size; i++)
					new (&items[i]) T(std::move(m_items[i]));

				// Could be done in the previous for but wouldn't match the expected output
				for (size_t i = 0; i < m_size; i++)
					m_items[i].~T();

				allocator.deallocate(m_items, m_capacity);
			}

			m_items = items;
		}

		m_capacity = capacity;
	}
}
