#include "renderer.h"
#include "egl_session.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <chrono>

namespace {

// Shader sources
const GLchar *vertexSource = R"(
attribute vec3 aPositionAndInfectionState;
varying float vInfectionState;
void main()
{
  gl_Position = vec4(aPositionAndInfectionState.xy, 0.0, 1.0);
  gl_PointSize = 5.0;
  vInfectionState = aPositionAndInfectionState.z;
}
)";

const GLchar *fragmentSource = R"(
precision mediump float;
varying float vInfectionState;
void main()
{
  float dist = length(vec2(0.5, 0.5) - gl_PointCoord);
  float opacity = smoothstep(0.45, 0.4, dist);

  vec3 color = vec3(0.0);
  if (vInfectionState < 0.5) {
    // kUninfected
    color = vec3(0.77);
  } else if (vInfectionState < 1.5) {
    // kInfectedWithoutSymptoms
    color = vec3(0.5);
  } else if (vInfectionState < 2.5) {
    // kInfectedWithSymptoms
    color = vec3(1.0, 0.15, 0.0);
  } else if (vInfectionState < 3.5) {
    // kRecovered
    color = vec3(0.1, 0.4, 0.9);
  }
  gl_FragColor = vec4(color, opacity);
}
)";

} // namespace


// -----------------------------------------------------------------------------
// Renderer:Impl implementation.
// -----------------------------------------------------------------------------
class Renderer::Impl {
 public:
   Impl() { egl_session_ = CreateEglSession(Eigen::Vector2i(1000, 1000)); }

  void Init(int subject_count);
  void RenderFrame(const std::vector<Subject> &subjects);

 private:
  std::unique_ptr<EglSession> egl_session_;
  std::chrono::system_clock::time_point start_time_;
  std::vector<GLfloat> vertex_data;
};

void Renderer::Impl::Init(int subject_count) {
  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);

  //GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
  vertex_data.resize(subject_count * 3);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(GLfloat),
               vertex_data.data(), GL_DYNAMIC_DRAW);

  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, nullptr);
  glCompileShader(vertexShader);

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
  glCompileShader(fragmentShader);

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "aPositionAndInfectionState");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

  start_time_ = std::chrono::system_clock::now();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::Impl::RenderFrame(const std::vector<Subject> &subjects) {
  const auto duration_since_start =
      start_time_ - std::chrono::system_clock::now();
  // move a vertex
  const uint32_t milliseconds_since_start =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          duration_since_start)
          .count();
  const uint32_t milliseconds_per_loop = 3000;

  //GLfloat vertices[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.9f, 0.0f, -0.5f, -0.5f, 0.0f};
  //vertex_data[0] = (milliseconds_since_start % milliseconds_per_loop) /
  //                  float(milliseconds_per_loop) -
  //              0.5f;

  for (int i = 0; i < subjects.size(); ++i) {
    const Subject& subject = subjects[i];
    vertex_data[i * 3] = subject.GetPosition()[0] * 2.0 - 1.0;
    vertex_data[i * 3 + 1] = subject.GetPosition()[1] * 2.0 - 1.0;
    vertex_data[i * 3 + 2] = static_cast<float>(subject.GetInfectionState());
  }

  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(GLfloat),
               vertex_data.data(), GL_DYNAMIC_DRAW);

  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_POINTS, 0, subjects.size());
}

// -----------------------------------------------------------------------------
// Renderer implementation.
// -----------------------------------------------------------------------------
Renderer::Renderer() : impl_(std::make_unique<Impl>()) {}

Renderer::~Renderer() {}

void Renderer::Init(int subject_count) { impl_->Init(subject_count); }

void Renderer::RenderFrame(const std::vector<Subject> &subjects) {
  impl_->RenderFrame(subjects);
}
