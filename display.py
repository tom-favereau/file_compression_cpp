import matplotlib.pyplot as plt
import numpy as np
import sys

def read_pixels_from_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
        height, width = map(int, lines[0].strip().split())
        pixels = []
        for line in lines[1:]:
            pixel_values = list(map(int, line.strip().split()))
            row = []
            for i in range(0, len(pixel_values), 3):
                row.append((pixel_values[i], pixel_values[i+1], pixel_values[i+2]))
            pixels.append(row)
        return pixels, height, width

def display_image(pixels, height, width):
    image = np.zeros((height, width, 3), dtype=np.uint8)
    for i in range(height):
        for j in range(width):
            image[i, j] = pixels[i][j]
    plt.imshow(image)
    plt.axis('off')
    plt.show()


if __name__ == "__main__":
    filename = sys.argv[1]
    pixels, height, width = read_pixels_from_file(filename)
    display_image(pixels, height, width)


