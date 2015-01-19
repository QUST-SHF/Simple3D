//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef MANAGED_PART_HEADER
#define MANAGED_PART_HEADER

#include <memory>   // shared_ptr
#include "part.h"
#include "boxSize.h"

class ManagedPart
{
public:
    // Constructor.
    explicit ManagedPart(std::shared_ptr<Part> part = std::shared_ptr<Part>());

    // Accessors.
    std::shared_ptr<Part> part() const { return m_part; }
    double volume() const { return m_volume; }
    const BoxSize & boxSize() const { return m_boxSize; }
    const Position & drawingPosition() const { return m_drawingPosition; }
    bool doRotateBeforeDrawing() const { return m_doRotateBeforeDrawing; }

    // Setters.
    void setVolume() { m_volume = m_part->computeVolume(); }
    void setDrawingPosition(const Position & position) { m_drawingPosition = position; }
    void setDoRotateBeforeDrawing(bool doRotate) { m_doRotateBeforeDrawing = doRotate; }

private:
    std::shared_ptr<Part> m_part;   // the part being managed
    double m_volume;                // volume of the part
    BoxSize m_boxSize;              // dimensions of the minimal bounding box
    Position m_drawingPosition;     // position of lower left corner for drawing
    bool m_doRotateBeforeDrawing;   // indicates if the part is to be rotated for drawing
};

#endif // MANAGED_PART_HEADER
