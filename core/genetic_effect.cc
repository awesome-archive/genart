#include <iostream>

#include <glog/logging.h>

#include "core/effect_visitor.h"
#include "core/fitness_l2_with_focii.h"
#include "core/genetic_effect.h"
#include "core/genome.h"
#include "core/genome_factory.h"

using genart::image::Image;

namespace genart {
namespace core {

GeneticEffect::GeneticEffect(Image input,
                             EffectParams params,
                             std::unique_ptr<GenomeFactory> genome_factory)
    : input_(input),
      params_(params),
      genome_factory_(std::move(genome_factory)),
      fitness_func_(new FitnessL2WithFocii) {
}

GeneticEffect::~GeneticEffect() {
}

void GeneticEffect::AddVisitor(int interval, EffectVisitor* visitor) {
  StepVisitor new_visitor;
  new_visitor.interval = interval;
  new_visitor.visitor = visitor;
  visitors_.push_back(new_visitor);
}

std::unique_ptr<Genome> GeneticEffect::Render() {
  std::unique_ptr<Genome> mother = genome_factory_->Random(
      params_.mutation_params());
  return RenderFromInitial(*mother);
}

std::unique_ptr<Genome> GeneticEffect::RenderFromInitial(
    const Genome& initial,
    double initial_fitness_threshold) {
  std::unique_ptr<Genome> mother = initial.Clone();
  std::unique_ptr<Genome> child = genome_factory_->Create();
  
  // Calculate an initial fitness, based on a blank black image.
  std::unique_ptr<Image> image, best_image;
  image = child->Render();
  best_image.reset(new Image(*image));
  double last_fitness = fitness_func_->Evaluate(input_, *image);
  LOG(INFO) << "Initial fitness = " << last_fitness << std::endl;
  const double start_fitness = last_fitness;

  // Check if our initial polygons pass the initial threshold, and if not
  // reset the mother to be a random genome.
  if (initial_fitness_threshold < 1.0) {
    image = mother->Render();
    double mother_fitness = fitness_func_->Evaluate(input_, *image);
    if (initial_fitness_threshold * start_fitness <
        mother_fitness) {
      LOG(INFO) << "fitness frac initial = " << mother_fitness / start_fitness
                << " > threshold " << initial_fitness_threshold
                << " -> starting fresh" << std::endl;
      mother = genome_factory_->Random(params_.mutation_params());
    }
  }

  for (int i = 0; i < params_.max_generations(); ++i) {
    // Try a child mutation.
    child = mother->Clone();
    child->Mutate(params_.mutation_params());
    image = child->Render();
    double child_fitness = fitness_func_->Evaluate(input_, *image);
    if (child_fitness < last_fitness) {
      mother.swap(child);
      last_fitness = child_fitness;
      best_image.reset(new Image(*image));
    }
    if (i % 100 == 0) {
      LOG(INFO) << "iter " << i << " "
                << "fitness = " << last_fitness << " frac initial = "
                << last_fitness / start_fitness << std::endl;
    }

    if (params_.has_fitness_threshold() &&
        last_fitness <= start_fitness * params_.fitness_threshold()) {
      LOG(INFO) << "fitness = " << last_fitness << " frac initial = "
                << last_fitness / start_fitness
                << " which is under threshold of "
                << params_.fitness_threshold() << " -> FINISHED." << std::endl;
      break;
    }

    for (const auto& step_visitor : visitors_) {
      if (i % step_visitor.interval == 0) {
        step_visitor.visitor->Visit(*best_image);
      }
    }
  }

  return mother;
}

}  // namespace core
}  // namespace genart
