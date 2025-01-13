#include "catch_amalgamated.hpp"
#include "Matrix.h"

TEST_CASE("Matrix get element", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);

	SECTION("Base case") {
		REQUIRE(mat_a.get(1, 1) == 1);
		REQUIRE(mat_a.get(1, 2) == 2);
		REQUIRE(mat_a.get(1, 3) == 3);
		REQUIRE(mat_a.get(2, 1) == 4);
		REQUIRE(mat_a.get(2, 2) == 5);
		REQUIRE(mat_a.get(2, 3) == 6);
		REQUIRE(mat_a.get(3, 1) == 7);
		REQUIRE(mat_a.get(3, 2) == 8);
		REQUIRE(mat_a.get(3, 3) == 9);
	}
}

TEST_CASE("Matrix set element", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);

	SECTION("Base case") {
		mat_a.set(11, 1, 1);
		mat_a.set(12, 1, 2);
		mat_a.set(13, 1, 3);
		mat_a.set(21, 2, 1);
		mat_a.set(22, 2, 2);
		mat_a.set(23, 2, 3);
		mat_a.set(31, 3, 1);
		mat_a.set(32, 3, 2);
		mat_a.set(33, 3, 3);

		REQUIRE(mat_a.get(1, 1) == 11);
		REQUIRE(mat_a.get(1, 2) == 12);
		REQUIRE(mat_a.get(1, 3) == 13);
		REQUIRE(mat_a.get(2, 1) == 21);
		REQUIRE(mat_a.get(2, 2) == 22);
		REQUIRE(mat_a.get(2, 3) == 23);
		REQUIRE(mat_a.get(3, 1) == 31);
		REQUIRE(mat_a.get(3, 2) == 32);
		REQUIRE(mat_a.get(3, 3) == 33);
	}
}

TEST_CASE("Matrix equality comparison", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);
	Mat mat_a_copy = Mat({ {mat_a.get(1, 1), mat_a.get(1, 2), mat_a.get(1, 3)}, {mat_a.get(2, 1), mat_a.get(2, 2), mat_a.get(2, 3)}, {mat_a.get(3, 1), mat_a.get(3, 2), mat_a.get(3, 3)} }, 3, 3);

	SECTION("Base case") {
		SECTION("Equality") {
			REQUIRE(mat_a == mat_a_copy);
		}

		SECTION("Inequality") {
			REQUIRE(mat_a != mat_b);
		}
	}
}

TEST_CASE("Matrix copy & reassignment", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);

	SECTION("Base case") {
		mat_a = mat_b;

		REQUIRE(mat_a == mat_b);
	}
}

TEST_CASE("Matrix reassignment", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);

	SECTION("Base case") {
		mat_a = Mat({ {mat_b.get(1, 1), mat_b.get(1, 2), mat_b.get(1, 3)}, {mat_b.get(2, 1), mat_b.get(2, 2), mat_b.get(2, 3)}, {mat_b.get(3, 1), mat_b.get(3, 2), mat_b.get(3, 3)}}, 3, 3);
		REQUIRE(mat_a == mat_b);
	}
}

TEST_CASE("Matrix transpose", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat original_mat_a = mat_a;

	SECTION("Base case") {
		mat_a = mat_a.transposed();
		REQUIRE(mat_a.get(1, 1) == 1);
		REQUIRE(mat_a.get(2, 1) == 2);
		REQUIRE(mat_a.get(3, 1) == 3);
		REQUIRE(mat_a.get(1, 2) == 4);
		REQUIRE(mat_a.get(2, 2) == 5);
		REQUIRE(mat_a.get(3, 2) == 6);
		REQUIRE(mat_a.get(1, 3) == 7);
		REQUIRE(mat_a.get(2, 3) == 8);
		REQUIRE(mat_a.get(3, 3) == 9);

		mat_a = mat_a.transposed();
		REQUIRE(mat_a.get(1, 1) == 1);
		REQUIRE(mat_a.get(1, 2) == 2);
		REQUIRE(mat_a.get(1, 3) == 3);
		REQUIRE(mat_a.get(2, 1) == 4);
		REQUIRE(mat_a.get(2, 2) == 5);
		REQUIRE(mat_a.get(2, 3) == 6);
		REQUIRE(mat_a.get(3, 1) == 7);
		REQUIRE(mat_a.get(3, 2) == 8);
		REQUIRE(mat_a.get(3, 3) == 9);
	}
}

