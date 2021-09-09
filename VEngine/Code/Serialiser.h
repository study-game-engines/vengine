#pragma once
#include <fstream>
#include <ostream>
#include <istream>
#include <string>

struct Properties;

enum class OpenMode
{
	Out = std::ios_base::out,
	In = std::ios_base::in
};

//TODO: split serialiser into Serialiser and Deserialiser and use std::ofstream/ifstream instead of o/istreams
struct Serialiser
{
	std::filebuf fb;
	std::ostream os;
	std::istream is;

	Serialiser(const std::string& filename, OpenMode mode);
	~Serialiser();
	void Serialise(Properties props);
	void Deserialise(Properties props);

	template <typename T>
	void WriteLine(T arg)
	{
		os << arg << "\n";
	}
};
