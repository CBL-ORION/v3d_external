//last touch by Hanchuan Peng, 20170615.

#ifndef __V3DR_GL_VR_H__
#define __V3DR_GL_VR_H__

#include <SDL.h>

#include "../basic_c_fun/v3d_interface.h"


#include <openvr.h>
#include "lodepng.h"

#include "Matrices.h"//todo-yimin: this header is removable
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <gltext.hpp>//include freetype and gltest library

#include "mainwindow.h"


struct Agent {
	QString name;
	bool isItSelf;
	int colorType;
	float position[16];

};


enum ModelControlR
{
	m_drawMode = 0,
	m_deleteMode,
	m_dragMode,
	m_markMode
};

class Shader;
class Sphere;
class Cylinder;
class My4DImage;
class MainWindow;
class CGLRenderModel
{
public:
	CGLRenderModel( const std::string & sRenderModelName );
	~CGLRenderModel();

	bool BInit( const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture );
	void Cleanup();
	void Draw();
	const std::string & GetName() const { return m_sModelName; }

private:
	GLuint m_glVertBuffer;
	GLuint m_glIndexBuffer;
	GLuint m_glVertArray;
	GLuint m_glTexture;
	GLsizei m_unVertexCount;
	std::string m_sModelName;
};
//-----------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
class CMainApplication
{
public:
	CMainApplication(int argc = 0, char *argv[] = 0);
	virtual ~CMainApplication();

	bool BInit();
	bool BInitGL();
	bool BInitCompositor();

	void UpdateNTList(QString &msg, int type);//add the receieved message/NT to sketchedNTList
    QString NT2QString(); // prepare the message to be sent from currentNT.
	void ClearCurrentNT();//clear the currently drawn stroke, and all the flags
	bool HandleOneIteration();//used in collaboration mode 
	QString getHMDPOSstr();//get current HMD position, and prepare the message to be sent to server
	void SetupCurrentUserInformation(string name, int typeNumber);
	void SetupAgentModels(vector<Agent> &curAgents);//generate spheres models to illustrate the locations of other users
	void RefineSketchCurve(int direction, NeuronTree &oldNT, NeuronTree &newNT);//use Virtual Finger to improve curve
	QString FindNearestSegment(glm::vec3 dPOS);
	bool DeleteSegment(QString segName);
	void abcdefg();//merge sketchedNTList to sketchedNT_merged
	void MergeNTListtoloadedNT(NeuronTree &ntree, const QList<NeuronTree> * NTlist);//merge loadedNTlist to loadedNT

	void SetupRenderModels();

	void Shutdown();

	void RunMainLoop();
	bool HandleInput();//handle controller and keyboard input
	void ProcessVREvent( const vr::VREvent_t & event );
	void RenderFrame();
	
	bool SetupTexturemaps();//load controller textures and setup properties
	void AddVertex( float fl0, float fl1, float fl2, float fl3, float fl4, std::vector<float> &vertdata );
	void SetupControllerTexture();//update texture coordinates according to controller's new location

	void SetupMorphologyLine(int drawMode);
	void SetupMorphologyLine(NeuronTree neuron_Tree,GLuint& LineModeVAO, GLuint& LineModeVBO, GLuint& LineModeIndex,unsigned int& Vertcount,int drawMode);
	void SetupMorphologySurface(NeuronTree neurontree,vector<Sphere*>& spheres,vector<Cylinder*>& cylinders,vector<glm::vec3>& spheresPos);

	void SetupMarkerSurface();
	void SetupMarkerandSurface(double x,double y,double z,int type =3);

	void RenderControllerAxes();//draw XYZ axes on the base point of the controllers 

	bool SetupStereoRenderTargets();
	void SetupCompanionWindow();
	void SetupCameras();
	void SetupCamerasForMorphology();

	void SetupGlobalMatrix();//matrix for glabal transformation
	void RenderStereoTargets();
	void RenderCompanionWindow();
	void RenderScene( vr::Hmd_Eye nEye );

	Matrix4 GetHMDMatrixProjectionEye( vr::Hmd_Eye nEye );
	Matrix4 GetHMDMatrixPoseEye( vr::Hmd_Eye nEye );
	Matrix4 GetCurrentViewProjectionMatrix( vr::Hmd_Eye nEye );
	void UpdateHMDMatrixPose();

	Matrix4 ConvertSteamVRMatrixToMatrix4( const vr::HmdMatrix34_t &matPose );

	GLuint CompileGLShader( const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader );
	bool CreateAllShaders();

	void SetupRenderModelForTrackedDevice( vr::TrackedDeviceIndex_t unTrackedDeviceIndex );
	CGLRenderModel *FindOrLoadRenderModel( const char *pchRenderModelName );

public:

