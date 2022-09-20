#include "CDatabase.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

static std::string tables = 
         /**< Create Table Caregiver */
         "CREATE TABLE IF NOT EXISTS Caregiver("                                          \
         "caregiver_id INTEGER PRIMARY KEY AUTOINCREMENT,"                                \
         "fingerprint_id INTEGER,"                                                        \
         "caregiver_name CHAR(32),"                                                        \
         "pin_code CHAR(4));"                      
         /**< Create Table Patient */
         "CREATE TABLE IF NOT EXISTS Patient("                                            \
         "patient_id INTEGER PRIMARY KEY AUTOINCREMENT,"                                  \
         "patient_name CHAR(32),"                                                         \
         "RFID_tag CHAR(16));"                                                            \
         /**< Create Table Medicine */
         "CREATE TABLE IF NOT EXISTS Medicine("                                           \
         "medicine_id INTEGER PRIMARY KEY AUTOINCREMENT,"                                 \
         "medicine_name CHAR(32),"                                                        \
         "quantity INTEGER);"                                                             \
         /**< Create Table Medicine Notication */
         "CREATE TABLE IF NOT EXISTS MedicineNotification("                               \
         "medicine_id INTEGER PRIMARY KEY,"                                               \
         "FOREIGN KEY(medicine_id) REFERENCES Medicine(medicine_id)"                      \
         "ON DELETE CASCADE ON UPDATE CASCADE);"                                          \
         /**< Create Table Medication Time */
         "CREATE TABLE IF NOT EXISTS MedicationTime("                                     \
         "time_id INTEGER PRIMARY KEY,"                                                   \
         "med_hour INTEGER,"                                                              \
         "med_min INTEGER);"                                                              \
          /**< Create Table Patient Notification */
         "CREATE TABLE IF NOT EXISTS PatientNotification("                                \
         "patient_id INTEGER,"                                                            \
         "time_id INTEGER,"                                                               \
         "day INTEGER,"                                                                   \
         "month INTEGER,"                                                                 \
         "year INTEGER,"                                                                  \
         "FOREIGN KEY(patient_id) REFERENCES Patient(patient_id)"                         \
         "ON DELETE CASCADE ON UPDATE CASCADE,"                                           \
         "FOREIGN KEY(time_id) REFERENCES MedicationTime(time_id)"                        \
         "ON DELETE CASCADE ON UPDATE CASCADE,"                                           \
         "PRIMARY KEY (patient_id, time_id));"                                            \
         /**< Create Table Prescription */
         "CREATE TABLE IF NOT EXISTS Prescription("                                       \
         "patient_id INTEGER,"                                                            \
         "medicine_id INTEGER,"                                                           \
         "time_id INTEGER,"                                                               \
         "quantity INTEGER,"                                                              \
         "FOREIGN KEY(patient_id) REFERENCES Patient(patient_id)"                         \
         "ON DELETE CASCADE ON UPDATE CASCADE,"                                           \
         "FOREIGN KEY(medicine_id) REFERENCES Medicine(medicine_id)"                      \
         "ON DELETE CASCADE ON UPDATE CASCADE,"                                           \
         "FOREIGN KEY(time_id) REFERENCES MedicationTime(time_id)"                        \
         "ON DELETE CASCADE ON UPDATE CASCADE,"                                           \
         "PRIMARY KEY (patient_id, medicine_id, time_id));";

static std::string text;

CDatabase::CDatabase()
{
   zErrMsg=0;

   database = "opt/smd.db";

   /* Open database */
   rc = sqlite3_open(database.c_str(), &db);

   if(rc) 
   {
      std::cerr << "Can't open SMD database: " << sqlite3_errmsg(db) << std::endl;
      return;
   } 
   else
      std::cerr << "SMD Database Opened successfully!" << std::endl;

   rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", callback, 0, &zErrMsg);

   sqlite3_stmt* stmt;
   sqlite3_prepare(db, "PRAGMA foreign_keys = ON;", -1, &stmt, 0); 
   sqlite3_step(stmt);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, tables.c_str(), callback, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
   }
   else
      std::cerr << "Table created successfully!" << std::endl;
}

CDatabase::~CDatabase(){
   //Close database
   sqlite3_close(db);
}

