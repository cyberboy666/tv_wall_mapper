#include "ofApp.h"
#include "GLFW/glfw3.h"

#define PROJECT_TARGET_OPENGLES

void ofApp::setup(){

    ofDisableArbTex();

    if(useGLES){
    ofLog() << "uses gles";
    glType = "gles/";
    }
    else{
    ofLog() << "uses glsl";
    glType = "glsl/";
    }

    font.load("arial.ttf", 50);


    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    ofSetFrameRate(framerate);
    jsonLoad("config.json");

    if(!setResolutions){
        inputWidth = ofGetScreenWidth();
        inputHeight = ofGetScreenHeight();
    }

    img.allocate(inputWidth, inputHeight, OF_IMAGE_COLOR);
    innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
    ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
        
    gui.setup(nullptr, true, ImGuiConfigFlags_ViewportsEnable );

    testcards = {"testcard_01.png", "testcard_grid.jpg"};

    createInputsAndOutputsList();

    ofLog() << "this many monitors: " << monitorDataList.size();

    if(monitorDataList.size() == 0){
        monitorData thisMonitor;
        thisMonitor.outputToMonitor = true;
        thisMonitor.outputToNDI = false;
        thisMonitor.isFullscreen = false;
        thisMonitor.displayIndex = 0;
        thisMonitor.tvNumber = 1;
        thisMonitor.tvFirst = 1;
        thisMonitor.tvLayout = "1";
        monitorDataList.push_back(thisMonitor);
    }
    if(tvDataList.size() == 0){
        tvData thisTv;
        tvDataList.push_back(thisTv);
    }

    if(useGLES){
        // only allowed one monitor in gles mode and dont allow output to monitor
        while(monitorDataList.size() > 1){
            monitorDataList.pop_back();
        }
        monitorDataList[0].outputToMonitor = false;
    }

    for (size_t i = 0; i < monitorDataList.size(); i++)
    {
        createMonitor(i);
    }
    loadInput();
    loadShader(getLayoutIndex());

    if(numberMonitors == 1 && !monitorDataList[monitorSelected].outputToMonitor && monitorDataList[monitorSelected].isFullscreen){
        ofLog() << "setting main to full screen..";
        ofSetFullscreen(true);
    }
}

void ofApp::createInputsAndOutputsList(){

    videoInputs = {};

    for (size_t i = 0; i < testcards.size(); i++)
    {
        inputType imageInput;
        imageInput.type = "TESTCARD";
        imageInput.typeId = i;
        imageInput.name = "TESTCARD_" + ofToString(i);
        videoInputs.push_back(imageInput); 
    }

    inputType testMap;
    testMap.type = "TESTMAP";
    testMap.typeId = 0;
    testMap.name = "TESTMAP";
    videoInputs.push_back(testMap);

    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable && devices[i].deviceName.find("broadcom") ==  std::string::npos){
            //log the device
            inputType videoInput;
            videoInput.type = "VIDEO";
            videoInput.typeId = devices[i].id ;
            videoInput.name = "VIDEO_" + ofToString(devices[i].id) + " :" + devices[i].deviceName;
            videoInputs.push_back(videoInput);            
        }
    }

    ndiReceiver.ReceiveImage(ndiTexture);

    try {
        if(ndiReceiver.GetNDIversion() == ""){ hasNDILoaded = false;}
        else{ hasNDILoaded = true;}
    } catch (...) { hasNDILoaded = false;}

    ofLog() << "hasNDILoaded: " << hasNDILoaded;
    
    int nsenders = ndiReceiver.GetSenderCount();
    ofLog() << "ndiReceiver.GetSenderCount(): " << ndiReceiver.GetSenderCount();
    for(int i = 0; i < nsenders; i++){
       inputType ndiInput;
        ndiInput.type = "NDI";
        ndiInput.typeId = i;
        ndiInput.name = "NDI_" + ofToString(i) + " : " + ndiReceiver.GetSenderName(i);
        videoInputs.push_back(ndiInput);
    }
    if(selectedInputIndex >= videoInputs.size()){selectedInputIndex = 0;}

    monitorOptions = {};

    int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    for (size_t i = 0; i < monitorCount; i++)
    {
        monitorType thisMonitor;
        thisMonitor.index = i;
        thisMonitor.name = "MONITOR_" + ofToString(i)  + " : " + glfwGetMonitorName(monitors[i]);
        ofLog() << thisMonitor.name;
        monitorOptions.push_back(thisMonitor);
    }


}

