//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "packing.h"
#include <cassert>   // assert
#include <utility>   // pair, make_pair
#include <QDebug>

using std::upper_bound;
using std::lower_bound;
using std::max_element;
using std::pair;
using std::make_pair;

typedef map<float, float>::const_iterator ConstMapIter;

// Constructors.
Packing::Packing(const BoxSize & masterBox,
                 const vector<BoxSize> & boxes,
                 const vector<int> & boxProcessingOrder,
                 const vector<Orientation> & orientations,
                 const vector<Position> & positions)
    : m_masterBox(masterBox),
      m_boxes(boxes),
      m_boxProcessingOrder(boxProcessingOrder),
      m_orientations(orientations),
      m_positions(positions),
      m_isInOriginalBoxOrder(false)
{
    m_numUsedBoxes = static_cast<int>(orientations.size());
}

Packing::Packing(const Packing & packing)
    : Packing(packing.m_masterBox,
              packing.m_boxes,
              packing.m_boxProcessingOrder,
              packing.m_orientations,
              packing.m_positions)
{
    m_xSkyline = packing.m_xSkyline;
    m_ySkyline = packing.m_ySkyline;
    m_isInOriginalBoxOrder = packing.m_isInOriginalBoxOrder;
}


Packing::~Packing() {}


//=============================================================================
// The function "swapIrresponsibly" swaps all data members except the three
// references without any concern whether those references refer to the same
// objects or not.
// INPUT: "Packing & p" is a packing to be swapped.
//=============================================================================
void Packing::swapIrresponsibly(Packing & p)
{
    std::swap(this->m_orientations, p.m_orientations);
    std::swap(this->m_positions, p.m_positions);
    std::swap(this->m_numUsedBoxes, p.m_numUsedBoxes);
    std::swap(this->m_xSkyline, p.m_xSkyline);
    std::swap(this->m_ySkyline, p.m_ySkyline);
    std::swap(this->m_isInOriginalBoxOrder, p.m_isInOriginalBoxOrder);
}

//=============================================================================
// The function "swap" swaps all data members except the three references which
// are expected to refer to the same objects.
// INPUT: "Packing & p" is a packing to be swapped.
//=============================================================================
void Packing::swap(Packing & p)
{
    assert((this->m_masterBox) == (p.m_masterBox));
    assert((this->m_boxes) == (p.m_boxes));
    assert((this->m_boxProcessingOrder) == (p.m_boxProcessingOrder));
    this->swapIrresponsibly(p);
}


//=============================================================================
// The function "slide" attempts to slide a given box toward a skyline as much
// as possible, and then slide toward a second skyline as much as possible.
// INPUT: "const BoxSize & box" is the box to be slid.
// "SlidingOrder slidingOrder" specifies how the given box is to be slid - down
// and to the left, or to the left and down.
// OUTPUT: "Position & finalPosition" returns the position of the lower left
// corner after the given box is slid to its final location.
// The function returns "true" if the given box was successfully added to the
// packing without sticking out of the master box, and "false" otherwise.
//=============================================================================
bool Packing::slide(const BoxSize & box, SlidingOrder slidingOrder, Position & finalPosition)
{
    if (slidingOrder == SlidingOrder::xy)
    {
        return slide(m_masterBox, box, m_xSkyline, m_ySkyline, finalPosition);
    }

    // Otherwise the sliding order is to the left and down (i.e., yx).
    BoxSize rotatedMasterBox(m_masterBox);
    rotatedMasterBox.rotate();
    BoxSize rotatedBox(box);
    rotatedBox.rotate();
    bool isSuccess = slide(rotatedMasterBox, rotatedBox, m_ySkyline, m_xSkyline, finalPosition);
    finalPosition.rotate();
    return isSuccess;
}


//=============================================================================
// The function "slide" attempts to slide a given box toward the x-skyline as
// much as possible, and then slide toward y-skyline as much as possible.
// INPUT: "const BoxSize & masterBox" is the master box in which smaller boxes
// are placed.
// "const BoxSize & box" is the box to be slid.
// "map<float, float> & skylineA" and "map<float, float> & skylineB" are
// the two skylines which get modified as the box is slid toward and
// incorporated in them.
// OUTPUT: "Position & finalPosition" returns the position of the lower left
// corner after the given box is slid to its final location.
// The function returns "true" if the given box was successfully added to the
// packing without sticking out of the master box, and "false" otherwise.
//=============================================================================
bool Packing::slide(const BoxSize & masterBox, const BoxSize & box, map<float, float> & xSkyline,
                    map<float, float> & ySkyline, Position & finalPosition)
{
    // Slide the box from the upper right corner straight down until it hits
    // the x-skyline.
    ConstMapIter cit = xSkyline.upper_bound(masterBox.x() - box.x());
    // Note that by construction the max element is guaranteed to exist.
    float yBottom = max_element(cit, xSkyline.cend(), compareSecond<double, double>)->second;
    float yTop = yBottom + box.y();
    if (yTop > masterBox.y())
        return false;

    // Next, slide the box to the left until it hits the y-skyline.
    ConstMapIter citBottom = ySkyline.upper_bound(yBottom);
    ConstMapIter citTop = ySkyline.lower_bound(yTop);
    ++citTop;        // this ensures that citTop != citBottom
    // Note that by construction the max element is guaranteed to exist.
    float xLeft = max_element(citBottom, citTop, compareSecond<double, double>)->second;
    float xRight = xLeft + box.x();
    if (xRight > masterBox.x())
        return false;

    // Set the final position of the lower left corner of the box.
    finalPosition.setX(xLeft);
    finalPosition.setY(yBottom);

    // Remove any breaks in the y-skyline that are covered by the y-projection
    // of the slid box.
    float oldX = citBottom->second;
    citTop = ySkyline.upper_bound(yTop);
    ySkyline.erase(citBottom, citTop);
    // Insert the two end points of the new y-interval.
    citTop = ySkyline.emplace_hint(citTop, yTop, xRight);
    ySkyline.emplace_hint(citTop, yBottom, oldX);

    // Remove any breaks in the x-skyline that are covered by the x-projection
    // of the slid box.
    ConstMapIter citLeft = xSkyline.upper_bound(xLeft);
    float oldY = citLeft->second;
    ConstMapIter citRight = xSkyline.upper_bound(xRight);
    xSkyline.erase(citLeft, citRight);
    // Insert the two end points of the new x-interval.
    citRight = xSkyline.emplace_hint(citRight, xRight, yTop);
    xSkyline.emplace_hint(citRight, xLeft, oldY);

    return true;
}


