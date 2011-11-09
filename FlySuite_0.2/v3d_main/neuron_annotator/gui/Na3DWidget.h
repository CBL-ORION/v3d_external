#ifndef NA3DWIDGET_H
#define NA3DWIDGET_H

#include "../../3drenderer/v3dr_glwidget.h"
#include "../../v3d/xformwidget.h"
#include "NaViewer.h"
#include "../geometry/Rotation3D.h"
#include "MouseClickManager.h"
#include <cmath>
#include "RendererNeuronAnnotator.h"
#include "NeuronContextMenu.h"

#if defined (_MSC_VER)
#include "../basic_c_fun/vcdiff.h"
#else
#endif

// Derived class of V3dR_GLWidget so we can use a simpler constructor
// for the convenience of Qt Designer,
// and to customize features for NeuronAnnotator.
class Na3DWidget : public V3dR_GLWidget, public NaViewer
{
    Q_OBJECT

public:
    Na3DWidget(QWidget* parent);
    virtual ~Na3DWidget();

    const Vector3D& focus() const {return cameraModel.focus();}
    float getZoomScale() const; // in viewport pixels per image voxel at focus

    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void wheelEvent(QWheelEvent * e); // zoom with scroll wheel
    virtual void mouseDoubleClickEvent(QMouseEvent * event); // double click to center
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

    virtual void resizeEvent(QResizeEvent * event);
    virtual void setDataFlowModel(const DataFlowModel&);
    void resetVolumeBoundary();
    virtual RendererNeuronAnnotator* getRendererNa()   {return dynamic_cast<RendererNeuronAnnotator*>(renderer);}
    virtual const RendererNeuronAnnotator* getRendererNa() const {return dynamic_cast<RendererNeuronAnnotator*>(renderer);} // const version CMB
    void setResizeEnabled(bool b) {bResizeEnabled = b;}
    int neuronAt(QPoint pos);
    void setContextMenus(QMenu* viewerMenu, NeuronContextMenu* neuronMenu);

signals:
    void neuronSelected(double x, double y, double z);
    void progressValueChanged(int);
    void progressComplete();
    void progressMessageChanged(QString);
    void progressAborted(QString);
    void landmarksChanged();
    void alphaBlendingChanged(bool);
    void quadStereoSupported(bool);
    void showCornerAxesChanged(bool);

public slots:
    void setShowCornerAxes(bool b);
    void setAlphaBlending(bool);
    void clearLandmarks();
    void setLandmarks(const QList<ImageMarker>);
    virtual void annotationModelUpdate(QString updateType);
    virtual void toggleNeuronDisplay(NeuronSelectionModel::NeuronIndex index, bool checked);
    virtual void updateFullVolume();
    void onVolumeDataChanged();
    void updateIncrementalColors();
    void showContextMenu(QPoint point);
    void resetView();
    void resetRotation() {
        cameraModel.setRotation(Rotation3D());
        update();
    }
    void translateImage(int dx, int dy);
    void showCrosshair(bool b) {NaViewer::showCrosshair(b); update();}
    void updateHighlightNeurons();
    void onMouseSingleClick(QPoint pos);
    void onNotSingleClick();
    void onPossibleSingleClickAlert();
    virtual void updateImageData();
    void onNeuronSelectionChanged(); // highlight selected neurons

    void setXCutLock(int b);
    void setYCutLock(int b);
    void setZCutLock(int b);

    void setStereoOff(bool);
    void setStereoLeftEye(bool);
    void setStereoRightEye(bool);
    void setStereoQuadBuffered(bool);
    void setStereoAnaglyphRedCyan(bool);
    void setStereoAnaglyphGreenMagenta(bool);
    void setStereoRowInterleaved(bool);
    void setStereoMode(int);

public:

    Vector3D getDefaultFocus() const;
    // Don't update if the current rotation is within 0.5 of the specified integer angle
    void setXYZBodyRotationInt(int rotX, int rotY, int rotZ)
    {
        Vector3D rotXYZInDegrees = cameraModel.rotation().convertBodyFixedXYZRotationToThreeAngles() * 180.0 / 3.14159;
        int oldRotX = round(rotXYZInDegrees.x());
        int oldRotY = round(rotXYZInDegrees.y());
        int oldRotZ = round(rotXYZInDegrees.z());
        if (eulerAnglesAreEquivalent(rotX, rotY, rotZ, oldRotX, oldRotY, oldRotZ))
            return; // no significant change
        Vector3D newRot = Vector3D(rotX, rotY, rotZ) * 3.14159 / 180.0;
        cameraModel.setRotation(Rotation3D().setRotationFromBodyFixedXYZAngles(newRot.x(), newRot.y(), newRot.z()));
        update();
    }

    static int radToDeg(double angleInRadians) {
        return round(angleInRadians * 180.0 / 3.14159);
    }
    static bool eulerAnglesAreEquivalent(int x1, int y1, int z1, int x2, int y2, int z2) // in degrees
    {
        if (   anglesAreEqual(x1, x2)
            && anglesAreEqual(y1, y2)
            && anglesAreEqual(z1, z2) )
        {
            return true;
        }
        // Euler angles are equivalent if y' = -y + 180, x' = x + 180, z' = z + 180
        int x3 = x2 + 180;
        // int x3 = x2;
        int y3 = -y2 + 180;
        int z3 = z2 + 180;
        if (   anglesAreEqual(x1, x3)
            && anglesAreEqual(y1, y3)
            && anglesAreEqual(z1, z3) )
        {
            return true;
        }
        // qDebug() << x1 << ", " << y1 << ", " << z1 << ", " << x2 << ", " << y2 << ", " << z2;
        return false;
    }

    bool screenShot(QString filename);

protected:
    bool tryUpdateFullVolume();
    bool bResizeEnabled; // to disable resizes during progressBar show/hide
    virtual void resizeGL(int, int);
    void updateCursor();
    void highlightNeuronAtPosition(QPoint pos);
    // Rotation helper methods
    static int round(double d) {return floor(d + 0.5);}
    static bool anglesAreEqual(int a1, int a2) // in degrees
    {
        if (a1 == a2)
            return true; // trivially equal
        else if (((a1 - a2) % 360) == 0)
            return true;
        else
            return false;
    }

protected slots:
    // focus setting should be done via cameraModel, not with these methods.
    void updateRendererZoomRatio(qreal relativeScale);
    void updateRotation(const Rotation3D&);
    void updateFocus(const Vector3D& f);
	
protected:
    virtual void paintGL();
    void paintFiducial(const Vector3D& v);
    void paintGrid();
    virtual void choiceRenderer();
    float glUnitsPerImageVoxel() const;
    void updateDefaultScale();
    // BrightnessCalibrator<unsigned char> brightnessCalibrator;
    const DataColorModel * incrementalDataColorModel;
    QCursor * rotateCursor;
    SlotStatus toggleNeuronDisplayStatus; // help coalesce multiple toggle neuron events
    SlotStatus updateFullVolumeStatus; // help coalesce multiple full update events
    QMenu* viewerContextMenu;
    NeuronContextMenu* neuronContextMenu;
    bool bHasQuadStereo;
    bool bShowCornerAxes;
    bool bAlphaBlending;
    bool bClickIsWaiting;
};

#endif // NA3DWIDGET_H