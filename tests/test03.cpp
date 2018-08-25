//
// Copyright - @adityaatluri
//

#include "Redox/BatchNorm.hpp"

int main() {
    Redox::Tensor<float> image(128, 128, 128, 128);
    Redox::Tensor<float> mean(1, 128, 1, 1);
    Redox::Tensor<float> variance(1, 128, 1, 1);

    Redox::BatchNorm(image, mean, variance);
}
