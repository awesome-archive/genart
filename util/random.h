#ifndef GENART_UTIL_RANDOM_H
#define GENART_UTIL_RANDOM_H

namespace util {

class Random {
 public:
  // Random number within a range.
  static int Integer(int range);
  static int Integer(int a, int b);
  
  static double Double(double range = 1.0);
  static double Double(double a, double b);
  
  static float Float(float range = 1.0);
  static float Float(float a, float b);
};
  
}  // util

#endif
