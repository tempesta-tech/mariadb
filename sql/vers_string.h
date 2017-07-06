#ifndef VERS_STRING_INCLUDED
#define VERS_STRING_INCLUDED

struct Compare_strncmp
{
  int operator()(const LEX_CSTRING& a, const LEX_CSTRING& b) const
  {
    return strncmp(a.str, b.str, a.length);
  }
  static CHARSET_INFO* charset()
  {
    return system_charset_info;
  }
};

template <CHARSET_INFO* &CS= system_charset_info>
struct Compare_my_strcasecmp
{
  int operator()(const LEX_CSTRING& a, const LEX_CSTRING& b) const
  {
    DBUG_ASSERT(a.str[a.length] == 0 && b.str[b.length] == 0);
    return my_strcasecmp(CS, a.str, b.str);
  }
  static CHARSET_INFO* charset()
  {
    return CS;
  }
};

typedef Compare_my_strcasecmp<files_charset_info> Compare_fs;
typedef Compare_my_strcasecmp<table_alias_charset> Compare_t;

template <class Storage= LEX_CSTRING>
struct LEX_STRING_u : public Storage
{
  LEX_STRING_u()
  {
    Storage::str= NULL;
    Storage::length= 0;
  }
  LEX_STRING_u(const char *_str, uint32 _len, CHARSET_INFO *)
  {
    Storage::str= _str;
    Storage::length= _len;
  }
  uint32 length() const
  {
    return Storage::length;
  }
  const char *ptr() const
  {
    return Storage::str;
  }
  void set(const char *_str, uint32 _len, CHARSET_INFO *)
  {
    Storage::str= _str;
    Storage::length= _len;
  }
  const LEX_CSTRING& lex_cstring() const
  {
    return *this;
  }
  const LEX_STRING& lex_string() const
  {
    return *(LEX_STRING *)this;
  }
};

template <class Compare= Compare_strncmp, class Storage= LEX_STRING_u<> >
struct XString : public Storage
{
public:
  XString() {}
  XString(const char *_str, size_t _len) :
    Storage(_str, _len, Compare::charset())
  {
  }
  XString(LEX_STRING& src) :
    Storage(src.str, src.length, Compare::charset())
  {
  }
  XString(const char *_str) :
    Storage(_str, strlen(_str), Compare::charset())
  {
  }
  XString& operator= (const char *_str)
  {
    DBUG_ASSERT(_str);
    Storage::set(_str, strlen(_str), Compare::charset());
    return *this;
  }
  bool operator== (const XString& b) const
  {
    return Storage::length() == b.length() && 0 == Compare()(this->lex_cstring(), b.lex_cstring());
  }
  bool operator!= (const XString& b) const
  {
    return !(*this == b);
  }
  operator const char* () const
  {
    return Storage::ptr();
  }
  operator LEX_CSTRING& () const
  {
    return this->lex_cstring();
  }
  operator LEX_STRING () const
  {
    LEX_STRING res;
    res.str= const_cast<char *>(this->ptr());
    res.length= this->length();
    return res;
  }
  operator bool () const
  {
    return Storage::ptr() != NULL;
  }
};

typedef XString<> LString;
typedef XString<Compare_fs> LString_fs;
typedef XString<Compare_my_strcasecmp<> > LString_i;

typedef XString<Compare_strncmp, String> SString;
typedef XString<Compare_fs, String> SString_fs;
typedef XString<Compare_t, String> SString_t;


#define XSTRING_WITH_LEN(X) (X).ptr(), (X).length()
#define DB_WITH_LEN(X) (X).db, (X).db_length
#define TABLE_NAME_WITH_LEN(X) (X).table_name, (X).table_name_length


#endif // VERS_STRING_INCLUDED
