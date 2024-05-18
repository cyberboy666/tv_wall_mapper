#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxJSON.h"
#include "GLFW/glfw3.h"
#include "ofxNDI.h" // NDI classes

// #define PROJECT_TARGET_OPENGLES

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		// void keyPressed(int key);
        void keyPressed(ofKeyEventArgs& keyArgs);
        void keyReleased(ofKeyEventArgs& keyArgs);
        void drawWindow(ofEventArgs &args);
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);

    void jsonLoad(string path);
    void createInputsAndOutputsList();
    void drawBoxAndLabel(int xPosRaw, int yPosRaw, int widthRaw, int heightRaw, string text, bool isFocus=false);
    void jsonSave();
    void createMonitor(int dataListIndex);
    void printFramerate();
    void drawFramerate();
    int getLayoutIndex();
    void loadShader(int item_current);
    void loadInput();


    #ifdef PROJECT_TARGET_OPENGLES
    bool useGLES = true;
    #else
    bool useGLES =false;
    #endif

    // control variables
    bool ctrlPressed = false;
    bool shiftPressed = false;
    bool altPressed = false;
    float lastMouseXPos;
    float lastMouseYPos;

	ofxJSONElement json;
	ofxImGui::Gui gui;
    ofVideoGrabber vidGrabber;
    ofxNDIreceiver ndiReceiver;
    bool hasNDILoaded = false;
    bool showFramerate = false;
    string glType;

	// data in config
    bool hideConfig=false;
    bool hideMaptest = false;
    bool hidePreview = false;
    bool setResolutions = false;
    int inputWidth;
    int inputHeight;
    int framerate=30;
    int numberTVs=9;
    int selectedInputIndex = 0;
    int canvasWidth=5000;
    int canvasHeight=5000;
    int numberMonitors=1;
    int monitorSelected=0;
    bool monitorFocus=false;

    // data generated or defaulted
    int grid_index = 0;

    struct monitorData{

        // data in config:
        bool outputToMonitor;
        bool outputToNDI;
        bool isFullscreen;
        int displayIndex;
        int tvNumber;
        int tvFirst;
        string tvLayout;
        // data generated or defaulted
        int width;
        int height;
        int xPos;
        int yPos;
        int tvFocus=0;
        

        ofShader shader;
        ofFbo mapTest;
        ofFbo fbo;
        shared_ptr<ofAppBaseWindow> window;

        const GLFWvidmode* videoMode;
        ofxNDIsender ndiSender;

        monitorData() : videoMode(nullptr) {}
    };

    vector<monitorData> monitorDataList;

    // tv data
    struct tvData{
        int width;
        int height;
        int xPos;
        int yPos;

        tvData() : width(0), height(0), xPos(0), yPos(0) {}
    };
    vector<tvData> tvDataList;


    // input devices
    struct inputType {
        string type;
        int typeId;
        string name;
    };

    vector<inputType> videoInputs;

    struct monitorType {
        int index;
        string name;
    };

    vector<monitorType> monitorOptions;

    ofFbo innerFbo;
    ofImage img;
    ofTrueTypeFont font;

    vector<string> testcards;
    
    const char* grid_options[5] = { "100", "50", "10", "5", "1" };

    const char* layout_options[3] = { "1", "2x2", "3x3"};

    ofTexture ndiTexture;
};
