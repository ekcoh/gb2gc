#ifndef GBM2GC_REDIRECT_H
#define GBM2GC_REDIRECT_H

#include <ostream>

struct redirect_guard
{
	redirect_guard(std::ostream& target, std::streambuf * new_buffer)
		: target(target), old(target.rdbuf(new_buffer))
	{ }

	~redirect_guard()
	{
		target.rdbuf(old);
	}

private:
	std::ostream& target;
	std::streambuf * old;
};

#endif // GBM2GC_REDIRECT_H