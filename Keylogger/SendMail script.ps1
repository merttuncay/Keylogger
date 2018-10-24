Param( [String]$Att,
       [String]$Subj,
       [String]$Body
      )

Function Send-EMail
{
    Param( [Parameter(`
           Mandatory=$true)]
          [String]$To,
           [Parameter(`
           Mandatory=$true)]
          [String]$From,
           [Parameter(`
           Mandatory=$true)]
          [String]$Password,
           [Parameter(`
           Mandatory=$true)]
          [String]$Subject,
           [Parameter(`
           Mandatory=$true)]
          [String]$Body,
           [Parameter(`
           Mandatory=$true)]
          [String]$Attachment
         )
    try
    {
        $Msg = New-Object System.Net.Mail.MailMessage($From, $To, $Subject, $Body)
        $Srv = "smtp.gmail.com"
        if($Attachment -ne $null)
        {
            try
            {
                $Attachments = $Attachment -split ("\:\:");

                ForEach($val in $Attachments)
                {
                    $attch = New-Object System.Net.Mail.Attachment($val)
                    $Msg.Attachments.Add($attch)
                     
                }

            }
            catch
            {
                exit 2;  
            }

            $Client = New-Object Net.Mail.SmtpClient($Srv, 587)
            $Client.EnableSsl = $true #ssl is the main problem which prevents the custom work
            $Client.Credentials = New-Object System.Net.NetworkCredential($From.Split("@"),$Password);

            #It is important that the program spends the lowest amount of resources possible

            Remove-Variable -Name Client
            Remove-Variable -Name Password
            exit 7; # means everything is OK
        }
    }
    catch
    {
            exit 3;
    }    
}

try
{
    Send-EMail 
        -Attachment $Att
        -To "Address of the recipient"
        -Body $Body
        -Subject $Subj
        -Password "Password for the sender"
        -From "Address of the sender"
}
catch
{
    exit 4; #There is a problem
}