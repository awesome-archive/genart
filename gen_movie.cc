#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include <gflags/gflags.h>
#include <GL/glx.h>
#include <opencv2/opencv.hpp>

#include "comparison_window.h"
#include "image/image.h"
#include "poly/polygon_effect.h"
#include "poly/polygon_image.pb.h"
#include "poly/util.h"
#include "util/recordio.h"

DEFINE_string(input_movie, "", "Source movie.");
DEFINE_string(output_movie, "", "Desired path of movie to output.");
DEFINE_int32(display_step, 100,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display.");
DEFINE_double(frame_finish_threshold, 0.25,
              "Fitness ratio threshold to consider a frame done.");
DEFINE_double(frame_restart_threshold, 0.32,
              "Initial fitness ratio, above which we restart a frame from "
              "scratch, instead of using the previous frame.");

using genart::ComparisonWindow;
using genart::RenderProgressVisitor;

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (XInitThreads() == 0) {
    std::cerr << "Failed to initialize thread support in xlib." << std::endl;
    return 1;
  }

  // Open the video input file.
  cv::VideoCapture src_video(FLAGS_input_movie);
  if (!src_video.isOpened()) {
    std::cerr << "Failed to open video file: " << FLAGS_input_movie
              << std::endl;
    return 1;
  }

  // Set a hook for a window to display output.
  std::unique_ptr<ComparisonWindow> window;
  std::unique_ptr<RenderProgressVisitor> render_progress_visitor;
  std::unique_ptr<std::thread> render_thread;
  if (FLAGS_display_step > 0) {
    window.reset(new ComparisonWindow());
    render_progress_visitor.reset(new RenderProgressVisitor(window.get()));
    render_thread.reset(new std::thread(&ComparisonWindow::Run, window.get()));
  }

  std::ofstream output_movie(FLAGS_output_movie);
  util::RecordWriter record_writer(&output_movie);

  std::unique_ptr<poly::output::PolygonImage> last_frame_polygons;
  for (;;) {
    // Grab the next frame from the video.
    if (!src_video.grab()) {
      std::cout << "Finished with the movie!" << std::endl;
      break;
    }
    cv::Mat frame;
    src_video.retrieve(frame);
    std::unique_ptr<image::Image> src_image(new image::Image(frame));

    poly::output::PolygonImage* output_polygons =
        new poly::output::PolygonImage();

    // Setup the effect.
    poly::PolygonEffect polygon_effect;
    polygon_effect.SetInput(src_image.get());
    polygon_effect.SetOutput(output_polygons);
    poly::EffectParams effect_params;
    effect_params.set_max_generations(1000000);
    effect_params.set_fitness_threshold(FLAGS_frame_finish_threshold);
    polygon_effect.SetParams(effect_params);

    // Setup the comparison window for this effect.
    window->SetSourceImage(new image::Image(*src_image.get()));
    polygon_effect.AddVisitor(FLAGS_display_step,
                              render_progress_visitor.get());

    if (last_frame_polygons.get() != nullptr) {
      polygon_effect.RenderFromInitial(*last_frame_polygons,
                                       FLAGS_frame_restart_threshold);
    } else {
      polygon_effect.Render();
    }
    last_frame_polygons.reset(output_polygons);

    if (!record_writer.WriteProtocolMessage(*output_polygons)) {
      std::cerr << "Failed to write movie frame!" << std::endl;
      return 1;
    }
    output_movie.flush();
  }

  return 0;
}