/**
 * int exec_callback( void* udp, int c_num, char** c_vals, char** c_names );
 * udp - An application-defined user-data pointer. This value is made available 
 *       to the exec callback function.
 * errmsg - An optional reference to a std::string pointer. If the function returns 
 *          anything other than SQLITE_OK, an error message will be passed back. 
 *          If no error is encountered, the pointer will be set to NULL. The reference 
 *          may be NULL to ignore error messages. Error messages must be freed with 
 *          sqlite3_free().
 * c_num - The number of columns in the current result row.
 * c_vals - An array of UTF-8 std::strings that contain the values of the current result row. 
 *          These values are generated with sqlite3_column_text() and use the same 
 *          conversion rules. There is no way to determine the original datatype of 
 *          the value.
 * c_names - An array of UTF-8 std::strings that contain the column names of the current 
 *           result row. These are generated with sqlite3_column_name().
 * @return An SQLite result code.
 */
int CDatabase::callback(void *NotUsed, int count, char **data, char **columns){

   for(int i=0;i<count;i++){
      if (i!=0) text+="  ";
      text+=data[i];
   }

   //text+="\n";
   return 0;
}

/*************************************/
/*          INSERT FUNCTION          */
/*************************************/
int CDatabase::insert(std::string table, std::string values)
{
   zErrMsg = 0;

   //Execute SQL statement
   std::string query = "INSERT INTO " + table 
                  + " VALUES("+values+");";

   rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      return(INSERT_ERROR);
   } 
   else
      std::cerr << "Data added successfully!" << std::endl << std::endl;

   return(0);
}


int CDatabase::update(std::string table,std::string att,std::string update,std::string primKey, std::string values)
{

   zErrMsg = 0;

   //Execute SQL statement
   std::string query = "UPDATE " + table 
                  + " SET " + att + " = " + update
                  + " WHERE " + primKey + " = " + values + ";";

   rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      return(INSERT_ERROR);
   } 
   else
      std::cerr << "Data updated successfully!" << std::endl << std::endl;

   return(0);
}

/*************************************/
/*          REMOVE FUNCTION          */
/*************************************/
int CDatabase::remove(std::string table, std::string primKey, std::string values){

   zErrMsg = 0;

   //Execute SQL statement
   std::string query = "DELETE FROM " + table 
                  + " WHERE " + primKey + " = " + values + ";";

   rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      return(REMOVE_ERROR);
   } 
   else 
      std::cerr << "Data deleted successfully!" << std::endl << std::endl;

   return(0);
}

/**
 * get last inserted function
 * @return SELECT column (or *) FROM table ORDER BY column DESC LIMIT 1;
 */
int CDatabase::getLastInserted(std::string table, std::string column, std::string order_by, std::string& d_text){

   zErrMsg = 0;

   std::string query = "SELECT " + column + " FROM " + table + " ORDER BY " + order_by + " DESC LIMIT 1;";

   rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      return(GET_ERROR);
   } 
   else
      std::cerr << "Data returned successfully!" << std::endl;

   d_text=text;
   text.clear();

   return(0);
}

int CDatabase::get(std::string table, std::string column, std::string primKey, std::string values, std::string& d_text){

   zErrMsg = 0;

   std::string query = "SELECT " + column + " FROM " + table + " WHERE " + primKey + " = " + values + ";";

   rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      return(GET_ERROR);
   } 
   else
      std::cerr << "Data returned successfully!" << std::endl;

   d_text=text;
   text.clear();

   return(0);
}

int CDatabase::callbackPrint(void* data, int columnsNum, char** fields, char** columns)
{
   int i;
   std::string str;
  
   for (i = 0; i < columnsNum; i++) 
   {
      fields[i] ? str = fields[i] : str = "NULL";
      std::cerr << columns[i] << " = " << str << std::endl;
   }
  
   std::cerr << std::endl;
   return 0;
}

int CDatabase::print(std::string table)
{
   zErrMsg = 0;

   std::string query = "SELECT * FROM " + table + ";";

   rc = sqlite3_exec(db, query.c_str(), callbackPrint, 0, &zErrMsg);

   if( rc != SQLITE_OK )
   {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      return(GET_ERROR);
   } 
/*   else 
      std::cerr << "Data printed successfully!" << std::endl;*/

   return(0);
}