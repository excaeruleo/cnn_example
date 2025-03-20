#include <../include/layer.hpp>

int main(void) {

  int n_input = 10;

	cnn::Layer input_layer = cnn::Layer(n_input); 
  for(int i = 0; i < n_input; i ++)
		input_layer[i] = 0.;
  input_layer[3] = 1.;
  std::cout << input_layer;
  /* uncomment below will show failed test because access out of bound */
  //input_layer[15] = 1.;
  //std::cout << input_layer;

  return 0;

}
