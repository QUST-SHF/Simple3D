#ifndef DIM_EDIT_DIALOG_HEADER
#define DIM_EDIT_DIALOG_HEADER

#include "ui_dimEditDialog.h"
#include "boxSize.h"
#include <QDialog>

namespace Ui { class DimEditDialog; }

class DimEditDialog : public QDialog, private Ui::DimEditDialog
{
    Q_OBJECT

public:
    // Constructors.
    explicit DimEditDialog(int xDim, int yDim, int zDim, QWidget *parent = nullptr);
    explicit DimEditDialog(const BoxSize & size = BoxSize(200.0f, 200.0f, 200.0f), QWidget *parent = nullptr);
    //explicit DimEditDialog();
    ~DimEditDialog() {}

    // Accessors.
    float xDim() const { return static_cast<float>(xSpinBox->value()); }
    float yDim() const { return static_cast<float>(ySpinBox->value()); }
    float zDim() const { return static_cast<float>(zSpinBox->value()); }

//    // Setters.
//    void setXDim(int x) { xSpinBox->setValue(x); }
//    void setYDim(int y) { ySpinBox->setValue(y); }
//    void setZDim(int z) { zSpinBox->setValue(z); }
};

#endif // DIM_EDIT_DIALOG_HEADER
