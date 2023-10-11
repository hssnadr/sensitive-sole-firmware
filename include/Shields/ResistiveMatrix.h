#if !defined(_MOVUINOESP32_RESISTIVEMATRIX_)
#define _MOVUINOESP32_RESISTIVEMATRIX_

#include <Arduino.h>

// row MUX pins
#define S0 13
#define S1 4
#define S2 15
#define S3 14
#define pinSigRow 12
int rowCtrlPin[] = {S0, S1, S2, S3};

// col MUX pins
#define A 33
#define B 32
#define C 26
#define D 27
#define pinSigCol 34 // 25
int colCtrlPin[] = {A, B, C, D};

int muxChannel[16][4] = {{0, 0, 0, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 0, 1}, {1, 1, 0, 1}, {0, 0, 1, 1}, {1, 0, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 1}};

class MovuinoResistiveMatrix
{
private:
  int _cols = 0; // Number of cols
  int _rows = 0; // Number of rows

  int _sensorValue = 0;

  int **_lastUpdate; // store read values

public:
  MovuinoResistiveMatrix(int cols, int rows);
  ~MovuinoResistiveMatrix();

  void begin();
  void update();
  String printRow(int row_);
  int readMux(int channel);
  int getValue(int col, int row);
  int rows() { return this->_rows; }
  int cols() { return this->_cols; }
};

MovuinoResistiveMatrix::MovuinoResistiveMatrix(int cols, int rows)
{
  this->_cols = cols;
  this->_rows = rows;
  this->_lastUpdate = new int *[cols];
  for (int i = 0; i < cols; i++)
  {
    _lastUpdate[i] = new int[rows];
  }
}

MovuinoResistiveMatrix::~MovuinoResistiveMatrix()
{
}

void MovuinoResistiveMatrix::begin()
{
  //--------------------------------
  //-------MULTIPLEXER row----------
  //--------------------------------
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(pinSigRow, OUTPUT);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  digitalWrite(pinSigRow, LOW);

  //--------------------------------
  //------MULTIPLEXER col-----------
  //--------------------------------
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(pinSigCol, INPUT);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(pinSigCol, 0); // Pull down
}

void MovuinoResistiveMatrix::update()
{
  // long toRemove_ = millis() / 1000.0; // remove

  for (int row = 0; row < this->_rows; row++)
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(rowCtrlPin[i], muxChannel[row][i]);
    } // read the value at the SIG pin

    // ACTIVATE row
    digitalWrite(pinSigRow, HIGH);

    // READ COL VALUES
    for (int col = 0; col < this->_cols; col++)
    {
      // this->_lastUpdate[col][row] = this->readMux(col);
      int offset_ = (row + col) * 100;
      this->_lastUpdate[col][row] = 1024 * (1.0 + cos(1.4 * millis() / 1000.0 + offset_)) / 2.; // fake data
    }

    // DEACTIVATE row
    digitalWrite(pinSigRow, LOW);
  }
}

int MovuinoResistiveMatrix::readMux(int col)
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(colCtrlPin[i], muxChannel[col][i]);
  } // read the value at the SIG pin

  int val = analogRead(pinSigCol); // return the value
  return val;
}

String MovuinoResistiveMatrix::printRow(int row_)
{
  String data_ = "z";
  data_ += String(row_);

  for (int j = 0; j < this->_cols; j++)
  {
    data_ += "x";
    data_ += String(this->_lastUpdate[j][row_]);
  }
  return data_;
}

int MovuinoResistiveMatrix::getValue(int col, int row)
{
  return _lastUpdate[col][row];
}

#endif