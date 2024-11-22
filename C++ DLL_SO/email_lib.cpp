#include "email_lib.h"
#include <curl/curl.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
#include <jsoncpp/json/json.h> // Use a JSON library like JSON for Modern C++ or any other
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

// Function to Base64 encode the email content

std::string base64Encode(const std::string& input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Disable line wrapping
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string output(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return output;
}

//Encodes the raw message into URL-safe Base64 format
std::string urlSafeBase64Encode(const std::string& input) {
    std::string output = base64Encode(input);
    std::replace(output.begin(), output.end(), '+', '-');
    std::replace(output.begin(), output.end(), '/', '_');
    output.erase(std::remove(output.begin(), output.end(), '='), output.end());
    return output;
}


//Sends an email using the Gmail API via curl
extern "C"{
bool sendEmail(const char* auth_header,const char* receiver, const char* subject, const char* body) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = nullptr;

    //Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    //Adds the OAuth2 authorization header and sets the content type to JSON
    headers = curl_slist_append(headers, auth_header);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string rawMessage = "From: youremail@gmail.com\r\nTo: " + std::string(receiver) +
                             "\r\nSubject: " + std::string(subject) + "\r\n\r\n" + std::string(body);
    std::string encodedMessage = urlSafeBase64Encode(rawMessage);
    if (encodedMessage.empty()) {
        std::cerr << "Base64 encoding failed" << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    //Prepares the JSON body required by Gmail API, with the encoded message in the "raw" field
    std::string json_data = "{\"raw\": \"" + encodedMessage + "\"}";

    //Sets the target URL, headers, POST data, and enables verbose mode for debugging
    curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages/send");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    //Turn to 0L if don't want the data about request, just makes it easy to debug where the request failed
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "Email sent successfully!" << std::endl;
    }

    //Don't forget to free resources to avoid memory leaks
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK);
}

//The functions below this are not used anywhere just wrote for testing and not used anywhere in code

//Another approach tried to move sending emails at scheduled time  on background thread
bool scheduleEmail(const char* dateTime){
   // Parse dateTime string (expected format: YYYY-MM-DD HH:MM)
    std::tm timeToSend = {};
    std::string dt(dateTime);
        
    std::string year = dt.substr(0, 4);
    std::string month = dt.substr(5, 2);
    std::string day = dt.substr(8, 2);
    std::string hour = dt.substr(11, 2);
    std::string minute = dt.substr(14, 2);
        
    // Set the time structure
    timeToSend.tm_year = std::stoi(year) - 1900; // Year since 1900
    timeToSend.tm_mon = std::stoi(month) - 1;    // Month is 0-11
    timeToSend.tm_mday = std::stoi(day);
    timeToSend.tm_hour = std::stoi(hour);
    timeToSend.tm_min = std::stoi(minute);
    timeToSend.tm_sec = 0;

    std::time_t sendTime = std::mktime(&timeToSend);
    std::time_t now = std::time(nullptr);
    
    // Calculate the time difference in seconds
    double secondsToWait = std::difftime(sendTime, now);
    
    if (secondsToWait > 0) {
        std::cout << "Waiting to send email..." << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(abs(static_cast<int>(secondsToWait))));
        return true;
    } else {
        std::cerr << "The specified time is in the past!" << std::endl;
        return false;
    }

}

}


// Callback function to write the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

//This is to fetch each Message Details for the Messages we'd access from inbox
void fetchMessageDetails(const std::string& messageId, const std::string& accessToken) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        // Construct the URL for the specific message
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId+ "?format=FULL";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the Authorization header with the access token
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Print the raw response for debugging
            std::cout << "Message Details Response:\n" << readBuffer << std::endl;

            // Parse the JSON response
            Json::CharReaderBuilder readerBuilder;
            Json::Value message;
            std::string errs;
            std::istringstream s(readBuffer);
            if (Json::parseFromStream(readerBuilder, s, &message, &errs)) {
                // Extract sender email and message body
                if (message.isMember("payload")) {
                    const Json::Value& payload = message["payload"];
                    if (payload.isMember("headers")) {
                        for (const auto& header : payload["headers"]) {
                            if (header["name"].asString() == "From") {
                                std::cout << "Sender: " << header["value"].asString() << std::endl;
                            }
                        }
                    }
                    // Check if the body is present
                    if (payload.isMember("body")) {
                        std::string body = payload["body"]["data"].asString();
                        // Decode the base64url encoded body
                        std::cout << "Body (base64url): " << body << std::endl;
                    }
                }
            } else {
                std::cerr << "Failed to parse JSON: " << errs << std::endl;
            }
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
// Function to fetch message IDs
void fetchMessageIDs(const std::string& accessToken) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Initialize curl
    curl = curl_easy_init();
    if (curl) {
        // Set the Gmail API URL to list messages with a limit of 5
        curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages?maxResults=5");

        // Set the Authorization header with the access token
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        //headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse the JSON response
            Json::CharReaderBuilder readerBuilder;
            Json::Value root;
            std::string errs;
            std::istringstream s(readBuffer);
            if (Json::parseFromStream(readerBuilder, s, &root, &errs)) {
                // Loop through the messages
                if (root.isMember("messages")) {
                    for (const auto& message : root["messages"]) {
                        std::string messageId = message["id"].asString();
                        std::cout << "Message ID: " << messageId << std::endl;

                        std::cout << "Raw Response:\n" << readBuffer << std::endl;
                        // Fetch the details of the message
                        fetchMessageDetails(messageId, accessToken);
                    }
                }
            } else {
                std::cerr << "Failed to parse JSON: " << errs << std::endl;
            }
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Function to fetch message details using message ID


int main() {
    // Replace with your actual access token              
    std::string accessToken = "ADD_YOUR_ACCESS_TOKEN";
    std::string auth_header="Authorization: Bearer ADD_YOUR_ACCESS_TOKEN";
    std::string receiver = "Receiver@gmail.com";
    std::string subject = "Test Subject";
    std::string body = "This is a test email.";

    //sendEmail(auth_header, receiver, subject, body);    
    return 0;
}