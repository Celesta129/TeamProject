#pragma once

class Finder_Tag
{
public:
	Finder_Tag(const wstring tag) : m_Tag(tag) {};
	~Finder_Tag() = default;
public:
	template <typename T>
	bool operator ()(T& Pair) {
		return Pair.first == m_Tag;
	}
private:
	const wstring m_Tag;
};