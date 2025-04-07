#include "common.h"
#define TTL_RS_enable 4  //chân điều khiển chế độ truyền và nhận trên TTL RS485
#define RECEIVE_ENABLE 0
#define TRANSMIT_ENABLE 1
#define BUFFER_SIZE 100
#define UART_BUFFER_SIZE 40  // Kích thước mỗi mảng con
#define TOTAL_UARTS 4        // Số lượng mảng con
#define id1 "lave 1"        // esp8266 rem
#define id2 "lave 2"        // esp8266 be ca
// cac mang de gui nhan du liechar cmd[BUFFER_SIZE];
char cmd[BUFFER_SIZE];
char data_main[BUFFER_SIZE];
char data1[UART_BUFFER_SIZE];
char data2[UART_BUFFER_SIZE];
char data3[UART_BUFFER_SIZE];
char data4[UART_BUFFER_SIZE];
char *data[TOTAL_UARTS] = {
  data1,
  data2,
  data3,
  data4
};

// cac bien de xu ly truyen nhan
uint8_t indexArrCpt = 0;
uint8_t indexArr = 0;                             //vi tri luu tai mang chung
uint8_t indexReadPos = 0;                         //vi tri doc tai mang chung
uint8_t indexDataWritePos[TOTAL_UARTS] = { 0 };  //vi tri ghi cua tung mang con
int8_t indexDataSubArr = 0;                       //so thu tu mang con de ghi
uint8_t indexDataReadPos = 0;                     //vi tri doc tai mang con
bool flag = false;                                //xac nhan truyen

//cac bien du lieu

//thông số ở slave 1
int8_t curtainPosition;
uint8_t switchState;

//thông số ở slave 2
int8_t temperature;
int turbidity;
uint8_t waterPump;

//cờ xác nhận trạng thái kết nối
bool flagSL1 = false;
bool flagSL2 = false;
unsigned long ms1 = 0;
unsigned long ms2 = 0;
const unsigned long intervalTime = 30000;
bool statusSL1 = false;
bool statusSL2 = false;

void init() {
  pinMode(TTL_RS_enable, OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
}

void running() {
  static unsigned long ms=0;
  // if(millis()-ms > 500)
  {
    receive_cmd();
    send_cmd();
  }
  receive_status();
  write_SubArr();
  update();
  // Serial.print("Slave 1 led: " + String(switchState) + " - ");
  // Serial.println("curtain: " + String(curtainPosition));
  // Serial.print("Slave2 tem: " + String(temperature) +" - ");
  // Serial.println("turnidity: " + String(turbidity));
  // check_status();
  // delay(1000);
}

//Hàm nhận lệnh từ Serial
void receive_cmd() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r' || c == '\0' || c == '#') {
      cmd[indexArrCpt] = '#';
      //cờ xác nhận quá trình nhận lệnh kết thúc
      flag = true;
      indexArrCpt = 0;
      // Serial.println();
      break;
    } else 
    {
      // Serial.print(C);
      cmd[indexArrCpt++] = c;
    }
  }
}

//Hàm gửi lệnh tới các slave
void send_cmd() {
  static unsigned long ms_selectSL = 0;
  digitalWrite(TTL_RS_enable, TRANSMIT_ENABLE);
  if (flag) 
  {
    //kích hoạt chế độ truyền
    digitalWrite(TTL_RS_enable, TRANSMIT_ENABLE);
    //gửi lệnh
    // Serial.println(cmd);  //hiển thị trên monitor
    Serial2.print(cmd);
    flag = false;
    Serial2.flush();
    digitalWrite(TTL_RS_enable, RECEIVE_ENABLE);
  }
  else
  {
    if(millis() - ms_selectSL >= 500)
    {
      digitalWrite(TTL_RS_enable, TRANSMIT_ENABLE);

      ms_selectSL = millis();
      static bool selectSL = false;
      if (selectSL)
      {
      Serial2.print("!#");
      selectSL = false;
      }
      else
      {
        Serial2.print("@#");
        selectSL = true;
      }
      Serial2.flush();
      digitalWrite(TTL_RS_enable, RECEIVE_ENABLE);
    }
  }
}

void receive_status() {
  // reset();
  digitalWrite(TTL_RS_enable, RECEIVE_ENABLE);
  while (Serial2.available()) 
  {
    char c = Serial2.read();
    //loại bỏ nhiễu
    if(c=='\0')
    {
      continue;
    }
    // Serial.print(c);
    data_main[indexArr++] = c;
    if (indexArr >= BUFFER_SIZE) {
      indexArr = 0;
    }                                      
  }
  // Serial.println();
}

