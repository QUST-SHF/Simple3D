//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "partStl.h"

#include <QFile>
#include <QString>
#include <QStringList>
#include <QIODevice>
#include <QDataStream>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <algorithm>  // swap

// Constructors.
PartStl::PartStl() : Part(), m_filename()
{}

PartStl::PartStl(const QString & filename) : Part(), m_filename(filename)
{
    readFile();
}

PartStl::PartStl(const PartStl & stlPart) : Part(stlPart), m_filename(stlPart.filename())
{}


// Write out data.
void PartStl::writeData(const QString & filename) const
{
    writeBinaryFile(filename);
}

//=============================================================================
// The function "readBinaryFile" reads in a given binary STL file.
//=============================================================================
bool PartStl::readFile()
{
    // Read in face normals and vertices from an STL file.
    QVector<QVector3D> faceNormals;
    bool success = readStlFile(m_filename, faceNormals, m_vertices);
    if (success == true)
    {
        // Get the number of triangles.
        m_numTriangles = faceNormals.size();

        // Check if the face normals match the vertices.
        if (areFaceNormalsCorrect(faceNormals, 0.001f) == false)
        {
            qDebug() << "Incorrect normals detected.";
            // Set the vertex normals using the computed face normals.
            setVertexNormals();
            //setVertexNormalsAsCrossProducts(); // normals that are not unit but proportional to triangle area
        }
        else
        {
            // Set the vertex normals using the read face normals.
            setFaceNormalsAsVertexNormals(faceNormals);
        }
    }
    return success;
}


//=============================================================================
// The function "writeBinaryFile" write out a given binary STL file.
// INPUT: "const QString & filename" is the name of the binary file to be
// written.
//=============================================================================
void PartStl::writeBinaryFile(const QString & filename) const
{
    writeBinaryStlFile(filename, m_vertices, m_vertexNormals, 3);
}


//=============================================================================
// The function "appendPartStl" appends a given stl part.
// INPUT: "const PartStl & stlPart" is an stl part to append.
//=============================================================================
void PartStl::appendPartStl(const PartStl & stlPart)
{
    m_numTriangles += stlPart.numTriangles();
    for (auto cit = stlPart.vertexBeginIter(); cit != stlPart.vertexEndIter(); ++cit)
        m_vertices.push_back(*cit);
    for (auto cit = stlPart.vertexNormalBeginIter(); cit != stlPart.vertexNormalEndIter(); ++cit)
        m_vertexNormals.push_back(*cit);
}


//=============================================================================
// The function "readBinaryStlFile" reads in a given binary STL file.
// INPUT: "const QString & fileName" is the name of the file to be read.
// OUTPUT: "QVector<QVector3D> & normals" returns the normals of the triangles.
// "QVector<QVector3D> & vertices" returns the vertices of the triangles.
// The function itself returns "false" if any discrepancy is encountered and
// "true" otherwise.
//=============================================================================
bool readBinaryStlFile(const QString & fileName, QVector<QVector3D> & normals,
                       QVector<QVector3D> & vertices)
{
    // The binary STL format:
    //    UINT8[80] – Header
    //    UINT32 – Number of triangles

    //    foreach triangle
    //    REAL32[3] – Normal vector
    //    REAL32[3] – Vertex 1
    //    REAL32[3] – Vertex 2
    //    REAL32[3] – Vertex 3
    //    UINT16 – Attribute byte count
    //    end

    // Specify and open the file for reading.
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly) == false)
    {
        QMessageBox::information(0, "Could not open the file " + fileName +
                                 " for binary reading.", file.errorString());
        return false;
    }

    // Read the data serialized from the file.
    QDataStream in(&file);

    // Set the floating precision so that floats a 32 bits.
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // Set the byte order to be the "little endian" which is commonly assumed
    // for STL files.
    in.setByteOrder(QDataStream::LittleEndian);

    // Skip the header.
    int skipLength = 80;
    int readBytes = in.skipRawData(skipLength);
    if (readBytes != skipLength)
        return false;

    // Read in the number of triangles.
    quint32 numTriangles;
    in >> numTriangles;

    // Clear the output vectors.
    normals.clear();
    vertices.clear();

    // Reserve sufficient memory to hold all normals and vertices of triangles.
    normals.reserve(numTriangles);
    vertices.reserve(3*numTriangles);

    // Read in the triangles.
    for (quint32 i = 0; i < numTriangles; ++i)
    {
        // Read in a normal.
        float x, y, z;
        in >> x >> y >> z;
        normals.push_back(QVector3D(x, y, z));

        // Read in three vertices.
        for (int k = 0; k < 3; ++k)
        {
            in >> x >> y >> z;
            vertices.push_back(QVector3D(x, y, z));
        }

        // Skip the two attribute bytes.
        quint16 t;
        in >> t;
    }
    file.close();
    return true;
}


