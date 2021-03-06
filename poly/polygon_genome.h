// Genome captures the polygon based genome of a drawing.

#ifndef GENART_POLY_POLYGON_GENOME_H_
#define GENART_POLY_POLYGON_GENOME_H_

#include <vector>

#include "core/genome.h"
#include "poly/polygon.h"

namespace genart {

namespace core {
// TODO(piotrf): use an extened poly-specific mutation params.
class MutationParams;
}  // namespace core
  
namespace poly {

class OfflinePolygonRenderer;
  
class PolygonGenome : public core::Genome {
 public:
  explicit PolygonGenome(OfflinePolygonRenderer* renderer);
  PolygonGenome(OfflinePolygonRenderer* renderer,
                const std::vector<Polygon> polygons);
  PolygonGenome(OfflinePolygonRenderer* renderer,
                const core::MutationParams& params,
                int num_polygons);
  virtual ~PolygonGenome();

  virtual std::unique_ptr<core::Genome> Clone() const;
  virtual void Mutate(const core::MutationParams& params);
  virtual std::unique_ptr<image::Image> Render() const;

  const std::vector<Polygon>& polygons() const {
    return polygons_;
  }
  
 private:
  OfflinePolygonRenderer* renderer_;  // Not owned.
  
  std::vector<Polygon> polygons_;
};

}  // namespace poly
}  // namespace genart

#endif  // GENART_POLY_GENOME_H_
