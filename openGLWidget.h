//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef OPEN_GL_WIDGET_HEADER
#define OPEN_GL_WIDGET_HEADER

#include <QList>
#include <QSet>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>

class Part;
class PartsModel;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    // Constructor.
    OpenGLWidget(QWidget *parent = 0, PartsModel * partsModel = nullptr);  // constructor
    virtual ~OpenGLWidget();                 // destructor

    virtual QSize sizeHint() const override; // sets (a hint for) reasonable size for the widget

    const QSet<int> & selectedParts() const { return m_selectedParts; }
    void clearSelectedParts() { m_selectedParts.clear(); }
    void createMasterBoxBuffer();

public slots:
    void addBuffer();
    void removeBuffer(int partIndex);
    void restoreFocus() { setFocus(); }  // restore the focus on the drawings
    void updateWhenMasterBoxChange();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;

    virtual void keyPressEvent(QKeyEvent * event) override;
    virtual void keyReleaseEvent(QKeyEvent * event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

signals:
    void selectedPartsChanged();

private: // member functions
    void setDistanceFromCameraToWorldOrigin(float dist) { m_cameraToWorldOriginDistance = dist; }
    void paintMasterBox();
    void initializePicking();
    void setColorsForPicking(int partIndex);
    void finishOffPicking();

private: // member variables
    QMatrix4x4 m_mMatrix;                           // model matrix
    QMatrix4x4 m_vMatrix;                           // view matrix
    QMatrix4x4 m_pMatrix;                           // projection matrix
    QOpenGLShaderProgram m_lightingShaderProgram;   // to manage lighting shaders
    PartsModel * m_partsModel;                      // parts model
    QList<QOpenGLBuffer> m_buffers;                 // buffers for the shading program, one per part
    QOpenGLBuffer m_masterBoxBuffer;                // buffer for the shading program, holds master box vertices

    QColor m_backgroundColor;    // background color
    QColor m_partColor;          // parts color
    QColor m_selectedPartColor;  // selected parts color
    QColor m_masterBoxEdgeColor; // master box (aka working volume) edge color
    QColor m_masterBoxFaceColor; // master box (aka working volume) face color
    double m_alpha;              // angle of rotation about the y-axis
    double m_beta;               // angle of rotation about the x-axis
    double m_cameraToWorldOriginDistance;           // distance from the viewer to the origin
    QPoint m_lastMousePosition;  // last position of the mouse
    QPoint m_selectedMousePosition; // position of the mouse when the last selection is made
    QOpenGLFramebufferObject * m_frameBuffer; // frame buffer used for picking
    QSet<int> m_selectedParts;   // indices of the selected parts
    bool m_isPicking;            // indicates if picking a part is in progress
    bool m_doKeepSelected;       // indicates if the previously selected parts are
                                 // to be kept selected when a new part is selected
};


// Non-members.
long int rgbToIndex(QRgb rgbColor);
QRgb indexToRgb(long int index);

#endif // OPEN_GL_WIDGET_HEADER