void write_SubArr() {
  if(indexReadPos != indexArr) {
    // Đọc dữ liệu từ mảng chung
    char databyte = data_main[indexReadPos++];
    if (indexReadPos >= BUFFER_SIZE) {
      indexReadPos = 0;
    }
    Serial.print(databyte);
    // Đảm bảo indexDataSubArr hợp lệ trước khi ghi dữ liệu
    if (indexDataSubArr < TOTAL_UARTS && indexDataWritePos[indexDataSubArr] < UART_BUFFER_SIZE ) {
      data[indexDataSubArr][indexDataWritePos[indexDataSubArr]++] = databyte;
    }

    // Khi gặp ký tự kết thúc, chuyển sang mảng con tiếp theo
    if (databyte == '#') 
    {
      Serial.println();
      data[indexDataSubArr][indexDataWritePos[indexDataSubArr]] = '\0';
      indexDataSubArr++;
      if (indexDataSubArr >= TOTAL_UARTS) 
      {
        indexDataSubArr = 0;  // Quay về mảng con đầu
      }
      indexDataWritePos[indexDataSubArr] = 0;  // Reset vị trí ghi

    }
  }
}

//cập nhật dữ liệu
void update() {
  for (int i = 0; i < TOTAL_UARTS; i++) 
  {
    //xử lý khi cuối mảng là ký tự kết thúc xâu được quy ước là #
    if (data[i][indexDataWritePos[i]-1] == '#' && data[i][0]!='\0') 
    {
      //tách ra khi gặp dấu ','
      char *token = strtok(data[i], ",");
      if (strstr(token, id1)) 
      {
        // Serial.println(id1);
        // //tiếp tục tách tới khi gặp ký tự NULL
        // while (token != nullptr) 
        // {
        //   token = strtok(nullptr, ",");
        //   Serial.println(token);
        // }
        flagSL2 = false;
        flagSL1 = true;
        ms1 = millis();
        token = strtok(nullptr, ",");
        switchState = atoi(token);
        token = strtok(nullptr, "#");
        curtainPosition = atoi(token);
        // Serial.print("led: " + String(switchState) + " - ");
        // Serial.println("curtain: " + String(curtainPosition));
      } 
      else if (strstr(token, id2)) 
      {
        // Serial.println(id2);
        // //tiếp tục tách tới khi gặp ký tự NULL
        // while (token != nullptr) 
        // {
        //   token = strtok(nullptr, ",");
        //   Serial.println(token);
        // }
        flagSL1 = false;
        flagSL2 = true;
        ms2 = millis();
        token = strtok(nullptr, ",");
        temperature = atoi(token);
        token = strtok(nullptr, "#");
        turbidity = atoi(token);
        // Serial.print("tem: " + String(temperature) +" - ");
        // Serial.println("turnidity: " + String(turbidity));
      }
      else 
      {
        flagSL1 = false;
        flagSL2 = false;
      }
      //khi mảng được xử lý xong, reset nó
      for (int j = 0; j < UART_BUFFER_SIZE; j++) 
      {
        data[i][j] = '\0';
      }
    }
  }
}

void check_status()
{
  if(flagSL1 == true)
  {
    statusSL1 = true;
  }
  else
  {
    if(millis() - ms1 >= intervalTime)
    {
      statusSL1 = false;
      Serial.println("Slave 1 disconnected");
    }
  }

  if(flagSL2 == true)
  {
    statusSL2 = true;
  }
  else
  {
    if(millis() - ms2 >= intervalTime)
    {
      statusSL2 = false;
      Serial.println("Slave 2 disconnected");
    }
  }
}
void reset() {
  indexArrCpt = 0;
  indexArr = 0;         //vị trí lưu vào mảng chung
  indexReadPos = 0;     //vị trí đọc tại mảng chung
  indexDataSubArr = 0;  //số thứ tự mảng con để ghi
  memset(cmd, 0, UART_BUFFER_SIZE);
  for (int i = 0; i < TOTAL_UARTS; i++) {
    memset(data[i], 0, UART_BUFFER_SIZE);  // Xóa toàn bộ mảng con
    memset(cmd, 0, UART_BUFFER_SIZE);
    indexDataWritePos[i] = { 0 };  // Vị trí ghi của từng mảng con
  }
  indexDataReadPos = 0;
}
