//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef PART_HEADER
#define PART_HEADER

#include <QObject>
#include <QString>
#include <QVector>
#include <QVector3D>

class Part
{
public:
    explicit Part();
    explicit Part(const Part & part);
    virtual ~Part() = 0;

    // Write out data.
    virtual void writeData(const QString & filename) const = 0;

    // Shift the vertices by a given 3D vector.
    void shift(const QVector3D & shift);

    // Rotate 90 degrees counterclockwise the vertices and the vertex normals.
    void rotate();

    // Find the x-, y-, and z-ranges of the part.
    void coordinateRanges(QVector3D & minCoord, QVector3D & maxCoord) const;

    // Shift and scale vertices so that the object is "centered" at the origin
    // and fits in the cube [-1,1]^3.
    void normalizeVertices();

    // Check if given face normals agree with those computed from triangles.
    bool areFaceNormalsCorrect(const QVector<QVector3D> & faceNormals, float threshold) const;

    // Set each vertex's normal to the one computed from its triangle.
    void setVertexNormals();
    void setVertexNormalsAsCrossProducts();

    // Set existing face normals as vertex normals.
    void setFaceNormalsAsVertexNormals(const QVector<QVector3D> & faceNormals);

    // Compute the volume encompassed by the closed surface.
    double computeVolume();

    // Smoothe the vertex normals by replacing the vertex normal in each
    // instance of a vertex in every triangle by the normalized sum of the
    // normals that vertex had in the different triangles.
    void smoothVertexNormals();

    // Accessors.
    qint32 numTriangles() const { return m_numTriangles; }
    long int numVertices() const { return m_vertices.size(); }

    QVector<QVector3D>::const_iterator vertexBeginIter() const { return m_vertices.cbegin(); }
    QVector<QVector3D>::const_iterator vertexEndIter() const { return m_vertices.cend(); }
    const QVector3D * vertexConstData() const { return &(*m_vertices.cbegin()); }
    const QVector<QVector3D> & vertices() const { return m_vertices; }
    QVector<QVector3D>::const_iterator vertexNormalBeginIter() const { return m_vertexNormals.cbegin(); }
    QVector<QVector3D>::const_iterator vertexNormalEndIter() const { return m_vertexNormals.cend(); }
    const QVector3D * vertexNormalConstData() const { return &(*m_vertexNormals.cbegin()); }
    const QVector<QVector3D> & vertexNormals() const { return m_vertexNormals; }

protected:
    qint32 m_numTriangles;               // number of triangles
    QVector<QVector3D> m_vertices;       // all vertices, 3 per triangle
    QVector<QVector3D> m_vertexNormals;  // all vertex normals, 3 per triangle
};


// Non-members.
void findCoordinateRanges(const QVector<QVector3D> & vertices,
                          QVector3D & minCoord, QVector3D & maxCoord);

#endif // PART_HEADER
