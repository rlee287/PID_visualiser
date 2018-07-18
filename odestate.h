#ifndef ODESTATE_H
#define ODESTATE_H

#include <boost/operators.hpp>
#include <iostream>

class ODEState
    : boost::additive1<
          ODEState, boost::additive2<ODEState, double, boost::multiplicative2<ODEState, double>>> {
  public:
    ODEState();
    double response = 0;
    double deriv = 0;
    double integratederror = 0;

    double &operator[](std::size_t idx);
    const double &operator[](std::size_t idx) const;

    ODEState &operator+=(const ODEState &p);
    ODEState &operator+=(const double a);
    ODEState &operator*=(const double a);

  private:
    double workaround_temp_refs = 0;
};

ODEState operator/(const ODEState &p1, const ODEState &p2);
ODEState abs(const ODEState &p);

/*namespace boost {
namespace numeric {
namespace odeint {
namespace vector_space_algebra {
template <> struct norm_inf<ODEState> {
    typedef double result_type;
    double operator()(const ODEState &p) const {
        using std::abs;
        using std::max;
        return max(max(abs(p.response), abs(p.deriv)), abs(p.integratederror));
    }
};
} // namespace vector_space_algebra
} // namespace odeint
} // namespace numeric
} // namespace boost
*/
#endif // ODESTATE_H
