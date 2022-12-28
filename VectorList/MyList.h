#pragma once

#include "SpyAllocator.h"

namespace my
{
	template <typename T, class Allocator = SpyAllocator<T>>
	class list
	{
		class Node;
	public:
		class const_iterator : public std::iterator<std::bidirectional_iterator_tag, T>
		{
			friend list;

		public:
			const_iterator();
			const_iterator(Node* node);
			const_iterator(const const_iterator& other);
			const_iterator(const_iterator&& other) noexcept;
			~const_iterator() = default;

			const_iterator&	operator=(const const_iterator& other);
			const_iterator&	operator=(const_iterator&& other) noexcept;

			bool			operator==(const const_iterator& other) const;
			bool			operator!=(const const_iterator& other) const;

			const_iterator&	operator++();
			const_iterator	operator++(int);

			const_iterator&	operator--();
			const_iterator	operator--(int);

			T&				operator*();
			T*				operator->();

		private:
			Node*	m_node;
		};

		typedef const_iterator iterator;

		list();
		list(const list& other);
		list(list&& other) noexcept;
		~list();

		list&			operator=(const list& other);
		list&			operator=(list&& other) noexcept;

		iterator		begin();
		iterator		end();

		const_iterator	begin() const;
		const_iterator	end() const;

		T&				front();
		T&				back();

		T				front() const;
		T				back() const;

		size_t			size() const;

		void			push_back(const T& item);
		void			push_back(T&& item);

		template		<typename... Args>
		void			emplace_back(Args... args);

		iterator		insert(const_iterator it, const T& item);
		iterator		insert(const const_iterator it, const size_t count, const T& item);
		template		<typename U>
		iterator		insert(const_iterator it, U first, U last);

		void			remove(const T& val);
		void			clear();

	private:
		class Node
		{
			friend	list;
		public:
			Node() = default;
			Node(const T& data, Node* prev, Node* next);
			Node(T&& data, Node* prev, Node* next);

			template <typename... Args>
			Node(Node* prev, Node* next, Args... args);

			Node(const Node& other);
			Node(Node&& other) noexcept;
			~Node();

			Node& operator=(const Node& other);
			Node& operator=(Node&& other) noexcept;

		private:
			T		m_data;
			Node*	m_prev;
			Node*	m_next;
		};

		typedef typename Allocator::template rebind<Node>::other NodeAllocator;
		
		Node*	m_head;
		Node*	m_tail;
		size_t	m_size;

		void	removeNode(Node* node);
	};

	template <typename T, class Allocator>
	list<T, Allocator>::const_iterator::const_iterator() = default;

	template <typename T, class Allocator>
	list<T, Allocator>::const_iterator::const_iterator(Node* node)
	{
		m_node = node;
	}

	template <typename T, class Allocator>
	list<T, Allocator>::const_iterator::const_iterator(const const_iterator& other) : m_node(other.m_node)
	{
	}

