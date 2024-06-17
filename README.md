# JPEG Decoder

This project implements a decoder for JPEG JFIF images.

To get started, open the project with an IDE that supports CMake, and change the image path in the main file.

## Algorithm Description

First, we decode the fundamental harmonic with RLE (Run-Length Encoding) and decode the end of the signal magnitude. Then, we apply Huffman coding (using two different tables) to decode the complete signal.

Since our eyes are more sensitive to luminance variation, the image is stored in the YCbCr format, and the Cb and Cr blocks are compressed.

The JFIF format allows four types of compression:

Horizontal compression, stored as Y1Y2CbCr
Vertical compression, stored as Y1Y2CbCr
Quadra compression, stored as Y1Y2Y3Y4CbCr
No compression, stored as YCbCr
After decoding all the signals, we have to upscale the chrominance blocks.

## Difficulties

### Unused Information
Some blocks are not used at the end of the file, and we had to account for this.

### Size
We have to pay attention to whether the size of the image is a multiple of 8.

### Error Propagation
Because the fundamental frequency is encoded by difference, if we fail to decode one part correctly, we may fail to decode the whole image.

## Example of Images

First decompression: The loop was not well calibrated, causing these grey bands.

![premiére décompression de Francois](illustration/first_francois.png)


Second decompression: The fundamental frequency was not well decoded, causing this.

![premiére décompression de Francois](illustration/francois2.png)


Third decompression: We did not find a way to correct this.

![premiére décompression de Francois](illustration/francois4.png)


Here, the image's size is not a multiple of 8, causing this shift.

![premiére décompression de Francois](illustration/renee.png)


Once it is corrected:

![premiére décompression de Francois](illustration/renee2.png)


One last image:

![premiére décompression de Francois](illustration/chappell.png)
