/* This unit proviedes small class template that creates a point with two dimensions
   Comparision operators are implemented (mainly to make a container sortable) */

module;

#include <format>
#include <iostream>
#include <stdexcept>

export module Point2D;

//import std; // TODO: import if IntelliSense works with it



// * * * * * * * * * * * * * * * * * * * * * * * * *

export namespace AOC {
	template<typename T>
	struct Point2D
	{
		T x{};
		T y{};

		Point2D operator-() const;
		Point2D& operator+=(Point2D p);
		Point2D& operator-=(Point2D p);

		// for use as index in arrays without waring
		std::size_t X() const;
		std::size_t Y() const;

		bool isInBounds(Point2D min, Point2D max) const;
	};



	template<typename T>
	Point2D<T> operator+(Point2D<T> p1, Point2D<T> p2);
	template<typename T>
	Point2D<T> operator-(Point2D<T> x, Point2D<T> y);

	template<typename T>
	bool operator==(Point2D<T> p1, Point2D<T> p2);
	template<typename T>
	bool operator!=(Point2D<T> x, Point2D<T> y);

	template<typename T>
	bool operator<(Point2D<T> p1, Point2D<T> p2);		// for sorting, atm no actual meaning
	template<typename T>
	bool operator>(Point2D<T> x, Point2D<T> y);
	template<typename T>
	bool operator<=(Point2D<T> x, Point2D<T> y);
	template<typename T>
	bool operator>=(Point2D<T> x, Point2D<T> y);

	template<typename T>
	std::ostream& operator<< (std::ostream& out, Point2D<T> p);
	template<typename T>
	std::istream& operator>> (std::istream& in, Point2D<T>& point);



	template<typename T>
	Point2D<T> Point2D<T>::operator-() const
	{
		return {-x, -y};
	}

	template<typename T>
	Point2D<T>& Point2D<T>::operator+=(Point2D<T> p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	template<typename T>
	Point2D<T>& Point2D<T>::operator-=(Point2D<T> p)
	{
		(*this) += (-p);
		return *this;
	}



	template<typename T>
	std::size_t Point2D<T>::X() const
	{
		if (x < 0) { throw std::underflow_error("Point2D.X()"); }
		return static_cast<std::size_t>(x);
	}

	template<typename T>
	std::size_t Point2D<T>::Y() const
	{
		if (y < 0) { throw std::underflow_error("Point2D.X()"); }
		return static_cast<std::size_t>(y);
	}



	template<typename T>
	bool Point2D<T>::isInBounds(Point2D<T> min, Point2D<T> max) const
	{
		return min.x <= x && x <= max.x
			&& min.y <= y && y <= max.y;
	}



	template<typename T>
	Point2D<T> operator+(Point2D<T> p1, Point2D<T> p2)
	{
		return Point2D<T>{p1.x + p2.x, p1.y + p2.y};
	}

	template<typename T>
	Point2D<T> operator-(Point2D<T> x, Point2D<T> y)
	{
		return x + -y;
	}



	template<typename T>
	bool operator==(Point2D<T> p1, Point2D<T> p2)
	{
		return p1.x == p2.x && p1.y == p2.y;
	}

	template<typename T>
	bool operator!=(Point2D<T> x, Point2D<T> y)
	{
		return !(x == y);
	}



	template<typename T>
	bool operator<(Point2D<T> p1, Point2D<T> p2)
	{
		if (p1.x == p2.x) {
			return p1.y < p2.y;
		} else {
			return p1.x < p2.x;
		}
	}

	template<typename T>
	bool operator>(Point2D<T> x, Point2D<T> y)
	{
		return y < x;
	}

	template<typename T>
	bool operator<=(Point2D<T> x, Point2D<T> y)
	{
		return !(y < x);
	}

	template<typename T>
	bool operator>=(Point2D<T> x, Point2D<T> y)
	{
		return !(x < y);
	}



	template<typename T>
	std::ostream& operator<< (std::ostream& out, Point2D<T> p)
	{
		out << std::format("(x: {}, y: {})", p.x, p.y);
		return out;
	}

	template<typename T>
	std::istream& operator>> (std::istream& in, Point2D<T>& point)
	{
		in >> point.x;
		in.ignore(1);		// throw away the separator char
		in >> point.y;

		return in;
	}
}