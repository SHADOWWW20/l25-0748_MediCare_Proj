#include "FileNotFoundException.h"

#include "CoreStrings.h"

// Builds a missing-file message for the given path.
FileNotFoundException::FileNotFoundException(const char* filepath)
    : HospitalException()
{
    char buf[220];
    buf[0] = '\0';
    core_str_cpy(buf, "Could not open file: ");
    core_str_cat(buf, filepath);
    core_str_cpy(this->message, buf);
}

// Releases file-not-found exception resources.
FileNotFoundException::~FileNotFoundException()
{
}