	template <typename T, class Allocator>
	list<T, Allocator>::const_iterator::const_iterator(const_iterator&& other) noexcept : m_node(other.m_node)
	{
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator& list<T, Allocator>::const_iterator::operator=(
		const const_iterator& other)
	{
		if (this == &other)
			return *this;

		this->m_node = other.m_node;

		return *this;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator& list<T, Allocator>::const_iterator::operator=(
		const_iterator&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->m_node = other.m_node;

		return *this;
	}

	template <typename T, class Allocator>
	bool list<T, Allocator>::const_iterator::operator==(const const_iterator& other) const
	{
		return other.m_node == m_node;
	}

	template <typename T, class Allocator>
	bool list<T, Allocator>::const_iterator::operator!=(const const_iterator& other) const
	{
		return other.m_node != m_node;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator& list<T, Allocator>::const_iterator::operator++()
	{
		m_node = m_node->m_next;
		return *this;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator list<T, Allocator>::const_iterator::operator++(int)
	{
		const_iterator tmp = *this;
		m_node = m_node->m_next;
		return tmp;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator& list<T, Allocator>::const_iterator::operator--()
	{
		m_node = m_node->m_prev;
		return *this;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator list<T, Allocator>::const_iterator::operator--(int)
	{
		const_iterator tmp = *this;
		m_node = m_node->m_prev;
		return tmp;
	}

	template <typename T, class Allocator>
	T& list<T, Allocator>::const_iterator::operator*()
	{
		return m_node->m_data;
	}

	template <typename T, class Allocator>
	T* list<T, Allocator>::const_iterator::operator->()
	{
		return &m_node->m_data;
	}

	template <typename T, class Allocator>
	list<T, Allocator>::list(): m_head(nullptr), m_tail(nullptr), m_size(0)
	{
	}

	template <typename T, class Allocator>
	list<T, Allocator>::list(const list& other)
	{
		// Necessary to be able to use push_back
		m_head = nullptr;
		m_tail = nullptr;
		m_size = 0;

		for (const_iterator i = other.begin(); i != other.end(); ++i)
			push_back(*i);
	}

	template <typename T, class Allocator>
	list<T, Allocator>::list(list&& other) noexcept
	{
		m_head = other.m_head;
		m_tail = other.m_tail;
		m_size = other.m_size;

		other.m_head = other.m_tail = nullptr;
		other.m_size = 0;
	}

	template <typename T, class Allocator>
	list<T, Allocator>::~list()
	{
		clear();
	}

	template <typename T, class Allocator>
	list<T, Allocator>& list<T, Allocator>::operator=(const list& other)
	{
		if (this == &other)
			return *this;

		clear();

		for (iterator i = other.begin(); i != other.end(); ++i)
			push_back(*i);

		return *this;
	}

	template <typename T, class Allocator>
	list<T, Allocator>& list<T, Allocator>::operator=(list&& other) noexcept
	{
		if (this == &other)
			return *this;

		clear();

		m_head = other.m_head;
		m_tail = other.m_tail;
		m_size = other.m_size;

		other.m_head = other.m_tail = nullptr;
		other.m_size = 0;

		return *this;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::iterator list<T, Allocator>::begin()
	{
		return iterator(m_head);
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::iterator list<T, Allocator>::end()
	{
		return iterator(nullptr);
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator list<T, Allocator>::begin() const
	{
		return const_iterator(m_head);
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::const_iterator list<T, Allocator>::end() const
	{
		return const_iterator(nullptr);
	}

	template <typename T, class Allocator>
	T& list<T, Allocator>::front()
	{
		if (m_size == 0)
			throw std::exception("Called front() on an empty list");

		return m_head->m_data;
	}

	template <typename T, class Allocator>
	T& list<T, Allocator>::back()
	{
		if (m_size == 0)
			throw std::exception("Called back() on an empty list");

		return m_tail->m_data;
	}

	template <typename T, class Allocator>
	T list<T, Allocator>::front() const
	{
		if (m_size == 0)
			throw std::exception("Called front() on an empty list");

		return m_head->m_data;
	}

	template <typename T, class Allocator>
	T list<T, Allocator>::back() const
	{
		if (m_size == 0)
			throw std::exception("Called back() on an empty list");

		return m_tail->m_data;
	}

	template <typename T, class Allocator>
	size_t list<T, Allocator>::size() const
	{
		return this->m_size;
	}

	template <typename T, class Allocator>
	void list<T, Allocator>::push_back(const T& item)
	{
		NodeAllocator	nodeAllocator;

		Node* node = nodeAllocator.allocate(1);
		nodeAllocator.construct(node, item, m_tail, nullptr);

		if (m_tail)
			m_tail->m_next = node;
		else
			m_head = node;

		m_tail = node;
		m_size++;
	}

	template <typename T, class Allocator>
	void list<T, Allocator>::push_back(T&& item)
	{
		NodeAllocator	nodeAllocator;

		Node* node = nodeAllocator.allocate(1);
		nodeAllocator.construct(node, std::move(item), m_tail, nullptr);

		if (m_tail)
			m_tail->m_next = node;
		else
			m_head = node;

		m_tail = node;
		m_size++;
	}

	template <typename T, class Allocator>
	template <typename ... Args>
	void list<T, Allocator>::emplace_back(Args... args)
	{
		NodeAllocator	nodeAllocator;

		Node* node = nodeAllocator.allocate(1);
		nodeAllocator.construct(node, m_tail, nullptr, args...);

		if (m_tail)
			m_tail->m_next = node;
		else
			m_head = node;

		m_tail = node;
		m_size++;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator it, const T& item)
	{
		if (it == end())
		{
			push_back(item);
			return iterator(m_tail);
		}

		NodeAllocator	nodeAllocator;

		Node* itNode = it.m_node;

		Node* newNode = nodeAllocator.allocate(1);
		nodeAllocator.construct(newNode, std::move(item), itNode->m_prev, itNode);

		if (itNode->m_prev)
			itNode->m_prev->m_next = newNode;

		itNode->m_prev = newNode;

		if (itNode == m_head)
			m_head = newNode;

		m_size++;

		return iterator(newNode);
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::iterator list<T, Allocator>::insert(const const_iterator it, const size_t count,
		const T& item)
	{
		if (count == 0)
			return it;

		iterator result = insert(it, item);

		for (size_t i = 1; i < count; i++)
			insert(it, item);

		return result;
	}

	template <typename T, class Allocator>
	template <typename U>
	typename list<T, Allocator>::iterator list<T, Allocator>::insert(const const_iterator it, U first, U last)
	{
		if (first == last)
			return it;

		iterator result = insert(it, *first++);

		for (U item = first; item != last; ++item)
			insert(it, *item);

		return result;
	}

	template <typename T, class Allocator>
	void list<T, Allocator>::remove(const T& val)
	{
		Node* node = m_head;

		while (node)
		{
			Node* next = node->m_next;

			if (val == node->m_data)
				removeNode(node);

			node = next;
		}
	}

	template <typename T, class Allocator>
	void list<T, Allocator>::clear()
	{
		if (m_size == 0)
			return;

		Node* node = m_head;

		while (node)
		{
			Node* next = node->m_next;

			removeNode(node);

			node = next;
		}
	}

	template <typename T, class Allocator>
	list<T, Allocator>::Node::Node(const T& data, Node* prev, Node* next): m_data(data), m_prev(prev), m_next(next)
	{
	}

	template <typename T, class Allocator>
	list<T, Allocator>::Node::Node(T&& data, Node* prev, Node* next): m_data(std::move(data)), m_prev(prev), m_next(next)
	{
	}

	template <typename T, class Allocator>
	template <typename ... Args>
	list<T, Allocator>::Node::Node(Node* prev, Node* next, Args... args): m_data(args...), m_prev(prev), m_next(next)
	{
	}

	template <typename T, class Allocator>
	list<T, Allocator>::Node::Node(const Node& other)
	{
		m_data(other.m_data);

		m_prev = NodeAllocator().allocate(1);
		m_next = NodeAllocator().allocate(1);

		(*m_prev)(*other.m_prev);
		(*m_next)(*other.m_next);
	}

	template <typename T, class Allocator>
	list<T, Allocator>::Node::Node(Node&& other) noexcept
	{
		m_data(std::move(other.m_data));

		m_prev = other.m_prev;
		m_next = other.m_next;

		other.m_prev = nullptr;
		other.m_next = nullptr;
	}

	template <typename T, class Allocator>
	list<T, Allocator>::Node::~Node() = default;

	template <typename T, class Allocator>
	typename list<T, Allocator>::Node& list<T, Allocator>::Node::operator=(const Node& other)
	{
		if (this == &other)
			return *this;

		m_data(other.m_data);

		m_prev = NodeAllocator().allocate(1);
		m_next = NodeAllocator().allocate(1);

		(*m_prev)(*other.m_prev);
		(*m_next)(*other.m_next);

		return *this;
	}

	template <typename T, class Allocator>
	typename list<T, Allocator>::Node& list<T, Allocator>::Node::operator=(Node&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_data(std::move(other.m_data));

		m_prev = other.m_prev;
		m_next = other.m_next;

		other.m_prev = nullptr;
		other.m_next = nullptr;

		return *this;
	}

	template <typename T, class Allocator>
	void list<T, Allocator>::removeNode(Node* node)
	{
		if (node == nullptr)
			return;

		if (node->m_prev)
			node->m_prev->m_next = node->m_next;

		if (node->m_next)
			node->m_next->m_prev = node->m_prev;

		if (node == m_head)
			m_head = node->m_next;

		if (node == m_tail)
			m_tail = node->m_prev;

		node->~Node();
		NodeAllocator().deallocate(node, 1);

		m_size--;
	}
}
