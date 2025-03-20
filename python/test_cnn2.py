#! /usr/bin/python3

import tensorflow as tf
import numpy as np

EPSILON=1.e-6

# Define the training data
input_data = np.array([[0, 0, 1, 0]], dtype=np.float32)  # Input data
expected_output = np.array([[0, 0, 1, 0]], dtype=np.float32)  # Expected output

# Build the neural network model
model = tf.keras.Sequential([
    #tf.keras.layers.Dense(4, activation='sigmoid', input_shape=(4,))  # Single layer, matching input-output dimensions
    tf.keras.layers.Dense(4, input_shape=(4,))  # Single layer, matching input-output dimensions
    ])

# Compile the model
optimizer = tf.keras.optimizers.Adam(learning_rate=0.01)
model.compile(optimizer=optimizer, loss='mean_squared_error')
model.summary()

# Define a custom callback to stop training when loss is below 1e-6
class LossThresholdCallback(tf.keras.callbacks.Callback):
    def on_epoch_end(self, epoch, logs=None):
        if logs.get('loss') < EPSILON:
            print(f"\nStopping training at epoch {epoch+1} as loss is below threshold")
            self.model.stop_training = True

# Train the model
model.fit(input_data, expected_output, epochs=500, verbose=1, callbacks=[LossThresholdCallback()])
print(model.get_weights())

# Save the model to a file
model.save("simple_model.h5")

# Load the model from the file
loaded_model = tf.keras.models.load_model("simple_model.h5")

# Test the model with the same input
output = loaded_model.predict(input_data)
print("Predicted output:", output)

