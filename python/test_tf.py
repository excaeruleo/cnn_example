#! /usr/bin/python3

import tensorflow as tf

# Check if TensorFlow is built with GPU support
print(tf.test.is_built_with_gpu_support())

# List available physical devices (GPUs)
print(tf.config.list_physical_devices('GPU'))
