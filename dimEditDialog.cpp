#include "dimEditDialog.h"


 DimEditDialog::DimEditDialog(const BoxSize & size, QWidget *parent)
     : QDialog(parent)
 {
     // Set up the GUI as designed in the Qt Designer.
     setupUi(this);

     // Set the allowed range for the spin boxes.
     xSpinBox->setValue(size.x());
     ySpinBox->setValue(size.y());
     zSpinBox->setValue(size.z());
 }

DimEditDialog::DimEditDialog(int xDim, int yDim, int zDim, QWidget * parent)
    : DimEditDialog(BoxSize(xDim, yDim, zDim), parent) {}
