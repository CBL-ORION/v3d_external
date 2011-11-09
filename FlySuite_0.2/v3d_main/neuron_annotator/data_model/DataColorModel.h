#ifndef DATACOLORMODEL_H
#define DATACOLORMODEL_H

#include "NaSharedDataModel.h"
#include "NaVolumeData.h"
#include <vector>
#include <qrgb.h>

class PrivateDataColorModel;

// DataColorModel converts multichannel 16-bit intensities
// into rgb colors by combining:
//  1 - a color for each data channel
//  2 - min and max "HDR" limits for each channel
//  3 - gamma brightness correction for each channel
// TODO - support both blend-SUM and blend-MAX
// TODO - allow custom hdr for gallery thumbnails (where does that even go?)
class DataColorModel : public NaSharedDataModel<PrivateDataColorModel>
{
    Q_OBJECT

public:
    DataColorModel();
    explicit DataColorModel(const NaVolumeData& volumeDataParam);
    explicit DataColorModel(const DataColorModel& rhs);
    void setIncrementalColorSource(const DataColorModel& desiredColors, const DataColorModel& currentColors);

public slots:
    void initialize();
    void colorizeIncremental();
    void setChannelColor(int index, /*QRgb*/ int color);
    void setChannelHdrRange(int index, qreal min, qreal max);
    void setChannelGamma(int index, qreal gamma);
    void setGamma(qreal gamma); // all channels
    void setChannelVisibility(int channel, bool isVisible);
    void resetColors();

protected:
    int getNumberOfDataChannels() const;

    // merge calls to setGamma(qreal)
    SlotStatus statusOfSetGammaSlot;
    qreal latestGamma;

protected:
    const NaVolumeData * volumeData;
    const DataColorModel * desiredColors; // upstream n-channel color model this 3-channel model colorizes
    const DataColorModel * currentColors;

private:
    typedef NaSharedDataModel<PrivateDataColorModel> super;


public:
    class Reader; friend class Reader;
    class Reader : public BaseReader
    {
    public:
        Reader(const DataColorModel& colorModelParam);
        int getNumberOfDataChannels() const;
        QRgb blend(const double channelIntensities[]) const;
        QRgb blend(const std::vector<double>& channelIntensities) const;
        QRgb getChannelColor(int channelIndex) const;
        qreal getReferenceScaledIntensity(qreal raw_intensity) const;
        qreal getChannelScaledIntensity(int channel, qreal raw_intensity) const;
        qreal getChannelGamma(int channel) const;
        qreal getChannelHdrMin(int channel) const;
        qreal getChannelHdrMax(int channel) const;
        qreal getChannelDataMin(int channel) const;
        qreal getChannelDataMax(int channel) const;
        bool getChannelVisibility(int index) const;
    };


    class Writer : public BaseWriter
    {
    public:
        Writer(DataColorModel& colorModelParam);
    };
};

#endif // DATACOLORMODEL_H