TEST_CASE("MATRIX + MATRIX", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);

	Mat correct_result = Mat({ {mat_a.get(1, 1) + mat_b.get(1, 1), mat_a.get(1, 2) + mat_b.get(1, 2), mat_a.get(1, 3) + mat_b.get(1, 3)}, {mat_a.get(2, 1) + mat_b.get(2, 1), mat_a.get(2, 2) + mat_b.get(2, 2), mat_a.get(2, 3) + mat_b.get(2, 3)}, {mat_a.get(3, 1) + mat_b.get(3, 1), mat_a.get(3, 2) + mat_b.get(3, 2), mat_a.get(3, 3) + mat_b.get(3, 3)} }, 3, 3);

	SECTION("Base case") {
		SECTION("MATRIX_A + MATRIX_B") {
			Mat result_ab = mat_a + mat_b;
			REQUIRE(result_ab == correct_result);
		}


		SECTION("MATRIX_B + MATRIX_A") {
			Mat result_ba = mat_b + mat_a;
			REQUIRE(result_ba == correct_result);
		}

	}
}

TEST_CASE("MATRIX += MATRIX", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);
	Mat original_mat_a = mat_a;
	Mat original_mat_b = mat_b;

	SECTION("Base case") {
		SECTION("MATRIX_A += MATRIX_B") {
			mat_a += mat_b;
			REQUIRE(mat_a == original_mat_a + mat_b);
		}


		SECTION("MATRIX_B += MATRIX_A") {
			mat_b += mat_a;
			REQUIRE(mat_b == original_mat_b + mat_a);
		}

	}
}

TEST_CASE("MATRIX - MATRIX", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);

	SECTION("Base case") {
		SECTION("MATRIX_A - MATRIX_B") {
			Mat result_ab = mat_a - mat_b;
			Mat correct_result_ab = Mat({ {mat_a.get(1, 1) - mat_b.get(1, 1), mat_a.get(1, 2) - mat_b.get(1, 2), mat_a.get(1, 3) - mat_b.get(1, 3)}, {mat_a.get(2, 1) - mat_b.get(2, 1), mat_a.get(2, 2) - mat_b.get(2, 2), mat_a.get(2, 3) - mat_b.get(2, 3)}, {mat_a.get(3, 1) - mat_b.get(3, 1), mat_a.get(3, 2) - mat_b.get(3, 2), mat_a.get(3, 3) - mat_b.get(3, 3)} }, 3, 3);
			REQUIRE(result_ab == correct_result_ab);
		}

		SECTION("MATRIX_B - MATRIX_A") {
			Mat result_ba = mat_b - mat_a;
			Mat correct_result_ba = Mat({ {mat_b.get(1, 1) - mat_a.get(1, 1), mat_b.get(1, 2) - mat_a.get(1, 2), mat_b.get(1, 3) - mat_a.get(1, 3)}, {mat_b.get(2, 1) - mat_a.get(2, 1), mat_b.get(2, 2) - mat_a.get(2, 2), mat_b.get(2, 3) - mat_a.get(2, 3)}, {mat_b.get(3, 1) - mat_a.get(3, 1), mat_b.get(3, 2) - mat_a.get(3, 2), mat_b.get(3, 3) - mat_a.get(3, 3)} }, 3, 3);
			REQUIRE(result_ba == correct_result_ba);
		}
	}
}

TEST_CASE("MATRIX -= MATRIX", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);
	Mat original_mat_a = mat_a;
	Mat original_mat_b = mat_b;

	SECTION("Base case") {
		SECTION("MATRIX_A -= MATRIX_B") {
			mat_a -= mat_b;
			REQUIRE(mat_a == original_mat_a - mat_b);
		}


		SECTION("MATRIX_B -= MATRIX_A") {
			mat_b -= mat_a;
			REQUIRE(mat_b == original_mat_b - mat_a);
		}

	}
}

