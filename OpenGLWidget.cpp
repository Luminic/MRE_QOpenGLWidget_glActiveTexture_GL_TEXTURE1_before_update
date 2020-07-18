#include "OpenGLWidget.hpp"

#include <QTimer>
#include <QDebug>
#include <QOpenGLDebugLogger>

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    setFormat(format);    
}

OpenGLWidget::~OpenGLWidget() {
    glDeleteVertexArrays(1, &frame_vao);
    glDeleteBuffers(1, &frame_vbo);
    
}


void OpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    #ifdef QT_DEBUG
        QOpenGLContext* ctx = QOpenGLContext::currentContext();
        QOpenGLDebugLogger* logger = new QOpenGLDebugLogger(this);
        if (!logger->initialize()) {
            qWarning("QOpenGLDebugLogger failed to initialize.");
        }
        if (!ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
            qWarning("KHR Debug extension unavailable.");
        }

        connect(logger, &QOpenGLDebugLogger::messageLogged, this,
            [](const QOpenGLDebugMessage& message){
                if (message.severity() == QOpenGLDebugMessage::HighSeverity) {
                    qCritical(message.message().toLatin1().constData());
                }
                else if (message.severity() != QOpenGLDebugMessage::NotificationSeverity) {
                    qWarning(message.message().toLatin1().constData());
                }
            }
        );
        logger->startLogging();
    #endif

    qDebug() << "GL Version:" << QString((const char*)glGetString(GL_VERSION));

    glEnable(GL_DEBUG_OUTPUT);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Create the frame
    float frame_vertices[] = {
        // Top left triangle
        -1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
        // Bottom left triangle
        -1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f
    };

    glGenVertexArrays(1, &frame_vao);
    glBindVertexArray(frame_vao);

    glGenBuffers(1, &frame_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, frame_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frame_vertices), frame_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ShaderStage shaders[] = {
        ShaderStage{GL_VERTEX_SHADER, "shaders/framebuffer_vs.glsl"},
        ShaderStage{GL_FRAGMENT_SHADER, "shaders/framebuffer_fs.glsl"}
    };

    frame_shader.load_shaders(shaders, 2);
    frame_shader.validate();

    connect(&timer, &QTimer::timeout, this, &OpenGLWidget::main_loop);
    timer.start(16);
}


void OpenGLWidget::resizeGL(int w, int h) {}


void OpenGLWidget::paintGL() {
    if (fp) {
        // First paint seems to always happen before the first update
        qDebug() << "First Paint";
        fp = false;
    }
    glClear(GL_COLOR_BUFFER_BIT);

    unsigned int tex;
    glGenTextures(1, &tex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    QImage img = QImage("textures/awesomeface.png").convertToFormat(QImage::Format_RGBA8888).mirrored();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUseProgram(frame_shader.get_id());

    glBindVertexArray(frame_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Clean up
    glDeleteTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    // This doesn't cause the textures to be black
    glActiveTexture(GL_TEXTURE1);
}


void OpenGLWidget::main_loop() {
    if (fu) {
        qDebug() << "First Update";
        fu = false;
    }

    // This causes the textures to completely black
    // Change GL_TEXTURE1 to GL_TEXTURE0 (or get rid of it glActiveTexture
    // completely) and textures will work once again
    glActiveTexture(GL_TEXTURE1);
    update();
}