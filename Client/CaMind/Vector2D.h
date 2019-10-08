#pragma once

template<typename T>
class Vector2D
{
public:
	T x;
	T y;

	Vector2D()
	{
	}

	Vector2D(const Vector2D<T>& dest)
		: x(dest.x), y(dest.y)
	{

	}

	Vector2D(Vector2D<T>&& dest)
		: x(dest.x), y(dest.y)
	{

	}

	Vector2D(T _x, T _y)
		: x(_x), y(_y)
	{

	}

	T operator*(Vector2D<T> dest)
	{
		return (this->x * dest.x + this->y * dest.y);
	}

	Vector2D operator+(Vector2D<T> dest)
	{
		return Vector2D(x + dest.x, y + dest.y);
	}

	Vector2D operator-(Vector2D<T> dest)
	{
		return Vector2D(x - dest.x, y - dest.y);
	}

	Vector2D& operator=(const Vector2D<T>& dest)
	{
		this->x = dest.x;
		this->y = dest.y;

		return *this;
	}

};
