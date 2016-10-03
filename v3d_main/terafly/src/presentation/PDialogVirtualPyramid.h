//------------------------------------------------------------------------------------------------
// Copyright (c) 2012  Alessandro Bria and Giulio Iannello (University Campus Bio-Medico of Rome).  
// All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef PDialogVirtualPyramid_H
#define PDialogVirtualPyramid_H

#include "theader.h"
#include "../control/CPlugin.h"
#include "QPrefixSuffixLineEdit.h"

namespace terafly
{
    class PDialogVirtualPyramid;
}

class terafly::PDialogVirtualPyramid : public QDialog
{
    Q_OBJECT

    private:

        PDialogVirtualPyramid(QWidget* parent){}

        // widgets
        QLabel* header;
        QLabel* pyramid_image_label;

        QPixmap *pyramid_image_full;
        QPixmap *pyramid_image_empty;
        QPixmap *pyramid_image_lowres;

        QGroupBox* subsampling_panel;
        QRadioButton* auto_radiobutton;
        QSpinBox *subsampling_spinbox;
        QSpinBox *lowerbound_spinbox;
        QPrefixSuffixLineEdit *subsamplings_line;
        QRadioButton* manual_radiobutton;
        QSpinBox *blockx_spinbox;
        QSpinBox *blocky_spinbox;
        QSpinBox *blockz_spinbox;

        QGroupBox* saveto_panel;
        QRadioButton* local_radiobutton;
        QRadioButton* storage_radiobutton;
        QLineEdit *volumepath_line;

        QGroupBox* lowres_panel;
        QRadioButton* imagefile_radiobutton;
        QLineEdit* imagefile_line;
        QPushButton* browse_button;
        QRadioButton* generate_radiobutton;
        QRadioButton* generate_all_radiobutton;
        QSpinBox* generate_spinbox;
        QRadioButton* noimage_radiobutton;


        QDialogButtonBox *qbuttons;

        std::string volumePath;

    public:

        // constructor
        PDialogVirtualPyramid(const std::string & _volumepath, QWidget* parent);

        // reset method
        void reset();

    public slots:

        void ok_button_clicked();
        void browse_button_clicked();
        void subsampling_radiobutton_changed();
        void storage_radiobutton_changed();
        void lowres_radiobutton_changed();
        void subsampling_spinbox_changed(int v);
};

#endif // PDialogVirtualPyramid_H
