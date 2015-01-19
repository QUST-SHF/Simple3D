//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "packer.h"
#include <numeric>   // iota
#include <algorithm> // sort, max
#include <stack>     // stack
#include <QVector2D>
#include <QDebug>


//=============================================================================
// Constructor.
// INPUT: "const BoxSize & masterBox" are the dimensions of the master box
// inside of which the other boxes are to be packed.
// "const vector<BoxSize> & boxes" are the boxes to be packed inside the given
// master box.
//=============================================================================
Packer::Packer(const BoxSize & masterBox, const vector<BoxSize> & boxes)
    : m_masterBox(masterBox), m_boxes(boxes), m_numBoxes(static_cast<int>(m_boxes.size()))
{
    // Define the order in which the boxes are to be processed while trying to
    // pack them inside the master box.
    setProcessingOrder();
}


//=============================================================================
// The function "setProcessingOrder" defines the order in which the boxes are
// to be processed while trying to pack them inside the master box.
//=============================================================================
void Packer::setProcessingOrder()
{
    // Allocate memory and initialize with box indices.
    m_boxProcessingOrder.resize(m_numBoxes);
    std::iota(m_boxProcessingOrder.begin(), m_boxProcessingOrder.end(), 0);
    // Sort in decreasing diagonal length.
    std::sort(m_boxProcessingOrder.begin(), m_boxProcessingOrder.end(),
              [this](int i, int j) { return m_boxes[i].toVector2D().lengthSquared() >
                m_boxes[j].toVector2D().lengthSquared(); });
}


//=============================================================================
// The function "pack" attempts to pack a given collection of boxes in a given
// master box.
// INPUT: "float minGapBetweenParts" is the minimal gap to be maintaied between
// adjacent parts.
// OUTPUT: "vector<Position> & positions" and
// "vector<Orientation> & orientations" return the positions and orientations
// in case of successful packing. Otherwise, they remained unchanged.
// The function itself returns "true" if boxes were successfully packed, and
// "false" otherwise.
//=============================================================================
bool Packer::pack(float minGapBetweenParts, vector<Position> & positions,
                  vector<Orientation> & orientations)
{
    if (m_numBoxes == 0)
    {
        positions.clear();
        orientations.clear();
        return true;
    }

    // Set a constant indicating how deep in the stack to go in search of a
    // packing. If a packing can not be found without going deeper, then the
    // complete search would take too much time to finish and we prefer to give
    // up and move on. The smaller this constant is, the faster packing will
    // finish and the more false negatives will occur.
    const int maxBackSearchDepth = 8;

    // Expand the boxes dimensions by the given minimal gap that needs to be
    // maintained.
    vector<BoxSize> boxes;
    boxes.reserve(m_numBoxes);
    BoxSize expansion(minGapBetweenParts, minGapBetweenParts);
    for (auto cit = m_boxes.cbegin(); cit != m_boxes.cend(); ++cit)
        boxes.push_back(*cit + expansion);

    // Create a stack to keep the partial packings covering the unexplored
    // combinations.
    std::stack<Packing *> pending;

    // Add the first box. Two different orientations are possible. The sliding
    // order does not really matter here.
    Packing * packing = new Packing(m_masterBox, boxes, m_boxProcessingOrder);
    if (packing->addNextBox(Orientation::YX, SlidingOrder::xy) == true)
    {
        pending.push(packing);
        packing = new Packing(m_masterBox, boxes, m_boxProcessingOrder);
    }
    if (packing->addNextBox(Orientation::XY, SlidingOrder::xy) == true)
    {
        pending.push(packing);
    }
    else
        delete packing;

    // Keep extending packings in a breath first fashion until a packing using
    // all boxes is found, or all possibilities are exhausted.
    packing = nullptr;
    int maxNumUsedBoxes = 1;
    while (pending.empty() == false)
    {
        // Pop a packing from the stack.
        packing = pending.top();
        pending.pop();
        // If that packing already uses all given boxes, then we are done.
        if (packing->numUsedBoxes() == m_numBoxes)
            break;

        // Do not go too deep in the stack. If you can not find packing without
        // going that deep, then give up for the complete search would take too
        // much time to finish.
        maxNumUsedBoxes = std::max(maxNumUsedBoxes, packing->numUsedBoxes());
        if (maxNumUsedBoxes - packing->numUsedBoxes() > maxBackSearchDepth)
        {
            delete packing;
            packing = nullptr;
            break;
        }

        // Try to add the next box in 4 different ways and push the successful
        // packings back on the stack.
        Packing * currPacking = new Packing(*packing);
        if (currPacking->addNextBox(Orientation::YX, SlidingOrder::yx) == true)
        {
            pending.push(currPacking);
            currPacking = new Packing(*packing);
        }

        if (currPacking->addNextBox(Orientation::YX, SlidingOrder::xy) == true)
        {
            pending.push(currPacking);
            currPacking = new Packing(*packing);
        }

        if (currPacking->addNextBox(Orientation::XY, SlidingOrder::yx) == true)
        {
            pending.push(currPacking);
        }
        else
        {
            delete currPacking;
            currPacking = nullptr;
        }

        if (packing->addNextBox(Orientation::XY, SlidingOrder::xy) == true)
        {
            pending.push(packing);
        }
        else
        {
            delete packing;
            packing = nullptr;
        }
    }

    // Release memory.
    while (pending.empty() == false)
    {
        delete pending.top();
        pending.pop();
    }

    if (packing != nullptr)
    {
        // Reorder the positions and orientations stored in the packing
        // according to the original order of the boxes so that they can be
        // properly placed.
        packing->reorderToOriginalBoxOrder();
        // Adjust each position from that of the lower left corner of an
        // expanded box to the position of the lower left corner of the
        // corresponding original box.
        packing->adjustPositionsToOriginalBoxSizes(minGapBetweenParts);
        // Set up the positions and orientations as output.
        packing->swapOrientations(orientations);
        packing->swapPositions(positions);
        delete packing;
        packing = nullptr;
        return true;
    }
    return false;
}
