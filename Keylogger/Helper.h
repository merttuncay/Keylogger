#ifndef HELPER_H
#define HELPER_H

#include <ctime>  // Retrieve time and able to reenter into our program in terms of mail sender triggers.
#include <string> // We simply convert values to string, because we use string to fill out log files
#include <sstream> // Using for converting strings
#include <fstream>

/*Using name space is not a good practice since
we will make our own types, so we do not want any conflict
*/

namespace Helper
{
    template <class T> // Accept generic types

    std::string ToString(const T &); // Compiler creates a function for each type with exactly the same body

    struct DateTime
    {
        // It creates a object of DataType type with the current system Time and Date !MACHINE THAT KEYLOGGER IS RUNNING*/
        DateTime()
        {
            time_t ms;  //represent the time which in ctime
            time(&ms); //takes system time and places into ms, prevent a lot of copying we passing the addresses

            struct tm *info = localtime(&ms); // pointer to a struct

            D = info->tm_mday;
            m = info->tm_mon + 1;
            y = 1900 + info->tm_year ;
            M = info->tm_min;
            H = info->tm_hour;
            S = info->tm_sec;
        }

        // This second constructor is for custom Date variable
        DateTime(int D, int m, int y, int H, int M, int S): D(D), m(m), y(y), H(H), M(M), S(S) {}
        DateTime(int D, int m, int y): D(D), m(m), y(y), H(0), M(0), S(0) {}

        DateTime Now() const
        {
            return DateTime();
        }

        int D, m, y, H, M, S;

        std::string GetDateString() const
        {
            return std::string(D < 10 ? "0" : "" ) + ToString(D) +
                   std::string(m < 10 ? ".0" : "" ) + ToString(m) + "."+
                   ToString(y);
        }

        /* ??(Why we need to give this parameter) It will format the time to specification
        so the parameter is a reference to a constant string "sep", and default separator is column*/
        std::string GetTimeString(const std::string &sep = ":") const
        {
            return std::string( H < 10 ? "0" : "") + ToString(H) + sep +
                   std::string( M < 10 ? "0" : "") + ToString(M) + sep +
                   std::string( S < 10 ? sep : "") + ToString(S);
        }

        std::string GetDateTimeString( const std::string  &sep = ":") const
        {
            return GetDateString() + " " + GetTimeString(sep) ;
        }
    };

    /*be able to pass a type to this function
    that supports an insertion operator*/
    template <class T>
    std::string ToString(const T &e)
    {
        std::ostringstream s;

        s << e ;
        return s.str();
    }

    /*Create a log file for the keylogger for debugging , it records the errors
    , program is not depending on this particular function anyway*/
    void WriteAppLog (const std::string &s)
    {
        // ios = InputOutputStream , app = append
        std::ofstream file("AppLog.txt", std::ios::app);
        file << "[" << Helper::DateTime().GetDateTimeString() << "]" << "\n" << s << std::endl << "\n" ;
        file.close();

    }

}


#endif // HELPER_H
