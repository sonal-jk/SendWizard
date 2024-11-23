# SendWizard
**SendWizard** is a robust C# console application designed for sending emails efficiently using the Gmail API and cURL. This application currently enables sending emails, with plans for expanded capabilities in the future. At its core, SendWizard leverages powerful C++ DLLs to handle low-level operations such as Base64 encoding and HTTP requests.

## Features
### Current Features
1. Send Emails: Seamlessly send emails through Gmail API.
2. C++ Integration: Uses C++ DLLs for Base64 encoding and efficient communication with Gmail API via cURL.
3. Cross-Platform Capabilities: Built using .NET for compatibility across Windows, Linux, and macOS.
4. Email Scheduling: Set specific dates and times to send emails.

### Planned Features
1. Email Templates: Save and reuse pre-defined email templates for consistent messaging.
2. Enhanced Security: Additional authentication options and encrypted email payloads.

## Getting Started
### Prerequisites
1. .NET SDK- Install the .NET SDK from Microsoft's [.NET Download Page](https://dotnet.microsoft.com/en-us/download).

2. Gmail API Setup- Enable the Gmail API from the [Google Cloud Console](https://console.cloud.google.com). Generate an OAuth 2.0 client ID and download the credentials JSON file.
3. C++ Compiler- Ensure you have a compatible C++ compiler for building the required DLLs. For Windows, Visual Studio is recommended.
4. cURL- Install ```cURL``` for executing HTTP requests.

### Installation
* Clone the repository:

```
git clone https://github.com/sonal-jk/SendWizard.git
cd SendWizard
```

* Build the C++ DLLs:
Navigate to the CPP-DLLs directory.
Compile the DLL files: Run the .bat or .sh script according to your OS.

* Build the C# application: ``` dotnet build ```
* Run the Application: ``` dotnet run ```

## Acknowledgments
- Google Cloud: For providing the Gmail API.
- cURL Library: For handling HTTP requests in C++.
- .NET Framework: For making cross-platform development simple and efficient.

With SendWizard, managing email communications has never been easier. 🚀

