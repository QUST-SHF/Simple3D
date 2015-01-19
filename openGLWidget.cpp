//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "openGLWidget.h"
#include "partStl.h"
#include "partsModel.h"

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLFramebufferObjectFormat>
#include <QScreen>
#include <QImage>


// Constructor.
OpenGLWidget::OpenGLWidget(QWidget *parent, PartsModel * partsModel)
    : QOpenGLWidget(parent), QOpenGLFunctions(), m_partsModel(partsModel),
      m_frameBuffer(nullptr), m_isPicking(false), m_doKeepSelected(false)
{
    // Set the angles of rotation about the y- and x-axis and the distance from
    // the viewer to the origin.
    m_alpha = -30;
    m_beta = -10;
    // Set the distance from the viewer to the origin.
    setDistanceFromCameraToWorldOrigin(- m_partsModel->masterBox().length());

    // Set the background color.
    m_backgroundColor = QColor(128, 128, 128);
    m_partColor = QColor(0, 141, 100);
    m_selectedPartColor = QColor(50, 100, 120);
    m_masterBoxEdgeColor = QColor(255, 165, 0);
    m_masterBoxFaceColor = QColor(255, 165, 0, 30);

    // Make the widget accept keyboard focus.
    setFocusPolicy(Qt::StrongFocus);
}

// Destructor.
OpenGLWidget::~OpenGLWidget()
{
    delete m_frameBuffer;
    m_frameBuffer = nullptr;
}

//=============================================================================
// The function "sizeHint" sets (a hint for) reasonable size for the widget.
//=============================================================================
QSize OpenGLWidget::sizeHint() const
{
    return QSize(640, 480);
}

//=============================================================================
// The function "initializeGL" sets the behavior of the rendering context and
// builds the shader programs.
//=============================================================================
void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);  // enable depth testing to properly show overlapping 3D objects
    glEnable(GL_CULL_FACE);   // enable face culling to ignore primitives not facing us and improve performance
    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(),
                 m_backgroundColor.blueF(), 1.0f); // set the background color

    // Add to the lighting shader program source files to be compiled and link them.
    m_lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShader.vsh");
    m_lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShader.fsh");
    m_lightingShaderProgram.link();

    // Create the master box vertex buffer.
    createMasterBoxBuffer();
}


//=============================================================================
// The function "updateWhenMasterBoxChange" updates the OpenGL widget's members
// that depend on the master box size.
//=============================================================================
void OpenGLWidget::updateWhenMasterBoxChange()
{
    // Set the distance from the viewer to the origin.
    setDistanceFromCameraToWorldOrigin(- m_partsModel->masterBox().length());

    // Recreate the master box vertex buffer.
    createMasterBoxBuffer();

    // Update the far clipping plane and hence the viewport.
    resizeGL(width(), height());
    // Redraw.
    update();
}


//=============================================================================
// The function "createMasterBoxBuffer" creates a vertex buffer on the GPU and
// fills it with the master box vertices.
//=============================================================================
void OpenGLWidget::createMasterBoxBuffer()
{
    // Get the master box dimensions.
    QVector3D masterBox = m_partsModel->masterBox();
    float x = masterBox.x();
    float y = masterBox.y();
    float z = masterBox.z();

    // Define the vertices of the master box as used to define the edges.
    QVector<QVector3D> vertices;
    vertices << QVector3D(0.0f, 0.0f, 0.0f) << QVector3D( x, 0.0f, 0.0f)
             << QVector3D( x,  y, 0.0f) << QVector3D(0.0f,  y, 0.0f) // Bottom edges
             << QVector3D(0.0f, 0.0f, z) << QVector3D(0.0f,  y, z)
             << QVector3D( x,  y, z) << QVector3D( x, 0.0f, z) // Top edges
             << QVector3D(0.0f, 0.0f, 0.0f) << QVector3D(0.0f, 0.0f, z)
             << QVector3D( x, 0.0f, 0.0f) << QVector3D( x, 0.0f, z)
             << QVector3D( x,  y, 0.0f) << QVector3D( x,  y, z)
             << QVector3D(0.0f,  y, 0.0f) << QVector3D(0.0f,  y, z)
             << QVector3D(0.0f, 0.0f, 0.0f) << QVector3D(0.0f, 0.0f, z); // Vertical edges
    // The repetition of the first vertical edge at the end is used when
    // vertical faces are drawing.

    // Define temporary storage where to store the coordinates as GLfloat's.
    QVector<GLfloat> buf;
    buf.reserve(vertices.size() * 3);
    for (auto cit = vertices.cbegin(); cit != vertices.cend(); ++cit)
    {
        buf.push_back(static_cast<GLfloat>(cit->x()));
        buf.push_back(static_cast<GLfloat>(cit->y()));
        buf.push_back(static_cast<GLfloat>(cit->z()));
    }

    // Create a buffer object, a general purpose array of data residing in the
    // graphics card’s memory, to store the vertices of the rendered object.
    m_masterBoxBuffer.destroy();  // clean in case previously created buffer exists
    m_masterBoxBuffer.create();
    m_masterBoxBuffer.bind();
    // Allocate memory and fill it in: for each vertex we need 3 vertex
    // coordinates.
    m_masterBoxBuffer.allocate(buf.constData(), buf.size() * sizeof(GLfloat));
    // Release the buffer.
    m_masterBoxBuffer.release();
}


