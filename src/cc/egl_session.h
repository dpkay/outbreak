#include <EGL/egl.h>
#include <Eigen/Core>
#include <iostream>
#include "absl/strings/str_cat.h"

#define EGL_OPENGL_ES3_BIT                0x00000040
#define EGL_CONTEXT_MAJOR_VERSION         0x3098

#define EXIT_IF_EGL_ERROR(egl_expr)                                          \
  do {                                                                         \
    (egl_expr);                                                                \
    auto error = eglGetError();                                                \
    if (error != EGL_SUCCESS) {                                                \
      std::cerr << absl::StrCat("EGL ERROR: 0x",                               \
                                absl::Hex(error, absl::kZeroPad4),            \
                                " file:", __FILE__, ", line: ", __LINE__);     \
      exit(1);                                                                 \
    }                                                                          \
  } while (false)

class EglSession {
 public:
  EglSession(EGLDisplay display, EGLSurface surface, EGLContext context)
      : display_(display), surface_(surface), context_(context) {}

  ~EglSession() {
    EXIT_IF_EGL_ERROR(eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                                       EGL_NO_CONTEXT));
    EXIT_IF_EGL_ERROR(eglDestroyContext(display_, context_));
    EXIT_IF_EGL_ERROR(eglDestroySurface(display_, surface_));
  }

 private:
  EGLDisplay display_;
  EGLSurface surface_;
  EGLContext context_;
};

EGLDisplay CreateAndInitDisplay() {
  EGLDisplay display;
  EXIT_IF_EGL_ERROR(display = eglGetDisplay(EGL_DEFAULT_DISPLAY));
  if (display == EGL_NO_DISPLAY) {
    std::cerr << "Failed to find valid EGL display.";
    exit(1);
  }

  EGLint major_ver = -1;
  EGLint minor_ver = -1;
  bool initialize_result;
  EXIT_IF_EGL_ERROR(initialize_result =
                          eglInitialize(display, &major_ver, &minor_ver));
  if (!initialize_result) {
    std::cerr << "eglInitialize failed: Could not create EGL context.";
    exit(1);
  }
  std::cerr << "EGL display initialized. EGL version: " << major_ver << ","
            << minor_ver;
  return display;
}

void BindEglApi() {
  // Bind the OpenGL ES API to this thread for subsequent EGL calls.
  EGLBoolean success;
  EXIT_IF_EGL_ERROR(success = eglBindAPI(EGL_OPENGL_ES_API));
  if (!success) {
    std::cerr << "Failed to bind OpenGL ES API.";
    exit(1);
  }
}

EGLConfig ChooseConfig(const EGLDisplay& display) {
  EGLint num_configs;
  EGLConfig config;

  // EGL Config settings, used to define what components are required, the
  // desired size in bits and various other settings.
  // clang-format off
  constexpr EGLint kConfigAttribs[] = {EGL_RED_SIZE, 8,
                                       EGL_GREEN_SIZE, 8,
                                       EGL_BLUE_SIZE, 8,
                                       EGL_DEPTH_SIZE, 16,
                                       EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                       EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                                       EGL_NONE};
  // clang-format on
  bool success;
  EXIT_IF_EGL_ERROR(success = eglChooseConfig(display, kConfigAttribs,
                                                &config, 1, &num_configs));
  if (!success) {
    std::cerr << "Failed to choose a valid an EGLConfig.";
    exit(1);
  }
  return config;
}

EGLSurface CreateSurface(EGLDisplay display, EGLConfig config,
                         const Eigen::Vector2i &size) {
  EGLSurface surface;
  EXIT_IF_EGL_ERROR(
      surface = eglCreateWindowSurface(
          display, config,
          reinterpret_cast<EGLNativeWindowType>(nullptr) /* native_window */,
          nullptr /* attrib_list */));
  return surface;
}

EGLContext CreateContext(EGLDisplay display, EGLConfig config) {
  // clang-format off
  constexpr EGLint kCreateContextAttribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_NONE,
  };
  // clang-format on
  EGLContext egl_context;
  EXIT_IF_EGL_ERROR(egl_context = eglCreateContext(
                        display, config, EGL_NO_CONTEXT /* share_context */,
                        kCreateContextAttribs));
  return egl_context;
}

std::unique_ptr<EglSession> CreateEglSession(const Eigen::Vector2i &size) {
  const EGLDisplay display = CreateAndInitDisplay();
  BindEglApi();
  const EGLConfig config = ChooseConfig(display);
  const EGLSurface surface = CreateSurface(display, config, size);
  const EGLContext context = CreateContext(display, config);

  EXIT_IF_EGL_ERROR(eglMakeCurrent(display, surface, surface, context));
  return std::make_unique<EglSession>(display, surface, context);
}
