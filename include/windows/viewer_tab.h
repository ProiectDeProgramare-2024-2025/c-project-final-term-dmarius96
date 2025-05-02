#ifndef APP_VIEWER_TAB_H
#define APP_VIEWER_TAB_H

#include "windows_viewer.h"
#include "table_struct.h"

typedef struct {
    char* header;
    size_t header_len;
    size_t lines;
    size_t highlight;
    size_t offset;
    TableData* page_prev;
    TableData* page_current;
    TableData* page_next;
} ViewerTab;

// Win->userdata
typedef struct {
    ViewerTab* tabs[APP_MENU_COUNT];
    MenuType on_focus;
} ViewerData;

// initialize a tab
ViewerTab* ViewerTab_init(size_t lines);

// populate a tab with data
void ViewerTab_populate_tab(
    ViewerTab** tabptr,
    size_t lines,
    const char* table_name,
    const char* fetch_query
);

// destroy a tab
void ViewerTab_destroy(ViewerTab** tabptr);

ViewerData* ViewerData_init();

void ViewerData_destroy(ViewerData** dataptr);

#endif