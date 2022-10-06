#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QVector3D>
#include <QMatrix4x4>

class Transform
{
public:
    Transform();
    Transform(const QVector3D &pos, const QVector3D &axis, float eulerAngle, const QVector3D &scale);

    [[nodiscard]] inline const QMatrix4x4& GetTransMat() const
    {
        return m_TransMat;
    }
    [[nodiscard]] inline const QMatrix4x4& GetInvTransMat() const
    {
        return m_InvTransMat;
    }

    void Calculate();

    QVector3D m_Position, m_RotateAxis, m_Scale;
    float m_EulerAngle;

private:
    QMatrix4x4 m_TransMat, m_InvTransMat;


    unsigned int m_BufferReading;
};

#endif // TRANSFORM_H
