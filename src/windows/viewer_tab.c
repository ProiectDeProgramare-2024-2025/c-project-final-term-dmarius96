#include "windows/viewer_tab.h"

// initialize a tab
ViewerTab* ViewerTab_init(const size_t columns, const size_t lines){
    ViewerTab* vt = malloc(sizeof(ViewerTab));
    vt->label = NULL;
    vt->label_len = 0;
    vt->columns = columns;
    vt->lines = lines;
    vt->page_prev = NULL;
    vt->page_current = NULL;
    vt->page_next = NULL;
    return vt;
}

// populate a tab with data
void ViewerTab_populate_tab(
    ViewerTab** tabptr,
    const size_t columns,
    const size_t lines,
    const char* table_name,
    const char* fetch_query)
{
    if(*tabptr == NULL) *tabptr = ViewerTab_init(columns, lines);
    // tabptr->label = NULL;
    // tabptr->label_len = 0;
    // tabptr->page_prev = NULL;
    (*tabptr)->page_current = fetch_table_chunk(__db, table_name, fetch_query, 0, lines-1);
    (*tabptr)->page_next = fetch_table_chunk(__db, table_name, fetch_query, lines, lines);
}

// destroy a tab
void ViewerTab_destroy(ViewerTab** tabptr){
    if(*tabptr == NULL) return;

    if((*tabptr)->label) free((*tabptr)->label);
    if((*tabptr)->page_prev) free_table((*tabptr)->page_prev);
    if((*tabptr)->page_current) free_table((*tabptr)->page_current);
    if((*tabptr)->page_next) free_table((*tabptr)->page_next);
    
    free(*tabptr);
    *tabptr = NULL; 
}

ViewerData* ViewerData_init(){
    ViewerData* vd = malloc(sizeof(ViewerData));
    vd->on_focus = MENU_NONE;
    for(size_t i = 0; i < APP_MENU_COUNT; ++i) vd->tabs[i] = NULL;
    return vd;
}

void ViewerData_destroy(ViewerData** dataptr){
    for(size_t i = 0; i < APP_MENU_COUNT; ++i) ViewerTab_destroy(&(*dataptr)->tabs[i]);
    free(*dataptr);
    *dataptr = NULL;
}