//=============================================================================
// The function "addBuffer" places the last part in the parts model to a new
// OpenGL vertex buffer.
//=============================================================================
void OpenGLWidget::addBuffer()
{
    // Get the index of the last part.
    int partIndex = m_partsModel->numParts() - 1;

    // Create a vector holding properly interleaved data for each triangle:
    // 3 vertices followed by 3 normals.
    const int numVertices = m_partsModel->numVertices(partIndex);

    // Define temporary storage where to interleave the vertices and vertex
    // normals.
    QVector<GLfloat> buf;
    buf.reserve(numVertices * 6);

    // Get constant iterator to the beginnings of the vectors of vertices and
    // vertex normals of the last part.
    auto citVert = m_partsModel->vertexBeginIter(partIndex);
    auto citNorm = m_partsModel->vertexNormalBeginIter(partIndex);
    for (int i = 0; i < numVertices; ++i, ++citVert, ++citNorm)
    {
        buf.push_back(static_cast<GLfloat>(citVert->x()));
        buf.push_back(static_cast<GLfloat>(citVert->y()));
        buf.push_back(static_cast<GLfloat>(citVert->z()));
        buf.push_back(static_cast<GLfloat>(citNorm->x()));
        buf.push_back(static_cast<GLfloat>(citNorm->y()));
        buf.push_back(static_cast<GLfloat>(citNorm->z()));
    }

    // Construct a new vertex buffer for the data being added.
    m_buffers.push_back(QOpenGLBuffer());
    // Get a reference to the new vertex buffer just created.
    QOpenGLBuffer & openGLBuffer = m_buffers.back();

    // Create a buffer object, a general purpose array of data residing in the
    // graphics card’s memory, to store the vertices and vertex normals of the
    // rendered object.
    openGLBuffer.create();
    openGLBuffer.bind();
    // Allocate memory and fill it in: for each vertex we need 3 vertex
    // coordinates + 3 normal coordinates.
    openGLBuffer.allocate(buf.constData(), buf.size() * sizeof(GLfloat));
    // Release the buffer.
    openGLBuffer.release();

    update();
}


//=============================================================================
// The function "removeBuffer" removes the OpenGL vertex buffer corresponding
// to a specified part.
//=============================================================================
void OpenGLWidget::removeBuffer(int partIndex)
{
    m_buffers.removeAt(partIndex);
    update();
}


