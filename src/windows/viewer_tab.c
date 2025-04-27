#include "windows/viewer_tab.h"

// initialize a tab
ViewerTab* ViewerTab_init(const size_t lines){
    log_message("ViewerTab: creating blank tab.");
    ViewerTab* vt = malloc(sizeof(ViewerTab));
    if (vt == NULL) {
        log_error("ViewerTab: failed to allocate memory for viewer tab.");
        return NULL;
    }
    vt->header = NULL;
    vt->header_len = 0;
    vt->lines = lines;
    vt->highlight = 0;
    vt->page_prev = NULL;
    vt->page_current = NULL;
    vt->page_next = NULL;
    log_message("ViewerTab: OK.");
    return vt;
}

// populate a tab with data
void ViewerTab_populate_tab(
    ViewerTab** tabptr,
    const size_t lines,
    const char* table_name,
    const char* fetch_query)
{
    log_message("ViewerTab: populating tab with data from table '%s'.", table_name);
    if(*tabptr == NULL) *tabptr = ViewerTab_init(lines);
    // tabptr->label = NULL;
    // tabptr->label_len = 0;
    // tabptr->page_prev = NULL;
    TableData* helper = fetch_table_chunk(__db, table_name, fetch_query, 0, lines-1);
    if (helper == NULL) {
        log_error("ViewerTab: failed to fetch middle page.");
        return;
    }
    (*tabptr)->page_current = helper;

    helper = fetch_table_chunk(__db, table_name, fetch_query, lines, lines);
    if (helper == NULL) {
        log_error("ViewerTab: failed to fetch next page.");
        return;
    }
    (*tabptr)->page_next = helper;

    log_message("ViewerTab: OK.");
}

// destroy a tab
void ViewerTab_destroy(ViewerTab** tabptr){
    log_message("ViewerTab: destroying tab.");
    if(*tabptr == NULL) {
        log_warning("ViewerTab: nothing to destroy.");
        return;
    }

    if((*tabptr)->header) free((*tabptr)->header);
    if((*tabptr)->page_prev) free_table((*tabptr)->page_prev);
    if((*tabptr)->page_current) free_table((*tabptr)->page_current);
    if((*tabptr)->page_next) free_table((*tabptr)->page_next);
    
    free(*tabptr);
    *tabptr = NULL;
    log_message("ViewerTab: OK.");
}

ViewerData* ViewerData_init(){
    log_message("ViewerData: allocating memory for viewer window data.");
    ViewerData* vd = malloc(sizeof(ViewerData));
    vd->on_focus = MENU_NONE;
    for(size_t i = 0; i < APP_MENU_COUNT; ++i) vd->tabs[i] = NULL;
    log_message("ViewerData: OK.");
    return vd;
}

void ViewerData_destroy(ViewerData** dataptr){
    log_message("ViewerData: destroying viewer window data.");
    for(size_t i = 0; i < APP_MENU_COUNT; ++i) ViewerTab_destroy(&(*dataptr)->tabs[i]);
    free(*dataptr);
    *dataptr = NULL;
    log_message("ViewerData: OK.");
}