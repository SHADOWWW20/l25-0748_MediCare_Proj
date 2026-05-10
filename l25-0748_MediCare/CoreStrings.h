#ifndef CORESTRINGS_H
#define CORESTRINGS_H

/**
 * MediCore — manual string and parsing helpers (no std::string, no libc string functions).
 * Array access uses pointer arithmetic only where inputs are char*.
 */

int core_str_len(const char* s);
void core_str_cpy(char* dest, const char* src);
int core_str_cmp(const char* a, const char* b);
void core_str_cat(char* dest, const char* src);
char core_char_lower(char c);
bool core_str_equal_ci(const char* a, const char* b);
void core_trim_newline(char* line);
int core_parse_int(const char* s);
float core_parse_float(const char* s);
void core_int_to_str(int v, char* out);
void core_float_to_str(float f, char* out);
void core_append_char(char* dest, char c, int dest_cap);
/* Compare DD-MM-YYYY date strings for sorting: negative if a<b, 0 equal, positive if a>b */
int core_compare_dates_dd_mm_yyyy(const char* a, const char* b);
/* Compare HH:MM time strings */
int core_compare_times_hh_mm(const char* a, const char* b);
void core_get_current_year(int* year_out);
void core_get_today_dd_mm_yyyy(char* buf11);
void core_timestamp_log(char* buf_out, int buf_cap);
/* Days from epoch for overdue bill check (midnight UTC approximation via struct tm) */
double core_seconds_since_epoch_for_date_dd_mm_yyyy(const char* date10);

#endif

