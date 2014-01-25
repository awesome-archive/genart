#include "poly/polygon.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "poly/polygon_mutator.h"
#include "util/random.h"

using util::Random;

namespace genart {
namespace poly {

Polygon::Polygon() {
}

Polygon::Polygon(const std::vector<Point> points, const RGBA& color)
    : points_(points),
      color_(color) {
}

Polygon::~Polygon() {
}

void Polygon::Randomize(double h) {  
  // Random, smallish triangle.
  double ox = 0.5 * h + (1.0 - h) * Random::Double();
  double oy = 0.5 * h + (1.0 - h) * Random::Double();
  for (int i = 0; i < 3; ++i) {
    Point p;
    p.x = ox + h * (Random::Double() - 0.5);
    p.y = oy + h * (Random::Double() - 0.5);
    // Assert that I did the above calculations right.
    assert(p.x >= 0.0 && p.x <= 1.0);
    assert(p.y >= 0.0 && p.y <= 1.0);
    points_.push_back(p);
  }

  // Require that the triangle has CCW orientation, flipping
  // 2 points if necessary.
  const Point& a = points_[0];
  const Point& b = points_[1];
  const Point& c = points_[2];
  double det = (a.x - b.x) * (c.y - b.y) - (a.y - b.y) * (c.x - b.x);
  if (det > 0) {
    std::swap(points_[1], points_[2]);
  }
}

void Polygon::Mutate(const PolygonMutator& mutator) {
  mutator(this, &this->points_);
}

void Polygon::MutateColor(double min_alpha, double max_alpha) { 
  // Random color.
  color_.r = Random::Float();
  color_.g = Random::Float();
  color_.b = Random::Float();
  color_.a = min_alpha + (max_alpha - min_alpha) * Random::Float();
}

double Polygon::ComputeInteriorAngle(int i) const {
  int next_i = (i + 1) % (points_.size());
  int last_i = (i - 1 + points_.size()) % (points_.size());
  const Point& a = points_[last_i];
  const Point& b = points_[i];
  const Point& c = points_[next_i];
  double dot = (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
  double mag_ab = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
  double mag_bc = sqrt((c.x - b.x) * (c.x - b.x) + (c.y - b.y) * (c.y - b.y));
  return acos(dot / (mag_ab * mag_bc));
}

bool Polygon::Convex() const {
  for (unsigned int i = 0; i < points_.size(); ++i) {
    int next_i = (i + 1) % (points_.size());
    int last_i = (i - 1 + points_.size()) % (points_.size());
    const Point& a = points_[last_i];
    const Point& b = points_[i];
    const Point& c = points_[next_i];
    // Check the turning direction of the side.  Since we have CCW orientation,
    // each subsequent point should be turning CCW, i.e.
    // (b - a) x (c - b) > 0
    double det = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
    if (det < 0) {
      return false;
    }
  }
  return true;
}

}  // namespace poly
}  // namespace genart
