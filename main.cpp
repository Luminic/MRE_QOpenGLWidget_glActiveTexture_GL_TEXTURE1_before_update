#include <QApplication>
#include "OpenGLWidget.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  OpenGLWidget opengl_widget;
  opengl_widget.show();

  return app.exec();
}
