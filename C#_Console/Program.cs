
using System;
using System.Runtime.InteropServices;
using System.Net.Mail;
using System.Threading;
using System.Globalization;
using System.Security.AccessControl;

public class EmailLib
{
    // Import the SendEmail function from the shared library
    [DllImport("../C++ DLL_SO/libemail_lib.so", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool sendEmail(string accestoken, string receiver, string subject, string body);
    
    [DllImport("../C++ DLL_SO/libemail_lib.so", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool scheduleEmail(string dateTime);

}
class Program
{

      public static bool IsValidEmail(string email)
    {
        try
        {
            //This class in System.Net.Mail can parse an email address to ensure it follows the correct syntax
            var mailAddress = new MailAddress(email);
            Console.WriteLine("Valid email address.");
            return true;
        }
        catch (FormatException)
        {
            Console.WriteLine("Invalid email address.");
            return false;
        }
    }
     public static int ShowMenu(int choice=1)
    {
       bool isExiting=false;
       string color="►   \u001b[32m";
       ConsoleKeyInfo keys;
       (int left,int top)=Console.GetCursorPosition();
       while(!isExiting){
            Console.SetCursorPosition(left,top);
            Console.WriteLine("\n\n");
            Console.WriteLine($"{(choice==1? color:"    ")}Send an Email \u001b[0m");
            Console.WriteLine($"{(choice==2? color:"    ")}Schedule Email \u001b[0m");
            Console.WriteLine($"{(choice==3? color:"    ")}Exit \u001b[0m");
            Console.WriteLine("\n");

            keys=Console.ReadKey(intercept:true);

            switch (keys.Key){
                case ConsoleKey.DownArrow:
                  choice=(choice==3? 1: choice+1);
                 // (left,int Top)=Console.GetCursorPosition();
                  break;
                case ConsoleKey.UpArrow:
                  choice=(choice==1? 3: choice-1);
                  //(left,Top)=Console.GetCursorPosition();
                  break;
                case ConsoleKey.Enter:
                  isExiting=true;
                  break;
             }
       } 
       return choice;         
}    

  
     public static void operation(int choice,string authHeader)
    {
        if (choice==1){
            Console.Write("Enter recipient email: ");
            string email = Console.ReadLine()?? string.Empty;
            if (IsValidEmail(email)){
                Console.Write("Enter subject: ");
                string subject = Console.ReadLine()?? string.Empty;

                Console.Write("Enter email body: ");
                string body = Console.ReadLine()?? string.Empty;

                if (EmailLib.sendEmail(authHeader,email, subject, body))
                    Console.WriteLine("Email sent!");
            }
        }
        else if (choice==2){
            Console.Write("Enter recipient email: ");
            string email = Console.ReadLine()?? string.Empty;
            if (IsValidEmail(email)){
                Console.Write("Enter subject: ");
                string subject = Console.ReadLine()?? string.Empty;

                Console.Write("Enter email body: ");
                string body = Console.ReadLine()?? string.Empty;
                  
                Console.Write("Enter send time (YYYY-MM-DD HH:MM): ");
                string dateTime = Console.ReadLine()?? string.Empty;
                  
                // Run the email sending function on a background thread
                if (EmailLib.scheduleEmail(dateTime)){
                    DateTime scheduledTime = DateTime.ParseExact(dateTime, "yyyy-MM-dd HH:mm", null);
                    TimeSpan delay = scheduledTime - DateTime.Now;

                    // Schedule email on a background thread with a delay
                    Task.Run(async () =>
                    {
                        await Task.Delay(delay);  // Wait until the scheduled time
                        EmailLib.sendEmail(authHeader,email, subject, body);
                        Console.WriteLine("Email sent at the scheduled time.");
                    });

                    Console.WriteLine("Email scheduling started on a background thread...");
                }
                else
                    Console.WriteLine("The specified time is in the past.");
                    
                
                Console.WriteLine("Email sent on a background thread.");
                      
            }
        }
        else if (choice==3){
            Console.Write("Exiting....");
        }
        else{
            Console.WriteLine("Invalid option.");
        }
        return;
    }

    static void Main(string[] args)
    {
        

        string accessToken = "Add_Your_Access_Token";
        string authHeader = "Authorization: Bearer " + accessToken;
        Console.ForegroundColor = ConsoleColor.Cyan;
        Console.WriteLine(@"
        ███████╗███████╗███╗   ██╗██████╗        ██╗    ██╗██╗███████╗  █████╗ ██████ ╗██████╗ 
        ██╔════╝██╔════╝████╗  ██║██╔══██╗       ██║    ██║██║     ██╝ ██╔══██╗██╔══██║██╔══██╗
        █████╗  █████╗  ██╔██╗ ██║██║  ██║       ██║ █╗ ██║██║  ███╗   ███████║██████╔╝██║  ██║
           ██║  ██╔══╝  ██║╚██╗██║██║  ██║       ██║███╗██║██║██╔══╝   ██╔══██║██╔██═╝ ██║  ██║
       ██████║  ███████╗██║ ╚████║██████╔╝       ╚███╔███╔╝██║███████╗ ██║  ██║██║ ██  ██████╔╝
       ╚═════╝  ╚══════╝╚═╝  ╚═══╝╚═════╝         ╚══╝╚══╝ ╚═╝╚══════╝ ╚═╝  ╚═╝╚═╝ ╚═╝ ╚═════╝ 
        ");
        
        Console.ForegroundColor = ConsoleColor.Yellow;
        Console.WriteLine("                                S E N D W I Z A R D");

        // Reset the console color
        Console.ResetColor();
        Console.ForegroundColor=ConsoleColor.Cyan;
        Console.WriteLine("\nWelcome to SendWizard!");
        Console.ResetColor();
        

        (int left,int top)=Console.GetCursorPosition();        
        Console.CursorVisible=false;
        int choice=1;
        while (choice!=3){
        choice=ShowMenu();
        operation(choice,authHeader);
        Console.Clear();
        } 

    Console.WriteLine("Press Enter to exit...");
    Console.ReadLine();
    }
}
