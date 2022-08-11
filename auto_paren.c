#include <yed/plugin.h>

static int do_delete_match;
static void completer_auto_match_buff_post_insert_handler(yed_event *event);
static void completer_auto_match_buff_pre_insert_handler(yed_event *event);
static void remover_auto_match_buff_pre_delete_back_handler(yed_event *event);
static void remover_auto_match_buff_post_delete_back_handler(yed_event *event);
static int  selection_insert(yed_event *event);

static void _quote_insert_selection(int n_args, char **args);
static void _dquote_insert_selection(int n_args, char **args);
static void _brace_insert_selection(int n_args, char **args);
static void _paren_insert_selection(int n_args, char **args);


int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler h;

    YED_PLUG_VERSION_CHECK();

    h.kind = EVENT_BUFFER_PRE_INSERT;
    h.fn   = completer_auto_match_buff_pre_insert_handler;
    yed_plugin_add_event_handler(self, h);

    h.kind = EVENT_BUFFER_POST_INSERT;
    h.fn   = completer_auto_match_buff_post_insert_handler;
    yed_plugin_add_event_handler(self, h);

    h.kind = EVENT_BUFFER_PRE_DELETE_BACK;
    h.fn   = remover_auto_match_buff_pre_delete_back_handler;
    yed_plugin_add_event_handler(self, h);

    h.kind = EVENT_BUFFER_POST_DELETE_BACK;
    h.fn   = remover_auto_match_buff_post_delete_back_handler;
    yed_plugin_add_event_handler(self, h);

    //Skipping over right hand symbol when pressed
    if(yed_get_var("auto-paren-skip") == NULL) {
        yed_set_var("auto-paren-skip", "no");
    }
    if(yed_get_var("auto-quote-skip") == NULL) {
        yed_set_var("auto-quote-skip", "no");
    }
    if(yed_get_var("auto-dquote-skip") == NULL) {
        yed_set_var("auto-dquote-skip", "no");
    }

    //Moving right hand symbol to the other side of word on right
    if(yed_get_var("auto-paren-jump-word") == NULL) {
        yed_set_var("auto-paren-jump-word", "yes");
    }
    if(yed_get_var("auto-dquote-jump-word") == NULL) {
        yed_set_var("auto-dquote-jump-word", "yes");
    }
    if(yed_get_var("auto-quote-jump-word") == NULL) {
        yed_set_var("auto-quote-jump-word", "yes");
    }
    if(yed_get_var("auto-dquote-selection") == NULL) {
        yed_set_var("auto-dquote-selection", "yes");
    }
    if(yed_get_var("auto-quote-selection") == NULL) {
        yed_set_var("auto-quote-selection", "yes");
    }
    if(yed_get_var("auto-paren-selection") == NULL) {
        yed_set_var("auto-paren-selection", "yes");
    }
    if(yed_get_var("auto-brace-selection") == NULL) {
        yed_set_var("auto-brace-selection", "yes");
    }

    yed_plugin_set_command(self, "quote_selection",_quote_insert_selection);
    yed_plugin_set_command(self, "dquote_selection",_dquote_insert_selection);
    yed_plugin_set_command(self, "brace_selection",_brace_insert_selection);
    yed_plugin_set_command(self, "paren_selection",_paren_insert_selection);

    return 0;
}

