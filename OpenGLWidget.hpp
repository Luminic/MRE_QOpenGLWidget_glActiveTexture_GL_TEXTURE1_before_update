#ifndef OPEN_GL_WIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>

#include "Shader.hpp"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
    Q_OBJECT;

public:
    OpenGLWidget(QWidget* parent=nullptr);
    ~OpenGLWidget();

    void main_loop();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    unsigned int frame_vbo;
    unsigned int frame_vao;
    Shader frame_shader;

    QTimer timer;

    bool fp = true;
    bool fu = true;
};

#endif