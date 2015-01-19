//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef PART_STL_HEADER
#define PART_STL_HEADER

#include <QString>
#include <QVector>
#include <QVector3D>
#include "part.h"

class PartStl : public Part
{
public:
    // Constructors.
    PartStl();
    PartStl(const QString & fileName);
    PartStl(const PartStl & part);
    virtual ~PartStl() override {}

    // Write out data.
    virtual void writeData(const QString & filename) const override;

    // Append a given stl part to this part.
    void appendPartStl(const PartStl & stlPart);

    // Write out a binary file.
    void writeBinaryFile(const QString & filename) const;

    // Accessors.
    QString filename() const { return m_filename; }

protected:
    // Read in a file.
    bool readFile();

private:
    QString m_filename;
};


// Non-members.
bool readStlFile(const QString & fileName, QVector<QVector3D> & normals, QVector<QVector3D> & vertices);

bool readBinaryStlFile(const QString & fileName, QVector<QVector3D> & normals,
                       QVector<QVector3D> & vertices);

void writeBinaryStlFile(const QString & fileName, const QVector<QVector3D> & vertices,
                        const QVector<QVector3D> & normals, int skipStep);

bool readAsciiStlFile(const QString & fileName, QVector<QVector3D> & normals,
                      QVector<QVector3D> & vertices);

void createLargeStlFile(const QString & filename, long int targetNumTriangles = 2000000);

#endif // PART_STL_HEADER
