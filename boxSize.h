//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef BOX_SIZE_HEADER
#define BOX_SIZE_HEADER

#include <QVector3D>

class BoxSize : public QVector3D
{
public:
    BoxSize() : QVector3D() {}
    BoxSize(float x, float y) : QVector3D(x, y, 0.0f) {}
    BoxSize(float x, float y, float z) : QVector3D(x, y, z) {}
    BoxSize(const QVector3D & box) : QVector3D(box) {}
    ~BoxSize() {}

    void rotate();
};

typedef BoxSize Position;

#endif // BOX_SIZE_HEADER
