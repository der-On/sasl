#ifndef __UNICODE_H__
#define __UNICODE_H__

#include <wchar.h>
#include <string>


namespace xa {

/// Convert multibyte string in UTF-8 encoding to unicode string
/// \param str string in UTF-8 encoding
std::wstring fromUtf8(const std::string &str);

};

#endif

