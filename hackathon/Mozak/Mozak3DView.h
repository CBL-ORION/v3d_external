#ifndef __MOZAK_3D_VIEW_H__
#define __MOZAK_3D_VIEW_H__

#include "v3dr_common.h"
#include "../terafly/src/control/CViewer.h"

namespace mozak
{
	class MozakUI;
	class Mozak3DView;
	struct CViewInfo;
}

struct mozak::CViewInfo
{
	int resIndex;
	Image4DSimple *img;
	int volV0, volV1;
	int volH0, volH1;
	int volD0, volD1;
	int volT0, volT1;
	int nchannels;
	int slidingViewerBlockID;
	int zoomThreshold;

	CViewInfo(	int _resIndex,
				Image4DSimple *_img,
				int _volV0, int _volV1,
				int _volH0, int _volH1,
				int _volD0, int _volD1,
				int _volT0, int _volT1,
				int _nchannels,
				int _slidingViewerBlockID,
				int _zoomThreshold	) {
		resIndex = _resIndex;
		img = _img;
		volV0 = _volV0;
		volV1 = _volV1;
		volH0 = _volH0;
		volH1 = _volH1;
		volD0 = _volD0;
		volD1 = _volD1;
		volT0 = _volT0;
		volT1 = _volT1;
		nchannels = _nchannels;
		slidingViewerBlockID = _slidingViewerBlockID;
		zoomThreshold = _zoomThreshold;
	}
	~CViewInfo() {
		if (img) img->setRawDataPointerToNull(); // don't clean underlying image data
		delete img;
	}
};

class mozak::Mozak3DView : protected teramanager::CViewer
{
	Q_OBJECT

	protected:
		/**************************************************************************************
		* Constructor needs to be protected because inherits from protected CViewer constructor
		***************************************************************************************/
		Mozak3DView(V3DPluginCallback2 *_V3D_env, int _resIndex, itm::uint8 *_imgData, int _volV0, int _volV1,
			int _volH0, int _volH1, int _volD0, int _volD1, int _volT0, int _volT1, int _nchannels, itm::CViewer *_prev, int _slidingViewerBlockID);
		virtual teramanager::CViewer* makeView(V3DPluginCallback2 *_V3D_env, int _resIndex, itm::uint8 *_imgData, int _volV0, int _volV1,
			int _volH0, int _volH1, int _volD0, int _volD1, int _volT0, int _volT1, int _nchannels, itm::CViewer *_prev, int _slidingViewerBlockID);
		virtual void onNeuronEdit();
		void updateRendererParams();
		void makeTracedNeuronsEditable();
        int findNearestNeuronNode(int cx, int cy, bool updateStartType=false);
		void loadNewResolutionData(	int _resIndex,
									Image4DSimple *_img,
									int _volV0, int _volV1,
									int _volH0, int _volH1,
									int _volD0, int _volD1,
									int _volT0, int _volT1	);
		void changeMode(Renderer::SelectMode mode, bool addThisCurve, bool turnOn);
		void updateTypeLabel();
		void updateResolutionLabel();
        void updateUndoLabel();
		void updateTranslateXYArrows();
		static int contrastValue;
		Image4DSimple* nextImg;
		QList<CViewInfo*> lowerResViews;
		bool loadingNextImg;
        QToolButton* buttonUndo; // use ours instead of PAnoToolBar since they are bound to the Ctrl+Z/Y commands
        QToolButton* buttonRedo; // use ours instead of PAnoToolBar since they are bound to the Ctrl+Z/Y commands
		QToolButton* invertImageButton;
		QToolButton* connectButton;
		QToolButton* extendButton;
		QToolButton* polyLineButton;
		QToolButton* retypeSegmentsButton;
		QToolButton* splitSegmentButton;
		QToolButton* deleteSegmentsButton;
		QLabel* currTypeLabel;
		QLabel* currZoomLabel;
		QLabel* currResolutionLabel;
        QLabel* currUndoLabel;

        int prevZCutMin;
        int prevZCutMax;
        int prevPolyZCut;

        QList <NeuronTree> undoRedoHistory;
        static const int MAX_history = 20;
	    int cur_history;

	public:

