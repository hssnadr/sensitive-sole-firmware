#if !defined(_ESP32_SPIFFS_)
#define _ESP32_SPIFFS_

#include <SPIFFS.h>

static bool isSPIFFS = false; // move to globals ?

// Initialize SPIFFS
static bool startSPIFFS()
{
    isSPIFFS = false;
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
        return false;
    }
    Serial.println("SPIFFS mounted successfully");
    isSPIFFS = true;
    return true;
}

void printDirectories() {
    Serial.println("-----------------------");
    Serial.println("Listing directories :");
    File dir = SPIFFS.open("/");
    File file = dir.openNextFile();
    while (file)
    {
        Serial.print(" - ");
        Serial.print(file.name());
        Serial.print(" ");
        Serial.println(file.size());
        file = dir.openNextFile();
    }
    Serial.println("-----------------------");
}

bool isFile(const char *path) {
    File file = SPIFFS.open(path);
    return !(!file || file.isDirectory());
}

String readFile(const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = SPIFFS.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("Failed to open file for reading");
        return String();
    }

    String fileContent = "";
    fileContent = file.readStringUntil('\n');
    while (file.available())
    {
        fileContent += " ";
        fileContent += file.readStringUntil('\n'); // maybe not the best way (cf. multine)
    }

    Serial.printf("Content: %s\r\n", fileContent);
    Serial.println(fileContent);

    return fileContent;
}

bool writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return false;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
        return true;
    }
    else
    {
        Serial.println("- write failed");
        return false;
    }
}

static bool formatSPIFFS()
{
    bool formatted_ = SPIFFS.format();
    if (formatted_)
        Serial.println("\nSuccess formatting");
    else
        Serial.println("\nError formatting");
    return formatted_;
}

void printStateSPIFFS()
{
    Serial.println("File system info");

    // Taille de la zone de fichier
    Serial.print("Total space:      ");
    Serial.print(SPIFFS.totalBytes());
    Serial.println("byte");

    // Espace total utilise
    Serial.print("Total space used: ");
    Serial.print(SPIFFS.usedBytes());
    Serial.println("byte");
    Serial.println();
}

#endif