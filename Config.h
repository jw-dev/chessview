#ifndef CONFIG_H
#define CONFIG_H

namespace Config 
    {
    // width in pixels of main window
    static const int WindowWidth = 480;
    // height in pixels of main window
    static const int WindowHeight = 480;
    // title of main window
    static const char* WindowTitle = "chessview";
    // milliseconds to wait after moves when watching two bots
    static const int AfterMoveDelay = 300; 
    }

#endif 