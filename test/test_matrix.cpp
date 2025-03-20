#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>

#include "typedef.hpp"

int main () {
    using namespace boost::numeric::ublas;
    matrix<double> m (3, 3);
    vector<double> n (3);
    for (unsigned i = 0; i < m.size1 (); ++ i)
        for (unsigned j = 0; j < m.size2 (); ++ j)
            m (i, j) = 3 * i + j;
    std::cout << m << std::endl;
    for (unsigned i = 0; i < n.size (); ++ i)
      n (i) = i;
    std::cout << n << std::endl;
    auto product = prod(m, n);
    std::cout << product << std::endl; 

    matrix<double> l;
    l = matrix<double> (3,3);
    vector<double> p (4);
    //p = 0.; // cannot do this.

    matrix<cnn::real_type> weights(1, 4, 4);
}
