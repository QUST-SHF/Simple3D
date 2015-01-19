//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "partsModel.h"
#include "partFactory.h"
#include "boxSize.h"
#include "packer.h"
#include "packing.h"
#include <algorithm>   // sort, swap
#include <functional>  // greater
#include <numeric>     // iota


// Constructor.
PartsModel::PartsModel(const BoxSize & masterBox, float minGapBetweenParts, QObject * parent)
    : QObject(parent),
      m_masterBox(masterBox),
      m_partFactory(new PartFactory()),
      m_totalVolume(0),
      m_minGapBetweenParts(minGapBetweenParts)
{}

// Destructor.
PartsModel::~PartsModel()
{
    delete m_partFactory;
    m_partFactory = nullptr;
}


//=============================================================================
// The function "repack" repacks the parts in the list of managed parts.
// INPUT: "double minGapBetweenParts" is the desired minimal gap between
// adjacent parts.
// OUTPUT: The function returns "true" if successful, and "false" otherwise.
//=============================================================================
bool PartsModel::repack(double minGapBetweenParts)
{
    // Try to pack the parts inside the master box.
    std::vector<BoxSize> boxes = this->boxes();
    Packer packer(m_masterBox, boxes);
    vector<Position> positions;
    vector<Orientation> orientations;
    float minGap = static_cast<float>(minGapBetweenParts);
    bool isSuccess = packer.pack(minGap, positions, orientations);

    // If packing failed, then emit a signal to restore the previous value of
    // the minimal gap double spin box, and return.
    if (isSuccess == false)
    {
        emit resettingGapNeeded(static_cast<double>(m_minGapBetweenParts));
        return false;
    }

    // If the packing was successful, then update the position and orientation
    // of each managed part.
    for (int i = 0; i < m_parts.size(); ++i)
    {
        m_parts[i].setDrawingPosition(positions[i]);
        m_parts[i].setDoRotateBeforeDrawing(orientations[i] == Orientation::YX);
    }
    // Adopt the new minimal gap.
    m_minGapBetweenParts = minGap;

    return true;
}


//=============================================================================
// The function "addPart" adds a part to the list of managed parts.
// INPUT: "const QString & fileName" is the name of the file from which the new
// part is to be read.
//=============================================================================
void PartsModel::addPart(const QString & fileName)
{
    // Create and add a new part.
    m_parts.push_back(ManagedPart(m_partFactory->makePart(fileName)));

    bool isSuccess = repack(m_minGapBetweenParts);

    // If packing failed, then signal it and return.
    if (isSuccess == false)
    {
        // Remove the last part that did not be fit.
        m_parts.removeLast();
        emit addingPartFailed();
        return;
    }

    // Compute the new total volume of the parts.
    m_totalVolume += m_parts.back().volume();

    emit partAdded();
}


//=============================================================================
// The function "removePart" removes a specific part from the list of managed
// parts.
// INPUT: "int partIndex" is the index of the part to be removed.
//=============================================================================
void PartsModel::removePart(int partIndex)
{
    if ((0 <= partIndex) && (partIndex < m_parts.size()))
    {
        // Adjust the total volume.
        m_totalVolume -= m_parts[partIndex].volume();

        // Remove the part.
        m_parts.removeAt(partIndex);

        emit partRemoved(partIndex);
    }
}


//=============================================================================
// The function "removeParts" removes a list of parts from the list of managed
// parts.
// INPUT: "const QSet<int> & partIndices" are the indices of the parts to be
// removed.
//=============================================================================
void PartsModel::removeParts(const QSet<int> & partIndices)
{
    // Make a list of indices to be removed and sort them in descending order.
    QList<int> indices;
    for (auto index : partIndices)
        indices.push_back(index);
    std::sort(indices.begin(), indices.end(), std::greater<int>());

    // Remove the parts one by one, the larger indices first so that the
    // indices of the remaining parts to be removed do not change.
    for (auto partIndex : indices)
        removePart(partIndex);
}


//=============================================================================
// The function "numVertices" returns the total number of vertices in a
// collection of parts.
// INPUT: "const QSet<int> & indices" are the indices of the parts.
// OUTPUT: The function returns total number of vertices of specified parts.
//=============================================================================
long int PartsModel::numVertices(const QSet<int> & indices) const
{
    long int numVertices = 0;
    for (auto i : indices)
    {
        numVertices += m_parts[i].part()->numVertices();
    }
    return numVertices;
}


//=============================================================================
// The function "totalNumVertices" returns the total number of vertices in all
// parts in the model.
// OUTPUT: The function returns total number of vertices.
//=============================================================================
long int PartsModel::totalNumVertices() const
{
    long int numVertices = 0;
    for (auto part : m_parts)
    {
        numVertices += part.part()->numVertices();
    }
    return numVertices;
}


//=============================================================================
// The function "boxes" constructs a vector holding the sizes of the bases of
// all managed parts.
// INPUT: The function returns the constructed vector.
//=============================================================================
std::vector<BoxSize> PartsModel::boxes() const
{
    std::vector<BoxSize> boxes;
    boxes.reserve(m_parts.size());

    for (auto part : m_parts)
    {
        boxes.emplace_back(part.boxSize());
    }
    return boxes;
}


//=============================================================================
// The function "volume" computes the total volume of collection of parts.
// INPUT: "const QSet<int> & indices" are the indices of the parts.
// OUTPUT: The function returns the total volume of the specified parts.
//=============================================================================
double PartsModel::volume(const QSet<int> & indices) const
{
    double volume = 0;
    for (auto i : indices)
    {
        volume += m_parts[i].volume();
    }
    return volume;
}


//=============================================================================
// The function "resizeMasterBox" changes the size of the master box and
// repacks the loaded parts, if possible.
// INPUT: "BoxSize newMasterSize" is the new size of the master box.
//=============================================================================
void PartsModel::resizeMasterBox(BoxSize newMasterSize)
{
    // Set the new size.
    std::swap(m_masterBox, newMasterSize);
    // Repack the loaded parts.
    bool isSuccess = repack(m_minGapBetweenParts);
    // If repacking within the new master box failed, then unload all parts.
    if (isSuccess == false)
    {
        while(numParts() > 0)
            removePart(0);
        emit repackingFailed();
    }
    emit masterBoxResized();
}
