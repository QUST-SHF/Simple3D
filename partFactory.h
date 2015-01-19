//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef PART_FACTORY_HEADER
#define PART_FACTORY_HEADER

#include "part.h"
#include <QString>
#include <memory>   // shared_prt

class PartFactory
{
public:
    PartFactory() {}

    std::shared_ptr<Part> makePart(const QString & fileName);
};

#endif // PART_FACTORY_HEADER
