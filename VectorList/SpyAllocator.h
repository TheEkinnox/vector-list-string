#pragma once

#include <vector>

class CMemorySpy
{
public:
	void    NotifyAlloc(size_t size, size_t count, void* address)
	{
		std::printf("*** Allocate block %d : %llu elem of %llu bytes\n", s_curId, count, size);
		m_blocks.push_back({ size, count, address, s_curId++ });
	}

	void    NotifyDealloc(void* address, size_t count)
	{
		for (size_t i = 0; i < m_blocks.size(); ++i)
		{
			if (m_blocks[i].address == address)
			{
				if (m_blocks[i].count == count)
				{
					std::printf("*** Deallocate block %d : %llu elem of %llu bytes\n", m_blocks[i].id, m_blocks[i].count, m_blocks[i].size);
				}
				else
				{
					std::printf("*** Trying to deallocate block %d of %llu elem while %llu where allocated !\n", m_blocks[i].id, count, m_blocks[i].count);
				}
				if (i + 1 < m_blocks.size())
				{
					m_blocks[i] = m_blocks[m_blocks.size() - 1];
				}
				m_blocks.resize(m_blocks.size() - 1);
				return;
			}
		}

		std::printf("*** Deallocate failed !\n");
	}

	void    CheckLeaks()
	{
		if (m_blocks.empty())
		{
			std::printf("*** No memory leak\n");
		}
		else
		{
			std::printf("*** %llu memory leaks detected !\n", m_blocks.size());
			for (const MemBlock& block : m_blocks)
			{
				std::printf("- Leak of block %d : %llu elem of size %llu \n", block.id, block.count, block.size);
			}
		}
	}

	static int    s_curId;

private:
	struct MemBlock
	{
		size_t	size;
		size_t	count;
		void*	address;
		int		id;
	};

	std::vector<MemBlock>    m_blocks;
};




extern CMemorySpy	g_memorySpy;


template <class T>
struct SpyAllocator : std::allocator<T>
{
	typedef T			value_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;


	SpyAllocator(/*vector args*/) = default;


	template<class U>
	SpyAllocator(const SpyAllocator<U>& other) {}


	template<class U>
	struct rebind
	{
		using other = SpyAllocator<U>;
	};


	T*    allocate(std::size_t n)
	{
		T* p = (T*) new char[sizeof(T) * n];

		g_memorySpy.NotifyAlloc(sizeof(T), n, p);
		return p;
	};


	void    deallocate(T* p, std::size_t n)
	{
		g_memorySpy.NotifyDealloc(p, n);
		delete (char*)p;
	}
};


template <class T, class U>
bool    operator==(const SpyAllocator<T>&, const SpyAllocator<U>&) { return false; }


template <class T, class U>
bool    operator!=(const SpyAllocator<T>&, const SpyAllocator<U>&) { return false; }