#ifndef STAN_MATH_REV_META_IS_VAR_HPP
#define STAN_MATH_REV_META_IS_VAR_HPP

#include <stan/math/rev/core/var.hpp>
#include <stan/math/prim/meta/is_var.hpp>
#include <stan/math/prim/meta.hpp>
#include <type_traits>

namespace stan {

namespace internal {
template <typename T>
struct is_var_impl : std::false_type {};

template <typename T, typename U>
struct is_var_impl<math::var_value<T, U>> : std::true_type {};
}  // namespace internal
/** \ingroup type_trait
 * Specialization for checking if value of T minus cv qualifier is a var_value.
 */
template <typename T>
struct is_var<T,
              std::enable_if_t<internal::is_var_impl<std::decay_t<T>>::value>>
    : std::true_type {};

namespace internal {

/** \ingroup type_trait
 * This underlying implementation is used when the type is not an std vector.
 */
template <typename... Args>
struct is_var_value_impl : std::false_type {};

/** \ingroup type_trait
 * This specialization implementation has a static member named value when the
 * template type is an std vector.
 */
template <typename... Args>
struct is_var_value_impl<math::var_value<Args...>> : std::true_type {};

}  // namespace internal

template <typename T>
struct is_var_value<
    T, std::enable_if_t<internal::is_var_value_impl<std::decay_t<T>>::value>>
    : std::true_type {};

template <typename T, typename = void>
struct get_var_value {
  using type = value_type_t<T>;
};

// until we figure out how to get inner type for vari_value
template <typename T>
struct get_var_value<T, std::enable_if_t<is_var<T>::value>> {
  using type = typename std::decay_t<T>::Scalar;
};

template <typename T, typename = void>
struct get_var_vari_value {
  using type = value_type_t<T>;
};

// until we figure out how to get inner type for vari_value
template <typename T>
struct get_var_vari_value<T, std::enable_if_t<is_var_value<T>::value>> {
  using type = typename std::decay_t<T>::vari_type;
};

template <typename T>
struct is_eigen_var
    : bool_constant<math::conjunction<is_eigen<T>,
                                      is_var_value<scalar_type_t<T>>>::value> {
};

// until we figure out how to get inner type for vari_value
template <typename T>
struct get_var_vari_value<T, std::enable_if_t<is_eigen_var<T>::value>> {
  using type
      = math::vari_value<Eigen::Matrix<value_type_t<T>, T::RowsAtCompileTime,
                                       T::ColsAtCompileTime>>;
};

template <typename T>
using get_var_vari_value_t = typename get_var_vari_value<std::decay_t<T>>::type;

template <typename T, typename = void>
struct get_var_scalar {
  using type = scalar_type_t<T>;
};

template <typename T>
struct get_var_scalar<T, require_var_value_t<T>> {
  using type = typename std::decay_t<T>::Scalar;
};

template <typename T>
using get_var_scalar_t = typename get_var_scalar<std::decay_t<T>>::type;

template <typename T>
struct scalar_type<T, std::enable_if_t<is_var_value<T>::value>> {
  using type = math::var_value<double>;
};

template <typename... Targs>
using require_any_var2 = math::disjunction<
  is_var<value_type_t<Targs>>...,
  std::is_same<std::decay_t<Targs>, math::var_value<Eigen::MatrixXd>>...,
  std::is_same<std::decay_t<Targs>, math::var_value<Eigen::VectorXd>>...,
  std::is_same<std::decay_t<Targs>, math::var_value<Eigen::RowVectorXd>>...>;

template <typename... Targs>
using require_any_var2_t = require_t<require_any_var2<Targs...>>;

template <typename T> 
using require_scalar2_t = require_t<std::is_same<std::decay_t<T>, scalar_type_t<std::decay_t<T>>>>;

template <typename T>
using require_matrix2 = math::disjunction<is_eigen<T>,
					  std::is_same<std::decay_t<T>, math::var_value<Eigen::MatrixXd>>,
					  std::is_same<std::decay_t<T>, math::var_value<Eigen::VectorXd>>,
					  std::is_same<std::decay_t<T>, math::var_value<Eigen::RowVectorXd>>>;

template <typename T>
using require_matrix2_t = require_t<require_matrix2<T>>;

template <typename T>
using require_vector2 = math::disjunction<is_eigen_col_vector<T>,
					 std::is_same<std::decay_t<T>, math::var_value<Eigen::VectorXd>>>;
  
template <typename T>
using require_vector2_t = require_t<require_vector2<T>>;

template <typename T>
using require_row_vector2 = math::disjunction<is_eigen_row_vector<T>,
					     std::is_same<std::decay_t<T>, math::var_value<Eigen::RowVectorXd>>>;

template <typename T>
using require_row_vector2_t = require_t<require_row_vector2<T>>;
  
}  // namespace stan
#endif
