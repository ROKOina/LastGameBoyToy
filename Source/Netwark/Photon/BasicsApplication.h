#pragma once

class UIListener;

class BasicsApplication
{
public:
	BasicsApplication::BasicsApplication(UIListener* listener);
	//Launch main loop 
	//this function takes uwnership of listener
	void run(/*UIListener* listener*/);
	void close();

	void ImGui();
private:
};