//=============================================================================
// The function "resizeGL" resets the projection matrix and the viewport
// whenever the widget is resized.
// INPUT: "int width" and "int height" are the new width and height.
//=============================================================================
void OpenGLWidget::resizeGL(int width, int height)
{
    initializeOpenGLFunctions();
    // Make sure division by zero is avoided.
    if (height == 0)
        height = 1;

    // Set the vertical field of view angle, and the distances from the viewer
    // to the front andn back clipping planes of the frustum.
    float verticalAngle = 60.0;
    float nearPlane = 0.1f;
    float farPlane = 10 * m_partsModel->masterBox().length();

    // Recompute the projection matrix from scratch. The vertical field of view
    // angle is specified and the horizontal one is computed to match the
    // aspect ratio of the given width and height.
    m_pMatrix.setToIdentity();
    m_pMatrix.perspective(verticalAngle, (float) width / (float) height, nearPlane, farPlane);

    // Reset the viewport according to the new size.
    glViewport(0, 0, width, height);
}


//=============================================================================
// The function "paintMasterBox" paints the edges and faces of the master box.
//=============================================================================
void OpenGLWidget::paintMasterBox()
{
    // Shift back to center the master box at the origin.
    m_mMatrix.setToIdentity();
    m_mMatrix.translate(- m_partsModel->masterBox() / 2);

    // Find the model-view matrix.
    QMatrix4x4 mvMatrix;
    mvMatrix = m_vMatrix * m_mMatrix;

    // Get the normal matrix corresponding to the model-view matrix. This
    // is the transpose of the inverse of the 3x3 upper left portion of the
    // model-view matrix which is used to transform normals from the object
    // space to the camera space. Yes, the transpose of the inverse
    // transforms normals from the object space to the camera space, just
    // as already said.
    QMatrix3x3 normalMatrix;
    normalMatrix = mvMatrix.normalMatrix();

    m_lightingShaderProgram.bind();

    // Hand on the transformation matrices and light position.
    m_lightingShaderProgram.setUniformValue("mvpMatrix", m_pMatrix * mvMatrix);
    m_lightingShaderProgram.setUniformValue("mvMatrix", mvMatrix);
    m_lightingShaderProgram.setUniformValue("normalMatrix", normalMatrix);

    // Make the diffuse and specular colors irrelevant.
    m_lightingShaderProgram.setUniformValue("diffuseReflection", (GLfloat) 0.0);
    m_lightingShaderProgram.setUniformValue("specularReflection", (GLfloat) 0.0);
    // Specify ambient color for the edges.
    m_lightingShaderProgram.setUniformValue("ambientColor", m_masterBoxEdgeColor);

    m_masterBoxBuffer.bind();
    // Specify how the shader program is to interpret the entries of the
    // buffer as inputs for its vertex shader.
    m_lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    m_masterBoxBuffer.release();

    // Enable the vertex array.
    m_lightingShaderProgram.enableAttributeArray("vertex");
    // Draw the edges.
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDrawArrays(GL_LINE_LOOP, 4, 4);
    glDrawArrays(GL_LINES, 8, 8);

    // Specify different ambient color for the faces.
    m_lightingShaderProgram.setUniformValue("ambientColor", m_masterBoxFaceColor);
    // Enable blending and set blending function.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the faces. Note that we use the same vertices as for the edges but
    // organize them differently.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 14, 4);
    // Disable the vertex array.
    m_lightingShaderProgram.disableAttributeArray("vertex");

    m_lightingShaderProgram.release();
}


