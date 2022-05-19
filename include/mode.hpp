#ifndef mode_hpp
#define mode_hpp "Access Mode"

#include "fmt.hpp"

namespace env::file
{
	enum mode
	{
		ex   = 1 << 000, // execute
		wr   = 1 << 001, // write
		rd   = 1 << 002, // read
		ok   = 1 << 003, // exists
		xu   = 1 << 004, // exclusive
		un   = 1 << 005, // truncate
		app  = 1 << 006, // append
		txt  = 1 << 007, // text
		bin  = 1 << 010, // binary
		blk  = 1 << 011, // block device
		chr  = 1 << 012, // character device
		dir  = 1 << 013, // directory
		fifo = 1 << 014, // byte stream
		lnk  = 1 << 015, // symbolic link
		reg  = 1 << 016, // regular file
		sock = 1 << 017, // domain socket

		rw   = rd | wr, // read/write
		wo   = rw | ok, // read/write/exists
		rwx  = rw | ex, // read/write/execute
		rx   = rd | ex, // read/execute
		wx   = wr | ex, // write/execute
		ov   = wo | un, // overwrite
	};

	constexpr int owner(int mask)
	{
		return (mask & rwx) << 6;
	}

	constexpr int group(int mask)
	{
		return (mask & rwx) << 3;
	}

	constexpr int other(int mask)
	{
		return (mask & rwx) << 0;
	}

	enum permit
	{
		owner_r = owner(rd),
		owner_w = owner(wr),
		owner_x = owner(ex),
		group_r = group(rd),
		group_w = group(wr),
		group_x = group(ex),
		other_r = other(rd),
		other_w = other(wr),
		other_x = other(ex),
	};

	inline permit owner(mode mask)
	{
		return static_cast<permit>(owner(static_cast<int>(mask)));
	}

	inline permit group(mode mask)
	{
		return static_cast<permit>(group(static_cast<int>(mask)));
	}

	inline permit other(mode mask)
	{
		return static_cast<permit>(other(static_cast<int>(mask)));
	}

	int to_mode(int); // file open mode
	int to_permit(int); // file permissions
	fmt::string to_string(int); // fopen style string

	// Check for access to the file at path
	bool fail(fmt::view path, mode = ok);
}

#endif // file
