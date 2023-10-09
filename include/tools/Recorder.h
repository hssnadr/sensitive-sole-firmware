#if !defined(_MOVUINO_RECORD_MANAGER_)
#define _MOVUINO_RECORD_MANAGER_

#include <Arduino.h>
#include <SPIFFS.h>

class Recorder
{
private:
    String _dirPath = "/records";
    String _fileName;
    String _filePath;

    File file;
    char sep = ',';
    bool _initRow = false; // avoid pushdata on same row as columns definition
    int _nRow = 0;         // count recorded row data

    long unsigned _timeRecord0;
    bool _isRecording = false;

    bool isEditable = false;
    bool isReadable = false;
    bool formatted;

    void initRecordFile();

public:
    Recorder();
    ~Recorder();

    void begin();

    void newRecord(String fileName_);
    void defineColumns(std::initializer_list<String> cols_);
    void newRow();

    template <typename T>
    void pushData(T data_);

    template <typename T>
    void pushData(std::initializer_list<T> array_);

    void stop();
    bool isRecording()
    {
        return this->_isRecording;
    };

    int getNumberRecords();

    void addNewRecord();
    void readAllRecords();
    void readRecord(String filePath_);
    void writeData();

    void deleteAllRecords();
    void listRecords();
};

Recorder::Recorder()
{
}

Recorder::~Recorder()
{
}

void Recorder::begin()
{
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

void Recorder::newRecord(String fileName_ = "untitled")
{
    /*
     * Create the file for the movuino
     */
    // add current file index
    int indx_ = this->getNumberRecords();
    char indxChar_[3];
    sprintf(indxChar_, "%03d", indx_);
    this->_fileName = indxChar_[0];
    this->_fileName += indxChar_[1];
    this->_fileName += indxChar_[2];

    // add file name
    this->_fileName += "_" + fileName_;
    this->_filePath = this->_dirPath + "/" + this->_fileName + ".txt";
    this->file = SPIFFS.open(this->_filePath, "w");

    if (!this->file)
    {
        Serial.println("Error opening file for writing");
        return;
    }

    Serial.print("Create ");
    Serial.println(this->_filePath);
    this->_isRecording = true;
    this->_timeRecord0 = millis();
    this->_initRow = true;
    this->_nRow = 0;
    Serial.println("Start recording...");
}

void Recorder::stop()
{
    if (_isRecording)
    {
        this->file.close();
        this->_isRecording = false;

        Serial.print("Stop recording in ");
        Serial.println(this->_filePath);
        Serial.print("Recorded ");
        Serial.print(this->_nRow);
        Serial.println(" data row(s)");
    }
}

void Recorder::readAllRecords()
{
    File dir = SPIFFS.open(this->_dirPath);
    this->file = dir.openNextFile();
    Serial.println("XXX_beginning");
    bool isFirst_ = true;
    while (this->file)
    {
        if (!isFirst_)
            Serial.println("XXX_newRecord");
        this->readRecord(this->file.name());
        this->file = dir.openNextFile();
        isFirst_ = false;
    }
    Serial.println("XXX_end");
}

void Recorder::deleteAllRecords()
{
    File dir = SPIFFS.open(this->_dirPath);
    this->file = dir.openNextFile();
    Serial.println("Start deleting all records...");
    int count_ = 0;
    while (this->file)
    {
        Serial.printf("  delete file: %s\r\n", this->file.name());
        String filePath_ = this->_dirPath + "/" + this->file.name();
        SPIFFS.remove(filePath_);
        this->file = dir.openNextFile();
        count_++;
    }
    Serial.printf("Done. %i records deleted.\r\n", count_);
}

void Recorder::readRecord(String fileName_)
{
    String filePath_ = this->_dirPath + "/" + fileName_;
    File file_ = SPIFFS.open(filePath_, "r");

    if (!file_)
    {
        Serial.println("Error opening file for reading");
        return;
    }

    String l_ = "";
    while (file_.available())
    {
        char c_ = file_.read();
        if (c_ != '\n')
        {
            l_ += c_;
        }
        else
        {
            Serial.println(l_);
            l_ = "";
        }
    }
    if (l_ != "")
        Serial.println(l_);
    file_.close();
}

void Recorder::defineColumns(std::initializer_list<String> cols_)
{
    this->file.print("time");
    for (auto col : cols_)
    {
        this->file.print(this->sep);
        this->file.print(col);
    }
    this->_initRow = false;
}

void Recorder::newRow()
{
    this->file.println();
    this->file.print(millis() - this->_timeRecord0);
    if (!this->_initRow)
        this->_initRow = true;
    this->_nRow++;
}

template <typename T>
void Recorder::pushData(T data_)
{
    if (this->_isRecording)
    {
        if (!this->_initRow)
            this->newRow();
        this->file.print(this->sep);
        this->file.print(data_);
    }
}

template <typename T>
void Recorder::pushData(std::initializer_list<T> array_)
{
    if (this->_isRecording)
    {
        if (!this->_initRow)
            this->newRow();
        for (auto data_ : array_)
        {
            this->file.print(this->sep);
            this->file.print(data_);
        }
    }
}

void Recorder::listRecords()
{
    /*
     * Print the directory of the spiffs and the size of each file
     */
    Serial.println("Listing dir :");
    File dir = SPIFFS.open(this->_dirPath);
    this->file = dir.openNextFile();
    while (this->file)
    {
        Serial.print(this->file.name());
        Serial.print(" ");
        Serial.println(this->file.size());
        this->file = dir.openNextFile();
    }
    Serial.println("End of listing");
}

int Recorder::getNumberRecords()
{
    int nFile_ = 0;
    File dir = SPIFFS.open(this->_dirPath);
    this->file = dir.openNextFile();
    while (this->file)
    {
        nFile_++;
        this->file = dir.openNextFile();
    }
    return nFile_;
}

#endif // _MOVUINO_RECORD_MANAGER_
