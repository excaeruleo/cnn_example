#include "optimizer.hpp"
#include "functions.hpp"

#include <string>

int main(){
	cnn::optimizer optimizer("test", cnn::linear, cnn::derivative_linear);
}
