//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "partFactory.h"
#include "partStl.h"

#include <QFileInfo>
#include <QMessageBox>

std::shared_ptr<Part> PartFactory::makePart(const QString & fileName)
{
    // Check if the given file is an STL file.
    QFileInfo fileInfo(fileName);
    if (QString::compare(fileInfo.suffix(), "stl", Qt::CaseInsensitive) != 0)
    {
        QMessageBox::information(0, "Input Error", "Reading from files with extension " +
                                 fileInfo.suffix() + " is not supported.");
        return std::shared_ptr<Part>(nullptr);
    }
    else
    {
        return std::shared_ptr<Part>(new PartStl(fileName));
    }
}
