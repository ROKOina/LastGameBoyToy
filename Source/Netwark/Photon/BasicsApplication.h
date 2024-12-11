#pragma once

class UIListener;
class PhotonLib;

class BasicsApplication
{
public:
    BasicsApplication::BasicsApplication(UIListener* listener);
    //Launch main loop
    //this function takes uwnership of listener
    void run(float elapsedTime);
    void close();

    void ImGui();

    PhotonLib* GetPhotonLib();
private:
};