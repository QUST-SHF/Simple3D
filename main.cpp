//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "partsModel.h"
#include "simple3d.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create the parts model.
    BoxSize masterBox(120, 130, 120);
    //BoxSize masterBox(20, 130, 120);
    //BoxSize masterBox(3000, 3000, 3000);
    //BoxSize masterBox(12, 13, 14);
    float minGapBetweenParts = 1;
    PartsModel partsModel(masterBox, minGapBetweenParts);

    // Create the main window.
    Simple3D w(&partsModel);
    w.show();

    return a.exec();
}
