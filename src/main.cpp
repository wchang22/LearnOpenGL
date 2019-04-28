#include "window.h"
#include "exception.h"


#include <iostream>

int main() {
  try {
    Window window;
    window.main_loop();
  } catch (std::runtime_error e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
