#ifndef __GUI_H__
#define __GUI_H__


namespace xap {


/// Initialize GUI menus
void initGui();


/// Enable or disable menus depending on server status
void enableMenus(bool serverRunning);


/// Destroy GUI objects
void doneGui();

};


#endif

