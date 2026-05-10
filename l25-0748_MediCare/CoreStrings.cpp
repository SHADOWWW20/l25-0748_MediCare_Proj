#include "CoreStrings.h"
#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include <cstdio>

// Returns the length of a C-style string.
int core_str_len(const char* s)
{
    int n = 0;
    while (s[n] != '\0')
    {
        n++;
    }
    return n;
}

// Copies one C-style string into another.
void core_str_cpy(char* dest, const char* src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Compares two C-style strings character by character.
int core_str_cmp(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return (int)(a[i]) - (int)(b[i]);
        }
        i++;
    }
    return (int)(a[i]) - (int)(b[i]);
}

// Appends one C-style string to another.
void core_str_cat(char* dest, const char* src)
{
    int d = core_str_len(dest);
    int i = 0;
    while (src[i] != '\0')
    {
        dest[d + i] = src[i];
        i++;
    }
    dest[d + i] = '\0';
}

// Converts one uppercase letter to lowercase and returns the converted character.
char core_char_lower(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return (char)(c + ('a' - 'A'));
    }
    return c;
}

// Compares two strings without case sensitivity.
bool core_str_equal_ci(const char* a, const char* b)
{
    int i = 0;
    for (;; i++)
    {
        char ca = core_char_lower(a[i]);
        char cb = core_char_lower(b[i]);
        if (ca == '\0' && cb == '\0')
        {
            return true;
        }
        if (ca != cb)
        {
            return false;
        }
    }
}

// Removes trailing newline characters from the input text in place.
void core_trim_newline(char* line)
{
    int n = core_str_len(line);
    int i = n - 1;
    while (i >= 0)
    {
        char c = line[i];
        if (c == '\n' || c == '\r')
        {
            line[i] = '\0';
            i--;
        }
        else
        {
            break;
        }
    }
}

// Converts a numeric string to int value.
int core_parse_int(const char* s)
{
    int sign = 1;
    int i = 0;
    if (s[0] == '-')
    {
        sign = -1;
        i = 1;
    }
    int val = 0;
    while (s[i] >= '0' && s[i] <= '9')
    {
        val = val * 10 + (int)(s[i] - '0');
        i++;
    }
    return val * sign;
}

// Converts a numeric string to float value.
float core_parse_float(const char* s)
{
    int i = 0;
    int sign = 1;
    if (s[0] == '-')
    {
        sign = -1;
        i = 1;
    }
    float intpart = 0.0f;
    while (s[i] >= '0' && s[i] <= '9')
    {
        intpart = intpart * 10.0f + (float)(s[i] - '0');
        i++;
    }
    float frac = 0.0f;
    if (s[i] == '.')
    {
        i++;
        float div = 10.0f;
        while (s[i] >= '0' && s[i] <= '9')
        {
            frac += (float)(s[i] - '0') / div;
            div *= 10.0f;
            i++;
        }
    }
    return sign * (intpart + frac);
}

// Converts an integer into a text string.
void core_int_to_str(int v, char* out)
{
    if (v == 0)
    {
        out[0] = '0';
        out[1] = '\0';
        return;
    }
    char tmp[32];
    int i = 0;
    int neg = 0;
    if (v < 0)
    {
        neg = 1;
        v = -v;
    }
    while (v > 0)
    {
        tmp[i] = (char)('0' + (v % 10));
        v /= 10;
        i++;
    }
    int j = 0;
    if (neg)
    {
        out[j] = '-';
        j++;
    }
    while (i > 0)
    {
        i--;
        out[j] = tmp[i];
        j++;
    }
    out[j] = '\0';
}

// Converts a float into text with two decimal places.
void core_float_to_str(float f, char* out)
{
    /* Two decimal places — sufficient for PKR amounts in files */
    int neg = 0;
    if (f < 0)
    {
        neg = 1;
        f = -f;
    }
    int whole = (int)f;
    float frac = f - (float)whole;
    int hundred = (int)(frac * 100.0f + 0.5f);
    if (hundred >= 100)
    {
        hundred -= 100;
        whole++;
    }
    int pos = 0;
    if (neg)
    {
        out[pos] = '-';
        pos++;
    }
    char wbuf[32];
    core_int_to_str(whole, wbuf);
    core_str_cpy(out + pos, wbuf);
    pos = core_str_len(out);
    out[pos] = '.';
    pos++;
    if (hundred < 10)
    {
        out[pos] = '0';
        pos++;
    }
    char fbuf[16];
    core_int_to_str(hundred, fbuf);
    core_str_cpy(out + pos, fbuf);
}

// Adds one character to the end of the buffer when there is free space.
void core_append_char(char* dest, char c, int dest_cap)
{
    int L = core_str_len(dest);
    if (L + 1 >= dest_cap)
    {
        return;
    }
    dest[L] = c;
    dest[L + 1] = '\0';
}

