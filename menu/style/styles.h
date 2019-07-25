#pragma once

class WindowStyle {
	public:
	// Draws a framed control area for dialogs
	virtual void Dialog( int x, int y, int w, int h ) = 0;
	// Draws a framed control area for small panels within another control area
	// - Returns the height of the namebar
	virtual int ControlPanel( int x, int y, int w, int h, const char* name = nullptr ) = 0;
	// Draws a button made to open dialogs
	// - Returns the height of the box
	virtual int DialogButton( int x, int y, int w, const char* text = nullptr, bool mouseOver = false ) = 0;

	// Draws a window top bar
	// - Returns the height of the bar
	virtual int TopBar( int x, int y, int w, const char* title = nullptr ) = 0;

};

class DefaultStyle : public WindowStyle {
	public:
	void Dialog( int x, int y, int w, int h );
	int ControlPanel( int x, int y, int w, int h, const char* name = nullptr );
	int DialogButton( int x, int y, int w, const char* text, bool mouseOver = false );

	int TopBar( int x, int y, int w, const char* title = nullptr );
};