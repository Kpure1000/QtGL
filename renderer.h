#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QTimer>

class Renderer : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    Renderer(QWidget* parent);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;


protected:
    void keyPressEvent(QKeyEvent* ev) override;
    void timerEvent(QTimerEvent* ev) override;


private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int shader_id;

    QBasicTimer timer;
};

#endif // RENDERER_H