//=============================================================================
// The function "paintGL" draws all parts and the master box.
//=============================================================================
void OpenGLWidget::paintGL()
{
    initializeOpenGLFunctions();

    // Initialize part picking.
    if (m_isPicking == true)
        initializePicking();

    // Clear the screen: fill the color buffer with the background color and
    // clear the depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Define the camera transformation matrix as composition of two rotations:
    // change in x-coordinate in screen space leads to rotation about z-axis in
    // camera space, and change in y-coordinate in screen space leads to
    // rotation about the x-axis in camera space.
    QMatrix4x4 cameraTransformation;
    cameraTransformation.rotate(m_alpha, 0, 0, 1);
    cameraTransformation.rotate(m_beta, 1, 0, 0);

    // Define the initial camera position and orientation in world space.
    QVector3D initCameraPosition(0, m_cameraToWorldOriginDistance, 0); // camera is on the y-axis in world space
    QVector3D initCameraUpDirection(0.0f, 0.0f, 1.0f);  // camera up direction is the z-axis in world space
    // Apply the two rotations to the camera position and up direction to get
    // a different camera position and orientation but still in world space.
    QVector3D cameraPosition = cameraTransformation * initCameraPosition;
    QVector3D cameraUpDirection = cameraTransformation * initCameraUpDirection;

    // Set the view matrix, i.e., the transformation from world space to camera
    // space. Note that this is the same for all parts. With the above camera
    // position and orientation we look at the origin of the world space.
    m_vMatrix.setToIdentity();
    m_vMatrix.lookAt(cameraPosition, QVector3D(0.0f, 0.0f, 0.0f), cameraUpDirection);

    // Set the position of the light source.
    QVector3D lightPosition = QVector3D(10, 10, 10);

    m_lightingShaderProgram.bind();

    m_lightingShaderProgram.setUniformValue("lightPosition", lightPosition);

    // Hand on the ambient, diffuse, and specular colors and reflections to the
    // fragment shader.
    // "ambientColor" is set in the loop below as needs.
    m_lightingShaderProgram.setUniformValue("diffuseColor", QColor(128, 128, 128));
    m_lightingShaderProgram.setUniformValue("specularColor", QColor(255, 255, 255));
    m_lightingShaderProgram.setUniformValue("ambientReflection", (GLfloat) 1.0);
    m_lightingShaderProgram.setUniformValue("diffuseReflection", (GLfloat) 1.0);
    m_lightingShaderProgram.setUniformValue("specularReflection", (GLfloat) 1.0);
    // Hand on the shininess.
    m_lightingShaderProgram.setUniformValue("shininess", (GLfloat) 100.0);

    // Get the object-space coordinates of the master box's center.
    QVector3D masterBoxCenterPosition = m_partsModel->masterBox() / 2;

    // Place the vertices and normals in an attribute buffer and enable them
    // for drawing.
    for (int i = 0; i < m_buffers.size(); ++i)
    {
        //=====================================================================
        // Note that the functions Matrix4x4::translate() and
        // Matrix4x4::translate() multiply the given matrix by a new one from
        // the right which is kind of awkward as one would have to build a
        // series of transformation in reverse of the order in which they are
        // to be applied to vectors, or use temporary matrices.
        //=====================================================================
        // Set the model matrix, i.e., the transformation from object space
        // to world space. If the part's orientation is to be changed, rotate
        // 90 degrees about the z-axis and slide the right until the lower left
        // corner is at the origin.
        m_mMatrix.setToIdentity();
        if (m_partsModel->doRotate(i) == true)
        {
            m_mMatrix.rotate(90, QVector3D(0.0f, 0.0f, 1.0f));
            QMatrix4x4 temp;
            temp.translate(QVector3D(m_partsModel->boxSize(i).y(), 0.0f, 0.0f));
            m_mMatrix = temp * m_mMatrix;
        }
        // Shift the part to the prescribed position and then shift back to
        // center the master box at the origin.
        QMatrix4x4 temp;
        temp.translate(m_partsModel->position(i) - masterBoxCenterPosition);
        m_mMatrix = temp * m_mMatrix;

        // Find the model-view matrix.
        QMatrix4x4 mvMatrix = m_vMatrix * m_mMatrix;

        // Get the normal matrix corresponding to the model-view matrix. This
        // is the transpose of the inverse of the 3x3 upper left portion of the
        // model-view matrix which is used to transform normals from the object
        // space to the camera space. Yes, the transpose of the inverse
        // transforms normals from the object space to the camera space, just
        // as already said.
        QMatrix3x3 normalMatrix = mvMatrix.normalMatrix();

        // Hand on the transformation matrices and light position.
        m_lightingShaderProgram.setUniformValue("mvpMatrix", m_pMatrix * mvMatrix);
        m_lightingShaderProgram.setUniformValue("mvMatrix", mvMatrix);
        m_lightingShaderProgram.setUniformValue("normalMatrix", normalMatrix);

        if (m_isPicking == true)
        {
            // Set each part's color differently in order to pick by color.
            setColorsForPicking(i);
        }
        else
        {
            // If the current part is selected, then change its ambient color.
            if (m_selectedParts.contains(i) == true)
                m_lightingShaderProgram.setUniformValue("ambientColor", m_selectedPartColor);
            else
                m_lightingShaderProgram.setUniformValue("ambientColor", m_partColor);
        }

        m_buffers[i].bind();
        // Specify how the shader program is to interpret the entries of the
        // buffer as inputs for its vertex shader.
        m_lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
        m_lightingShaderProgram.setAttributeBuffer("normal", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
        // Get the number of vertices in the current buffer.
        int numVertices = m_buffers[i].size() / (6 * sizeof(GLfloat));
        m_buffers[i].release();

        // Enable the vertex and vertex normal arrays.
        m_lightingShaderProgram.enableAttributeArray("vertex");
        m_lightingShaderProgram.enableAttributeArray("normal");
        // Draw the triangles.
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        // Disable the vertex and vertex normal arrays.
        m_lightingShaderProgram.disableAttributeArray("vertex");
        m_lightingShaderProgram.disableAttributeArray("normal");
    }
    m_lightingShaderProgram.release();

    // Draw the edges and faces of the master box.
    paintMasterBox();

    // Finish off part picking.
    if (m_isPicking == true)
        finishOffPicking();
}


//=============================================================================
// The function "initializePicking" creates an auxiliary frame buffer for
// off-screen drawing.
//=============================================================================
void OpenGLWidget::initializePicking()
{

    // Create a new frame buffer of the same size as the default frame
    // buffer used for rendering the parts. Attach a depth buffer to it.
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    m_frameBuffer = new QOpenGLFramebufferObject(size(), format);
}


//=============================================================================
// The function "setColorsForPicking" sets the colors of the different parts to
// be used in the auxiliary frame buffer for off-screen drawing.
// INPUT: "int partIndex" is the index of a part.
//=============================================================================
void OpenGLWidget::setColorsForPicking(int partIndex)
{
    // If picking, keep the ambient reflection as 1, and set the diffuse and
    // specular reflections to 0 (so that the diffuse and specular colors
    // become immaterial). Set a unique ambient color for each buffer so that
    // it can be identified by that color.
    m_lightingShaderProgram.setUniformValue("diffuseReflection", (GLfloat) 0.0);
    m_lightingShaderProgram.setUniformValue("specularReflection", (GLfloat) 0.0);
    m_lightingShaderProgram.setUniformValue("ambientColor", QColor(indexToRgb(partIndex)));
}


//=============================================================================
// The function "finishOffPicking" completes the part picking by identifying
// the selected part, and releasing and destroying the auxiliary frame buffer
// used for off-screen drawing.
//=============================================================================
void OpenGLWidget::finishOffPicking()
{
    GLfloat pixelComponents[3];
    // Take into account that the function "glReadPixels" assumes the
    // origin is at the lower left corner while the mouse assumes it is at
    // the top left corner.
    glReadPixels(m_selectedMousePosition.x(), size().height() - 1 - m_selectedMousePosition.y(),
                 1, 1, GL_RGB, GL_FLOAT, static_cast<void *>(pixelComponents));
    int selectedPart = rgbToIndex(qRgb(static_cast<int>(255 * pixelComponents[0]),
                                  static_cast<int>(255 * pixelComponents[1]),
            static_cast<int>(255 * pixelComponents[2])));

    //        // This also works but it seems that creating the whole image would
    //        // require more computing which can be avoided by querying for the
    //        // particular pixel.
    //        //
    //        // Export the frame buffer to an image.
    //        QImage frame = m_frameBuffer->toImage();
    //        // Get the pixel at the position where the mouse was released.
    //        QRgb pix = frame.pixel(m_selectedMousePosition);
    //        int selectedPart = rgbToIndex(pix);

    // Clear previously selected parts if necessary.
    if (m_doKeepSelected == false)
        m_selectedParts.clear();
    // Store the selection unless it is background. If the picked part has been
    // already selected, then deselect it.
    if ((0 <= selectedPart)&&(selectedPart <= m_buffers.size()))
    {
        if (m_selectedParts.contains(selectedPart) == false)
            m_selectedParts.insert(selectedPart);
        else
            m_selectedParts.remove(selectedPart);
    }

    // Stop picking, release the frame buffer, and delete it.
    m_isPicking = false;
    m_frameBuffer->release();
    delete m_frameBuffer;
    m_frameBuffer = nullptr;

    // Repaint to show the selected part.
    update();

    // Emit a signal that a new part has been selected or all parts were
    // deselected.
    emit selectedPartsChanged();
}


//=============================================================================
// The function "keyPressEvent" looks for pressed Ctrl key.
// INPUT: "QKeyEvent * event" is a keyboard event.
//=============================================================================
void OpenGLWidget::keyPressEvent(QKeyEvent * event)
{
    // If the Ctrl key is pressed then all currently selected parts will be
    // kept selected even if a new part is clicked-on.
    if (event->key() == Qt::Key_Control)
    {
        m_doKeepSelected = true;
    }
    else
    {
        // Call the base class version.
        QOpenGLWidget::keyPressEvent(event);
    }
}


//=============================================================================
// The function "keyPressEvent" looks for released Ctrl key.
// INPUT: "QKeyEvent * event" is a keyboard event.
//=============================================================================
void OpenGLWidget::keyReleaseEvent(QKeyEvent * event)
{
    // If the Ctrl key is released then all currently selected parts will be
    // unselected as soon as a new part or the background is clicked-on.
    if (event->key() == Qt::Key_Control)
    {
        m_doKeepSelected = false;
    }
    else
    {
        // Call the base class version.
        QOpenGLWidget::keyReleaseEvent(event);
    }
}


//=============================================================================
// The function "mouseReleaseEvent" looks for the mouse position when the mouse
// button is released.
// INPUT: "QMouseEvent * event" is a mouse event.
//=============================================================================
void OpenGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    // Get the mouse position when the button was released.
    m_selectedMousePosition = event->pos();
    // Enter picking mode.
    m_isPicking = true;

    // Repaint in picking mode (i.e., off screen) in order to identify the
    // clicked-on part.
    update();
    emit event->accept();
}


