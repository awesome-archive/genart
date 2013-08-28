#ifndef GENART_IMAGE_IMAGE_H
#define GENART_IMAGE_IMAGE_H

#include <string>

namespace cv {
  class Mat;
}  // namespace cv

namespace image {

class Image {
 public:
  // Initialize from the image stored in the given file, uses CImg
  // to load jpg, png, bmp, etc.
  explicit Image(const std::string& filename);

  // Initialize from an OpenCV Mat type.
  explicit Image(const cv::Mat& cv_image);

  // Initialize from an array of pixels, the Image object takes ownership
  // of the array.
  Image(unsigned char* pixels, int width, int height);

  Image(const Image& image);
  Image(Image&& image);
  ~Image();

  int width() const;
  int height() const;

  const unsigned char* pixels() const;
  unsigned char* mutable_pixels();

 private:
  Image& operator=(const Image&);

  int width_, height_;
  unsigned char* pixels_;
};

}  // namespace image

#endif
