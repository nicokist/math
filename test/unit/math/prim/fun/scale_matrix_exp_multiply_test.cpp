#include <stan/math/prim.hpp>
#include <test/unit/util.hpp>
#include <test/unit/math/prim/util.hpp>
#include <gtest/gtest.h>
#include <vector>

template <int N, int M>
inline void test_scale_matrix_exp_multiply() {
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using stan::math::scale_matrix_exp_multiply;

  std::srand(1999);

  Eigen::MatrixXd A = Eigen::MatrixXd::Random(N, N);
  Eigen::Matrix<double, -1, M> B = Eigen::MatrixXd::Random(N, M);
  Eigen::Matrix<double, Dynamic, Dynamic> A0 = A;

  // brute force
  Eigen::Matrix<double, N, M> expAB
      = stan::math::multiply(stan::math::matrix_exp(A0), B);

  // matrix_exp_multiply
  const double t = 1.0;
  Eigen::Matrix<double, N, M> res = scale_matrix_exp_multiply(t, A, B);
  EXPECT_EQ(res.size(), expAB.size());
  for (int l = 0; l < res.size(); ++l) {
    EXPECT_FLOAT_EQ(res(l), expAB(l));
  }
}

TEST(MathMatrixPrimMat, scale_matrix_exp_multiply) {
  // the helper above doesn't handle 0 size inputs
  const double t = 1.0;
  Eigen::MatrixXd A(0, 0);
  Eigen::MatrixXd B(0, 0);
  EXPECT_EQ(stan::math::scale_matrix_exp_multiply(t, A, B).size(), 0);

  Eigen::MatrixXd C(0, 2);
  Eigen::MatrixXd M = stan::math::scale_matrix_exp_multiply(t, A, C);
  EXPECT_EQ(A.rows(), M.rows());
  EXPECT_EQ(C.cols(), M.cols());

  test_scale_matrix_exp_multiply<1, 1>();
  test_scale_matrix_exp_multiply<1, 5>();
  test_scale_matrix_exp_multiply<5, 1>();
  test_scale_matrix_exp_multiply<5, 5>();
  test_scale_matrix_exp_multiply<20, 2>();
}

TEST(MathMatrixPrimMat, scale_matrix_exp_multiply_exception) {
  using stan::math::scale_matrix_exp_multiply;
  const double t = 1.0;
  {  // multiplicable
    Eigen::MatrixXd A(0, 0);
    Eigen::MatrixXd B = Eigen::MatrixXd::Random(1, 2);
    EXPECT_THROW(scale_matrix_exp_multiply(t, A, B), std::invalid_argument);
    EXPECT_THROW(scale_matrix_exp_multiply(t, B, A), std::invalid_argument);
  }

  {  // multiplicable
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(2, 2);
    Eigen::MatrixXd B = Eigen::MatrixXd::Random(3, 2);
    EXPECT_THROW(scale_matrix_exp_multiply(t, A, B), std::invalid_argument);
  }

  {  // square
    Eigen::MatrixXd A(0, 1);
    Eigen::MatrixXd B(1, 2);
    EXPECT_THROW(scale_matrix_exp_multiply(t, A, B), std::invalid_argument);
  }

  {  // square
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(2, 3);
    Eigen::MatrixXd B = Eigen::MatrixXd::Random(3, 2);
    EXPECT_THROW(scale_matrix_exp_multiply(t, A, B), std::invalid_argument);
  }
}