		// helping functions copied from renderer_gl2.h
		inline void set_colormap_curve(QPolygonF &curve, qreal x, int iy) // 0.0<=(x)<=1.0, 0<=(iy)<=255
		{
			x = qMax(0.0, qMin(1.0,  x));
			qreal y = qMax(0.0, qMin(1.0,  iy/255.0));
			curve << QPointF(x, y);
		}
		inline void set_colormap_curve(QPolygonF &curve, qreal x, qreal y) // 0.0<=(x, y)<=1.0
		{
			x = qMax(0.0, qMin(1.0,  x));
			y = qMax(0.0, qMin(1.0,  y));
			curve << QPointF(x, y);
		}

        // For use with determining intersection with XY translation arrows
        // Oversimplified: just determining if within certain dist of center of bounding box
        inline bool isMouseInBB(const BoundingBox &BB, const int mouseX, const int mouseY, const Renderer_gl2* curr_renderer)
        {
            GLdouble px0, py0, px1, py1, pz, dx, dy, dxm, dym;
            gluProject(BB.x0, BB.y0, BB.z0, curr_renderer->markerViewMatrix, curr_renderer->projectionMatrix, curr_renderer->viewport, &px0, &py0, &pz);
            py0 = curr_renderer->viewport[3]-py0;
            gluProject(BB.x1, BB.y1, BB.z1, curr_renderer->markerViewMatrix, curr_renderer->projectionMatrix, curr_renderer->viewport, &px1, &py1, &pz);
            py1 = curr_renderer->viewport[3]-py1;
            // Calc dist between BB extremes
            dx = px1 - px0;
            dy = py1 - py0;
            // Calc dist from center of BB to mouse
            dxm = mouseX - 0.5*(px1 + px0);
            dym = mouseY - 0.5*(py1 + py0);
            return (dxm*dxm + dym*dym < 0.5 * (dx*dx + dy*dy));
        }

        inline void checkXyArrowMouseCollision(const int mouseX, const int mouseY, Renderer_gl2* curr_renderer, bool &needRepaint)
        {
            bool foundArr = false;
            // Check for collision with XY translation arrows, as soon as one collision found, turn off all other arrows
            // +X
            if (curr_renderer->iPosXTranslateArrowEnabled && curr_renderer->posXTranslateBB != 0)
            {
                if (isMouseInBB(*curr_renderer->posXTranslateBB, mouseX, mouseY, curr_renderer))
                {
                    foundArr = true;
                    if (curr_renderer->iPosXTranslateArrowEnabled != 2 || 
                        curr_renderer->iNegXTranslateArrowEnabled == 2 || 
                        curr_renderer->iNegYTranslateArrowEnabled == 2 || 
                        curr_renderer->iPosYTranslateArrowEnabled == 2)
                        needRepaint = true;
                    curr_renderer->iPosXTranslateArrowEnabled = 2;
                    if (curr_renderer->iNegXTranslateArrowEnabled == 2)
                        curr_renderer->iNegXTranslateArrowEnabled = 1;
                    if (curr_renderer->iNegYTranslateArrowEnabled == 2)
                        curr_renderer->iNegYTranslateArrowEnabled = 1;
                    if (curr_renderer->iPosYTranslateArrowEnabled == 2)
                        curr_renderer->iPosYTranslateArrowEnabled = 1;
                }
                else if (curr_renderer->iPosXTranslateArrowEnabled == 2)
                {
                    curr_renderer->iPosXTranslateArrowEnabled = 1; // unhighlight
                    needRepaint = true;
                }
            }
            // -X
            if (!foundArr && curr_renderer->iNegXTranslateArrowEnabled && curr_renderer->negXTranslateBB != 0)
            {
                if (isMouseInBB(*curr_renderer->negXTranslateBB, mouseX, mouseY, curr_renderer))
                {
                    foundArr = true;
                    if (curr_renderer->iNegXTranslateArrowEnabled != 2 || 
                        curr_renderer->iNegYTranslateArrowEnabled == 2 || 
                        curr_renderer->iPosYTranslateArrowEnabled == 2)
                        needRepaint = true;
                    curr_renderer->iNegXTranslateArrowEnabled = 2;
                    if (curr_renderer->iNegYTranslateArrowEnabled == 2)
                        curr_renderer->iNegYTranslateArrowEnabled = 1;
                    if (curr_renderer->iPosYTranslateArrowEnabled == 2)
                        curr_renderer->iPosYTranslateArrowEnabled = 1;
                }
                else if (curr_renderer->iNegXTranslateArrowEnabled == 2)
                {
                    curr_renderer->iNegXTranslateArrowEnabled = 1; // unhighlight
                    needRepaint = true;
                }
            }
            // -Y
            if (!foundArr && curr_renderer->iNegYTranslateArrowEnabled && curr_renderer->negYTranslateBB != 0)
            {
                if (isMouseInBB(*curr_renderer->negYTranslateBB, mouseX, mouseY, curr_renderer))
                {
                    foundArr = true;
                    if (curr_renderer->iNegYTranslateArrowEnabled != 2 || 
                        curr_renderer->iPosYTranslateArrowEnabled == 2)
                        needRepaint = true;
                    curr_renderer->iNegYTranslateArrowEnabled = 2;
                    if (curr_renderer->iPosYTranslateArrowEnabled == 2)
                        curr_renderer->iPosYTranslateArrowEnabled = 1;
                }
                else if (curr_renderer->iNegYTranslateArrowEnabled == 2)
                {
                    curr_renderer->iNegYTranslateArrowEnabled = 1; // unhighlight
                    needRepaint = true;
                }
            }
            // +Y
            if (!foundArr && curr_renderer->iPosYTranslateArrowEnabled && curr_renderer->posYTranslateBB != 0)
            {
                if (isMouseInBB(*curr_renderer->posYTranslateBB, mouseX, mouseY, curr_renderer))
                {
                    foundArr = true;
                    if (curr_renderer->iPosYTranslateArrowEnabled != 2)
                        needRepaint = true;
                    curr_renderer->iPosYTranslateArrowEnabled = 2;
                }
                else if (curr_renderer->iPosYTranslateArrowEnabled == 2)
                {
                    curr_renderer->iPosYTranslateArrowEnabled = 1; // unhighlight
                    needRepaint = true;
                }
            }
        }

