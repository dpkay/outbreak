#include "subject.h"
#include <memory>
#include <vector>

class Renderer {
 public:
  Renderer();
  ~Renderer();
  void Init(int subject_count);
  void RenderFrame(const std::vector<Subject> &subjects);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