//=============================================================================
// The function "writeBinaryStlFile" writes to a binary STL file.
// INPUT: "const QString & fileName" is the name of the file to be written to.
// "QVector<QVector3D> & normals" are the normals of the triangles.
// "QVector<QVector3D> & vertices" are the vertices of the triangles.
// "int skipStep" indicates how many of the normals are to be skipped between
// two writings. It is used with value of 3 when the face normals are given.
//=============================================================================
void writeBinaryStlFile(const QString & fileName, const QVector<QVector3D> & vertices,
                        const QVector<QVector3D> & normals, int skipStep)
{
    // The binary STL format:
    //    UINT8[80] – Header
    //    UINT32 – Number of triangles

    //    foreach triangle
    //    REAL32[3] – Normal vector
    //    REAL32[3] – Vertex 1
    //    REAL32[3] – Vertex 2
    //    REAL32[3] – Vertex 3
    //    UINT16 – Attribute byte count
    //    end

    // Specify and open the file for writing.
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly) == false)
    {
        QMessageBox::information(0, "Could not open the file " + fileName +
                                 " for writing.", file.errorString());
    }

    // Write the data serialized to the file.
    QDataStream out(&file);

    // Set the floating precision so that floats a 32 bits.
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // Set the byte order to be the "little endian" which is commonly assumed
    // for STL files.
    out.setByteOrder(QDataStream::LittleEndian);

    // Write an empty header.
    qint8 t = 0;
    for (int i = 0; i < 80; ++i)
        out << t;

    // Write out the number of triangles.
    quint32 numTriangles = normals.size();
    out << numTriangles;

    // Write out the triangles.
    for (quint32 i = 0; i < numTriangles; ++i)
    {
        // Write out a normal.
        out << normals[skipStep*i].x() << normals[skipStep*i].y() << normals[skipStep*i].z();

        // Write out three vertices.
        for (int k = 0; k < 3; ++k)
            out << vertices[3*i + k].x() << vertices[3*i + k].y() << vertices[3*i + k].z();

        // Write the two attribute bytes.
        quint16 t = 0;
        out << t;
    }
    file.close();
}


//=============================================================================
// The function "readStlFile" reads in a given STL file (binary or ascii).
// INPUT: "const QString & fileName" is the name of the file to be read.
// OUTPUT: "QVector<QVector3D> & normals" returns the normals of the triangles.
// "QVector<QVector3D> & vertices" returns the vertices of the triangles.
// The function itself returns "false" if any discrepancy is encountered and
// "true" otherwise.
//=============================================================================
bool readStlFile(const QString & fileName, QVector<QVector3D> & normals, QVector<QVector3D> & vertices)
{
    // Specify and open the file for reading.
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly) == false)
    {
        QMessageBox::information(0, "Could not open the file " + fileName +
                                 " for reading.", file.errorString());
    }

    // Read in the first line of text from the file.
    QTextStream inText(&file);
    QString line = inText.readLine();
    file.close();
    // If the first line strats with the word "solid" then assume ascii.
    bool success;
    if (line.startsWith("solid ") == true)
    {
        success = readAsciiStlFile(fileName, normals, vertices);
        if (success == false)
            QMessageBox::information(0, "Read Error", "Ascii STL format seems violated.");
    }
    else
    {
        success = readBinaryStlFile(fileName, normals, vertices);
        if (success == false)
            QMessageBox::information(0, "Read Error", "Binary STL format seems violated.");
    }
    return success;
}


