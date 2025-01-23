#include "Matrix.h"
#include <string>

Mat::Mat(const std::initializer_list<std::initializer_list<double>> array, uint32_t rows, uint32_t cols) {
	if (array.size() != rows) {
		throw std::runtime_error("Matrix has invalid row dimensions. Your matrix has " + std::to_string(array.size()) + " row(s), but you specified it has " + std::to_string(rows) + " row(s).");
	}

	this->rows = rows;
	this->cols = cols;
	this->array = (double**)malloc(rows * sizeof(double*));

	for (uint8_t c_row = 0; c_row < rows; c_row++) {

		const std::initializer_list<double>* row = array.begin() + c_row;

		if (row->size() != cols) {
			throw std::runtime_error("Matrix has invalid column dimensions. Your matrix has " + std::to_string(row->size()) + " column(s) at row " + std::to_string(1 + c_row) + ", but you specified it has " + std::to_string(cols) + " column(s).");
		}

		this->array[c_row] = (double*)malloc(cols * sizeof(double));
		for (uint8_t c_col = 0; c_col < cols; c_col++) {
			double value = *(row->begin() + c_col);

			this->array[c_row][c_col] = value;
		}
	}
}

Mat::Mat(double** array, uint8_t rows, uint8_t cols) {
	this->rows = rows;
	this->cols = cols;

	Mat::anua_array(this->array, array, rows, cols);
}

Mat::~Mat() {
	Mat::free_resources(this->array, this->rows);
};

Mat::Mat(const Mat& other) {
	this->rows = other.rows;
	this->cols = other.cols;
	Mat::anua_array(this->array, other.array, this->rows, this->cols);
}

void Mat::allocate_array(double**& array, uint8_t rows, uint8_t cols) {
	array = (double**)malloc(rows * sizeof(double*));

	for (uint8_t c_row = 0; c_row < rows; c_row++) {
		array[c_row] = (double*)malloc(cols * sizeof(double));
	}
}

void Mat::free_resources(double** array, uint8_t rows) {
	if (rows == 0 && array == nullptr) {
		throw std::runtime_error("Error: Attempted to free resources from invalid Matrix. Ignoring.");
	}

	for (uint8_t c_row = 0; c_row < rows; c_row++) {
		free(array[c_row]);
	}

	free(array);
}

void Mat::anua_array(double**& new_array, double** original_array, uint8_t rows, uint8_t cols) {
	new_array = (double**)malloc(sizeof(double*) * rows);

	for (uint8_t c_row = 0; c_row < rows; c_row++) {
		new_array[c_row] = (double*)malloc(sizeof(double) * cols);
		for (uint8_t c_col = 0; c_col < cols; c_col++) {
			new_array[c_row][c_col] = original_array[c_row][c_col];
		}
	}
}

void Mat::anua_value(double**& new_array, double value, uint8_t rows, uint8_t cols) {
	new_array = (double**)malloc(sizeof(double*) * rows);

	for (uint8_t c_row = 0; c_row < rows; c_row++) {
		new_array[c_row] = (double*)malloc(sizeof(double) * cols);
		for (uint8_t c_col = 0; c_col < cols; c_col++) {
			new_array[c_row][c_col] = value;
		}
	}
}

Mat Mat::operator+(const Mat& other) const {
	if (this->rows != other.rows || this->cols != other.cols) {
		throw std::runtime_error("Error: Cannot add matrices of different dimensions (" + std::to_string(this->rows) + "x" + std::to_string(this->cols) + " + " + std::to_string(other.rows) + "x" + std::to_string(other.cols) + ")");
	}

	Mat new_mat = *this;

	for (int c_row = 0; c_row < this->rows; c_row++) {
		for (int c_col = 0; c_col < this->cols; c_col++) {
			double value_a = this->array[c_row][c_col];
			double value_b = other.array[c_row][c_col];

			double value = value_a + value_b;

			new_mat.set(value, 1 + c_row, 1 + c_col);
		}
	}

	return new_mat;
}

void Mat::operator+=(const Mat& other) {
	*this = *this + other;
}

Mat Mat::operator-(const Mat& other) const {
	if (this->rows != other.rows || this->cols != other.cols) {
		throw std::runtime_error("Error: Cannot subtract matrices of different dimensions (" + std::to_string(this->rows) + "x" + std::to_string(this->cols) + " - " + std::to_string(other.rows) + "x" + std::to_string(other.cols) + ")");
	}

	Mat new_mat = *this;

	for (int c_row = 0; c_row < this->rows; c_row++) {
		for (int c_col = 0; c_col < this->cols; c_col++) {
			double value_a = this->array[c_row][c_col];
			double value_b = other.array[c_row][c_col];

			double value = value_a - value_b;

			new_mat.set(value, 1 + c_row, 1 + c_col);
		}
	}

	return new_mat;
}

