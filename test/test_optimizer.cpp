/**
 * @file test_optimizer.cpp
 * @author David Huang, Derek Huang
 * @brief C++ program testing optimizer construction + stream
 * @copyright MIT License
 */

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <type_traits>
#include <vector>

#include "functions.hpp"
#include "optimizer.hpp"

namespace {

/**
 * Ensure the `optimizer` produces the expected activation values.
 *
 * @note We have to check the values since comparing the addresses of function
 *  defined in a shared library is complicated due to thunking on platforms
 *  like Windows, where modules get the address of the thunk instead of the
 *  actual function address in the defining DLL.
 *
 * @param opt Optimizer instance to check
 * @returns Number of errors
 */
auto check(const cnn::optimizer& opt)
{
  auto errs = 0u;
  // corresponding activation from name
  auto act = cnn::get_activation(opt.name());
  // ensure f, g values are as expected on a range of values
  // note: values are symmetric around zero, e.g. -4.5, -3.5, ... 3.5, 4.5
  std::vector<cnn::real_type> values(10u);
  std::iota(values.begin(), values.end(), cnn::real_type{-4.5f});
  // values should be exactly the same
  for (auto i = 0u; i < values.size(); i++) {
    // compute function + gradient values
    auto v = values[i];
    auto act_f = act.f(v);
    auto act_g = act.g(v);
    auto opt_f = opt(v);
    auto opt_g = opt.grad(v);
    // compare function values
    if (act_f != opt_f) {
      std::cerr << "ERROR: " << opt << ": act.f(" << v << ") != opt(" <<
        v << ") [" << act_f << " != " << opt_f << "]" << std::endl;
      errs++;
    }
    // compare gradient values
    if (act_g != opt_g) {
      std::cerr << "ERROR: " << opt << ": act.g(" << v << ") != opt.grad(" <<
        v << ") [" << act_g << " != " << opt_g << "]" << std::endl;
      errs++;
    }
  }
  // return number of errors
  return errs;
}

/**
 * Ensure `optimizer` objects produce the expected activation values.
 *
 * This calls `check()` in a fold expression.
 *
 * @tparam Ts `optimizer`
 *
 * @param opt_1 First `optimizer` instance
 * @param opt_2 Second `optimizer` instance
 * @param opts Additional optimizer instances to check
 * @returns Number of errors
 */
template <typename... Ts>
auto check(
  const cnn::optimizer& opt_1,
  const cnn::optimizer& opt_2,
  const Ts&... opts)
{
  // fold over pack
  static_assert((std::is_convertible_v<Ts, cnn::optimizer> && ...));
  return check(opt_1) + check(opt_2) + (check(opts) + ...);
}

}  // namespace

int main()
{
  // create a few optimizers with different activations
  cnn::optimizer opt_1{"linear"};
  cnn::optimizer opt_2{"relu"};
  cnn::optimizer opt_3{"sigmoid"};
  cnn::optimizer opt_4{"softmax"};
  // stream
  std::cout <<
    opt_1 << "\n" <<
    opt_2 << "\n" <<
    opt_3 << "\n" <<
    opt_4 << "\n" << std::flush;
  // check values. if no errors, success
  return !check(opt_1, opt_2, opt_3, opt_4) ? EXIT_SUCCESS : EXIT_FAILURE;
}
