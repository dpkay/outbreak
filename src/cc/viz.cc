#include "renderer.h"
#include "simulation.h"
#include <emscripten.h>
#include <functional>
#include <iostream>
#include <json/writer.h>

// -----------------------------------------------------------------------------
// Interface from C++ to JS.
//
// Keep in sync with main.js.
// -----------------------------------------------------------------------------
EM_JS(void, ReportSimulationStateJson, (const char* json), {  //
  return ccToJs_reportSimulationStateJson(UTF8ToString(json));
});

// -----------------------------------------------------------------------------
// Interface from JS to C++.
//
// Keep in sync with main.js.
// -----------------------------------------------------------------------------
extern "C" {

void EMSCRIPTEN_KEEPALIVE on_canvas_clicked() {
  std::cout << "CLICKED!" << std::endl;
}

}

// -----------------------------------------------------------------------------
// Main implementation.
// -----------------------------------------------------------------------------
class App {
public:
  App() {
    const int subject_count = 5000;
    renderer_.Init(subject_count);
    simulation_.Init(subject_count);
  }

  void DoFrame() {
    // Update simulation.
    const Duration dt = Hours(1);
    simulation_.Update(dt);

    // Render.
    renderer_.RenderFrame(simulation_.GetSubjects());

    // Maybe print something.
    static int s_iterations = 0;
    s_iterations++;
    if (s_iterations % 20 == 0) {
      const auto infection_state_counts =
          simulation_.ComputeInfectionStateHistogram();

      // Assemble a JSON for consumption by JS.
      Json::Value infection_state_histogram(Json::arrayValue);
      for (int i = 0; i < infection_state_counts.size(); ++i) {
        Json::Value entry;
        std::stringstream ss;
        ss << static_cast<InfectionState>(i);
        entry["state"] = ss.str();
        entry["count"] = infection_state_counts[i];
        infection_state_histogram.append(entry);
      }

      Json::Value root;
      root["infectionStateHistogram"] = infection_state_histogram;
      root["hoursElapsed"] =
          static_cast<double>(std::chrono::duration_cast<std::chrono::hours>(
                                  simulation_.GetElapsedSimulationTime())
                                  .count());
      std::stringstream ss;
      ss << root;
      ReportSimulationStateJson(ss.str().c_str());
    }
  }

 private:
  Simulation simulation_;
  Renderer renderer_;
};

void MainLoop(void* app_voidptr) {
  App* app = static_cast<App*>(app_voidptr);
  app->DoFrame();
}

int main() {
  App app;
  emscripten_set_main_loop_arg(MainLoop, &app, /*fps=*/0,
                               /*simulate_infinite_loop=*/true);
  return EXIT_SUCCESS;
}
