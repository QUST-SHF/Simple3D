//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef PARTS_MODEL_HEADER
#define PARTS_MODEL_HEADER

#include <QObject>
#include <QSet>
#include <QList>
#include <vector>   // vector
#include <memory>   // shared_ptr
#include <cassert>  // assert
#include "boxSize.h"
#include "managedPart.h"

using std::vector;

class PartFactory;

class PartsModel : public QObject
{
    Q_OBJECT

public:
    PartsModel(const BoxSize & masterBox, float minGapBetweenParts = 1.0f, QObject * parent = 0);
    ~PartsModel();

    // Accessors.
    int numParts() const { return m_parts.size(); }
    long int numVertices(int i) const { return m_parts[i].part()->numVertices(); }
    long int numVertices(const QSet<int> & indices) const;
    long int totalNumVertices() const;

    QVector<QVector3D>::const_iterator vertexBeginIter(int i) const { return m_parts[i].part()->vertexBeginIter(); }
    QVector<QVector3D>::const_iterator vertexNormalBeginIter(int i) const { return m_parts[i].part()->vertexNormalBeginIter(); }

    BoxSize masterBox() const { return m_masterBox; }
    std::vector<BoxSize> boxes() const;
    double volume(int i) const { assert((0 <= i) && (i < m_parts.size())); return m_parts[i].volume(); }
    double volume(const QSet<int> & indices) const;
    double totalVolume() const { return m_totalVolume; }
    float minGapBetweenParts() const { return m_minGapBetweenParts; }

    const BoxSize & boxSize(int i) const { return m_parts[i].boxSize(); }
    const Position & position(int i) const { return m_parts[i].drawingPosition(); }
    bool doRotate(int i) const { return m_parts[i].doRotateBeforeDrawing(); }

public slots:
    bool repack(double minGapBetweenParts);
    void addPart(const QString & fileName);
    void removePart(int partIndex);
    void removeParts(const QSet<int> & partIndices);
    void setMinGapBetweenParts(float minGapBetweenParts) { m_minGapBetweenParts = minGapBetweenParts; }
    void updateMinimalGap(double minGap) { m_minGapBetweenParts = static_cast<float>(minGap); }
    void resizeMasterBox(BoxSize newMasterSize);

signals:
    void partAdded();
    void addingPartFailed();
    void partRemoved(int partIndex);
    void repackingFailed();
    void resettingGapNeeded(double value);
    void masterBoxResized();

private:
    BoxSize m_masterBox;
    PartFactory * m_partFactory;
    QList<ManagedPart> m_parts;
    double m_totalVolume;
    float m_minGapBetweenParts;
};

#endif // PARTS_MODEL_HEADER
