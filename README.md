# JPEG Decoder


This project implement a decoder for JPEG JFIF

## Algorithm description

First we decode the fondamental harmonic with rle, 
and we decode the end of the signal magnitude.
Then we apply Huffman (with two diferent table) to
decode the complete signal.

As our eyes are more sensible to luminence variation.
The image is store in format YCbCr and the block Cb and Cr 
are compressed.

The format JFIF allow four type of compression :

- Horizontal compression, stored Y1Y2CbCr
- Vertical compression, stored Y1Y2CbCr
- Quadra compression, stored Y1Y2Y3Y4CbCr
- No compression, stored YCbCr

After decode all the signal we have to upscale crominence block.

## Dificulty

### Unused information

Some blocks are not used at the end of the file and we had to take it into acount.


### 


