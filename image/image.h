#ifndef GENART_IMAGE_IMAGE_H
#define GENART_IMAGE_IMAGE_H

#include <string>

namespace cimg_library {
  template<class T> class CImg;
}  // namespace cimg_library

namespace image {

class Image {
 public:
  // Initialize from the image stored in the given file, uses CImg
  // to load jpg, png, bmp, etc.
  explicit Image(const std::string& filename);

  // Initialize from an array of pixels, the Image object takes ownership
  // of the array.
  Image(unsigned char* pixels, int width, int height);
  
  ~Image();

  int width() const;
  int height() const;

  const unsigned char* pixels() const;
  unsigned char* mutable_pixels();

 private:
  Image(const Image&);
  Image& operator=(const Image&);

  int width_, height_;
  unsigned char* pixels_;
  
  cimg_library::CImg<unsigned char>* cimg_;
};

}  // namespace image

#endif