// Splits a DD-MM-YYYY string into day, month, and year numbers.
static int extract_dd_mm_yyyy(const char* d, int* dd, int* mm, int* yyyy)
{
    char db[8];
    char mb[8];
    char yb[16];
    int bi = 0;
    int phase = 0;
    int i = 0;
    while (d[i] != '\0')
    {
        char ch = d[i];
        if (ch == '-' && phase == 0)
        {
            db[bi] = '\0';
            phase = 1;
            bi = 0;
            i++;
            continue;
        }
        if (ch == '-' && phase == 1)
        {
            mb[bi] = '\0';
            phase = 2;
            bi = 0;
            i++;
            continue;
        }
        if (phase == 0)
        {
            db[bi] = ch;
            bi++;
        }
        else if (phase == 1)
        {
            mb[bi] = ch;
            bi++;
        }
        else
        {
            yb[bi] = ch;
            bi++;
        }
        i++;
    }
    yb[bi] = '\0';
    *dd = core_parse_int(db);
    *mm = core_parse_int(mb);
    *yyyy = core_parse_int(yb);
    return 1;
}

// Compares two DD-MM-YYYY dates for sorting.
int core_compare_dates_dd_mm_yyyy(const char* a, const char* b)
{
    int ad = 0;
    int am = 0;
    int ay = 0;
    int bd = 0;
    int bm = 0;
    int by = 0;
    extract_dd_mm_yyyy(a, &ad, &am, &ay);
    extract_dd_mm_yyyy(b, &bd, &bm, &by);
    if (ay != by)
    {
        return ay - by;
    }
    if (am != bm)
    {
        return am - bm;
    }
    return ad - bd;
}

// Splits an HH:MM string into hour and minute numbers.
static int extract_hh_mm(const char* t, int* hh, int* mm)
{
    char hb[8];
    char mb[8];
    int bi = 0;
    int phase = 0;
    int i = 0;
    while (t[i] != '\0')
    {
        char ch = t[i];
        if (ch == ':' && phase == 0)
        {
            hb[bi] = '\0';
            phase = 1;
            bi = 0;
            i++;
            continue;
        }
        if (phase == 0)
        {
            hb[bi] = ch;
            bi++;
        }
        else
        {
            mb[bi] = ch;
            bi++;
        }
        i++;
    }
    mb[bi] = '\0';
    *hh = core_parse_int(hb);
    *mm = core_parse_int(mb);
    return 1;
}

// Compares two HH:MM time values for sorting.
int core_compare_times_hh_mm(const char* a, const char* b)
{
    int ah = 0;
    int am = 0;
    int bh = 0;
    int bm = 0;
    extract_hh_mm(a, &ah, &am);
    extract_hh_mm(b, &bh, &bm);
    if (ah != bh)
    {
        return ah - bh;
    }
    return am - bm;
}

// Fills a tm struct from local time and returns true when conversion succeeds.
static bool core_fill_local_tm(std::time_t now, std::tm* out)
{
    std::tm* lt = std::localtime(&now);
    if (lt == nullptr)
    {
        return false;
    }
    *out = *lt;
    return true;
}

// Gets the current calendar year from the system clock.
void core_get_current_year(int* year_out)
{
    std::time_t now = std::time(nullptr);
    std::tm local_tm = {};
    if (!core_fill_local_tm(now, &local_tm))
    {
        *year_out = 1970;
        return;
    }
    *year_out = local_tm.tm_year + 1900;
}

// Gets today's date in DD-MM-YYYY format.
void core_get_today_dd_mm_yyyy(char* buf11)
{
    std::time_t now = std::time(nullptr);
    std::tm local_tm = {};
    if (!core_fill_local_tm(now, &local_tm))
    {
        core_str_cpy(buf11, "01-01-1970");
        return;
    }
    int dd = local_tm.tm_mday;
    int mm = local_tm.tm_mon + 1;
    int yy = local_tm.tm_year + 1900;
    char ds[8];
    char ms[8];
    char ys[16];
    core_int_to_str(dd, ds);
    core_int_to_str(mm, ms);
    core_int_to_str(yy, ys);
    buf11[0] = '\0';
    if (dd < 10)
    {
        core_str_cat(buf11, "0");
    }
    core_str_cat(buf11, ds);
    core_str_cat(buf11, "-");
    if (mm < 10)
    {
        core_str_cat(buf11, "0");
    }
    core_str_cat(buf11, ms);
    core_str_cat(buf11, "-");
    core_str_cat(buf11, ys);
}

// Builds a readable date-time stamp for logs.
void core_timestamp_log(char* buf_out, int buf_cap)
{
    std::time_t now = std::time(nullptr);
    std::tm local_tm = {};
    if (!core_fill_local_tm(now, &local_tm))
    {
        core_str_cpy(buf_out, "unknown-time");
        return;
    }
    /* DD-MM-YYYY HH:MM:SS */
    char tmp[64];
    if (strftime(tmp, sizeof(tmp), "%d-%m-%Y %H:%M:%S", &local_tm) == 0)
    {
        core_str_cpy(buf_out, "unknown-time");
        return;
    }
    core_str_cpy(buf_out, tmp);
    (void)buf_cap;
}

// Converts a DD-MM-YYYY date into epoch seconds.
double core_seconds_since_epoch_for_date_dd_mm_yyyy(const char* date10)
{
    int dd = 0;
    int mm = 0;
    int yy = 0;
    extract_dd_mm_yyyy(date10, &dd, &mm, &yy);
    std::tm t = {};
    t.tm_mday = dd;
    t.tm_mon = mm - 1;
    t.tm_year = yy - 1900;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    std::time_t sec = mktime(&t);
    return (double)sec;
}
