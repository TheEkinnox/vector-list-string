#pragma once
#include <cstdio>


#define USE_MOVE 1

class    Foo
{
public:

	Foo()
	{
		m_id = s_count++;
		std::printf("Create Foo_%d\n", m_id);
	}

	Foo(int id) :
		m_id(id)
	{
		std::printf("Create Foo_%d\n", m_id);
	}

	Foo(const Foo& foo)
	{
		m_id = s_count++;
		std::printf("Create Foo_%d copy of Foo_%d\n", m_id, foo.m_id);
	}

	Foo& operator=(const Foo& foo)
	{
		std::printf("Copying Foo_%d into Foo_%d\n", foo.m_id, m_id);
		return *this;
	}

#if USE_MOVE
	Foo& operator=(Foo&& foo) noexcept
	{
		std::printf("Moving Foo_%d into Foo_%d\n", foo.m_id, m_id);
		return *this;
	}

	Foo(Foo&& foo) noexcept
	{
		m_id = s_count++;
		std::printf("Create Foo_%d move copy of Foo_%d\n", m_id, foo.m_id);
	}
#endif

	virtual ~Foo()
	{
		std::printf("Destroy Foo_%d\n", m_id);
	}

	int MyCount() const
	{
		return m_id;
	}


	static void ResetCount()
	{
		s_count = 0;
	}


	static int	Count()
	{
		return s_count;
	}

private:
	int    m_id;

	static int s_count;
};
