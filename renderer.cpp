#include "renderer.h"

#include <QDebug>
#include <QTransform>
#include <qmath.h>
#include "transform.h"

#include <QMainWindow>

#ifndef INFO_BUFFER_SIZE
#define INFO_BUFFER_SIZE 1024
#endif

Renderer::Renderer(QWidget* parent) : QOpenGLWidget(parent)
{
}

Transform globalTrans;
QMatrix4x4 viewMat, projMat;
void Renderer::initializeGL()
{
    QOpenGLFunctions::initializeOpenGLFunctions();
    globalTrans.m_RotateAxis = {1.f,.5f,.2f};


    viewMat.lookAt({.0f,.0f,-100.f},{.0f,.0f,.0f},{.0f,1.f,.0f});

    projMat.perspective(qDegreesToRadians(80.f),4.f/3.f,0.01f,200.f);


    int isCompileSuccessed;
    char infoLog[INFO_BUFFER_SIZE];
    memset(infoLog, 0, INFO_BUFFER_SIZE * sizeof(char));

    qDebug() << "GL Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << "GL Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qDebug() << "GL Render: " << reinterpret_cast<const char*>(glGetString(GL_RENDER));
    qDebug() << "GLSL Version: " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::string vsStr = R"(
#version 120
attribute vec3 inCol;
attribute vec3 inPos;
varying vec3 fInCol;
uniform mat4 _model;
void main()
{
    gl_Position = _model * vec4(inPos , 1.0);
    fInCol=inCol;
}
)";
    std::string fsStr = R"(
#version 120
varying vec3 fInCol;
void main()
{
    gl_FragColor = vec4(fInCol, 1.0);
}
)";
    //  create vertex shader
    uint32_t vShader;
    {
        vShader = glCreateShader(GL_VERTEX_SHADER);
        //  load shader code
        const char *vShaderCode = vsStr.c_str();
        glShaderSource(vShader, 1, &vShaderCode, NULL);
        //  compile code
        glCompileShader(vShader);
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &isCompileSuccessed);
        if (!isCompileSuccessed) {
            glGetShaderInfoLog(vShader, INFO_BUFFER_SIZE, NULL, infoLog);
            qDebug() << "Vertex shader compilation failed, Details:\n" << infoLog;
        }
    }
    memset(infoLog, 0, INFO_BUFFER_SIZE * sizeof(char));

    //  create fragment shader
    uint32_t fShader;
    {
        fShader = glCreateShader(GL_FRAGMENT_SHADER);
        //  load shader code
        const char *fShaderCode = fsStr.c_str();
        glShaderSource(fShader, 1, &fShaderCode, NULL);
        //  compile code
        glCompileShader(fShader);
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &isCompileSuccessed);
        if (!isCompileSuccessed) {
            glGetShaderInfoLog(fShader, INFO_BUFFER_SIZE, NULL, infoLog);
            qDebug() << "Fragment shader compilation failed, Details:\n" << infoLog;
        }
    }
    memset(infoLog, 0, INFO_BUFFER_SIZE * sizeof(char));

    shader_id = glCreateProgram();
    //  add shader to program
    glAttachShader(shader_id, vShader);
    glAttachShader(shader_id, fShader);
    //  link shaders from program
    glLinkProgram(shader_id);

    char pinfoLog[INFO_BUFFER_SIZE];
    glGetProgramiv(shader_id, GL_LINK_STATUS, &isCompileSuccessed);
    if (!isCompileSuccessed) {
        glGetProgramInfoLog(shader_id, INFO_BUFFER_SIZE, NULL, pinfoLog);
        qDebug() << "Shader program link failed, Details:\n" << pinfoLog;
    }

    //  release shaders
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    float vert[] = {
        -.5f, -.5f,  .5f, 0.5f, 0.5f, 1.0f,
         .5f, -.5f,  .5f, 0.5f, 1.0f, 0.5f,
        -.5f,  .5f,  .5f, 1.0f, 1.0f, 0.5f,
         .5f,  .5f,  .5f, 1.0f, 0.5f, 1.0f,
        -.5f, -.5f, -.5f, 0.5f, 0.5f, 1.0f,
         .5f, -.5f, -.5f, 0.5f, 1.0f, 0.5f,
        -.5f,  .5f, -.5f, 1.0f, 1.0f, 0.5f,
         .5f,  .5f, -.5f, 1.0f, 0.5f, 1.0f,
    };

    unsigned int indi[] = {
        0,1,2,
        1,3,2,
        1,5,7,
        1,7,3,
        5,4,7,
        4,6,7,
        2,3,7,
        2,7,6,
        4,0,2,
        4,2,6,
        4,5,1,
        4,1,0
    };

    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indi), indi, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    timer.start(12, this);
}

void Renderer::resizeGL(int w, int h)
{
}

void Renderer::paintGL()
{
    globalTrans.m_EulerAngle += 1.1f;

    globalTrans.Calculate();

    glClearColor(.2f, .2f, .2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_id);

    auto mvp = projMat * viewMat * globalTrans.GetTransMat();

    glUniformMatrix4fv(glGetUniformLocation(shader_id, "_model"),
                       1, GL_FALSE, mvp.data());

    glDrawElements(GL_TRIANGLES, 16*3, GL_UNSIGNED_INT, (void*)0);
}

void Renderer::keyPressEvent(QKeyEvent *ev)
{
    qDebug() << "press A";
    if (ev->key() == Qt::Key_A)
    {
        globalTrans.m_Position[0] -= .1f;
    }
    if (ev->key() == Qt::Key_D)
    {
        globalTrans.m_Position[0] += .1f;
    }
    if (ev->key() == Qt::Key_W)
    {
        globalTrans.m_Position[1] += .1f;
    }
    if (ev->key() == Qt::Key_S)
    {
        globalTrans.m_Position[1] -= .1f;
    }
    else
    {
        QWidget::keyPressEvent(ev);
    }
}

void Renderer::timerEvent(QTimerEvent *)
{
    update();
}
