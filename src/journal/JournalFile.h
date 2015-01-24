#ifndef joedb_JournalFile_declared
#define joedb_JournalFile_declared

#include "Listener.h"

namespace joedb
{
 class File;
 class Database;

 class JournalFile: public Listener
 {
  public:
   JournalFile(File &file);

   enum class state_t
   {
    no_error,
    unsupported_version,
    bad_format,
    crash_check
   };

   state_t get_state() const {return state;}
   void checkpoint();
   void replay_log(Listener &listener);

   void after_create_table(const std::string &name);
   void after_drop_table(table_id_t table_id);
   void after_add_field(table_id_t table_id,
                        const std::string &name,
                        Type type);
   void after_drop_field(table_id_t table_id,
                         field_id_t field_id);
   void after_insert(table_id_t table_id, record_id_t record_id);
   void after_delete(table_id_t table_id, record_id_t record_id);
   void after_update(table_id_t table_id,
                     record_id_t record_id,
                     field_id_t field_id,
                     const Value &value);

   ~JournalFile();

  private:
   static const uint32_t version_number;
   static const int64_t header_size;

   File &file;
   unsigned checkpoint_index;
   uint64_t checkpoint_position;
   state_t state;

   Type read_type();

   enum class operation_t: uint8_t
   {
    end_of_file   = 0x00,
    create_table  = 0x01,
    drop_table    = 0x02,
    add_field     = 0x03,
    drop_field    = 0x04,
    insert_into   = 0x05,
    delete_from   = 0x06,
    update        = 0x07
   };
 };
}

#endif