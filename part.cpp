//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "part.h"

#include <QFile>
#include <QString>
#include <QStringList>
#include <QIODevice>
#include <QDataStream>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <algorithm>  // swap, lexicographical_compare

// Constructors.
Part::Part() : m_numTriangles(0), m_vertices(), m_vertexNormals()
{}

Part::Part(const Part & part) :
    m_numTriangles(part.numTriangles()),
    m_vertices(part.vertices()),
    m_vertexNormals(part.vertexNormals())
{}

Part::~Part() {}


//=============================================================================
// The function "shift" shifts the vertices by a given 3D vector.
// INPUT: "const QVector3D & shift" is a 3D vector to be added to all vertices.
//=============================================================================
void Part::shift(const QVector3D & shift)
{
    for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
        *it += shift;
}


//=============================================================================
// The function "rotate" rotates a given 3D vector 90 degrees counterclockwise
// in the xy-plane.
// INPUT: "QVector3D & v" is a 3D vector.
// NOTE: This is a non-member function.
//=============================================================================
void rotate(QVector3D & v)
{
    float xTemp = v.x();
    v.setX(-v.y());
    v.setY(xTemp);
}


//=============================================================================
// The function "rotate" rotates all vertices and vertex normals 90 degrees
// counterclockwise.
//=============================================================================
void Part::rotate()
{
    for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
        ::rotate(*it);
    for (auto it = m_vertexNormals.begin(); it != m_vertexNormals.end(); ++it)
        ::rotate(*it);
}


//=============================================================================
// The function "coordinateRanges" finds the x-, y-, and z-ranges of the part.
// OUTPUT: "QVector3D & minCoord" and "QVector3D & maxCoord" return the
// smallest and the largest coordinates of the part.
//=============================================================================
void Part::coordinateRanges(QVector3D & minCoord, QVector3D & maxCoord) const
{
    findCoordinateRanges(m_vertices, minCoord, maxCoord);
}


//=============================================================================
// The function "normalizeVertices" shifts and scales vertices so that the
// object is "centered" at the origin and fits in the cube [-1,1]^3.
//=============================================================================
void Part::normalizeVertices()
{
    // Find the ranges of the coordinates of the vertices.
    QVector3D minCoord;
    QVector3D maxCoord;
    findCoordinateRanges(m_vertices, minCoord, maxCoord);
    qDebug() << "Min coordinates: " << minCoord;
    qDebug() << "Max coordinates: " << maxCoord;

    // Shift and scale all vertices.
    QVector3D shift = (maxCoord + minCoord) / 2;
    QVector3D diameter = (maxCoord - minCoord) / 2;
    float scale = qMax(diameter.x(), qMax(diameter.y(), diameter.z()));
    for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
        *it = (*it - shift) / scale;
}


//=============================================================================
// The function "setFaceNormalsAsVertexNormals" sets given face normals as
// vertex normals by simply setting three consecutive copies of the face
// normal, one for each vertex.
// INPUT: "const QVector<QVector3D> & faceNormals" are face (triangle) normals.
//=============================================================================
void Part::setFaceNormalsAsVertexNormals(const QVector<QVector3D> & faceNormals)
{
    m_vertexNormals.clear();
    m_vertexNormals.reserve(3 * faceNormals.size());
    for (auto cit = faceNormals.cbegin(); cit != faceNormals.cend(); ++cit)
    {
        m_vertexNormals.push_back(*cit);
        m_vertexNormals.push_back(*cit);
        m_vertexNormals.push_back(*cit);
    }
}


//=============================================================================
// The function "areFaceNormalsCorrect" checks if all read normals agree with
// those computed from the triangles assuming the normal points toward the
// viewer when the vertices of the triangle are listed counterclockwise.
// INPUT: "const QVector<QVector3D> & faceNormals" are face (triangle) normals.
// "float threshold" specifies the maximal squared length of the difference of
// two 3D vectors for which they are considered the same.
// OUTPUT: The function returns "true" is all normals are correct and "false"
// otherwise.
//=============================================================================
bool Part::areFaceNormalsCorrect(const QVector<QVector3D> & faceNormals, float threshold) const
{
    auto citTriangle = faceNormals.cbegin();
    for (auto cit = m_vertices.cbegin(); cit != m_vertices.cend(); cit += 3, ++citTriangle)
    {
        // Compute the current triangle's normal.
        QVector3D diff = *citTriangle - QVector3D::normal(*cit, *(cit + 1), *(cit + 2));
        if (diff.lengthSquared() > threshold)
            return false;
    }
    return true;
}


//=============================================================================
// The function "setVertexNormals" computes each triangle's normal and sets it
// as a vertex normal for each vertex of that triangle.
//=============================================================================
void Part::setVertexNormals()
{
    m_vertexNormals.clear();
    m_vertexNormals.reserve(m_vertices.size());
    for (auto cit = m_vertices.cbegin(); cit != m_vertices.cend(); cit += 3)
    {
        QVector3D currNormal = QVector3D::normal(*cit, *(cit + 1), *(cit + 2));
        m_vertexNormals.push_back(currNormal);
        m_vertexNormals.push_back(currNormal);
        m_vertexNormals.push_back(currNormal);
    }
}


