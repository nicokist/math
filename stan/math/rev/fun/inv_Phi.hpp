#ifndef STAN_MATH_REV_FUN_INV_PHI_HPP
#define STAN_MATH_REV_FUN_INV_PHI_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/prim/fun/constants.hpp>
#include <stan/math/prim/fun/inv_Phi.hpp>
#include <cmath>

namespace stan {
namespace math {

namespace internal {
class inv_Phi_vari : public op_v_vari {
 public:
  explicit inv_Phi_vari(vari* avi) : op_v_vari(inv_Phi(avi->val_), avi) {}
  void chain() {
    static const double NEG_HALF = -0.5;
    avi_->adj_ += adj_ * SQRT_TWO_PI / std::exp(NEG_HALF * val_ * val_);
  }
};
}  // namespace internal

/**
 * The inverse of unit normal cumulative density function.
 *
 * See inv_Phi() for the double-based version.
 *
 * The derivative is the reciprocal of unit normal density function,
 *
 * @param p Probability
 * @return The unit normal inverse cdf evaluated at p
 */
inline var inv_Phi(var p) {
  return var(new internal::inv_Phi_vari(p.vi_));
}

}  // namespace math
}  // namespace stan
#endif
