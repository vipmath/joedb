#include "dump.h"
#include "Database.h"
#include "Listener.h"

#include <map>

/////////////////////////////////////////////////////////////////////////////
void joedb::dump(const Database &db, Listener &listener)
/////////////////////////////////////////////////////////////////////////////
{
 //
 // Dump tables
 //
 std::map<table_id_t, table_id_t> table_map;
 {
  table_id_t table_id = 0;
  for (auto table: db.get_tables())
  {
   ++table_id;
   listener.after_create_table(table.second.get_name());
   table_map[table.first] = table_id;
  }
 }

 //
 // Dump fields
 //
 std::map<table_id_t, std::map<field_id_t, field_id_t>> field_maps;
 {
  table_id_t table_id = 0;
  for (auto table: db.get_tables())
  {
   ++table_id;
   field_id_t field_id = 0;
   for (const auto &field: table.second.get_fields())
   {
    ++field_id;
    auto type = field.second.get_type();
    if (type.get_type_id() == Type::type_id_t::reference)
     type = Type::reference(table_map[type.get_table_id()]);
    listener.after_add_field(table_map[table.first],
                             field.second.get_name(),
                             type);
    field_maps[table.first][field.first] = field_id;
   }
  }
 }

 //
 // Dump records
 //
 for (auto table: db.get_tables())
 {
  const auto &fields = table.second.get_fields();

  const auto &freedom = table.second.get_freedom();

  for (size_t i = freedom.get_first_used(); i != 0; i = freedom.get_next(i))
  {
   record_id_t record_id = i - 1;
   listener.after_insert(table_map[table.first], record_id);

   for (const auto &field: fields)
   {
    switch(field.second.get_type().get_type_id())
    {
     case Type::type_id_t::null:
     break;

     #define TYPE_MACRO(type, return_type, type_id, R, W)\
     case Type::type_id_t::type_id:\
      listener.after_update_##type_id(table_map[table.first], record_id, field_maps[table.first][field.first], table.second.get_##type_id(record_id, field.first));\
     break;
     #include "TYPE_MACRO.h"
     #undef TYPE_MACRO
    }
   }
  }
 }
}
