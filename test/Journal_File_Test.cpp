#include "Journal_File.h"
#include "File.h"
#include "DB_Listener.h"
#include "gtest/gtest.h"
#include "dump.h"

using namespace joedb;

class Journal_File_Test: public::testing::Test
{
 protected:
  virtual void TearDown()
  {
   std::remove("test.joedb");
  }
};

/////////////////////////////////////////////////////////////////////////////
TEST_F(Journal_File_Test, bad_file)
{
 File file("this_does_not_exists", File::mode_t::read_existing);
 EXPECT_FALSE(file.is_good());
 Journal_File journal(file);
 EXPECT_EQ(Journal_File::state_t::bad_file, journal.get_state());
}

/////////////////////////////////////////////////////////////////////////////
TEST_F(Journal_File_Test, basic_operations)
{
 Database db1;

 {
  File file("test.joedb", File::mode_t::create_new);
  Journal_File journal(file);
  db1.set_listener(journal);
  db1.drop_table(db1.create_table("deleted"));
  const table_id_t table_id = db1.create_table("table_test");
  db1.insert_into(table_id, 1);
  const field_id_t field_id = db1.add_field(table_id, "field", Type::int32());
  db1.update_int32(table_id, 1, field_id, 1234);
  db1.delete_from(table_id, 1);
  db1.insert_into(table_id, 2);
  db1.update_int32(table_id, 2, field_id, 4567);
  db1.drop_field(table_id, field_id);

  const field_id_t big_field_id = db1.add_field(table_id, "big_field", Type::int64());
  db1.update_int64(table_id, 2, big_field_id, 1234567ULL);

  const field_id_t new_field =
   db1.add_field(table_id, "new_field", Type::reference(table_id));
  db1.update_reference(table_id, 2, new_field, 2);
  const field_id_t name_id = db1.add_field(table_id, "name", Type::string());
  db1.update_string(table_id, 2, name_id, "Aristide");

  {
   const table_id_t table_id = db1.create_table("type_test");
   db1.insert_into(table_id, 1);

   const field_id_t string_field_id =
    db1.add_field(table_id, "string", Type::string());
   const field_id_t int32_field_id =
    db1.add_field(table_id, "int32", Type::int32());
   const field_id_t int64_field_id =
    db1.add_field(table_id, "int64", Type::int64());
   const field_id_t reference_field_id =
    db1.add_field(table_id, "reference", Type::reference(table_id));

   db1.update_string(table_id, 1, string_field_id, "SuperString");
   db1.update_int32(table_id, 1, int32_field_id, 1234);
   db1.update_int64(table_id, 1, int64_field_id, 123412341234LL);
   db1.update_reference(table_id, 1, reference_field_id, 1);
  }
  journal.checkpoint();
  file.commit();
 }

 Database db2;

 {
  File file("test.joedb", File::mode_t::read_existing);
  Journal_File journal(file);
  DB_Listener db_listener(db2);
  journal.replay_log(db_listener);
  EXPECT_TRUE(db_listener.is_good());
  EXPECT_EQ(Journal_File::state_t::no_error, journal.get_state());
 }

 std::ostringstream oss1;
 std::ostringstream oss2;

 joedb::dump(oss1, db1);
 joedb::dump(oss2, db2);

 EXPECT_EQ(oss1.str(), oss2.str());
}