static void _quote_insert_selection(int n_args, char **args) {
    yed_frame *frame;
    int        save_col_left;
    int        save_col_right;
    int        save_row;
    char       match_left  = 0;
    char       match_right = 0;

    if ( !ys->active_frame ) {
        return;
    }

    frame = ys->active_frame;

    if ( !frame->buffer ) {
        return;
    }

    if ( !yed_var_is_truthy("disable-auto-quote") ) {
        if (frame->buffer
        && frame->buffer->has_selection
        && frame->buffer->selection.kind == RANGE_NORMAL
        && frame->buffer->selection.anchor_row == frame->buffer->selection.cursor_row) {
            match_left  = '\'';
            match_right = '\'';
            if (frame->buffer->selection.anchor_col < frame->buffer->selection.cursor_col) {
                save_col_left  = frame->buffer->selection.anchor_col;
                save_col_right = frame->buffer->selection.cursor_col;
            } else {
                save_col_left  = frame->buffer->selection.cursor_col;
                save_col_right = frame->buffer->selection.anchor_col;
            }
        }
    }

    if ( match_left == 0 || match_right == 0) return;

    save_row = frame->buffer->selection.anchor_row;
    if(save_col_left <= 1 || save_col_right <= 1) {
        return;
    }

    yed_start_undo_record(frame, frame->buffer);
    yed_insert_into_line(frame->buffer, save_row, save_col_right, G(match_right));
    yed_insert_into_line(frame->buffer, save_row, save_col_left, G(match_left));
    yed_end_undo_record(frame, frame->buffer);
}

static void _dquote_insert_selection(int n_args, char **args) {
    yed_frame *frame;
    int        save_col_left;
    int        save_col_right;
    int        save_row;
    char       match_left  = 0;
    char       match_right = 0;

    if ( !ys->active_frame ) {
        return;
    }

    frame = ys->active_frame;

    if ( !frame->buffer ) {
        return;
    }

    if ( !yed_var_is_truthy("disable-auto-dquote") ) {
        if (frame->buffer
        && frame->buffer->has_selection
        && frame->buffer->selection.kind == RANGE_NORMAL
        && frame->buffer->selection.anchor_row == frame->buffer->selection.cursor_row) {
            match_left  = '"';
            match_right = '"';
            if (frame->buffer->selection.anchor_col < frame->buffer->selection.cursor_col) {
                save_col_left  = frame->buffer->selection.anchor_col;
                save_col_right = frame->buffer->selection.cursor_col;
            } else {
                save_col_left  = frame->buffer->selection.cursor_col;
                save_col_right = frame->buffer->selection.anchor_col;
            }
        }
    }

    if ( match_left == 0 || match_right == 0) return;

    save_row = frame->buffer->selection.anchor_row;
    if(save_col_left <= 1 || save_col_right <= 1) {
        return;
    }

    yed_start_undo_record(frame, frame->buffer);
    yed_insert_into_line(frame->buffer, save_row, save_col_right, G(match_right));
    yed_insert_into_line(frame->buffer, save_row, save_col_left, G(match_left));
    yed_end_undo_record(frame, frame->buffer);
}

static void _brace_insert_selection(int n_args, char **args) {
    yed_frame *frame;
    int        save_col_left;
    int        save_col_right;
    int        save_row;
    char       match_left  = 0;
    char       match_right = 0;

    if ( !ys->active_frame ) {
        return;
    }

    frame = ys->active_frame;

    if ( !frame->buffer ) {
        return;
    }

    if ( !yed_var_is_truthy("disable-auto-brace") ) {
        if (frame->buffer
        && frame->buffer->has_selection
        && frame->buffer->selection.kind == RANGE_NORMAL
        && frame->buffer->selection.anchor_row == frame->buffer->selection.cursor_row) {
            match_left  = '[';
            match_right = ']';
            if (frame->buffer->selection.anchor_col < frame->buffer->selection.cursor_col) {
                save_col_left  = frame->buffer->selection.anchor_col;
                save_col_right = frame->buffer->selection.cursor_col;
            } else {
                save_col_left  = frame->buffer->selection.cursor_col;
                save_col_right = frame->buffer->selection.anchor_col;
            }
        }
    }

    if ( match_left == 0 || match_right == 0) return;

    save_row = frame->buffer->selection.anchor_row;
    if(save_col_left <= 1 || save_col_right <= 1) {
        return;
    }

    yed_start_undo_record(frame, frame->buffer);
    yed_insert_into_line(frame->buffer, save_row, save_col_right, G(match_right));
    yed_insert_into_line(frame->buffer, save_row, save_col_left, G(match_left));
    yed_end_undo_record(frame, frame->buffer);
}

