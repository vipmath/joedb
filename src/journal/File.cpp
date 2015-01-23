#include "File.h"

/////////////////////////////////////////////////////////////////////////////
bool joedb::File::open(const char *file_name, mode_t new_mode)
{
 mode = new_mode;
 static const char *mode_string[3] = {"rb", "r+b", "w+b"};
 file = std::fopen(file_name, mode_string[static_cast<size_t>(mode)]);
 return file != 0;
}

/////////////////////////////////////////////////////////////////////////////
void joedb::File::set_position(int64_t position)
{
 std::fseek(file, long(position), SEEK_SET);
}

/////////////////////////////////////////////////////////////////////////////
int64_t joedb::File::get_position() const
{
 return int64_t(std::ftell(file));
}

/////////////////////////////////////////////////////////////////////////////
void joedb::File::write_string(const std::string &s)
{
 write<uint64_t>(uint64_t(s.size()));
 std::fwrite(&s[0], 1, s.size(), file);
}

/////////////////////////////////////////////////////////////////////////////
std::string joedb::File::read_string()
{
 std::string s;
 const uint64_t size = read<uint64_t>();
 s.resize(size_t(size));
 const size_t result = std::fread(&s[0], 1, size, file);
 if (result < size)
  s.resize(result);
 return s;
}

/////////////////////////////////////////////////////////////////////////////
void joedb::File::flush()
{
 fflush(file);
}

/////////////////////////////////////////////////////////////////////////////
joedb::File::~File()
{
 if (file)
  fclose(file);
}
