#pragma once


template<typename T, class Allocator>
class    MonContainer
{
public:
	MonContainer()
	{
		// Create array of 5 elements
		m_elements = Allocator().allocate(5); // allocate data
		for (int i = 0; i < 5; ++i)
		{
			new (&m_elements[i]) T; // construct each element with placement new
		}
	}

	~MonContainer()
	{
		for (int i = 0; i < 5; ++i)
		{
			m_elements[i].~T(); // destroy each element
		}

		// Destroy the 5 elements
		Allocator().deallocate(m_elements, 5);
	}

private:
	T*    m_elements = nullptr;
};

