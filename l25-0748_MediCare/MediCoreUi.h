#ifndef MEDICOREUI_H
#define MEDICOREUI_H

/**
 * Console input/output helpers — keeps menus thin and readable.
 */
void medi_ui_print_blank_line();
void medi_ui_print_str(const char* s);
void medi_ui_print_line(const char* s);
void medi_ui_read_line(char* buf, int cap);
bool medi_ui_read_int(int* out);

#endif

