#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	// ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context

// #define PROJECT_TARGET_OPENGLES
#ifdef PROJECT_TARGET_OPENGLES
	ofLog() << "setting up gles";
	ofGLESWindowSettings settings;
	settings.glesVersion=2;
#else
	ofLog() << "setting up glsl";
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3,2);
#endif
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

	ofAppGLFWWindow* win;
    win = dynamic_cast<ofAppGLFWWindow *> (ofGetWindowPtr());
    win->setWindowIcon("icon.png");

	#if defined(TARGET_WIN32)
		HICON hWindowIcon = NULL;
		HICON hWindowIconBig = NULL;
		HWND hwnd = ofGetWin32Window();
		string stricon = "data/icon.ico";

		if (hWindowIcon != NULL) DestroyIcon (hWindowIcon);
		if (hWindowIconBig != NULL) DestroyIcon (hWindowIconBig);
		hWindowIcon = (HICON) LoadImageA (GetModuleHandle (NULL), stricon.c_str (), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		hWindowIconBig = (HICON) LoadImageA (GetModuleHandle (NULL), stricon.c_str (), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		SendMessage (hwnd, WM_SETICON, ICON_SMALL, (LPARAM) hWindowIcon);
		SendMessage (hwnd, WM_SETICON, ICON_BIG, (LPARAM) hWindowIconBig);
	#elif defined(TARGET_LINUX)
	win->setWindowTitle("TV_WALL_MAPPER");
	#endif


	shared_ptr<ofApp> mainApp(new ofApp);
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();

}
