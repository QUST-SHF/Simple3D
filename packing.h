//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef PACKING_HEADER
#define PACKING_HEADER

#include "boxSize.h"
#include <utility>   // pair
#include <vector>    // vector
#include <map>       // map
#include <algorithm> // lower_bound, upper_bound, max_element, swap
#include <cassert>   // assert

using std::vector;
using std::map;

enum class Orientation {XY, YX};  // encodes whether the box is used as given or rotated 90 degrees
enum class SlidingOrder {xy, yx}; // encodes whether the box slid down and left, or left and down

class Packing
{
public:
    explicit Packing(const BoxSize & masterBox, const vector<BoxSize> & boxes, const vector<int> & boxProcessingOrder,
                     const vector<Orientation> & orientations = vector<Orientation>(),
                     const vector<Position> & positions = vector<Position>());
    explicit Packing(const Packing & packing);
    ~Packing();

    // Accessors.
    int numUsedBoxes() const { return m_numUsedBoxes; }
    Orientation orientation(int i) const { assert((0 <= i)&&(i < m_numUsedBoxes)); return m_orientations[i]; }
    const Position & position(int i) const { assert((0 <= i)&&(i < m_numUsedBoxes)); return m_positions[i]; }

    // Setters.
    void setOrientations(const vector<Orientation> & orientations) { m_orientations = orientations; }
    void setPosition(const vector<Position> & positions) { m_positions = positions; }
    void swapIrresponsibly(Packing & p);
    void swap(Packing & p);
    void swapOrientations(vector<Orientation> & orientations) { std::swap(m_orientations, orientations); }
    void swapPositions(vector<Position> & positions) { std::swap(m_positions, positions); }
    void printBoxes() const;

    bool addNextBox(Orientation orientation, SlidingOrder slidingOrder);
    void reorderToOriginalBoxOrder();
    void adjustPositionsToOriginalBoxSizes(float minGapBetweenParts);

private:  // member functions
    bool slide(const BoxSize & box, SlidingOrder slidingOrder, Position & finalPosition);
    bool slide(const BoxSize & masterBox, const BoxSize & box, map<float, float> & xSkyline,
               map<float, float> & ySkyline, Position & finalPosition);

private:  // member variables
    const BoxSize & m_masterBox;
    const vector<BoxSize> & m_boxes;
    const vector<int> & m_boxProcessingOrder;
    int m_numUsedBoxes;
    vector<Orientation> m_orientations;
    vector<Position> m_positions;
    // A skyline is a piecewise constant function defined on an interval of
    // positive numbers starting at zero. It is encoded as a map, where for
    // each pair in the map the key represents the end of an interval of
    // constancy and the value represents the constant value of the function on
    // that interval.
    map<float, float> m_xSkyline;
    map<float, float> m_ySkyline;
    // Indicates if the positions and orientations of the boxes are listed in
    // the order the boxes are listed (versus the specified processing order).
    bool m_isInOriginalBoxOrder;
};


//=============================================================================
// The function "compareSecond" compares two pairs by comparing their second
// elements only. It can be used for applying algorithms (e.g., max_element) on
// the values of a map.
//=============================================================================
template <class T1, class T2>
bool compareSecond(const std::pair<T1,T2> & lhs, const std::pair<T1,T2> & rhs)
{
    return (lhs.second < rhs.second);
}

//bool compareSecond(const std::pair<float, float> & lhs, const std::pair<float, float> & rhs);

#endif // PACKING_HEADER