TEST_CASE("MATRIX / SCALAR", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	double scalar = 3;

	SECTION("Base case") {
		Mat result = mat_a / scalar;
		Mat correct_result = Mat({ {mat_a.get(1, 1) / scalar, mat_a.get(1, 2) / scalar, mat_a.get(1, 3) / scalar}, {mat_a.get(2, 1) / scalar, mat_a.get(2, 2) / scalar, mat_a.get(2, 3) / scalar}, {mat_a.get(3, 1) / scalar, mat_a.get(3, 2) / scalar, mat_a.get(3, 3) / scalar} }, 3, 3);

		REQUIRE(result == correct_result);
	}
}

TEST_CASE("MATRIX /= SCALAR", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat original_mat_a = mat_a;
	double scalar = 3;

	SECTION("Base case") {
		mat_a /= scalar;
		REQUIRE(mat_a == original_mat_a / scalar);
	}
}

TEST_CASE("MATRIX * MATRIX", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);

	SECTION("Base case") {
		SECTION("MATRIX_A * MATRIX_B") {
			Mat result_ab = mat_a * mat_b;
			double ab_11 = (mat_a.get(1, 1) * mat_b.get(1, 1)) + (mat_a.get(1, 2) * mat_b.get(2, 1)) + (mat_a.get(1, 3) * mat_b.get(3, 1));
			double ab_12 = (mat_a.get(1, 1) * mat_b.get(1, 2)) + (mat_a.get(1, 2) * mat_b.get(2, 2)) + (mat_a.get(1, 3) * mat_b.get(3, 2));
			double ab_13 = (mat_a.get(1, 1) * mat_b.get(1, 3)) + (mat_a.get(1, 2) * mat_b.get(2, 3)) + (mat_a.get(1, 3) * mat_b.get(3, 3));

			double ab_21 = (mat_a.get(2, 1) * mat_b.get(1, 1)) + (mat_a.get(2, 2) * mat_b.get(2, 1)) + (mat_a.get(2, 3) * mat_b.get(3, 1));
			double ab_22 = (mat_a.get(2, 1) * mat_b.get(1, 2)) + (mat_a.get(2, 2) * mat_b.get(2, 2)) + (mat_a.get(2, 3) * mat_b.get(3, 2));
			double ab_23 = (mat_a.get(2, 1) * mat_b.get(1, 3)) + (mat_a.get(2, 2) * mat_b.get(2, 3)) + (mat_a.get(2, 3) * mat_b.get(3, 3));

			double ab_31 = (mat_a.get(3, 1) * mat_b.get(1, 1)) + (mat_a.get(3, 2) * mat_b.get(2, 1)) + (mat_a.get(3, 3) * mat_b.get(3, 1));
			double ab_32 = (mat_a.get(3, 1) * mat_b.get(1, 2)) + (mat_a.get(3, 2) * mat_b.get(2, 2)) + (mat_a.get(3, 3) * mat_b.get(3, 2));
			double ab_33 = (mat_a.get(3, 1) * mat_b.get(1, 3)) + (mat_a.get(3, 2) * mat_b.get(2, 3)) + (mat_a.get(3, 3) * mat_b.get(3, 3));
			Mat correct_result_ab = Mat({ {ab_11, ab_12, ab_13}, {ab_21, ab_22, ab_23}, {ab_31, ab_32, ab_33} }, 3, 3);

			REQUIRE(result_ab == correct_result_ab);
		}

		SECTION("MATRIX_B * MATRIX_A") {
			Mat result_ba = mat_b * mat_a;
			double ba_11 = (mat_b.get(1, 1) * mat_a.get(1, 1)) + (mat_b.get(1, 2) * mat_a.get(2, 1)) + (mat_b.get(1, 3) * mat_a.get(3, 1));
			double ba_12 = (mat_b.get(1, 1) * mat_a.get(1, 2)) + (mat_b.get(1, 2) * mat_a.get(2, 2)) + (mat_b.get(1, 3) * mat_a.get(3, 2));
			double ba_13 = (mat_b.get(1, 1) * mat_a.get(1, 3)) + (mat_b.get(1, 2) * mat_a.get(2, 3)) + (mat_b.get(1, 3) * mat_a.get(3, 3));

			double ba_21 = (mat_b.get(2, 1) * mat_a.get(1, 1)) + (mat_b.get(2, 2) * mat_a.get(2, 1)) + (mat_b.get(2, 3) * mat_a.get(3, 1));
			double ba_22 = (mat_b.get(2, 1) * mat_a.get(1, 2)) + (mat_b.get(2, 2) * mat_a.get(2, 2)) + (mat_b.get(2, 3) * mat_a.get(3, 2));
			double ba_23 = (mat_b.get(2, 1) * mat_a.get(1, 3)) + (mat_b.get(2, 2) * mat_a.get(2, 3)) + (mat_b.get(2, 3) * mat_a.get(3, 3));

			double ba_31 = (mat_b.get(3, 1) * mat_a.get(1, 1)) + (mat_b.get(3, 2) * mat_a.get(2, 1)) + (mat_b.get(3, 3) * mat_a.get(3, 1));
			double ba_32 = (mat_b.get(3, 1) * mat_a.get(1, 2)) + (mat_b.get(3, 2) * mat_a.get(2, 2)) + (mat_b.get(3, 3) * mat_a.get(3, 2));
			double ba_33 = (mat_b.get(3, 1) * mat_a.get(1, 3)) + (mat_b.get(3, 2) * mat_a.get(2, 3)) + (mat_b.get(3, 3) * mat_a.get(3, 3));

			Mat correct_result_ba = Mat({ {ba_11, ba_12, ba_13}, {ba_21, ba_22, ba_23}, {ba_31, ba_32, ba_33} }, 3, 3);
			REQUIRE(result_ba == correct_result_ba);
		}
	}
}

