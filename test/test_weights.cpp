#include "../include/weights.hpp"
#include "typedef.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>

int main(void) {

using namespace boost::numeric::ublas;
cnn::Weights weights;
using it = weight_index_type;

weights.w[it{1, 1, 1}] = cnn::Connector(1,1, 1);

vector<cnn::real_type> x_0(3);
x_0[0]= 1;
x_0[1]= 2;
x_0[2]= 3;
vector<cnn::real_type> x_1(3);
x_1[0] = 1;
x_1[1] = 1;
x_1[2] = 1;

vector<cnn::real_type> x_2 = x_1 - x_0;

std::cout << x_2;

return 0;

}
