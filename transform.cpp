#include "transform.h"

#include <QTransform>

#define CALCULATE_TRANS(_trans)     Qt3DCore::QTransform _trans;\
_trans.setTranslation(m_Position);\
_trans.setRotation(Qt3DCore::QTransform::fromAxisAndAngle(m_RotateAxis, m_EulerAngle));\
_trans.setScale3D(m_Scale)

Transform::Transform()
    : m_Position({.0f,.0f,.0f}),
      m_RotateAxis({.0f,1.f,.0f}),
      m_Scale({1.f,1.f,1.f}),
      m_EulerAngle(.0f),
      m_BufferReading(0)
{
    CALCULATE_TRANS(_trans);
    m_TransMat = _trans.matrix();
    m_InvTransMat = m_TransMat.inverted();
}

Transform::Transform(const QVector3D &pos, const QVector3D &axis, float eulerAngle, const QVector3D &scale)
    : m_Position(pos),
      m_RotateAxis(axis),
      m_Scale(scale),
      m_EulerAngle(eulerAngle),
      m_BufferReading(0)
{
    CALCULATE_TRANS(_trans);
    m_TransMat = _trans.matrix();
    m_InvTransMat = m_TransMat.inverted();
}

void Transform::Calculate()
{
    CALCULATE_TRANS(_trans);
    m_TransMat = _trans.matrix();
    m_InvTransMat = m_TransMat.inverted();
}