//=============================================================================
// The function "addNextBox" attempts to add the next box in the predefined
// processing order to this packing with specified orientation and sliding
// order.
// INPUT: "Orientation orientation" is the desired orientation.
// "SlidingOrder slidingOrder" is the desired sliding order (i.e, down and left
// or left and down).
// OUTPUT: The function returns "true" if the next box was successfully added
// to the packing without sticking out of the master box, and "false"
// otherwise.
//=============================================================================
bool Packing::addNextBox(Orientation orientation, SlidingOrder slidingOrder)
{
    assert((0 <= m_numUsedBoxes) && (m_numUsedBoxes < static_cast<int>(m_boxes.size())));

    // Get the next box, properly rotated if needed.
    BoxSize nextBox(m_boxes[m_boxProcessingOrder[m_numUsedBoxes]]);
    if (orientation == Orientation::YX)
        nextBox.rotate();

    // If the packing is empty, i.e., no box has been placed in, then try
    // placing the first box.
    Position finalPosition;
    if (m_numUsedBoxes == 0)
    {
        // Check if the first box, properly rotated, if needed, fits within the
        // master box.
        if ((nextBox.x() > m_masterBox.x()) || (nextBox.y() > m_masterBox.y()))
            return false;

        // Set the final position of the lower left corner of the box.
        finalPosition.setX(0.0f);
        finalPosition.setY(0.0f);
        // Build the two skylines.
        m_xSkyline.emplace(nextBox.x(), nextBox.y());
        m_xSkyline.emplace(m_masterBox.x(), 0.0f);
        m_ySkyline.emplace(nextBox.y(), nextBox.x());
        m_ySkyline.emplace(m_masterBox.y(), 0.0f);
    }
    else if (slide(nextBox, slidingOrder, finalPosition) == false)
        return false;

    ++m_numUsedBoxes;
    m_orientations.push_back(orientation);
    m_positions.push_back(finalPosition);
    return true;
}


//=============================================================================
// The function "reorderToOriginalBoxOrder" reorders the positions and
// orientations stored in this packing according to the original order of the
// packed boxes.
//=============================================================================
void Packing::reorderToOriginalBoxOrder()
{
    // If the packing is already in the original order, we are done.
    if (m_isInOriginalBoxOrder == true)
        return;

    // If this is an incomplete packing, i.e., not all boxes are packed, then
    // reordering is not applicable.
    if (m_numUsedBoxes != static_cast<int>(m_boxes.size()))
    {
        m_isInOriginalBoxOrder = false;
        return;
    }

    // Allocate memory for the reordered orientations and positions.
    vector<Orientation> orientations(m_numUsedBoxes);
    vector<Position> positions(m_numUsedBoxes);
    // Fill them in.
    for (int i = 0; i < m_numUsedBoxes; ++i)
    {
        orientations[m_boxProcessingOrder[i]] = m_orientations[i];
        positions[m_boxProcessingOrder[i]] = m_positions[i];
    }

    // Replace the orientations and positions with the reordered ones.
    std::swap(m_orientations, orientations);
    std::swap(m_positions, positions);
    m_isInOriginalBoxOrder = true;
}


//=============================================================================
// The function "adjustPositionsToOriginalBoxSizes" adjusts each position
// stored in this packing from the position of the lower left corner of an
// expanded box to the position of the lower left corner of the corresponding
// original box.
// INPUT: "float minGapBetweenParts" is the amount by which each dimension of
// each original box was expanded.
//=============================================================================
void Packing::adjustPositionsToOriginalBoxSizes(float minGapBetweenParts)
{
    Position correction(minGapBetweenParts / 2, minGapBetweenParts / 2);
    for (auto it = m_positions.begin(); it != m_positions.end(); ++it)
        *it += correction;
}


//=============================================================================
// The function "printBoxes" prints to the screen position of the lower left
// and upper right corners of all boxes as arranged in this packing.
//=============================================================================
void Packing::printBoxes() const
{
    for (int i = 0; i < m_numUsedBoxes; ++i)
    {
        BoxSize box = m_boxes[i];
        if (m_orientations[i] == Orientation::YX)
            box.rotate();
        qDebug() << "(" << m_positions[i].x() << "," << m_positions[i].y() << "," << m_positions[i].z() << ")"
                 << "-- (" << m_positions[i].x() + box.x() << "," << m_positions[i].y() + box.y()
                 << "," << m_positions[i].y() + box.z() << ")";
    }
    qDebug();
}