static void _paren_insert_selection(int n_args, char **args) {
    yed_frame *frame;
    int        save_col_left;
    int        save_col_right;
    int        save_row;
    char       match_left  = 0;
    char       match_right = 0;

    if ( !ys->active_frame ) {
        return;
    }

    frame = ys->active_frame;

    if ( !frame->buffer ) {
        return;
    }

    if ( !yed_var_is_truthy("disable-auto-paren") ) {
        if (frame->buffer
        && frame->buffer->has_selection
        && frame->buffer->selection.kind == RANGE_NORMAL
        && frame->buffer->selection.anchor_row == frame->buffer->selection.cursor_row) {
            match_left  = '(';
            match_right = ')';
            if (frame->buffer->selection.anchor_col < frame->buffer->selection.cursor_col) {
                save_col_left  = frame->buffer->selection.anchor_col;
                save_col_right = frame->buffer->selection.cursor_col;
            } else {
                save_col_left  = frame->buffer->selection.cursor_col;
                save_col_right = frame->buffer->selection.anchor_col;
            }
        }
    }

    if ( match_left == 0 || match_right == 0) return;

    save_row = frame->buffer->selection.anchor_row;
    if(save_col_left <= 1 || save_col_right <= 1) {
        return;
    }

    yed_start_undo_record(frame, frame->buffer);
    yed_insert_into_line(frame->buffer, save_row, save_col_right, G(match_right));
    yed_insert_into_line(frame->buffer, save_row, save_col_left, G(match_left));
    yed_end_undo_record(frame, frame->buffer);
}

int selection_insert(yed_event *event) {
    yed_frame *frame;
    int        save_col_left;
    int        save_col_right;
    int        save_row;
    char       match_left  = 0;
    char       match_right = 0;

    if ( !event->frame ) {
        return 0;
    }

    if ( !event->frame->buffer ) {
        return 0;
    }

    if ( event->key == '\'' ) {
        if ( !yed_var_is_truthy("disable-auto-quote") ) {
            if (yed_var_is_truthy("auto-quote-selection")
            && event->buffer
            && event->buffer->has_selection
            && event->buffer->selection.kind == RANGE_NORMAL
            && event->buffer->selection.anchor_row == event->buffer->selection.cursor_row) {
                match_left  = '\'';
                match_right = '\'';
                if (event->buffer->selection.anchor_col < event->buffer->selection.cursor_col) {
                    save_col_left  = event->buffer->selection.anchor_col;
                    save_col_right = event->buffer->selection.cursor_col;
                } else {
                    save_col_left  = event->buffer->selection.cursor_col;
                    save_col_right = event->buffer->selection.anchor_col;
                }
            }
        }
    } else if ( event->key == '"' ) {
        if ( !yed_var_is_truthy("disable-auto-dquote") ) {
            if (yed_var_is_truthy("auto-dquote-selection")
            && event->buffer
            && event->buffer->has_selection
            && event->buffer->selection.kind == RANGE_NORMAL
            && event->buffer->selection.anchor_row == event->buffer->selection.cursor_row) {
                match_left  = '"';
                match_right = '"';
                if (event->buffer->selection.anchor_col < event->buffer->selection.cursor_col) {
                    save_col_left  = event->buffer->selection.anchor_col;
                    save_col_right = event->buffer->selection.cursor_col;
                } else {
                    save_col_left  = event->buffer->selection.cursor_col;
                    save_col_right = event->buffer->selection.anchor_col;
                }
            }
        }
    } else if ( event->key == '(' ) {
        if ( !yed_var_is_truthy("disable-auto-paren") ) {
            if (yed_var_is_truthy("auto-paren-selection")
            && event->buffer
            && event->buffer->has_selection
            && event->buffer->selection.kind == RANGE_NORMAL
            && event->buffer->selection.anchor_row == event->buffer->selection.cursor_row) {
                match_left  = '(';
                match_right = ')';
                if (event->buffer->selection.anchor_col < event->buffer->selection.cursor_col) {
                    save_col_left  = event->buffer->selection.anchor_col;
                    save_col_right = event->buffer->selection.cursor_col;
                } else {
                    save_col_left  = event->buffer->selection.cursor_col;
                    save_col_right = event->buffer->selection.anchor_col;
                }
            }
        }
    } else if ( event->key == '[' ) {
        if ( !yed_var_is_truthy("disable-auto-brace") ) {
            if (yed_var_is_truthy("auto-brace-selection")
            && event->buffer
            && event->buffer->has_selection
            && event->buffer->selection.kind == RANGE_NORMAL
            && event->buffer->selection.anchor_row == event->buffer->selection.cursor_row) {
                match_left  = '[';
                match_right = ']';
                if (event->buffer->selection.anchor_col < event->buffer->selection.cursor_col) {
                    save_col_left  = event->buffer->selection.anchor_col;
                    save_col_right = event->buffer->selection.cursor_col;
                } else {
                    save_col_left  = event->buffer->selection.cursor_col;
                    save_col_right = event->buffer->selection.anchor_col;
                }
            }
        }
    }

    if ( match_left == 0 || match_right == 0) return 0;

    frame = event->frame;

    save_row = event->buffer->selection.anchor_row;
    if(save_col_left <= 1 || save_col_right <= 1) {
        return 0;
    }

    yed_start_undo_record(frame, frame->buffer);
    yed_insert_into_line(frame->buffer, save_row, save_col_right, G(match_right));
    yed_insert_into_line(frame->buffer, save_row, save_col_left, G(match_left));
    yed_end_undo_record(frame, frame->buffer);
}