//=============================================================================
// The function "mousePressEvent" looks for the mouse position when the mouse
// button is pressed.
// INPUT: "QMouseEvent * event" is a mouse event.
//=============================================================================
void OpenGLWidget::mousePressEvent(QMouseEvent * event)
{
    // Get the mouse position when the button was pressed.
    m_lastMousePosition = event->pos();

    emit event->accept();
}


//=============================================================================
// The function "mouseMoveEvent" looks for the mouse's change of position when
// the mouse is moved and converts it into a rotation.
// INPUT: "QMouseEvent * event" is a mouse event.
//=============================================================================
void OpenGLWidget::mouseMoveEvent(QMouseEvent * event)
{
    int deltaX = event->x() - m_lastMousePosition.x();
    int deltaY = event->y() - m_lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        m_alpha -= deltaX;
        while (m_alpha < 0) {
            m_alpha += 360;
        }
        while (m_alpha >= 360) {
            m_alpha -= 360;
        }

        m_beta -= deltaY;
        if (m_beta < -90) {
            m_beta = -90;
        }
        if (m_beta > 90) {
            m_beta = 90;
        }

        update();
    }

    m_lastMousePosition = event->pos();

    emit event->accept();
}


//=============================================================================
// The function "wheelEvent" looks for wheel scrolling to deduct the user's
// zooming intentions.
// INPUT: "QWheelEvent * event" is a wheel event.
//=============================================================================
void OpenGLWidget::wheelEvent(QWheelEvent * event)
{
    int delta = event->delta();

    if (event->orientation() == Qt::Vertical) {
        if (delta < 0) {
            m_cameraToWorldOriginDistance *= 1.1;
        } else if (delta > 0) {
            m_cameraToWorldOriginDistance *= 0.9;
        }

        update();
    }

    emit event->accept();
}


// Non-members converting RGB color to index and vice versa.
long int rgbToIndex(QRgb rgbColor)
{
    return qRed(rgbColor) + 256*qGreen(rgbColor) + 256*256*qBlue(rgbColor);
}

QRgb indexToRgb(long int index)
{
    return qRgb(index % 256, (index / 256) % 256, (index / (256 * 256)) % 256);
}