void ofApp::createMonitor(int dataListIndex){
        int monitorCount;
	    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        if(monitorDataList[dataListIndex].displayIndex >= monitorCount){
            monitorDataList[dataListIndex].displayIndex = 0;
        }
        int monitorIndex = monitorOptions[monitorDataList[dataListIndex].displayIndex].index;
    
        monitorDataList[dataListIndex].videoMode = glfwGetVideoMode(monitors[monitorIndex]);
        monitorDataList[dataListIndex].width = monitorDataList[dataListIndex].videoMode->width;
        monitorDataList[dataListIndex].height = monitorDataList[dataListIndex].videoMode->height;
        monitorDataList[dataListIndex].xPos = 0;
        monitorDataList[dataListIndex].yPos = 0;
        glfwGetMonitorPos(monitors[monitorIndex], &monitorDataList[dataListIndex].xPos, &monitorDataList[dataListIndex].yPos);
        // set the input resolution to the highest of the connected monitors
        if(monitorDataList[dataListIndex].width * monitorDataList[dataListIndex].height > inputWidth * inputHeight){
            inputWidth = monitorDataList[dataListIndex].width;
            inputHeight = monitorDataList[dataListIndex].height;
            innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
            ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
            ofLog() << "inputWidth is now set to: " << inputWidth;
            ofLog() << "inputHeight is now set to: " << inputHeight;
        }

        ofLog() << "monitor position xPos: " <<  monitorDataList[dataListIndex].xPos;
        ofLog() << "monitor position yPos: " <<  monitorDataList[dataListIndex].yPos;
        ofLog() << "monitor position width: " <<  monitorDataList[dataListIndex].width;
        ofLog() << "monitor position height: " <<  monitorDataList[dataListIndex].height;

        monitorDataList[dataListIndex].fbo.allocate(monitorDataList[dataListIndex].width, monitorDataList[dataListIndex].height, GL_RGB);
        monitorDataList[dataListIndex].mapTest.allocate(monitorDataList[dataListIndex].width, monitorDataList[dataListIndex].height, GL_RGB);

        if(monitorDataList[dataListIndex].outputToMonitor){
            ofGLFWWindowSettings settings;
            // settings.glesVersion=2;
            settings.setGLVersion(3,2);
            glm::vec2 position(monitorDataList[dataListIndex].xPos, monitorDataList[dataListIndex].yPos);
            settings.setPosition(position);
            settings.setSize(monitorDataList[dataListIndex].width, monitorDataList[dataListIndex].height);
            if(monitorDataList[dataListIndex].isFullscreen){settings.windowMode = OF_FULLSCREEN;}
            settings.shareContextWith = ofGetCurrentWindow();  

            monitorDataList[dataListIndex].window = ofCreateWindow(settings);
            monitorDataList[dataListIndex].window->setVerticalSync(false);
            ofAppGLFWWindow* win;
            win = dynamic_cast<ofAppGLFWWindow *> (monitorDataList[dataListIndex].window.get());

            win->setWindowTitle("TV_WALL_MAPPER - MONITOR " + ofToString(dataListIndex));

            #if defined(TARGET_WIN32)
                HICON hWindowIcon = NULL;
                HICON hWindowIconBig = NULL;
                HWND hwnd = win->getWin32Window();
                string stricon = "data/icon.ico";

                if (hWindowIcon != NULL) DestroyIcon (hWindowIcon);
                if (hWindowIconBig != NULL) DestroyIcon (hWindowIconBig);
                hWindowIcon = (HICON) LoadImageA (GetModuleHandle (NULL), stricon.c_str (), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
                hWindowIconBig = (HICON) LoadImageA (GetModuleHandle (NULL), stricon.c_str (), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
                SendMessage (hwnd, WM_SETICON, ICON_SMALL, (LPARAM) hWindowIcon);
                SendMessage (hwnd, WM_SETICON, ICON_BIG, (LPARAM) hWindowIconBig);
            #elif defined(TARGET_LINUX)
                win->setWindowIcon("icon.png");
            #endif

            ofAddListener(monitorDataList[dataListIndex].window->events().draw, this, &ofApp::drawWindow);
            ofAddListener(monitorDataList[dataListIndex].window->events().mousePressed, this, &ofApp::mousePressedWindow);
            ofAddListener(monitorDataList[dataListIndex].window->events().exit, this, &ofApp::exitWindow);
        }


        if(monitorDataList[dataListIndex].tvLayout == "1"){monitorDataList[dataListIndex].shader.load(glType + "default.vert",glType + "1output.frag");}
        else if(monitorDataList[dataListIndex].tvLayout == "2x2"){monitorDataList[dataListIndex].shader.load(glType + "default.vert",glType + "2x2output.frag");}
        else if(monitorDataList[dataListIndex].tvLayout == "3x3"){monitorDataList[dataListIndex].shader.load(glType + "default.vert",glType + "3x3output.frag");}
}

void ofApp::drawBoxAndLabel(int xPosRaw, int yPosRaw, int widthRaw, int heightRaw, string text, bool isFocus){
    // scale values

    float xPos = static_cast<float>(xPosRaw) / canvasWidth * monitorDataList[monitorSelected].mapTest.getWidth();
    float yPos = static_cast<float>(yPosRaw) / canvasHeight * monitorDataList[monitorSelected].mapTest.getHeight();
    float width = static_cast<float>(widthRaw) / canvasWidth * monitorDataList[monitorSelected].mapTest.getWidth();
    float height = static_cast<float>(heightRaw) / canvasHeight * monitorDataList[monitorSelected].mapTest.getHeight();

    float scaleX = (width * 0.66) / font.stringWidth(text);
    float scaleY = (height * 0.66) / font.stringHeight(text);
    
    if(isFocus){ofSetColor(255, 0, 25);}
    else{ofSetColor(0, 0, 255);}

    ofDrawRectangle(xPos, yPos, width, height);
    ofSetColor(0, 255, 255);
    float innerX = xPos + (width - font.stringWidth(text) * scaleX) * 0.5;
    float innerY = yPos + (height - font.stringHeight(text) * scaleY) * 0.5;
    ofSetColor(0, 255, 255);

    ofPushMatrix();
        
        ofTranslate(0 + innerX, font.stringHeight(text) * scaleY + innerY);
        ofScale(scaleX, scaleY);
        if(abs(widthRaw) < abs(heightRaw)){
            ofRotateDeg(90);
            ofTranslate(-45,5);
            }
        font.drawString(text, 0, 0);
    ofPopMatrix();

    ofSetColor(255);

}


void ofApp::update(){

    innerFbo.begin();
        ofClear(255, 255, 255, 0); // Clear the FBO with a transparent background
        ofSetColor(255); // Set color to white
        if(videoInputs[selectedInputIndex].type == "VIDEO"){
            vidGrabber.update();
            vidGrabber.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
        else if(videoInputs[selectedInputIndex].type == "TESTCARD"){
            img.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
        else if(videoInputs[selectedInputIndex].type == "TESTMAP"){
            monitorDataList[monitorSelected].mapTest.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
        else if(videoInputs[selectedInputIndex].type == "NDI"){
            ndiReceiver.ReceiveImage(ndiTexture);
            ndiTexture.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());
        }
    innerFbo.end();

    for (size_t i = 0; i < monitorDataList.size(); i++){

        monitorDataList[i].fbo.begin();
            monitorDataList[i].shader.begin();
                monitorDataList[i].shader.setUniform1f("u_time", ofGetElapsedTimef());
                monitorDataList[i].shader.setUniform2f("u_resolution", innerFbo.getWidth(), innerFbo.getHeight());
                monitorDataList[i].shader.setUniform2i("u_canvas", canvasWidth, canvasHeight);
                monitorDataList[i].shader.setUniformTexture("u_tex0", innerFbo.getTexture(), 0);
                int tvStartIndex = monitorDataList[i].tvFirst - 1;
                for (int j = 0; j < monitorDataList[i].tvNumber; j++) {
                    monitorDataList[i].shader.setUniform4i(
                        "u_tvdata" + ofToString(j), 
                        tvDataList[tvStartIndex + j].width, 
                        tvDataList[tvStartIndex + j].height, 
                        tvDataList[tvStartIndex + j].xPos, 
                        tvDataList[tvStartIndex + j].yPos
                    );
                }
                innerFbo.draw(0,0, monitorDataList[i].fbo.getWidth(), monitorDataList[i].fbo.getHeight());
                // innerFbo.draw(0,0, innerFbo.getWidth(), innerFbo.getHeight());

            monitorDataList[i].shader.end();
        monitorDataList[i].fbo.end();

        if(monitorDataList[i].outputToNDI && monitorDataList[i].ndiSender.SenderCreated()){
            monitorDataList[i].ndiSender.SendImage(monitorDataList[i].fbo);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    if(numberMonitors == 1 && !monitorDataList[monitorSelected].outputToMonitor && monitorDataList[monitorSelected].isFullscreen){
        ofClear(0);
        monitorDataList[monitorSelected].fbo.draw(0,0, ofGetScreenWidth(), ofGetScreenHeight());
        drawFramerate();
        return;
    }

    printFramerate();

    if(!hidePreview){   
        monitorDataList[monitorSelected].fbo.draw(600,400, ofGetScreenWidth() / 3, ofGetScreenHeight() / 3);
    }
    if(!hideMaptest){
        float max_canvas = static_cast<float>MAX(canvasWidth, canvasHeight);
        monitorDataList[monitorSelected].mapTest.draw(600, 20, (ofGetScreenWidth() / 3) * (canvasWidth/ max_canvas ), (ofGetScreenHeight() / 3) * (canvasHeight/ max_canvas ));
    }
    
    ImGuiWindowFlags window_flags = 0;
    // window_flags |= ImGuiWindowFlags_NoTitleBar;
    // window_flags |= ImGuiWindowFlags_NoScrollbar;
    // window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoNav;
    // window_flags |= ImGuiWindowFlags_NoBackground;
    // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_NoDocking;
    // window_flags |= ImGuiWindowFlags_UnsavedDocument;


    gui.begin();
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, ofGetWindowHeight() - 150), ImGuiCond_FirstUseEver);
        ImGui::Begin("TV_WALL_MAPPER SETTINGS", NULL, window_flags);

        if (ImGui::CollapsingHeader("HELP")){
            ImGui::Text("MAPPING WITH A MOUSE:");
            ImGui::BulletText("CLICK on a TV to select it (RED is selected)");
            ImGui::BulletText("CLICK and drag a TV to position");
            ImGui::BulletText("hold SHIFT and drag to resize it");
            ImGui::BulletText("or RIGHT CLICK and drag to resize it");
            ImGui::Separator();
            ImGui::Text("MAPPING WITH A KEYBOARD:");
            ImGui::BulletText("press ALT plus LEFT/RIGHT to select a TV");
            ImGui::BulletText("press ARROW KEYS to position it");
            ImGui::BulletText("press SHIFT plus ARROW KEYS to resize it");
            ImGui::BulletText("press ALT plus UP/DOWN to set grid size (how much each press moves)");
            ImGui::Separator();
            ImGui::BulletText("NOTE: TVs with width less than height are automatically rotated");
            ImGui::BulletText("NOTE: resizing can be negative to support all possible orientations");
            ImGui::Separator();
            ImGui::Text("OTHER KEYBOARD SHORTCUTS:");
            ImGui::BulletText("press F to toggle ON/OFF FULLSCREEN");
            ImGui::BulletText("press R to toggle ON/OFF FPS ON OUTPUT");
        }

        ImGui::SeparatorText("MODE SETTING");

        if(ImGui::Button("save config")){
            jsonSave();
        }
        ImGui::SameLine();
        if(ImGui::Button("load config")){
            jsonLoad("config.json");
        }
        ImGui::SameLine();
        if(ImGui::Button("refresh inputs/outputs")){
            createInputsAndOutputsList();
        }

        const char** input_names = new const char*[videoInputs.size()];
        for (int i = 0; i < videoInputs.size(); i++)
        {
            input_names[i] = videoInputs[i].name.c_str();
        }
        if(ImGui::Combo("VIDEO SOURCE", &selectedInputIndex, input_names, videoInputs.size())){
            loadInput();
        }

        if(setResolutions){
            if(ImGui::InputInt("FRAMERATE", &framerate)){
                vidGrabber.setDesiredFrameRate(framerate);
                monitorDataList[monitorSelected].ndiSender.SetFrameRate(framerate);
                ofSetFrameRate(framerate);
                loadInput();
            }

            if(ImGui::InputInt("INPUT WIDTH", &inputWidth) || ImGui::InputInt("INPUT HEIGHT", &inputHeight)){
                innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
                ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
                ofLog() << "inputWidth is now set to: " << inputWidth;
                ofLog() << "inputHeight is now set to: " << inputHeight;
                loadInput();
            }
        }

        string resolutionText = "INPUT RESOLUTION: " + ofToString(inputWidth) + "X" + ofToString(inputHeight);
        ImGui::Text(resolutionText.c_str());

        ImGui::InputInt("NUMBER OF TVS", &numberTVs);
        if(numberTVs < tvDataList.size()){
            if(numberTVs <= 1){ numberTVs = 1;}
            for (int i = 0; i < tvDataList.size() - numberTVs; i++) {
                tvDataList.pop_back();
            }
        }
        else if(numberTVs > tvDataList.size()){
            for (int i = 0; i < numberTVs - tvDataList.size(); i++) {
            tvData thisTv;
            tvDataList.push_back(thisTv);
            }
        }

        ImGui::Checkbox("HIDE MAPTEST", &hideMaptest);
        ImGui::SameLine();
        ImGui::Checkbox("HIDE PREVIEW", &hidePreview);
        ImGui::SameLine();
        if(ImGui::Checkbox("OVERWRITE DEFAULTS", &setResolutions) && !setResolutions){
            inputWidth = ofGetScreenWidth();
            inputHeight = ofGetScreenHeight();
            framerate = 30;
            innerFbo.allocate(inputWidth, inputHeight, GL_RGB);
            ndiTexture.allocate(inputWidth, inputHeight, GL_RGBA);
            vidGrabber.setDesiredFrameRate(framerate);
            monitorDataList[monitorSelected].ndiSender.SetFrameRate(framerate);
            ofSetFrameRate(framerate);
            loadInput();
            ofLog() << "inputWidth is now set to: " << inputWidth;
            ofLog() << "inputHeight is now set to: " << inputHeight;
        }

        ImGui::SeparatorText("MONITOR SETTING");

        if(useGLES){
            ImGui::BeginDisabled();
        }
        if(ImGui::InputInt("NUMBER OF MONITORS", &numberMonitors)){
            if(numberMonitors <= 0){ numberMonitors = 1;}
            else if(numberMonitors > monitorOptions.size() + 1){
            numberMonitors = monitorOptions.size() + 1;
            }
            if(monitorSelected >=numberMonitors){monitorSelected = 0;}

            if(numberMonitors < monitorDataList.size()){
                for (int i = numberMonitors; i < monitorDataList.size(); i++) {
                    ofLog() << "removing monitor with index " << i;
                    if(monitorDataList[i].outputToMonitor){monitorDataList[i].window->setWindowShouldClose();}
                    monitorDataList.pop_back();
                }
            }
            else if(numberMonitors > monitorDataList.size()){
                for (int i = monitorDataList.size(); i < numberMonitors; i++) {
                    monitorData thisMonitor;
                    thisMonitor.outputToMonitor = true;
                    thisMonitor.outputToNDI = false;
                    thisMonitor.isFullscreen = false;
                    thisMonitor.displayIndex = 0;
                    thisMonitor.tvNumber = 1;
                    thisMonitor.tvFirst = 1;
                    thisMonitor.tvLayout = "1";
                    monitorDataList.push_back(thisMonitor);
                    createMonitor(i);
                }
            }
        }
        if(useGLES){
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
            {
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("ONLY SINGLE WINDOW IN GLES MODE");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        for (int i = 0; i < numberMonitors - 1; i++)
        {
            ImGui::RadioButton(("MONITOR " + ofToString(i)).c_str(), &monitorSelected, i); ImGui::SameLine();
        }
        ImGui::RadioButton(("MONITOR " + ofToString(numberMonitors - 1)).c_str(), &monitorSelected, numberMonitors - 1);
        
        ImGui::Checkbox("only show tvs for this monitor", &monitorFocus);

        if(useGLES){
            ImGui::BeginDisabled();
        }
        if(ImGui::Checkbox("OUTPUT TO MONITOR ", &monitorDataList[monitorSelected].outputToMonitor)){
            if(monitorDataList[monitorSelected].outputToMonitor){
                createMonitor(monitorSelected);
            }
            if(!monitorDataList[monitorSelected].outputToMonitor){
                if(useGLES){
                ofLog() << "do not close any window here";
                }
                else{
                    monitorDataList[monitorSelected].window->setWindowShouldClose();
                }
            }
        }
        if(useGLES){
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
            {
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("ONLY SINGLE WINDOW IN GLES MODE");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        ImGui::SameLine();

        if(!hasNDILoaded){
            ImGui::BeginDisabled();
        }
        // ofLog() << monitorDataList[monitorSelected].ndiSender.GetNDIversion();
        if(ImGui::Checkbox("OUTPUT TO NDI ", &monitorDataList[monitorSelected].outputToNDI)){
            if(monitorDataList[monitorSelected].outputToNDI && !monitorDataList[monitorSelected].ndiSender.SenderCreated()){
                monitorDataList[monitorSelected].ndiSender.SetReadback();
                string ndiName = "TV_WALL_MAPPER_OUT_" + ofToString(monitorSelected);
                monitorDataList[monitorSelected].ndiSender.SetFrameRate(framerate);
                monitorDataList[monitorSelected].ndiSender.SetAsync();
                monitorDataList[monitorSelected].ndiSender.CreateSender(ndiName.c_str(), monitorDataList[monitorSelected].width, monitorDataList[monitorSelected].height);
            }
            if(!monitorDataList[monitorSelected].outputToNDI){
                monitorDataList[monitorSelected].ndiSender.ReleaseSender();
            }
        }
        if(!hasNDILoaded){
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
            {
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("NDI SDK MAY NOT BE LOADED");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        // bool testCheck = false;
        // ImGui::Checkbox("TEST CHECKBOX ", &testCheck);

        const char** output_names = new const char*[monitorOptions.size()];
        for (int i = 0; i < monitorOptions.size(); i++)
        {
            output_names[i] = monitorOptions[i].name.c_str();
        }
        if(ImGui::Combo("MONITOR DISPLAY", &monitorDataList[monitorSelected].displayIndex, output_names, monitorOptions.size())){
            if(monitorDataList[monitorSelected].outputToMonitor){
                monitorDataList[monitorSelected].window->setWindowShouldClose();
                createMonitor(monitorSelected);
            }
        }

        string monitorResolutionText = "MONITOR RESOLUTION: " + ofToString(monitorDataList[monitorSelected].width) + "X" + ofToString(monitorDataList[monitorSelected].height);
        ImGui::Text(monitorResolutionText.c_str());

        if(ImGui::Checkbox("monitor fullscreen (press f)", &monitorDataList[monitorSelected].isFullscreen)){
            // switch window to fullscreen
            if(monitorDataList[monitorSelected].outputToMonitor){
                monitorDataList[monitorSelected].window->setFullscreen(monitorDataList[monitorSelected].isFullscreen);
            }
            else if(numberMonitors == 1 && !monitorDataList[monitorSelected].outputToMonitor){
                ofSetFullscreen(monitorDataList[monitorSelected].isFullscreen);
            }
        }
        ImGui::SameLine();
        ImGui::Checkbox("show framerate (press r)", &showFramerate);

        static int item_current = getLayoutIndex();

        if(ImGui::Combo("TV LAYOUT", &item_current, layout_options, IM_ARRAYSIZE(layout_options))){
            loadShader(item_current);
        }

        ImGui::InputInt("NUMBER OF TVS ON OUTPUT", &monitorDataList[monitorSelected].tvNumber);

        ImGui::InputInt("FIRST TV ON OUTPUT", &monitorDataList[monitorSelected].tvFirst);

        ImGui::InputInt("TV FOCUS", &monitorDataList[monitorSelected].tvFocus);



        ImGui::SeparatorText("CANVAS VALUES");
        ImGui::InputInt("CANVAS WIDTH (mm)", &canvasWidth);
        ImGui::InputInt("CANVAS HEIGHT (mm)", &canvasHeight);

        ImGui::SeparatorText("CANVAS VALUES");

        if(ImGui::InputInt("SELECTED TV", &monitorDataList[monitorSelected].tvFocus)){
            if(monitorDataList[monitorSelected].tvFocus > numberTVs){monitorDataList[monitorSelected].tvFocus = 0;}
            else if(monitorDataList[monitorSelected].tvFocus < 0){monitorDataList[monitorSelected].tvFocus = numberTVs;}
        }
        ImGui::Combo("GRID SIZE (mm)", &grid_index, grid_options, IM_ARRAYSIZE(grid_options));

        ImGui::SeparatorText("TV VALUES");
        int tvStart = 1;
        int tvEnd = numberTVs;
        if(monitorFocus){
            tvStart = monitorDataList[monitorSelected].tvFirst;
            tvEnd = MIN(tvStart + monitorDataList[monitorSelected].tvNumber - 1, numberTVs);
        }
        monitorDataList[monitorSelected].mapTest.begin();
        ofClear(0);
        for (int i = tvStart; i <= tvEnd; i++) {
            
                bool isFocus = i == monitorDataList[monitorSelected].tvFocus;
                if(!hideMaptest || videoInputs[selectedInputIndex].type == "TESTMAP"){
                    drawBoxAndLabel(tvDataList[i-1].xPos, tvDataList[i-1].yPos, tvDataList[i-1].width, tvDataList[i-1].height, ofToString(i), isFocus);
                }
                
            if(ImGui::TreeNode(("TV " + ofToString(i)).c_str())){
                ImGui::InputInt(("TV " + ofToString(i) + " WIDTH (mm)").c_str(), &tvDataList[i-1].width);
                ImGui::InputInt(("TV " + ofToString(i) + " HEIGHT (mm)").c_str(), &tvDataList[i-1].height);
                ImGui::InputInt(("TV " + ofToString(i) + " X POS (mm)").c_str(), &tvDataList[i-1].xPos);
                ImGui::InputInt(("TV " + ofToString(i) + " Y POS (mm)").c_str(), &tvDataList[i-1].yPos);
            }
        }
        monitorDataList[monitorSelected].mapTest.end();

        ImGui::End();
    gui.end();
}

int ofApp::getLayoutIndex(){
    int item_current = 0;
    for (int i = 0; i < IM_ARRAYSIZE(layout_options); i++)
    {
        if (monitorDataList[monitorSelected].tvLayout == layout_options[i]) {
            return i; // Match found, store the index
        }
    }
    return 0;
}

void ofApp::loadShader(int item_current){
    monitorDataList[monitorSelected].tvLayout = layout_options[item_current];
    if(layout_options[item_current] == "1"){
        monitorDataList[monitorSelected].tvNumber = 1;
        monitorDataList[monitorSelected].shader.load(glType + "default.vert",glType + "1output.frag");
    }
    else if(layout_options[item_current] == "2x2"){
        monitorDataList[monitorSelected].tvNumber = 4;
        monitorDataList[monitorSelected].shader.load(glType + "default.vert",glType + "2x2output.frag");
    }
    else if(layout_options[item_current] == "3x3"){
        monitorDataList[monitorSelected].tvNumber = 9;
        ofLog() << "shader loaded is: " << glType + "3x3output.frag";
        monitorDataList[monitorSelected].shader.load(glType + "default.vert",glType + "3x3output.frag");
    }
}

void ofApp::loadInput(){
    if(videoInputs[selectedInputIndex].type == "VIDEO"){
        if(vidGrabber.isInitialized()){vidGrabber.close();}
        vidGrabber.setDesiredFrameRate(framerate);
        vidGrabber.setDeviceID(videoInputs[selectedInputIndex].typeId);
        vidGrabber.initGrabber(inputWidth, inputHeight);
    }
    else{vidGrabber.close();}

    if(videoInputs[selectedInputIndex].type == "TESTCARD"){
        
        img.load(testcards[videoInputs[selectedInputIndex].typeId]);
    }

    if(videoInputs[selectedInputIndex].type == "NDI"){
        ndiReceiver.SetSenderIndex(videoInputs[selectedInputIndex].typeId);
        
    }
    else{
        ndiReceiver.ReleaseReceiver();
    }
}

void ofApp::drawWindow(ofEventArgs &args) {

    int i = -1;
    for (int j = 0; j < monitorDataList.size(); ++j) {
        if (monitorDataList[j].window.get() == ofGetWindowPtr()) {
            i = j; // Found the index
            break;
        }
    }

    if(monitorDataList[i].outputToMonitor){
        monitorDataList[i].fbo.draw(0,0, monitorDataList[i].window->getWidth(),monitorDataList[i].window->getHeight());
    }
    drawFramerate();
}

void ofApp::mousePressedWindow(ofMouseEventArgs &args) {

    int i = -1;
    for (int j = 0; j < monitorDataList.size(); ++j) {
        if (monitorDataList[j].window.get() == ofGetWindowPtr()) {
            i = j; // Found the index
            break;
        }
    }

    if(args.button == 1){toggleFullscreen();}
}

void ofApp::exitWindow(ofEventArgs &args) {

    int i = -1;
    for (int j = 0; j < monitorDataList.size(); ++j) {
        if (monitorDataList[j].window.get() == ofGetWindowPtr()) {
            i = j; // Found the index
            break;
        }
    }

    monitorDataList[i].outputToMonitor = false;
}

void ofApp::drawFramerate(){
    if(showFramerate){
        stringstream info;
        float framerate = roundf(ofGetFrameRate());
        info << "FPS: " << framerate ;
        ofPushMatrix();
            ofScale(2, 2);
            ofDrawRectangle(font.getStringBoundingBox(info.str(), 0, font.stringHeight(info.str())));
            ofSetColor(255,0,0);
            font.drawString(info.str(), 0, font.stringHeight(info.str()));
            ofSetColor(255);
        ofPopMatrix();
    }

}

void ofApp::printFramerate(){
    stringstream info;
    float framerate = roundf(ofGetFrameRate());
    info << "FPS: " << framerate ;
    ofDrawBitmapStringHighlight(info.str(), 600, 12, ofColor::black, ofColor::yellow);

}

void ofApp::keyPressed(ofKeyEventArgs& keyArgs){

    if (keyArgs.key == OF_KEY_CONTROL) {
        ctrlPressed = true;
    }
    if (keyArgs.key == OF_KEY_SHIFT) {
        shiftPressed = true;
    }
    if (keyArgs.key == OF_KEY_ALT) {
        altPressed = true;
    }

    int gridSize = ofToInt(grid_options[grid_index]);
    // set position with arrow keys
    if(keyArgs.key == OF_KEY_UP && !ctrlPressed && !shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].yPos -= gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_DOWN && !ctrlPressed && !shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].yPos += gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_LEFT && !ctrlPressed && !shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].xPos -= gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_RIGHT && !ctrlPressed && !shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].xPos += gridSize; 
        }
    }
    // set size with ctrl + arrow keys
    if(keyArgs.key == OF_KEY_UP && !ctrlPressed && shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].height -= gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_DOWN && !ctrlPressed && shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].height += gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_LEFT && !ctrlPressed && shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].width -= gridSize; 
        }
    }
    if(keyArgs.key == OF_KEY_RIGHT && !ctrlPressed && shiftPressed && !altPressed){
        if(monitorDataList[monitorSelected].tvFocus > 0){
            tvDataList[monitorDataList[monitorSelected].tvFocus-1].width += gridSize; 
        }
    }
    // set focus and grid with alt + arrow keys
        int tvStart = 1;
        int tvEnd = numberTVs;
        if(monitorFocus){
            tvStart = monitorDataList[monitorSelected].tvFirst;
            tvEnd = MIN(tvStart + monitorDataList[monitorSelected].tvNumber - 1, numberTVs);
        }
    if(keyArgs.key == OF_KEY_RIGHT && !ctrlPressed && !shiftPressed && altPressed){
        if(monitorDataList[monitorSelected].tvFocus >= tvEnd){monitorDataList[monitorSelected].tvFocus = tvStart;}
        else{ monitorDataList[monitorSelected].tvFocus++;}
    }
    if(keyArgs.key == OF_KEY_LEFT && !ctrlPressed && !shiftPressed && altPressed){
        if(monitorDataList[monitorSelected].tvFocus <= tvStart){monitorDataList[monitorSelected].tvFocus = tvEnd;}
        else{ monitorDataList[monitorSelected].tvFocus--;}
    }
    if(keyArgs.key == OF_KEY_DOWN && !ctrlPressed && !shiftPressed && altPressed){
        if(grid_index == IM_ARRAYSIZE(grid_options) - 1){ grid_index = 0; }
        else{ grid_index++; }
    }
    if(keyArgs.key == OF_KEY_UP && !ctrlPressed && !shiftPressed && altPressed){
        if(grid_index == 0){ grid_index = IM_ARRAYSIZE(grid_options) - 1; }
        else{ grid_index--; }
    }

    if (keyArgs.key == 'f' || keyArgs.key == 'F') {
        toggleFullscreen();
    }
    if (keyArgs.key == 'r' || keyArgs.key == 'R') {
        showFramerate = !showFramerate;
    }
    if (keyArgs.key == 'a' || keyArgs.key == 'A') {
        ofLog() << "width: " << monitorDataList[0].window->getWidth() << "height: " << monitorDataList[0].window->getHeight();
        ofLog() << "position: " << monitorDataList[0].window->getWindowPosition();
        ofLog() << "window size: " << monitorDataList[0].window->getWindowSize();
        ofLog() << "screen size: " << monitorDataList[0].window->getScreenSize();
    }
}

void ofApp::keyReleased(ofKeyEventArgs& keyArgs) {
    // Check if Ctrl key is released
    if (keyArgs.key == OF_KEY_CONTROL) {
        ctrlPressed = false;
    }

    // Check if Shift key is released
    if (keyArgs.key == OF_KEY_SHIFT) {
        shiftPressed = false;
    }

    // Check if Alt key is released
    if (keyArgs.key == OF_KEY_ALT) {
        altPressed = false;
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if(button == 1){
        toggleFullscreen();
        return;
    }
    if(hideMaptest){return;}
    ofRectangle fboBounds(600, 20, ofGetScreenWidth() / 3, ofGetScreenHeight() / 3);
    if (!fboBounds.inside(x, y)) {
        return; 
        }
    float max_canvas = static_cast<float>MAX(canvasWidth, canvasHeight);
    float xPos = static_cast<float>(x - 600) / (ofGetScreenWidth() / 3) * max_canvas;
    float yPos = static_cast<float>(y - 20) / (ofGetScreenHeight() / 3) * max_canvas;
    int tvStart = 1;
    int tvEnd = numberTVs;
    if(monitorFocus){
        tvStart = monitorDataList[monitorSelected].tvFirst;
        tvEnd = MIN(tvStart + monitorDataList[monitorSelected].tvNumber - 1, numberTVs);
    }
    bool isFocused = false;
    for (int i = tvEnd; i >= tvStart; i--) {
        ofRectangle tvBounds(tvDataList[i-1].xPos, tvDataList[i-1].yPos, tvDataList[i-1].width, tvDataList[i-1].height);
        if (tvBounds.inside(xPos, yPos)){
            monitorDataList[monitorSelected].tvFocus = i;
            isFocused = true;
            break;
        }
    }
    if(!isFocused && !shiftPressed){monitorDataList[monitorSelected].tvFocus = 0;}
    lastMouseXPos = xPos - tvDataList[monitorDataList[monitorSelected].tvFocus-1].xPos;
    lastMouseYPos = yPos - tvDataList[monitorDataList[monitorSelected].tvFocus-1].yPos;
}

void ofApp::mouseDragged(int x, int y, int button) {
    if(hideMaptest){return;}
    float xPos = static_cast<float>(x - 600) / (ofGetScreenWidth() / 3) * canvasWidth;
    float yPos = static_cast<float>(y - 20) / (ofGetScreenHeight() / 3) * canvasHeight;

    if(!(shiftPressed || button == 2)){
        tvDataList[monitorDataList[monitorSelected].tvFocus-1].xPos = (xPos - lastMouseXPos);
        tvDataList[monitorDataList[monitorSelected].tvFocus-1].yPos = (yPos - lastMouseYPos);
    }
    else if(shiftPressed || button == 2){
        tvDataList[monitorDataList[monitorSelected].tvFocus-1].width = xPos - tvDataList[monitorDataList[monitorSelected].tvFocus-1].xPos;
        tvDataList[monitorDataList[monitorSelected].tvFocus-1].height = yPos - tvDataList[monitorDataList[monitorSelected].tvFocus-1].yPos;
    }

}

void ofApp::toggleFullscreen(){
    monitorDataList[monitorSelected].isFullscreen = !monitorDataList[monitorSelected].isFullscreen;
    if(monitorDataList[monitorSelected].outputToMonitor){
            monitorDataList[monitorSelected].window->setFullscreen(monitorDataList[monitorSelected].isFullscreen);
    }
    else if(numberMonitors == 1 && !monitorDataList[monitorSelected].outputToMonitor){
        ofSetFullscreen(monitorDataList[monitorSelected].isFullscreen);
    }
}

void ofApp::jsonLoad(string path){
    if (json.open(path)) {
        // Access configuration values
        hideMaptest = json["hide_maptest"].asBool();
        hidePreview = json["hide_preview"].asBool();
        setResolutions = json["set_resolutions"].asBool();
        inputWidth = json["input_width"].asInt();
        inputHeight = json["input_height"].asInt();
        framerate = json["framerate"].asInt();
        numberTVs = json["number_tvs"].asInt();
        selectedInputIndex = json["selected_input_index"].asInt();
        canvasWidth = json["canvas_width"].asInt();
        canvasHeight = json["canvas_height"].asInt();
        numberMonitors = json["number_monitors"].asInt();
        monitorSelected = json["monitor_selected"].asInt();
        monitorFocus = json["monitor_focus"].asBool();
        // Load monitor_data
        const ofxJSONElement& monitorDataJson = json["monitor_data"];
        for (int i = 0; i < monitorDataJson.size(); i++) {
            monitorData thisMonitor;
            thisMonitor.outputToMonitor = monitorDataJson[i]["output_to_monitor"].asBool();
            thisMonitor.outputToNDI = monitorDataJson[i]["output_to_ndi"].asBool();
            thisMonitor.isFullscreen = monitorDataJson[i]["is_fullscreen"].asBool();
            thisMonitor.displayIndex = monitorDataJson[i]["display_index"].asInt();
            thisMonitor.tvNumber = monitorDataJson[i]["tv_number"].asInt();
            thisMonitor.tvFirst = monitorDataJson[i]["tv_first"].asInt();
            thisMonitor.tvLayout = monitorDataJson[i]["tv_layout"].asString();
            monitorDataList.push_back(thisMonitor);
        }
        // Load tv_data values
        const ofxJSONElement& tvMappings = json["tv_mappings"];
        for (int i = 0; i < tvMappings.size(); i++) {
            tvData thisTv;
            thisTv.width = tvMappings[i]["width"].asInt();
            thisTv.height = tvMappings[i]["height"].asInt();
            thisTv.xPos = tvMappings[i]["x_pos"].asInt();
            thisTv.yPos = tvMappings[i]["y_pos"].asInt();
            tvDataList.push_back(thisTv);
        }
    }

}

void ofApp::jsonSave(){
        json["hide_maptest"] = hideMaptest;
        json["hide_preview"] = hidePreview;
        json["set_resolutions"] = setResolutions;
        json["input_width"] = inputWidth;
        json["input_height"] = inputHeight;
        json["framerate"] = framerate;
        json["number_tvs"] = numberTVs;
        json["selected_input_index"] = selectedInputIndex;
        json["canvas_width"] = canvasWidth;
        json["canvas_height"] = canvasHeight;
        json["number_monitors"] = numberMonitors;
        json["monitor_selected"] = monitorSelected;
        json["monitor_focus"] = monitorFocus;
        
        json["monitor_data"] = {};
        for (int i = 0; i < monitorDataList.size(); i++) {
            json["monitor_data"][i]["output_to_monitor"] = monitorDataList[i].outputToMonitor;
            json["monitor_data"][i]["output_to_ndi"] = monitorDataList[i].outputToNDI;
            json["monitor_data"][i]["is_fullscreen"] = monitorDataList[i].isFullscreen;
            json["monitor_data"][i]["display_index"] = monitorDataList[i].displayIndex;
            json["monitor_data"][i]["tv_number"] = monitorDataList[i].tvNumber;
            json["monitor_data"][i]["tv_first"] = monitorDataList[i].tvFirst;
            json["monitor_data"][i]["tv_layout"] = monitorDataList[i].tvLayout;
        }
        json["tv_mappings"] = {};
        for (int i = 0; i < tvDataList.size(); i++) {
            json["tv_mappings"][i]["width"] = tvDataList[i].width;
            json["tv_mappings"][i]["height"] = tvDataList[i].height;
            json["tv_mappings"][i]["x_pos"] = tvDataList[i].xPos;
            json["tv_mappings"][i]["y_pos"] = tvDataList[i].yPos;
        }

        json.save("config.json", true);
}

void ofApp::exit(){
    ofLog() << "called on exit now ..";
    
    for (int i = 0; i < monitorDataList.size(); ++i) {
        if(monitorDataList[i].outputToMonitor){
            monitorDataList[i].window->setWindowShouldClose();
        }
    }
    ofSleepMillis(100);
    ofLog() << "got to here...";
}