//=============================================================================
// The function "setVertexNormalsAsCrossProducts" computes each triangle's
// normal as the cross product of two of its sizes, i.e., it is a normal vector
// whose length is proportional to the triangle's area rather than being one,
// and sets it as a vertex normal for each vertex of that triangle.
//=============================================================================
void Part::setVertexNormalsAsCrossProducts()
{
    m_vertexNormals.clear();
    m_vertexNormals.reserve(m_vertices.size());
    for (auto cit = m_vertices.cbegin(); cit != m_vertices.cend(); cit += 3)
    {
        QVector3D currNormal = QVector3D::crossProduct(*(cit + 1) - *cit, *(cit + 2) - *cit);
        m_vertexNormals.push_back(currNormal);
        m_vertexNormals.push_back(currNormal);
        m_vertexNormals.push_back(currNormal);
    }
}


//=============================================================================
// The function "computeVolume" computes the volume encompassed by the closed
// surface. Coordinates are asuumed to be in milimeters (mm).
// OUTPUT: The function returns the computed volume in mililiters (mL).
//=============================================================================
double Part::computeVolume()
{
    double volume = 0.0f;
    for (auto cit = m_vertices.cbegin(); cit != m_vertices.cend(); cit += 3)
        volume += QVector3D::dotProduct(QVector3D::crossProduct(*cit, *(cit + 1)), *(cit + 2));
    // Divide by 6 to get volume and further by 1000 to convert from cubic mm
    // into cubic cm (aka mL).
    return qAbs(volume / 6000.0f);  // in mL
}


//=============================================================================
// The function "findCoordinateRanges" finds the smallest and largest of
// coordinates of a collection of 3D vectors.
// INPUT: "const QVector<QVector3D> & vertices" is a collection of 3D vectors.
// OUTPUT: "QVector3D & minCoord" returns the smallest x, y, and z coordinates.
// "QVector3D & maxCoord" returns the largest x, y, and z coordinates.
//=============================================================================
void findCoordinateRanges(const QVector<QVector3D> & vertices,
                          QVector3D & minCoord, QVector3D & maxCoord)
{
    // Initialize using the first 3D vector.
    QVector3D vert = *vertices.cbegin();
    float xMin, xMax, yMin, yMax, zMin, zMax;
    xMin = xMax = vert.x();
    yMin = yMax = vert.y();
    zMin = zMax = vert.z();

    // Find the ranges of the coordinates of the vertices.
    for (auto cit = vertices.cbegin(); cit != vertices.cend(); ++cit)
    {
        if (xMin > cit->x())
            xMin = cit->x();
        else if (xMax < cit->x())
            xMax = cit->x();
        if (yMin > cit->y())
            yMin = cit->y();
        else if (yMax < cit->y())
            yMax = cit->y();
        if (zMin > cit->z())
            zMin = cit->z();
        else if (zMax < cit->z())
            zMax = cit->z();
    }

    // Set the output.
    minCoord.setX(xMin);
    minCoord.setY(yMin);
    minCoord.setZ(zMin);
    maxCoord.setX(xMax);
    maxCoord.setY(yMax);
    maxCoord.setZ(zMax);
}




#include <unordered_map>  // unordered_map
#include <utility>        // make_pair
#include <QVector3D>
#include <boost/functional/hash.hpp> // hash_combine

//=============================================================================
// The class "vector3D_hash" computes a hash value for a 3D vectors.
//=============================================================================
struct vector3D_hash
{
    std::size_t operator()(const QVector3D & vect) const
    {
        size_t seed = 0;
        for(int i = 0; i < 3; ++i)
            boost::hash_combine(seed, vect[i]);
        return seed;
    }
};

//=============================================================================
// The class "vector3D_equal_fn" compares two 3D vectors for being the same.
//=============================================================================
struct vector3D_equal_fn
{
    bool operator() (QVector3D const & v1, QVector3D const & v2) const
    {
      return qFuzzyCompare(v1, v2);
    }
};


//=============================================================================
// The function "smoothVertexNormals" smoothes the vertex normals by replacing
// the vertex normal in each instance of a vertex in every triangle by the
// normalized sum of the normals that vertex had in the different triangles.
// REMARK: The procedure does not seem to have good effect on geometric shapes;
// keeping different normals for the same vertex in the different triangles
// seems to be visually more pleasant.
//=============================================================================
void Part::smoothVertexNormals()
{
    // Place all vertices as keys in a hash table where the value of each key
    // is the average of all normals that that vertex has in the different
    // triangles in which it participates.
    std::unordered_map<QVector3D, QVector3D, vector3D_hash, vector3D_equal_fn> vertexToNormal;

    for (auto citVert = m_vertices.cbegin(), citNorm = m_vertexNormals.cbegin();
         citVert != m_vertices.cend(); ++citVert, ++citNorm)
    {
        vertexToNormal[*citVert] += *citNorm;
    }

    // Normalize the averaged normals.
    for (auto & vt : vertexToNormal)
        vt.second.normalize();

    // Assign the averaged normal to each vertex.
    for (auto itVert = m_vertices.begin(), itNorm = m_vertexNormals.begin();
         itVert != m_vertices.end(); ++itVert, ++itNorm)
    {
        *itNorm = vertexToNormal[*itVert];
    }
}
