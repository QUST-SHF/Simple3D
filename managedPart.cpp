//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "managedPart.h"

// Constructor.
ManagedPart::ManagedPart(std::shared_ptr<Part> part) : m_part(part)
{
    // Compute and set the volume.
    setVolume();
    // Compute and set the positions of the lower left and upper right corners.
    QVector3D maxCoord;
    QVector3D minCoord;
    m_part->coordinateRanges(minCoord, maxCoord);
    // Set the size of the minimal containing box.
    m_boxSize = maxCoord - minCoord;
    // Shift the part so that the lower left corner is at the origin.
    m_part->shift(-minCoord);

    // Smooth the vertex normals - replace the vertex normal in each instance
    // of that vertex in every triangle by the normalized sum of the normals
    // that vertex had in the different triangles. Does not seem to have good
    // effect on geometric shapes; keeping different normals for the same
    // vertex in the different triangles seems to be visually more pleasant.
    //m_part->smoothVertexNormals();
}