void completer_auto_match_buff_pre_insert_handler(yed_event *event) {
    yed_frame *frame;
    int        save_col;
    int        save_row;
    char       match = 0;
    char       key_first;
    char       key_second;
    char       key_third;
    char       tmp;
    yed_line  *line;
    int        i;
    yed_glyph *g;

    if (selection_insert(event)) {
        event->cancel = 1;
        return;
    }

    if ( !event->frame
    ||   !event->frame->buffer
    ||   event->frame->buffer->kind != BUFF_KIND_FILE
    ||   event->frame->buffer->has_selection) {
        return;
    }

    frame = event->frame;
    save_col = frame->cursor_col;
    save_row = frame->cursor_line;

    line = yed_buff_get_line(event->frame->buffer, save_row);
    if(!line) { return; }

    if ( save_col <= 1 ) {
        return;
    }

    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col-1);
    if (!g) { return; }
    key_first = g->c;

    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col);
    if (!g) { return; }
    key_second = g->c;

    i=0;
    if ( event->key == ')' && key_second == ')' && (yed_var_is_truthy("auto-paren-skip") || key_first == '(')) {
        g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
        if (!g) {
            yed_append_to_line(event->frame->buffer, save_row, G(' '));
            g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
            if (!g) {
                return;
            }
        }
        key_third = save_col >= line->visual_width+1?0:g->c;
        if((isalnum(key_third) || key_third == '_') && (yed_var_is_truthy("auto-paren-jump-word"))) {
            tmp = key_third;
            while(((save_col+i) < line->visual_width+1) && (isalnum(tmp) || tmp == '_')) {
                i++;
                g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                if (!g) {
                    yed_append_to_line(event->frame->buffer, save_row, G(' '));
                    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                    if (!g) {
                        return;
                    }
                }
                tmp = g->c;
            }
            event->cancel = 1;
            yed_move_cursor_within_frame(frame, 0, i);
            yed_buff_insert_string(event->frame->buffer, ")", save_row, save_col+i);
            yed_delete_from_line(event->frame->buffer, save_row, save_col);
        }else{
            if ( !yed_var_is_truthy("disable-auto-paren") ) {
                event->cancel = 1;
                yed_move_cursor_within_frame(frame, 0, 1);
            }
        }
    } else if ( event->key == ']' && key_second == ']' && (yed_var_is_truthy("auto-paren-skip") || key_first == '[') ) {
        g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
        if (!g) {
            yed_append_to_line(event->frame->buffer, save_row, G(' '));
            g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
            if (!g) {
                return;
            }
        }
        key_third = save_col >= line->visual_width+1?0:g->c;
        if((isalnum(key_third) || key_third == '_') && (yed_var_is_truthy("auto-paren-jump-word"))) {
            tmp = key_third;
            while(((save_col+i) < line->visual_width+1) && (isalnum(tmp) || tmp == '_')) {
                i++;
                g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                if (!g) {
                    yed_append_to_line(event->frame->buffer, save_row, G(' '));
                    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                    if (!g) {
                        return;
                    }
                }
                tmp = g->c;
            }
            event->cancel = 1;
            yed_move_cursor_within_frame(frame, 0, i);
            yed_buff_insert_string(event->frame->buffer, "]", save_row, save_col+i);
            yed_delete_from_line(event->frame->buffer, save_row, save_col);
        }else{
            if ( !yed_var_is_truthy("disable-auto-bracket") ) {
                event->cancel = 1;
                yed_move_cursor_within_frame(frame, 0, 1);
            }
        }
    } else if ( event->key == '}' && key_second == '}' && (yed_var_is_truthy("auto-paren-skip") || key_first == '{')) {
        g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
        if (!g) {
            yed_append_to_line(event->frame->buffer, save_row, G(' '));
            g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
            if (!g) {
                return;
            }
        }
        key_third = save_col >= line->visual_width+1?0:g->c;
        if((isalnum(key_third) || key_third == '_') && (yed_var_is_truthy("auto-paren-jump-word"))) {
            tmp = key_third;
            while(((save_col+i) < line->visual_width+1) && (isalnum(tmp) || tmp == '_')) {
                i++;
                g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                if (!g) {
                    yed_append_to_line(event->frame->buffer, save_row, G(' '));
                    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                    if (!g) {
                        return;
                    }
                }
                tmp = g->c;
            }
            event->cancel = 1;
            yed_move_cursor_within_frame(frame, 0, i);
            yed_buff_insert_string(event->frame->buffer, "}", save_row, save_col+i);
            yed_delete_from_line(event->frame->buffer, save_row, save_col);
        }else{
            if ( !yed_var_is_truthy("disable-auto-brace") ) {
                event->cancel = 1;
                yed_move_cursor_within_frame(frame, 0, 1);
            }
        }
    } else if ( event->key == '"' && key_second == '"' && (yed_var_is_truthy("auto-dquote-skip") || key_first =='"')) {
        g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
        if (!g) {
            yed_append_to_line(event->frame->buffer, save_row, G(' '));
            g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
            if (!g) {
                return;
            }
        }
        key_third = save_col >= line->visual_width+1?0:g->c;
        if((isalnum(key_third) || key_third == '_') && (yed_var_is_truthy("auto-dquote-jump-word")) && key_first == '"' ) {
            tmp = key_third;
            while(((save_col+i) < line->visual_width+1) && (isalnum(tmp) || tmp == '_')) {
                i++;
                g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                if (!g) {
                    yed_append_to_line(event->frame->buffer, save_row, G(' '));
                    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                    if (!g) {
                        return;
                    }
                }
                tmp = g->c;
            }
            event->cancel = 1;
            yed_move_cursor_within_frame(frame, 0, i);
            yed_buff_insert_string(event->frame->buffer, "\"", save_row, save_col+i);
            yed_delete_from_line(event->frame->buffer, save_row, save_col);
        }else{
            if ( !yed_var_is_truthy("disable-auto-dquote") ) {
                event->cancel = 1;
                yed_move_cursor_within_frame(frame, 0, 1);
            }
        }
    } else if ( event->key == '\'' && key_second == '\'' && (yed_var_is_truthy("auto-quote-skip") || key_first == '\'')) {
        g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
        if (!g) {
            yed_append_to_line(event->frame->buffer, save_row, G(' '));
            g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+1);
            if (!g) {
                return;
            }
        }
        key_third = save_col >= line->visual_width+1?0:g->c;
        if((isalnum(key_third) || key_third == '_') && (yed_var_is_truthy("auto-quote-jump-word")) && key_first == '\'') {
            tmp = key_third;
            while(((save_col+i) < line->visual_width+1) && (isalnum(tmp) || tmp == '_')) {
                i++;
                g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                if (!g) {
                    yed_append_to_line(event->frame->buffer, save_row, G(' '));
                    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col+i);
                    if (!g) {
                        return;
                    }
                }
                tmp = g->c;
            }
            event->cancel = 1;
            yed_move_cursor_within_frame(frame, 0, i);
            yed_buff_insert_string(event->frame->buffer, "\'", save_row, save_col+i);
            yed_delete_from_line(event->frame->buffer, save_row, save_col);
        }else{
            if ( !yed_var_is_truthy("disable-auto-quote") ) {
                event->cancel = 1;
                yed_move_cursor_within_frame(frame, 0, 1);
            }
        }
    }

    if (event->key == ENTER && key_first == '{' &&  key_second == '}') {
        int       i, j, brace_col, tabw, idx, len;
        yed_glyph  *git;

        tabw = yed_get_tab_width();

        if (tabw <= 0) {
            return;
        }

        brace_col = frame->cursor_col - 1;


        i = 0;

        while (i < brace_col
        &&     yed_line_col_to_glyph(line, i + 1)->c == ' ') {
            i += 1;
        }

        yed_buff_insert_line(frame->buffer, save_row+1);
        yed_buff_insert_line(frame->buffer, save_row+2);

        for (j=0; j<i; j++){
            yed_append_to_line(frame->buffer, save_row+2, G(' '));
        }
        len = 0;

        line = yed_buff_get_line(frame->buffer, frame->cursor_line);
        if (!line) { return; }

        idx = yed_line_col_to_idx(line, frame->cursor_col);
        if (idx == -1) { return; }

        yed_line_glyph_traverse_from(*line, git, idx) {
            yed_append_to_line(frame->buffer, save_row+2, *git);
            len += 1;
        }
        for (; len > 0; len -= 1) {
            yed_pop_from_line(frame->buffer, save_row);
        }

        for (j=0; j<i+tabw; j++){
            yed_append_to_line(frame->buffer, save_row+1, G(' '));
        }

        yed_set_cursor_within_frame(frame, save_row+1, j+1);

        line = yed_buff_get_line(frame->buffer, frame->cursor_line);
        if (!line) { return; }

        event->cancel = 1;

    }

    if (event->key == ENTER && key_first == '[' &&  key_second == ']') {
        int       i, j, brace_col, tabw;

        tabw = yed_get_tab_width();

        if (tabw <= 0) {
            return;
        }

        brace_col = frame->cursor_col - 1;

        yed_delete_from_line(frame->buffer, save_row, save_col);

        i = 0;

        while (i < brace_col
        &&     yed_line_col_to_glyph(line, i + 1)->c == ' ') {
            i += 1;
        }

        yed_buff_insert_line(frame->buffer, save_row+1);
        yed_buff_insert_line(frame->buffer, save_row+2);

        for (j=0; j<i; j++){
            yed_append_to_line(frame->buffer, save_row+2, G(' '));
        }
        yed_append_to_line(frame->buffer, save_row+2, G(']'));

        for (j=0; j<i+tabw; j++){
            yed_append_to_line(frame->buffer, save_row+1, G(' '));
        }

        yed_set_cursor_within_frame(frame, save_row+1, j+1);

        line = yed_buff_get_line(frame->buffer, frame->cursor_line);
        if (!line)    { return; }


        event->cancel = 1;
    }

    if (event->key == ENTER && key_first == '(' &&  key_second == ')') {
        int       i, j, brace_col, tabw;

        tabw = yed_get_tab_width();

        if (tabw <= 0) {
            return;
        }

        brace_col = frame->cursor_col - 1;

        yed_delete_from_line(frame->buffer, save_row, save_col);

        i = 0;

        while (i < brace_col
        &&     yed_line_col_to_glyph(line, i + 1)->c == ' ') {
            i += 1;
        }

        yed_buff_insert_line(frame->buffer, save_row+1);
        yed_buff_insert_line(frame->buffer, save_row+2);

        for (j=0; j<i; j++){
            yed_append_to_line(frame->buffer, save_row+2, G(' '));
        }
        yed_append_to_line(frame->buffer, save_row+2, G(')'));

        for (j=0; j<i+tabw; j++){
            yed_append_to_line(frame->buffer, save_row+1, G(' '));
        }

        yed_set_cursor_within_frame(frame, save_row+1, j+1);

        line = yed_buff_get_line(frame->buffer, frame->cursor_line);
        if (!line)    { return; }

        event->cancel = 1;
    }
}

