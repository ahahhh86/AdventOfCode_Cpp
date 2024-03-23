/* This module contains a one dimensional vector, which can be accessed by two dimensions. */
// TODO: use std::mdspan, if available

module;

#include <algorithm>
#include <iostream>
#include <vector>

export module Grid2D;

//import std; // TODO: import if IntelliSense works with it
export import Point2D;
import Errors;



// * * * * * * * * * * * * * * * * * * * * * * * * *

export namespace AOC {
	template<typename T>
	class Grid2D
	{
	public:
		using Vector = std::vector<T>;
		using Point = Point2D<int>;

		Grid2D(Point size);

		T& operator[] (Point pos);
		const T& operator[] (Point pos) const;

		auto begin();
		auto end();
		auto cbegin() const;
		auto cend() const;

		auto getGridSize() const;
		void forSub(Point from, Point to, auto fnc);

	private:
		constexpr std::size_t calc1D(Point pos) const;

		Point m_size{};
		std::vector<T> m_vec1D{};
	};



	template<typename T>
	Grid2D<T>::Grid2D(Point size):
		m_size{size}
	{
#pragma warning(suppress : 4365) // C4365 conversion from 'int' to 'const unsigned __int64', signed/unsigned mismatch
		m_vec1D.resize(size.x * size.y);
	}



	template<typename T>
	T& Grid2D<T>::operator[] (Point pos)
	{
		return m_vec1D[calc1D(pos)];
	}



	template<typename T>
	const T& Grid2D<T>::operator[] (Point pos) const
	{
		return m_vec1D[calc1D(pos)];
	}



	template<typename T>
	auto Grid2D<T>::begin()
	{
		return m_vec1D.begin();
	}



	template<typename T>
	auto Grid2D<T>::end()
	{
		return m_vec1D.end();
	}



	template<typename T>
	auto Grid2D<T>::cbegin() const
	{
		return m_vec1D.cbegin();
	}



	template<typename T>
	auto Grid2D<T>::cend() const
	{
		return m_vec1D.cend();
	}



	template<typename T>
	auto Grid2D<T>::getGridSize() const
	{
		return m_size;
	}



	template<typename T>
	void Grid2D<T>::forSub(Point from, Point to, auto fnc)
	{
		if (!from.isInBounds({0, 0}, {m_size.x - 1, m_size.y - 1})
		   || !to.isInBounds({0, 0}, {m_size.x - 1, m_size.y - 1})) {
			throw AOC::aocError("Point2D::forsub(): out of bounds");
		}

		for (int x{from.x}; x <= to.x; ++x) {
			for (int y{from.y}; y <= to.y; ++y) {
				fnc(m_vec1D[calc1D({x, y})]);
			}
		}
	}



	template<typename T>
	constexpr std::size_t Grid2D<T>::calc1D(Point pos) const
	{
		return static_cast<std::size_t>(pos.x) + static_cast<std::size_t>(m_size.x) * static_cast<std::size_t>(pos.y);
	}
}