TEST_CASE("MATRIX *= MATRIX", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat mat_b = Mat({ {9, 8, 7}, {6, 5, 4}, {3, 2, 1} }, 3, 3);
	Mat original_mat_a = mat_a;
	Mat original_mat_b = mat_b;

	SECTION("Base case") {
		SECTION("MATRIX_A *= MATRIX_B") {
			mat_a *= mat_b;
			REQUIRE(mat_a == original_mat_a * mat_b);
		}

		SECTION("MATRIX_B *= MATRIX_A") {
			mat_b *= mat_a;
			REQUIRE(mat_b == original_mat_b * mat_a);
		}

	}
}


TEST_CASE("MATRIX * SCALAR", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	double scalar = 3;

	Mat correct_result = Mat({ {mat_a.get(1, 1) * scalar, mat_a.get(1, 2) * scalar, mat_a.get(1, 3) * scalar}, {mat_a.get(2, 1) * scalar, mat_a.get(2, 2) * scalar, mat_a.get(2, 3) * scalar}, {mat_a.get(3, 1) * scalar, mat_a.get(3, 2) * scalar, mat_a.get(3, 3) * scalar } }, 3, 3);

	SECTION("Base case") {
		SECTION("SCALAR * MATRIX") {
			Mat result_sa = scalar * mat_a;
			REQUIRE(result_sa == correct_result);
		}

		SECTION("MATRIX * SCALAR") {
			Mat result_as = mat_a * scalar;
			REQUIRE(result_as == correct_result);
		}
	}
}

TEST_CASE("MATRIX *= SCALAR", "[matrix]") {
	Mat mat_a = Mat({ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }, 3, 3);
	Mat original_mat_a = mat_a;
	double scalar = 3;

	SECTION("Base case") {
		mat_a *= scalar;
		REQUIRE(mat_a == original_mat_a * scalar);
	}
}