void Mat::operator-=(const Mat& other) {
	*this = *this - other;
}

Mat Mat::operator*(const Mat& other) const {
	if (this->cols != other.rows) {
		throw std::runtime_error("Error: Cannot multiply matrices of different row and column dimensions (" + std::to_string(this->rows) + "x" + std::to_string(this->cols) + " * " + std::to_string(other.rows) + "x" + std::to_string(other.cols) + ")");
	}

	uint8_t new_rows = this->rows;
	uint8_t new_cols = other.cols;

	double** new_array = nullptr;
	Mat::anua_value(new_array, 0, new_rows, new_cols);

	Mat new_mat = Mat(new_array, new_rows, new_cols);

	Mat::free_resources(new_array, new_rows);

	for (int c_row = 0; c_row < this->rows; c_row++) {
		double sum = 0;

		for (uint8_t col_vec = 0; col_vec < other.cols; col_vec++) {
			for (uint8_t c_col = 0; c_col < this->cols; c_col++) {
				double value_a = this->array[c_row][c_col];
				double value_b = other.array[c_col][col_vec];

				double value = value_a * value_b;

				sum += value;
			}

			new_mat.set(sum, 1 + c_row, 1 + col_vec);
			sum = 0;
		}
	}

	return new_mat;
}

void Mat::operator*=(const Mat& other) {
	*this = *this * other;
}

Mat Mat::operator*(const double scalar) const{
	if (scalar == 0) {
		double** new_array = nullptr;
		anua_value(new_array, 0, this->rows, this->cols);

		Mat new_mat = Mat(new_array, this->rows, this->cols);
		return new_mat;
	}

	return *this / (1 / scalar);
}

Mat operator*(const double scalar, const Mat& mat) {
	Mat copy = mat;
	return copy * scalar;
}

void Mat::operator*=(const double scalar) {
	*this = *this * scalar;
}

Mat Mat::operator/(const double scalar) const{
	if (scalar == 0) {
		throw std::runtime_error("Error: Attempted to divide matrix by 0.");
	}

	Mat new_mat = *this;

	for (uint8_t c_row = 0; c_row < this->rows; c_row++) {
		for (uint8_t c_col = 0; c_col < this->cols; c_col++) {
			double value = new_mat.get(c_row + 1, c_col + 1);
			double new_value = value / scalar;

			new_mat.set(new_value, c_row + 1, c_col + 1);
		}
	}

	return new_mat;
}

void Mat::operator/=(const double scalar) {
	*this = *this / scalar;
}

bool Mat::operator==(const Mat& other) const {
	if (this->rows != other.rows || this->cols != other.cols) return false;

	for (uint8_t c_row = 0; c_row < this->rows; c_row++) {
		for (uint8_t c_col = 0; c_col < this->cols; c_col++) {
			if (this->array[c_row][c_col] != other.array[c_row][c_col]) return false;
		}
	}

	return true;
}

bool Mat::operator!=(const Mat& other) const{
	return !(*this == other);
}

void Mat::operator=(Mat other) {
	Mat::free_resources(this->array, this->rows);

	this->rows = other.rows;
	this->cols = other.cols;
	Mat::anua_array(this->array, other.array, this->rows, this->cols);
}

std::ostream& operator<<(std::ostream& os, const Mat& mat) {
	for (int c_row = 0; c_row < mat.rows; c_row++) {
		os << "[ ";
		for (int c_col = 0; c_col < mat.cols; c_col++) {
			double value = mat.get(1 + c_row, 1 +  c_col);
			os << value;

			if (c_col != mat.cols - 1) os << ",";
			os << " ";
		}

		os << "]" << std::endl;
	}

	return os;
}

double Mat::get(uint32_t row, uint32_t col) const {
	if (row < 1 || row > this->rows || col < 1 || col > this->cols) {
		throw std::runtime_error("Error: Invalid element index: (" + std::to_string(row) + ", " + std::to_string(col) + "). Matrix dimensions: " + std::to_string(this->rows) + "x" + std::to_string(this->cols));
	}

	return this->array[row - 1][col - 1];
}

double Mat::set(double value, uint8_t row, uint8_t col) {
	if (row < 1 || row > this->rows || col < 1 || col > this->cols) {
		throw std::runtime_error("Error: Invalid element index: (" + std::to_string(row) + ", " + std::to_string(col) + "). Matrix dimensions: " + std::to_string(this->rows) + "x" + std::to_string(this->cols));
	}

	this->array[row - 1][col - 1] = value;
	return value;
}

