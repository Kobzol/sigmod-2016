#pragma once

#include <cstring>

template <typename T>
class Vector {
public:
	Vector() : size(0), capacity(0), data(nullptr)
	{

	}
	Vector(size_t initial_capacity) : size(0), capacity(initial_capacity),  data(nullptr)
	{
		this->reallocate(initial_capacity);
	}
	~Vector()
	{
		delete[] this->data;
	}

	inline void reserve(size_t capacity)
	{
		if (capacity > this->capacity)
		{
			this->reallocate(capacity);
		}
	}

	inline void push_back_no_allocation(const T& t)
	{
		this->data[this->size++] = t;
	}

	inline void push_back_block(const T* t, size_t length)
	{
		size_t s = this->size;
		this->size += length;
		if (this->size >= this->capacity)
		{
			this->reallocate();
		}
		memcpy(&(this->data[s]), t, length * sizeof(T));
	}

	inline void push_back(const T& t)
	{
		this->data[this->size] = t;
		this->size++;

		if (this->size == this->capacity)
		{
			this->reallocate();
		}
	}

	inline T& operator[](int index)
	{
		return this->data[index];
	}

	inline void clear()
	{
		this->size = 0;
	}

	size_t size;
	size_t capacity;
	T* data;

private:
	//Vector(Vector& ref) = delete;
	//Vector& operator=(Vector& other) = delete;

	void reallocate(size_t capacity)
	{
		if (this->data != nullptr)
		{
			T* new_data = new T[capacity];
			memcpy(new_data, this->data, this->size * sizeof(T));

			delete[] this->data;

			this->data = new_data;
		}
		else this->data = new T[capacity];

		this->capacity = capacity;

	}
	void reallocate()
	{
		this->reallocate(this->capacity * 2);
	}
};