TEST_CASE("VECTOR DOT PRODUCT", "[matrix]") {
	Mat vec_a = Mat({ {1}, {2}, {1} }, 3, 1);
	Mat vec_b = Mat({ {0}, {1}, {1} }, 3, 1);

	SECTION("Base case") {
		SECTION("VECTOR_A IS A COLUMN VECTOR") {
			SECTION("VECTOR_B IS A COLUMN VECTOR") {
				SECTION("COLUMN(VECTOR_A) * COLUMN(VECTOR_B)") {
					double dot = (vec_a.get(1, 1) * vec_b.get(1, 1)) + (vec_a.get(2, 1) * vec_b.get(2, 1)) + (vec_a.get(3, 1) * vec_b.get(3, 1));
					REQUIRE(Mat::dot(vec_a, vec_b) == dot);
				}

				SECTION("COLUMN(VECTOR_B) * COLUMN(VECTOR_A)") {
					double dot = (vec_b.get(1, 1) * vec_a.get(1, 1)) + (vec_b.get(2, 1) * vec_a.get(2, 1)) + (vec_b.get(3, 1) * vec_a.get(3, 1));
					REQUIRE(Mat::dot(vec_b, vec_a) == dot);
				}
			}

			SECTION("VECTOR_B IS A ROW VECTOR") {
				vec_b = vec_b.transposed();

				SECTION("COLUMN(VECTOR_A) * ROW(VECTOR_B)") {
					double dot = (vec_a.get(1, 1) * vec_b.get(1, 1)) + (vec_a.get(2, 1) * vec_b.get(1, 2)) + (vec_a.get(3, 1) * vec_b.get(1, 3));
					REQUIRE(Mat::dot(vec_a, vec_b) == dot);
				}

				SECTION("ROW(VECTOR_B) * COLUMN(VECTOR_A)") {
					double dot = (vec_b.get(1, 1) * vec_a.get(1, 1)) + (vec_b.get(1, 2) * vec_a.get(2, 1)) + (vec_b.get(1, 3) * vec_a.get(3, 1));
					REQUIRE(Mat::dot(vec_b, vec_a) == dot);
				}
			}
		}

		SECTION("VECTOR_A IS A ROW VECTOR") {
			vec_a = vec_a.transposed();
			SECTION("VECTOR_B IS A COLUMN VECTOR") {
				SECTION("ROW(VECTOR_A) * COLUMN(VECTOR_B)") {
					double dot = (vec_a.get(1, 1) * vec_b.get(1, 1)) + (vec_a.get(1, 2) * vec_b.get(2, 1)) + (vec_a.get(1, 3) * vec_b.get(3, 1));
					REQUIRE(Mat::dot(vec_a, vec_b) == dot);
				}

				SECTION("COLUMN(VECTOR_B) * ROW(VECTOR_A)") {
					double dot = (vec_b.get(1, 1) * vec_a.get(1, 1)) + (vec_b.get(2, 1) * vec_a.get(1, 2)) + (vec_b.get(3, 1) * vec_a.get(1, 3));
					REQUIRE(Mat::dot(vec_b, vec_a) == dot);
				}
			}

			SECTION("VECTOR_B IS A ROW VECTOR") {
				vec_b = vec_b.transposed();

				SECTION("ROW(VECTOR_A) * ROW(VECTOR_B)") {
					double dot = (vec_a.get(1, 1) * vec_b.get(1, 1)) + (vec_a.get(1, 2) * vec_b.get(1, 2)) + (vec_a.get(1, 3) * vec_b.get(1, 3));
					REQUIRE(Mat::dot(vec_a, vec_b) == dot);
				}

				SECTION("ROW(VECTOR_B) * ROW(VECTOR_A)") {
					double dot = (vec_b.get(1, 1) * vec_a.get(1, 1)) + (vec_b.get(1, 2) * vec_a.get(1, 2)) + (vec_b.get(1, 3) * vec_a.get(1, 3));
					REQUIRE(Mat::dot(vec_b, vec_a) == dot);
				}
			}
		}
	}
}

TEST_CASE("VECTOR NORM(LENGTH)", "[matrix]") {
	Mat vec_a = Mat({ {1}, {2}, {1} }, 3, 1);

	SECTION("Base case") {
		double norm = sqrt(pow(vec_a.get(1, 1), 2) + pow(vec_a.get(2, 1), 2) + pow(vec_a.get(3, 1), 2));
		REQUIRE(vec_a.norm() == norm);
	}
}