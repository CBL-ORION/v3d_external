#include "DataFlowModel.h"
#include "gui/RendererNeuronAnnotator.h"
#include <QtAlgorithms>
#include <iostream>
#include <cassert>

using namespace std;

const int DataFlowModel::REFERENCE_MIP_INDEX=0;
const int DataFlowModel::BACKGROUND_MIP_INDEX=1;

/* explicit */
DataFlowModel::DataFlowModel(QObject* parentParam /* = NULL */)
    : QObject(parentParam)
    , multiColorImageStackNode(NULL)
    , neuronAnnotatorResultNode(NULL)
    // Allocate data flow objects, in order, to automatically set up multithreaded data stream
    , volumeData(/* this */) // load from disk (cannot move qobject with a parent to a QThread)
    , neuronSelectionModel(volumeData) // which layers are shown?
    , dataColorModel(volumeData) // choose colors
    , neuronFragmentData(volumeData)
    , zSliceColors(volumeData, dataColorModel, neuronSelectionModel)
    , mipFragmentData(volumeData /* , this */) // project in Z, slice on fragment index
    , mipFragmentColors(mipFragmentData, dataColorModel) // color 'em
    , galleryMipImages(mipFragmentColors) // shrink 'em
    , mipMergedData(volumeData, mipFragmentData, dataColorModel, neuronSelectionModel)
    // TODO slow3DColorModel should depend in VolumeColors, which has not been implemented yet
    , slow3DColorModel(volumeData) // for initial testing, slow model has default values
    // , volumeColors(volumeData, dataColorModel, neuronSelectionModel)
{
    // Prepare to load 16-bit volume data from disk in a separate QThread
    connect(this, SIGNAL(volumeDataNeeded()),
            &volumeData, SLOT(loadVolumeDataFromFiles()));

    // wire up 3d viewer fast color update system
    fast3DColorModel.setIncrementalColorSource(dataColorModel, slow3DColorModel);

    // TODO - deprecate these DataFlowModel neuron visiblity slots.
    connect(&neuronSelectionModel, SIGNAL(overlayVisibilityChanged(int,bool)),
            this, SLOT(updateNeuronMaskFull()));
    connect(&neuronSelectionModel, SIGNAL(neuronVisibilityChanged(int,bool)),
            this, SLOT(updateNeuronMask(int,bool)));
    connect(&neuronSelectionModel, SIGNAL(multipleVisibilityChanged()),
            this, SLOT(updateNeuronMaskFull()));
}

DataFlowModel::~DataFlowModel()
{
    if (multiColorImageStackNode!=0) {
        delete multiColorImageStackNode;
    }
    if (neuronAnnotatorResultNode!=0) {
        delete neuronAnnotatorResultNode;
    }
}

// TBD
bool DataFlowModel::save() {
    return true;
}

// TBD
bool DataFlowModel::load(long annotationSessionID) {
    return true;
}

bool DataFlowModel::loadLsmMetadata() {
    QStringList lsmMetadataFilepathList=multiColorImageStackNode->getPathsToLsmMetadataFiles();
    if (lsmMetadataFilepathList.size()==0) {
        qDebug() << "DataFlowModel::loadLsmMetadata() received empty list of lsm metadata files";
        return false;
    } else {
        QString filePath=lsmMetadataFilepathList.at(0);
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            cerr << "Could not open file=" << filePath.toStdString() << " to read\n";
            return false;
        }
        QStringList fileContents;
        while(!file.atEnd()) {
            fileContents.append(file.readLine());
        }
        file.close();
        bool parseSuccess=false;
        for (int i=fileContents.size()-1;i>=0;i--) {
            QString line=fileContents.at(i);
            if (line.trimmed().length()>0) {
                QStringList doubleArgs = line.trimmed().split(QRegExp("\\s+"));
                if (doubleArgs.length()!=3) {
                    qDebug() << "Could not parse line which was expected to have 3 doubles = " << line;
                    for (int j=0;j<doubleArgs.length();j++) {
                        qDebug() << j << " " << doubleArgs.at(j);
                    }
                    return false;
                }
                QString d1String=doubleArgs.at(0);
                double d1=d1String.toDouble();
                QString d2String=doubleArgs.at(1);
                double d2=d2String.toDouble();
                QString d3String=doubleArgs.at(2);
                double d3=d3String.toDouble();
                zRatio=d3/d1;
                qDebug() << "Using lsm dimension ratios " << d1 << " " << d2 << " " << d3 << " setting zRatio=" << zRatio;
                parseSuccess=true;
                break;
            }
        }
        if (!parseSuccess) {
            qDebug() << "DataFlowModel::loadLsmMetadata could not parse file to determine zRatio";
            return false;
        }
        return true;
    }

}

bool DataFlowModel::loadVolumeData()
{
    {
        // Allocate writer on the stack so write lock will be automatically released when method returns
        NaVolumeData::Writer volumeWriter(volumeData);

        // Set file names of image files so VolumeData will know what to load.
        volumeWriter.setOriginalImageStackFilePath(
                multiColorImageStackNode->getPathToOriginalImageStackFile());
        volumeWriter.setMaskLabelFilePath(
                multiColorImageStackNode->getPathToMulticolorLabelMaskFile());
        volumeWriter.setReferenceStackFilePath(
                multiColorImageStackNode->getPathToReferenceStackFile());
    } // release locks before emit
    emit volumeDataNeeded(); // load data in a separate QThread

    return true;
}

// tell 3d viewer to perform a surgical texture update
// TODO - move logic for this into 3D viewer.
void DataFlowModel::updateNeuronMask(int index, bool status)
{
    int statusValue = (status ? 1 : 0);
    // qDebug() << "DataFlowModel::updateNeuronMask index=" << index << " status=" << status;
    QString updateNeuronMaskString=QString("NEURONMASK_UPDATE %1 %2").arg(index).arg(statusValue);
    emit modelUpdated(updateNeuronMaskString);
}

// tell 3d viewer to update all textures
void DataFlowModel::updateNeuronMaskFull() {
    // qDebug() << "NeuronSelectionModel::updateNeuronMaskFull() - emitting modelUpdated()";
    QString updateString=QString("FULL_UPDATE");
    emit modelUpdated(updateString);
}


