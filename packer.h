//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef PACKER_HEADER
#define PACKER_HEADER

#include "boxSize.h"
#include "packing.h"
#include <memory>   // shared_ptr

using std::vector;

//=============================================================================
// This class is responsible for placing the parts to be printed within the
// working area without overlaps. Each part is represented by its minimal
// bounding box (with edges parallel to the coordinate axes). The underlying
// algorithm attempts to arrange the boxes on the fixed working area. Boxes are
// not flipped on their sides (i.e., their original base and z-direction are
// kept) and stacked on top of each other. Currently, the height of the boxes
// is not checked and it is assumed that the working space of the printer can
// accommodate any height (which is probably unreasonable and needs to be
// addressed better).
//=============================================================================
class Packer
{
public:
    explicit Packer(const BoxSize & masterBox, const vector<BoxSize> & boxes);
    ~Packer() {}

    bool pack(float minGapBetweenParts, vector<Position> & positions, vector<Orientation> & orientations);

private:  // member functions
    void setProcessingOrder();

private:  // member variables
    const BoxSize & m_masterBox;
    const vector<BoxSize> & m_boxes;
    int m_numBoxes;
    vector<int> m_boxProcessingOrder;
};

#endif // PACKER_HEADER