//=============================================================================
// The function "readAsciiStlFile" reads in a given ascii STL file.
// INPUT: "const QString & fileName" is the name of the file to be read.
// OUTPUT: "QVector<QVector3D> & normals" returns the normals of the triangles.
// "QVector<QVector3D> & vertices" returns the vertices of the triangles.
// The function itself returns "false" if any discrepancy is encountered and
// "true" otherwise.
//=============================================================================
bool readAsciiStlFile(const QString & fileName, QVector<QVector3D> & normals,
                      QVector<QVector3D> & vertices)
{
    // The ascii STL format:
    // solid name
    // facet normal ni nj nk
    //     outer loop
    //         vertex v1x v1y v1z
    //         vertex v2x v2y v2z
    //         vertex v3x v3y v3z
    //     endloop
    // endfacet
    // endsolid name

    // Specify and open the file for reading.
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly) == false)
    {
        QMessageBox::information(0, "Could not open the file " + fileName +
                                 " for ascii reading.", file.errorString());
        return false;
    }

    // Read the text from the file.
    QTextStream in(&file);
    // Read in the first line.
    QString line = in.readLine();

    // Define temporary storage.
    QVector<QVector3D> tempNormals;
    QVector<QVector3D> tempVertices;
    QStringList fields;
    // Read in the second line.
    line = in.readLine();
    // Keep reading until the end of the file or a line strating with the word
    // "endsolid" is reached.
    while ((in.atEnd() == false) && (line.simplified().startsWith("endsolid ") == false))
    {
        // Split the line into space-separated strings.
        fields = line.simplified().split(" ");
        if ((fields.size() != 5) || (fields[0] != "facet") || (fields[1] != "normal"))
            return false;
        // Store the normal.
        tempNormals.push_back(QVector3D(fields[2].toDouble(), fields[3].toDouble(), fields[4].toDouble()));

        // Read in the next line.
        line = in.readLine();
        if (line.simplified().startsWith("outer loop") == false)
            return false;

        // Read in three vertices.
        for (int k = 0; k < 3; ++k)
        {
            // Read in the next line.
            line = in.readLine();
            // Split the line into space-separated strings.
            fields = line.simplified().split(" ");
            if ((fields.size() != 4) || (fields[0] != "vertex"))
                return false;
            // Store the vertex.
            tempVertices.push_back(QVector3D(fields[1].toDouble(), fields[2].toDouble(), fields[3].toDouble()));
        }

        // Read in the next line.
        line = in.readLine();
        if (line.simplified().startsWith("endloop") == false)
            return false;

        // Read in the next line.
        line = in.readLine();
        if (line.simplified().startsWith("endfacet") == false)
            return false;

        // Read in the next line.
        line = in.readLine();
    }

    // Set up the output.
    std::swap(normals, tempNormals);
    std::swap(vertices, tempVertices);
    return true;
}


//=============================================================================
// The function "createLargeStlFile" creates a large STL file by replicating
// the object in a given STL file as many times as necessary to achieve a
// specified minimal number of triangles in the new STL file.
// INPUT: "const QString & filename" is an STL file.
// "long int targetNumTriangles" is the desired minimal number of triangles to
// be in the new STL file.
//=============================================================================
void createLargeStlFile(const QString & filename, long int targetNumTriangles)
{
    // Read in the given file.
    PartStl stlPart(filename);

    // Compute a reasonable shift that guarantees that the copies do not
    // overlap.
    QVector3D minCoord;
    QVector3D maxCoord;
    findCoordinateRanges(stlPart.vertices(), minCoord, maxCoord);
    float shiftStep = maxCoord.y() - minCoord.y() + 1.0f;

    // Keep adding copies along the y-axis.
    PartStl largeStlPart;
    long int currNumTriangles = 0;
    while (currNumTriangles < targetNumTriangles)
    {
        stlPart.shift(QVector3D(0, shiftStep, 0));
        largeStlPart.appendPartStl(stlPart);
        currNumTriangles += stlPart.numTriangles();
    }

    // Come up with a name for the large file.
    QString largeFilename = filename;
    largeFilename.insert(filename.length() - 4, QString("Large"));
    qDebug() << largeFilename;
    largeStlPart.writeBinaryFile(largeFilename);
}