Mat Mat::transposed() const {
	const Mat* subject = this;
	const uint8_t new_rows = subject->cols;
	const uint8_t new_cols = subject->rows;

	double** array = nullptr;

	Mat::allocate_array(array, new_rows, new_cols);

	for (uint8_t c_row = 0; c_row < subject->rows; c_row++) {
		for (uint8_t c_col = 0; c_col < subject->cols; c_col++) {
			array[c_col][c_row] = subject->array[c_row][c_col];
		}
	}

	Mat new_mat = Mat(array, new_rows, new_cols);

	Mat::free_resources(array, new_rows);

	return new_mat;
}

Mat Mat::identity_matrix(uint8_t dimension) {
	if (dimension == 2) {
		return Mat(
			{ 
				{1, 0},
				{0, 1}
			}
		, 2, 2);
	}
	else if (dimension == 3) {
		return Mat(
			{
				{1, 0, 0},
				{0, 1, 0},
				{0, 0, 1}
			}
		, 3, 3);
	}
	else if (dimension == 4) {
		return Mat(
			{
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 1}
			}
		, 4, 4);
	}
}

void Mat::print() const {
	for (int c_row = 0; c_row < this->rows; c_row++) {
		std::cout << "[ ";
		for (int c_col = 0; c_col < this->cols; c_col++) {
			double value = this->array[c_row][c_col];
			std::cout << value;

			if (c_col != this->cols - 1) std::cout << ",";
			std::cout << " ";
		}

		std::cout << "]" << std::endl;
	}

	printf("\n");
}

double Mat::dot(const Mat& a, const Mat& b) {
	if (a.cols != 1 && a.rows != 1) {
		throw std::runtime_error("Error: Matrix A is not a vector.");
	}

	if (b.cols != 1 && b.rows != 1) {
		throw std::runtime_error("Error: Matrix B is not a vector.");
	}

	// If A is a row vector
	else if (a.rows == 1) {
		// If B is a row vector
		if (b.rows == 1) {
			if (a.cols != b.cols) {
				throw std::runtime_error("Error: Vectors have differing dimensions.");
			}

			double sum = 0;
			for (uint8_t c_col = 0; c_col < a.cols; c_col++) {
				sum += a.get(1, 1 + c_col) * b.get(1, 1 + c_col);
			}

			return sum;
		}

		// If B is a column vector
		else if (b.cols == 1) {
			if (a.cols != b.rows) {
				throw std::runtime_error("Error: Vectors have differing dimensions.");
			}

			double sum = 0;
			for (uint8_t c_idx = 0; c_idx < a.cols; c_idx++) {
				sum += a.get(1, 1 + c_idx) * b.get(1 + c_idx, 1);
			}

			return sum;
		}
	}

	// If A is a column vector
	else if (a.cols == 1) {
		// If B is a row vector
		if (b.rows == 1) {
			if (a.rows != b.cols) {
				throw std::runtime_error("Error: Vectors have differing dimensions.");
			}

			double sum = 0;
			for (uint8_t c_idx = 0; c_idx < a.rows; c_idx++) {
				sum += a.get(1 + c_idx, 1) * b.get(1, c_idx + 1);
			}

			return sum;
		}

		// If B is a column vector
		else if (b.cols == 1) {
			if (a.rows != b.rows) {
				throw std::runtime_error("Error: Vectors have differing dimensions.");
			}

			double sum = 0;
			for (uint8_t c_row = 0; c_row < a.rows; c_row++) {
				sum += a.get(1 + c_row, 1) * b.get(1 + c_row, 1);
			}

			return sum;
		}
	}
}

double Mat::norm() const {
	return sqrt(Mat::dot(*this, *this));
}

void Mat::normalize() {
	if (this->cols != 1 && this->rows != 1) {
		std::runtime_error("Could not normalize matrix, it is not a vector, meaning none of the dimensions is 1.");
	}

	double length = this->norm();
	if (length == 0) return;

	if (this->cols == 1) {
		for (uint8_t row = 1; row <= this->rows; row++) {
			this->set(this->get(row, 1) / length, row, 1);
		}
	}
	else if (this->rows == 1) {
		for (uint8_t col = 1; col <= this->cols; col++) {
			this->set(this->get(1, col) / length, 1, col);
		}
	}
}

Mat Mat::translation_matrix(double tx, double ty, double tz) {
	Mat translation_matrix = Mat(
		{
			{1, 0, 0, tx},
			{0, 1, 0, ty},
			{0, 0, 1, tz},
			{0, 0, 0, 1}
		}
	, 4, 4);

	return translation_matrix;
}

Mat Mat::scale_matrix(double sx, double sy, double sz) {
	Mat scale_matrix = Mat(
		{
		{sx, 0, 0, 0},
		{0, sy, 0, 0},
		{0, 0, sz, 0},
		{0, 0, 0, 1}
		}
	, 4, 4);

	return scale_matrix;
}