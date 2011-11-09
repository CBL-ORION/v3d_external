#ifndef FRAGMENTGALLERYWIDGET_H
#define FRAGMENTGALLERYWIDGET_H

#include <QAbstractScrollArea>
#include "GalleryButton.h"
#include "../data_model/NeuronSelectionModel.h"

// FragmentGalleryWidget manages a virtual canvas of neuron fragment thumbnails,
// to compensate for horrible inadequacies of Mac OS X QScrollArea widget, which
// should have been the right widget for this purpose.
class FragmentGalleryWidget : public QAbstractScrollArea
{
    Q_OBJECT

public:
    enum SortOrder {
        SORT_BY_SIZE,
        SORT_BY_INDEX,
        SORT_BY_COLOR
    };

public:
    typedef NeuronSelectionModel::NeuronIndex NeuronIndex;

    explicit FragmentGalleryWidget(QWidget *parent = 0);
    virtual ~FragmentGalleryWidget();
    void clear(); // delete all fragment widgets
    // FragmentGalleryWidget takes ownership of the GalleryButton
    void appendFragment(GalleryButton * button);
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);
    void setDataFlowModel(const DataFlowModel&);

signals:
    void scrollPixelChanged(int);

public slots:
    void scrollToFragment(NeuronSelectionModel::NeuronIndex);
    void setScrollPixel(int pixel);
    void sortByIndex();
    void sortByColor();
    void sortBySize();
    void updateSortTables();

protected:
    void updateScrollBar();
    void updateThumbnailPositions();

    QList<GalleryButton *> contents;
    int buttonWidth;
    int buttonHeight;
    int leftPixel;
    SortOrder sortOrder;
    std::vector<int> indexOrder;
    std::vector<int> sizeOrder;
    std::vector<int> colorOrder;
    std::vector<int> * order;
    const NeuronFragmentData * neuronFragmentData;

private:
    typedef QAbstractScrollArea super;
};

#endif // FRAGMENTGALLERYWIDGET_H