	MainWindow *mainwindow;
	My4DImage *img4d;
	QList<NeuronTree> *loadedNTList; // neuron trees brought to the VR view from the 3D view.	
	bool READY_TO_SEND;
	bool isOnline;
	ModelControlR  m_modeR;
	QString delName;
	QString markerPOS;

private: 
	std::string current_agent_color;
	std::string current_agent_name;
	bool m_bDebugOpenGL;
	bool m_bVerbose;
	bool m_bPerf;
	bool m_bVblank;
	bool m_bGlFinishHack;
	bool m_bShowMorphologyLine;
	bool m_bShowMorphologySurface;
	
	int  sketchNum; // a unique ID for neuron strokes, useful in deleting neurons
	NeuronTree loadedNT_merged; // merged result of loadedNTList
	
	QList<NeuronTree> sketchedNTList; //neuron trees drawn in the VR view.	
	NeuronTree sketchedNT_merged;//merged result of sketchedNTList, sketchedNT_merged
	NeuronTree currentNT;// currently drawn stroke of neuron
	
	NeuronTree tempNT;//used somewhere, can be change to a local variable
	BoundingBox swcBB;
	QList<ImageMarker> drawnMarkerList;

	vr::IVRSystem *m_pHMD;
	vr::IVRRenderModels *m_pRenderModels;
	std::string m_strDriver;
	std::string m_strDisplay;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ]; //note: contain everything: validity, matrix, ...
	Matrix4 m_rmat4DevicePose[ vr::k_unMaxTrackedDeviceCount ]; //note: store device transform matrices, copied from m_rTrackedDevicePose
	bool m_rbShowTrackedDevice[ vr::k_unMaxTrackedDeviceCount ];

	//gltext::Font * font_VR;//font for render text

private: // SDL bookkeeping
	SDL_Window *m_pCompanionWindow;
	uint32_t m_nCompanionWindowWidth;
	uint32_t m_nCompanionWindowHeight;

	SDL_GLContext m_pContext;

