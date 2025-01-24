#pragma once
#include <vector>
#include <iostream>

// Matrix dimension variables do not need to be 32 bit long, 8 bit is sufficient for the purposes of graphics programming, however, std::cout prints a character instead of a number when the value is 8 bits long, which is why I went with 32 bit values where I need to print, and 8 bit values where I only need to perform calculations.
class Mat {
private:
	double** array = nullptr;

	/// <summary>
	/// Another class constructor. Can only be triggered from the class' member functions, used for internal purposes. This alternative takes in a pre-existing double 2-dimensional array as input (not an initializer list).
	/// </summary>
	/// <param name="array">A 2-dimensional array containing the elements.</param>
	/// <param name="rows">The number of rows.</param>
	/// <param name="cols">The number of columns.</param>
	Mat(double** array, uint8_t rows, uint8_t cols);
public:
	uint32_t rows = 0;
	uint32_t cols = 0;

	/// <summary>
	/// The class constructor.
	/// </summary>
	/// <param name="array">A 2-dimensional array containing the elements.</param>
	/// <param name="rows">The number of rows.</param>
	/// <param name="cols">The number of columns.</param>
	Mat(const std::initializer_list<std::initializer_list<double>> array, uint32_t rows, uint32_t cols);

	/// <summary>
	/// The copy constructor. Copies the contents of the matrix `other` (right hand side) to the left hand side variable. Triggered when setting one matrix to another (i.e. matrix_a = matrix_b), where `matrix_b` is the right hand side (the `other` variable.
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	Mat(const Mat& other);

	/// <summary>
	/// The class destructor. Frees the memory allocated for the array which holds the values.
	/// </summary>
	~Mat();

	/// <summary>
	/// Allocates memory for the given array.
	/// </summary>
	/// <param name="array">A 2-dimensional array containing the elements.</param>
	/// <param name="rows">The number of rows.</param>
	/// <param name="cols">The number of columns.</param>
	static void allocate_array(double**& array, uint8_t rows, uint8_t cols);

	/// <summary>
	/// Allocates and updates the new array with the values in the given array. (anua is an acronym for allocate and update array)
	/// </summary>
	/// <param name="new_array">The new array variable pointer. Its taken as reference, so this value gets updated.</param>
	/// <param name="original_array">The array which new array will get its values from.</param>
	/// <param name="rows">The number of rows.</param>
	/// <param name="cols">The number of columns.</param>
	static void anua_array(double**& new_array, double** original_array, uint8_t rows, uint8_t cols);

	/// <summary>
	/// Allocates and updates the new array with the same (double) value everywhere. (anua is an acronym for allocate and update array)
	/// </summary>
	/// <param name="new_array">The new array variable pointer. Its taken as reference, so this value gets updated.</param>
	/// <param name="value">The array which the new array will get its values from.</param>
	/// <param name="rows">The number of rows.</param>
	/// <param name="cols">The number of columns.</param>
	static void anua_value(double**& new_array, double value, uint8_t rows, uint8_t cols);

	/// <summary>
	/// Frees the memory allocated for the given array.
	/// </summary>
	/// <param name="array">A 2-dimensional array containing the elements.</param>
	/// <param name="rows">The number of rows.</param>
	static void free_resources(double** array, uint8_t rows);

	/// <summary>
	/// Operator overloading for adding a matrix to another.
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	/// <returns>The matrix resulting from the addition.</returns>
	Mat operator+(const Mat& other) const;

	/// <summary>
	/// Operator overloading for addition and reassignment. (i.e. matrix_a += other).
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	void operator+=(const Mat& other);

	/// <summary>
	/// Operator overloading for subtracting a matrix from the other.
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	/// <returns>The matrix resulting from the subtraction.</returns>
	Mat operator-(const Mat& other) const;

	/// <summary>
	/// Operator overloading for subtraction and reassignment. (i.e. matrix_a -= other).
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	void operator-=(const Mat& other);

	/// <summary>
	/// Operator overloading for multiplying a matrix by another matrix.
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	/// <returns>The matrix resulting from the multiplication.</returns>
	Mat operator*(const Mat& other) const;



	/// <summary>
	/// Operator overloading for multiplication and reassignment. (i.e. matrix_a *= other).
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	void operator*=(const Mat& other);

	/// <summary>
	/// Operator overloading for a matrix multiplying a scalar.
	/// </summary>
	/// <param name="scalar">A scalar value.</param>
	/// <returns>A copy of the matrix after the multiplication.</returns>
	Mat operator*(const double scalar) const;