void completer_auto_match_buff_post_insert_handler(yed_event *event) {
    yed_frame *frame;
    int save_col;
    int save_row;
    char match = 0;

    if ( event->key == '(' ) {
        if ( !yed_var_is_truthy("disable-auto-paren") ) {
            match = ')';
        }
    } else if ( event->key == '[' ) {
        if ( !yed_var_is_truthy("disable-auto-bracket") ) {
            match = ']';
        }
    } else if ( event->key == '"' ) {
        if ( !yed_var_is_truthy("disable-auto-dquote") ) {
            match = '"';
        }
    } else if ( event->key == '\'' ) {
        if ( !yed_var_is_truthy("disable-auto-quote") ) {
            match = '\'';
        }
    } else if ( event->key == '{' ) {
        if ( !yed_var_is_truthy("disable-auto-brace") ) {
            match = '}';
        }
    }

    if ( match == 0 ) return;

    if ( !event->frame ) {
        return;
    }

    if ( !event->frame->buffer ) {
        return;
    }

    frame = event->frame;

    save_col = frame->cursor_col;
    save_row = frame->cursor_line;
    if(save_col <= 1) {
        return;
    }

    yed_insert_into_line(frame->buffer, save_row, save_col, G(match));
}

void remover_auto_match_buff_pre_delete_back_handler(yed_event *event) {
    yed_frame *frame;
    int        save_col;
    int        save_row;
    char        match = 0;
    char        key_first = 0;
    char        key_second = 0;
    yed_glyph  *g;

    if ( !event->frame ) {
        return;
    }

    if ( !event->frame->buffer ) {
        return;
    }

    if ( event->frame->buffer->has_selection ) {
        return;
    }

    frame = event->frame;
    save_col = frame->cursor_col;
    save_row = frame->cursor_line;

    if ( save_col == 1 ) {
        return;
    }

    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col-1);
    if (!g) { return; }
    key_first = g->c;

    g = yed_buff_get_glyph(event->frame->buffer, save_row, save_col);
    if (!g) { return; }
    key_second = g->c;

    if ( key_first == '(' ) {
        if ( !yed_var_is_truthy("disable-auto-paren") ) {
            match = ')';
        }
    } else if ( key_first == '[' ) {
        if ( !yed_var_is_truthy("disable-auto-bracket") ) {
            match = ']';
        }
    } else if ( key_first == '"' ) {
        if ( !yed_var_is_truthy("disable-auto-dquote") ) {
            match = '"';
        }
    } else if ( key_first == '\'' ) {
        if ( !yed_var_is_truthy("disable-auto-quote") ) {
            match = '\'';
        }
    } else if ( key_first == '{' ) {
        if ( !yed_var_is_truthy("disable-auto-brace") ) {
            match = '}';
        }
    }


    if ( match != key_second ) {
        return;
    }

    if ( match == 0 ) return;

    do_delete_match = 1;
}

void remover_auto_match_buff_post_delete_back_handler(yed_event *event){
    if ( !do_delete_match ) {
        return;
    }

    YEXE("delete-forward");

    do_delete_match = 0;
}
