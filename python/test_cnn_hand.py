#! /usr/bin/python3

import tensorflow as tf
import numpy as np
from tensorflow.keras.preprocessing.image import img_to_array, load_img
from tensorflow.keras.datasets import mnist
import cv2  # For image resizing

# Load and preprocess the MNIST dataset for training
(x_train, y_train), (x_test, y_test) = mnist.load_data()

# Filter out only the images of "8" (label 8)
x_train_8 = x_train[y_train == 8]
x_test_8 = x_test[y_test == 8]
y_train_8 = y_train[y_train == 8]
y_test_8 = y_test[y_test == 8]

# Normalize images
x_train_8 = x_train_8 / 255.0
x_test_8 = x_test_8 / 255.0

# Reshape to add channel dimension for CNN input
x_train_8 = x_train_8.reshape(-1, 28, 28, 1)
x_test_8 = x_test_8.reshape(-1, 28, 28, 1)

# Build the neural network model
model = tf.keras.Sequential([
    tf.keras.layers.Conv2D(32, (3, 3), activation='relu', input_shape=(28, 28, 1)),
    tf.keras.layers.MaxPooling2D((2, 2)),
    tf.keras.layers.Conv2D(64, (3, 3), activation='relu'),
    tf.keras.layers.MaxPooling2D((2, 2)),
    tf.keras.layers.Flatten(),
    tf.keras.layers.Dense(64, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid')  # Binary classification: "8" vs "not 8"
])

# Compile the model
model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])

# Train the model
model.fit(x_train_8, np.ones(len(x_train_8)), epochs=5, validation_data=(x_test_8, np.ones(len(x_test_8))))

# Save the model
model.save("digit_8_recognition_model.h5")

# Function to preprocess and predict on a new image
def preprocess_and_predict(image_path):
    # Load and preprocess the image
    img = load_img(image_path, color_mode='grayscale')  # Load as grayscale
    img = img.resize((28, 28))  # Resize to 28x28
    img_array = img_to_array(img) / 255.0  # Normalize pixel values
    img_array = np.expand_dims(img_array, axis=0)  # Add batch dimension
    img_array = np.expand_dims(img_array, axis=-1)  # Add channel dimension

    # Load the trained model
    model = tf.keras.models.load_model("digit_8_recognition_model.h5")

    # Predict
    prediction = model.predict(img_array)
    if prediction >= 0.5:
        print("The image is likely a handwritten '8'.")
    else:
        print("The image is likely not a handwritten '8'.")

# Example usage with a test image
preprocess_and_predict("8.png")

