#include "optimizer.hpp"
#include "functions.hpp"

int main() {
  cnn::optimizer optimizer("test", cnn::linear, cnn::derivative_linear);
  return 0;
}