private: // OpenGL bookkeeping
	int m_iTrackedControllerCount;
	int m_iTrackedControllerCount_Last;
	int m_iValidPoseCount;
	int m_iValidPoseCount_Last;
	bool m_bFrozen; //freeze the view

	///control edit mode
	int  m_modeControlL;
	int m_modeControlR;
	bool m_translationMode;
	bool m_rotateMode;
	bool m_zoomMode;
	bool m_TouchFirst;
	bool m_pickUpState;
	/////store the pos every first time touch on the touchpad
	float m_fTouchOldXL;
	float m_fTouchOldYL;

	int pick_point;

	float detX;
	float detY;
	

	std::string m_strPoseClasses;                            // what classes we saw poses for this frame
	char m_rDevClassChar[ vr::k_unMaxTrackedDeviceCount ];   // for each device, a character representing its class


	
	float m_fNearClip;

	float m_fFarClip;
	//wwbmark
	GLuint m_iTexture;
	GLuint m_ControllerTexVAO;
	GLuint m_ControllerTexVBO;
	GLuint m_unCtrTexProgramID;
	GLint m_nCtrTexMatrixLocation;
	unsigned int m_uiControllerTexIndexSize;
	
	// controller index , get them in HandleInput()
	int	m_iControllerIDLeft;
	int	m_iControllerIDRight;

	//unsigned int m_uiVertcount;

	//VAO/VBO for surface and line of loaded neuron
	vector<Sphere*> loaded_spheres;
	vector<Cylinder*> loaded_cylinders;
	vector<glm::vec3> loaded_spheresPos;
	vector<glm::vec3> loaded_spheresColor;

	vector<Sphere*> Agents_spheres;
	vector<glm::vec3> Agents_spheresPos;
	vector<glm::vec3> Agents_spheresColor;

	vector<Sphere*> Markers_spheres;
	vector<glm::vec3> Markers_spheresPos;
	vector<glm::vec3> Markers_spheresColor;

	GLuint m_unMorphologyLineModeVAO;
	GLuint m_glMorphologyLineModeVertBuffer;
	GLuint m_glMorphologyLineModeIndexBuffer;
	unsigned int m_uiMorphologyLineModeVertcount;

	//VAO/VBO for surface and line of loaded neuron
	vector<Sphere*> sketch_spheres; //2017/11/13, wym: kind of obselete, drawn curves are not shown in surface mode 
	vector<Cylinder*> sketch_cylinders;
	vector<glm::vec3> sketch_spheresPos;

	GLuint m_unSketchMorphologyLineModeVAO;//for local sketch swc
	GLuint m_glSketchMorphologyLineModeVertBuffer;
	GLuint m_glSketchMorphologyLineModeIndexBuffer;
	unsigned int m_uiSketchMorphologyLineModeVertcount;


	GLuint m_unRemoteMorphologyLineModeVAO;//for remote Remote swc
	GLuint m_glRemoteMorphologyLineModeVertBuffer;
	GLuint m_glRemoteMorphologyLineModeIndexBuffer;
	unsigned int m_uiRemoteMorphologyLineModeVertcount;



	GLuint m_unCompanionWindowVAO; //two 2D boxes
	GLuint m_glCompanionWindowIDVertBuffer;
	GLuint m_glCompanionWindowIDIndexBuffer;
	unsigned int m_uiCompanionWindowIndexSize;

	GLuint m_glControllerVertBuffer;
	GLuint m_unControllerVAO;//note: axes for controller
	unsigned int m_uiControllerVertcount;

	Matrix4 m_mat4HMDPose;//note: m_rmat4DevicePose[hmd].invert()
	Matrix4 m_mat4eyePosLeft;
	Matrix4 m_mat4eyePosRight;

	Matrix4 m_mat4ProjectionCenter;
	Matrix4 m_mat4ProjectionLeft;
	Matrix4 m_mat4ProjectionRight;

	//for morphology rendering
	glm::mat4 m_HMDTrans;
	glm::mat4 m_EyeTransLeft;//head to eye
	glm::mat4 m_EyeTransRight; 
	glm::vec3 m_EyePosLeft;
	glm::vec3 m_EyePosRight;
	glm::mat4 m_ProjTransLeft;
	glm::mat4 m_ProjTransRight;

	float m_globalScale; // m_globalScale is consistent with m_globalMatrix, and is required somewhere
	glm::mat4 m_globalMatrix;
	

	//matrices to store frozen state
	Matrix4 m_frozen_mat4HMDPose;
	glm::mat4 m_frozen_HMDTrans;
	glm::mat4 m_frozen_globalMatrix;


	struct VertexDataScene//question: why define this? only used for sizeof()
	{
		Vector3 position;
		Vector2 texCoord;
	};

	struct VertexDataWindow//question: companion window just uses the projected data points from HMD?
	{
		Vector2 position;
		Vector2 texCoord;

		VertexDataWindow( const Vector2 & pos, const Vector2 tex ) :  position(pos), texCoord(tex) {	}
	};

	Shader* morphologyShader;
	GLuint m_unCompanionWindowProgramID;
	GLuint m_unControllerTransformProgramID;
	GLuint m_unRenderModelProgramID;

	GLint m_nControllerMatrixLocation;
	GLint m_nRenderModelMatrixLocation;

	struct FramebufferDesc
	{
		GLuint m_nDepthBufferId;
		GLuint m_nRenderTextureId;
		GLuint m_nRenderFramebufferId;
		GLuint m_nResolveTextureId;
		GLuint m_nResolveFramebufferId;
	};
	FramebufferDesc leftEyeDesc;
	FramebufferDesc rightEyeDesc;

	bool CreateFrameBuffer( int nWidth, int nHeight, FramebufferDesc &framebufferDesc );
	
	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;

	std::vector< CGLRenderModel * > m_vecRenderModels; //note: a duplicated access to below. used in shutdown destroy, check existence routines;
	CGLRenderModel *m_rTrackedDeviceToRenderModel[ vr::k_unMaxTrackedDeviceCount ]; //note: maintain all the render models for VR devices; used in drawing

/***********************************
***    volume image rendering    ***
***********************************/
public:
	void SetupCubeForImage4D();
	GLuint initTFF1DTex(const char* filename);
	GLuint initFace2DTex(GLuint texWidth, GLuint texHeight);
	GLuint initVol3DTex();
	void initFrameBufferForVolumeRendering(GLuint texObj, GLuint texWidth, GLuint texHeight);
	void SetupVolumeRendering();
	bool CreateVolumeRenderingShaders();
	void RenderImage4D(Shader* shader, vr::Hmd_Eye nEye, GLenum cullFace);
	void SetUinformsForRayCasting();

public:
	bool m_bHasImage4D;
private:
	
	GLuint m_clipPatchVAO;
	GLuint m_VolumeImageVAO;
	Shader* backfaceShader;//back face, first pass
	Shader* raycastingShader;//ray casting front face, second pass
	Shader* clipPatchShader;//ray casting front face, second pass

	GLuint g_winWidth; //todo: may be removable. wym
	GLuint g_winHeight;
	GLuint g_frameBufferBackface; //render backface to frameBufferBackface
	GLuint g_tffTexObj;	// transfer function
	GLuint g_bfTexObj;
	GLuint g_texWidth;
	GLuint g_texHeight;
	GLuint g_volTexObj;
};


#endif