	/// <summary>
	/// Operator overloading for a scalar multiplying a matrix. (i.e. scalar * matrix)
	/// </summary>
	/// <param name="scalar">A scalar value.</param>
	/// <param name="mat">The class instance.</param>
	/// <returns>A copy of the matrix after the multiplication.</returns>
	friend Mat operator*(const double scalar, const Mat& mat);

	/// <summary>
	/// Operator overloading for multiplication and reassignment. (i.e. matrix *= scalar).
	/// </summary>
	/// <param name="scalar">A scalar value.</param>
	void operator*=(const double scalar);

	/// <summary>
	/// Operator overloading for a division.
	/// </summary>
	/// <param name="scalar">A scalar value.</param>
	/// <returns>A copy of the matrix after the division.</returns>
	Mat operator/(const double scalar) const;

	/// <summary>
	/// Operator overloading for division and reassignment. (i.e. matrix /= scalar).
	/// </summary>
	/// <param name="scalar">A scalar value.</param>
	void operator/=(const double scalar);

	/// <summary>
	/// Operator overloading for a comparison.
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	/// <returns>Whether the two matrices have equivalent values.</returns>
	bool operator==(const Mat& other) const;

	/// <summary>
	/// Operator overloading for a comparison.
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	/// <returns>Whether the two matrices have equivalent values.</returns>
	bool operator!=(const Mat& other) const;

	/// <summary>
	/// Operator overloading for reassignment (i.e. matrix_a = matrix_b).
	/// </summary>
	/// <param name="other">The other matrix (right hand side).</param>
	void operator=(Mat other);

	/// <summary>
	/// Operator overloading for the output stream (i.e. std::cout << mat).
	/// </summary>
	/// <param name="os">An output stream object (i.e. std::ostream& / std::cout). (The left hand side)</param>
	/// <param name="mat">The class instance.</param>
	/// <returns>The output stream after being formatted.</returns>
	friend std::ostream& operator<<(std::ostream& os, const Mat& mat);

	/// <summary>
	/// Gets the element at a given row and column.
	/// </summary>
	/// <returns>(double) The element at the given row and column.</returns>
	double get(uint32_t row, uint32_t col) const;

	/// <summary>
	/// Updates the element at a given row and column.
	/// </summary>
	/// <returns>(double) The new element at the given row and column.</returns>
	double set(double value, uint8_t row, uint8_t col);

	/// <summary>
	/// Transposes a copy of the matrix.
	/// </summary>
	/// <returns>(double) A copy of the current matrix transposed.</returns>
	Mat transposed() const;

	/// <summary>
	/// Returns the square identity matrix with the given dimension (hardcoded for 2, 3, and 4 dimensions)
	/// </summary>
	/// <returns>(Mat) A square identity matrix with the requested dimension.</returns>
	static Mat identity_matrix(uint8_t dimension);

	/// <summary>
	/// Prints the matrix.
	/// </summary>
	void print() const;	

	/// <summary>
	/// Returns the dot product of vector a and b. (Can also be defined as `vec_a * vec_b.transpose()` where both `vec_a` and `vec_b` are row vectors, although I didn't)
	/// </summary>
	/// <param name="a">Vector</param>
	/// <param name="b">Vector</param>
	/// <returns>(double) The dot product between the two vectors.</returns>
	static double dot(const Mat& a, const Mat& b);
	
	/// <summary>
	/// The norm (length) of the vector.
	/// </summary>
	/// <returns>(double) The norm (length) of the vector.</returns>
	double norm() const;

	/// <summary>
	/// Divides each element in the vector by the vector norm.
	/// </summary>
	void normalize();

	/// <summary>
	/// Returns a 4x4 translation matrix with the provided parameters
	/// </summary>
	/// <param name="tx">Translation along the x-axis</param>
	/// <param name="ty">Translation along the y-axis</param>
	/// <param name="tz">Translation along the z-axis</param>
	/// <returns>A 4x4 translation matrix</returns>
	static Mat translation_matrix(double tx, double ty, double tz);

	/// <summary>
	/// Returns a 4x4 scaling matrix with the provided parameters
	/// </summary>
	/// <param name="sx">Scaling along the x-axis</param>
	/// <param name="sy">Scaling along the y-axis</param>
	/// <param name="sz">Scaling along the z-axis</param>
	/// <returns>A 4x4 scaling matrix</returns>
	static Mat scale_matrix(double sx, double sy, double sz);
};