		virtual void show();
        virtual void storeAnnotations() throw (itm::RuntimeException);
        virtual void loadAnnotations() throw (itm::RuntimeException);
        virtual void clearAnnotations() throw (itm::RuntimeException);
		virtual bool eventFilter(QObject *object, QEvent *event);
		
		friend class MozakUI;
		
		QScrollBar *contrastSlider;
		


		/**********************************************************************************
        * Generates a new viewer using the given coordinates.
        * Called by the current <CExplorerWindow> when the user zooms in and the higher res-
        * lution has to be loaded.
        ***********************************************************************************/
        virtual void
        newViewer(
            int x, int y, int z,                //can be either the VOI's center (default)
                                                //or the VOI's ending point (see x0,y0,z0)
            int resolution,                     //resolution index of the view requested
            int t0, int t1,                     //time frames selection
            bool fromVaa3Dcoordinates = false,  //if coordinates were obtained from Vaa3D
            int dx=-1, int dy=-1, int dz=-1,    //VOI [x-dx,x+dx), [y-dy,y+dy), [z-dz,z+dz)
            int x0=-1, int y0=-1, int z0=-1,    //VOI [x0, x), [y0, y), [z0, z)
            bool auto_crop = true,              //whether to crop the VOI to the max dims
            bool scale_coords = true,           //whether to scale VOI coords to the target res
            int sliding_viewer_block_ID = -1    //block ID in "Sliding viewer" mode
        );

        void appendHistory();
        void performUndo();
        void performRedo();


	public slots:
		void updateContrast(int con);
        void buttonUndoClicked();
        void buttonRedoClicked();
		void invertImageButtonToggled(bool checked);
		void connectButtonToggled(bool checked);
		void extendButtonToggled(bool checked);
		void polyLineButtonToggled(bool checked);
		void retypeSegmentsButtonToggled(bool checked);
		void splitSegmentButtonToggled(bool checked);
		void deleteSegmentsButtonToggled(bool checked);
		void updateZoomLabel(int zr);

		/*********************************************************************************
        * Receive data (and metadata) from <CVolume> throughout the loading process
        **********************************************************************************/
        virtual void receiveData(
                itm::uint8* data,                   // data (any dimension)
                itm::integer_array data_s,          // data start coordinates along X, Y, Z, C, t
                itm::integer_array data_c,          // data count along X, Y, Z, C, t
                QWidget* dest,                         // address of the listener
                bool finished,                      // whether the loading operation is terminated
                itm::RuntimeException* ex = 0,      // exception (optional)
                qint64 elapsed_time = 0,            // elapsed time (optional)
                QString op_dsc="",                  // operation descriptor (optional)
                int step=0);                        // step number (optional)

};

#endif
