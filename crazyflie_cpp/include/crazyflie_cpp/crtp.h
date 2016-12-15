#pragma once

// Header
struct crtp
{
  constexpr crtp(uint8_t port, uint8_t channel)
    : channel(channel)
    , link(3)
    , port(port)
  {
  }

  crtp(uint8_t byte)
  {
    channel = (byte >> 0) & 0x3;
    link    = (byte >> 2) & 0x3;
    port    = (byte >> 4) & 0xF;
  }

  bool operator==(const crtp& other) {
    return channel == other.channel && port == other.port;
  }

  uint8_t channel:2;
  uint8_t link:2;
  uint8_t port:4;
} __attribute__((packed));

// Port 0 (Console)
struct crtpConsoleResponse
{
    static bool match(const Crazyradio::Ack& response) {
      return crtp(response.data[0]) == crtp(0, 0);
    }

    crtp header;
    char text[31];
};

// Port 2 (Parameters)

struct crtpParamTocGetItemRequest
{
  crtpParamTocGetItemRequest(
    uint8_t id)
    : header(2, 0)
    , command(0)
    , id(id)
    {
    }

    const crtp header;
    const uint8_t command;
    uint8_t id;
} __attribute__((packed));

struct crtpParamTocGetItemResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size > 5 &&
           crtp(response.data[0]) == crtp(2, 0) &&
           response.data[1] == 0;
  }

  crtpParamTocGetItemRequest request;
  uint8_t length:2; // one of ParamLength
  uint8_t type:1;   // one of ParamType
  uint8_t sign:1;   // one of ParamSign
  uint8_t res0:2;   // reserved
  uint8_t readonly:1;
  uint8_t group:1;  // one of ParamGroup
  char text[28]; // group, name
} __attribute__((packed));

struct crtpParamTocGetInfoRequest
{
  crtpParamTocGetInfoRequest()
    : header(2, 0)
    , command(1)
    {
    }

    const crtp header;
    const uint8_t command;
} __attribute__((packed));

struct crtpParamTocGetInfoResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size == 7 &&
           crtp(response.data[0]) == crtp(2, 0) &&
           response.data[1] == 1;
  }

  crtpParamTocGetInfoRequest request;
  uint8_t numParam;
  uint32_t crc;
} __attribute__((packed));

struct crtpParamReadRequest
{
  crtpParamReadRequest(
    uint8_t id)
    : header(2, 1)
    , id(id)
    {
    }

    const crtp header;
    const uint8_t id;
} __attribute__((packed));

template <class T>
struct crtpParamWriteRequest
{
  crtpParamWriteRequest(
    uint8_t id,
    const T& value)
    : header(2, 2)
    , id(id)
    , value(value)
    {
    }

    const crtp header;
    const uint8_t id;
    const T value;
} __attribute__((packed));

struct crtpParamValueResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size > 2 &&
           (crtp(response.data[0]) == crtp(2, 1) ||
            crtp(response.data[0]) == crtp(2, 2));
  }

  crtp header;
  uint8_t id;
  union {
    uint8_t valueUint8;
    int8_t valueInt8;
    uint16_t valueUint16;
    int16_t valueInt16;
    uint32_t valueUint32;
    int32_t valueInt32;
    float valueFloat;
  };
} __attribute__((packed));

// Port 3 (Commander)

struct crtpSetpointRequest
{
  crtpSetpointRequest(
    float roll,
    float pitch,
    float yawrate,
    uint16_t thrust)
    : header(0x03, 0)
    , roll(roll)
    , pitch(pitch)
    , yawrate(yawrate)
    , thrust(thrust)
  {
  }
  const crtp header;
  float roll;
  float pitch;
  float yawrate;
  uint16_t thrust;
}  __attribute__((packed));

// Port 4 (Memory access)

// Port 5 (Data logging)

struct crtpLogGetInfoRequest
{
  crtpLogGetInfoRequest()
    : header(5, 0)
    , command(1)
    {
    }

    const crtp header;
    const uint8_t command;
} __attribute__((packed));

struct crtpLogGetInfoResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size == 9 &&
           crtp(response.data[0]) == crtp(5, 0) &&
           response.data[1] == 1;
  }

  crtpLogGetInfoRequest request;
  // Number of log items contained in the log table of content
  uint8_t log_len;
  // CRC values of the log TOC memory content. This is a fingerprint of the copter build that can be used to cache the TOC
  uint32_t log_crc;
  // Maximum number of log packets that can be programmed in the copter
  uint8_t log_max_packet;
  // Maximum number of operation programmable in the copter. An operation is one log variable retrieval programming
  uint8_t log_max_ops;
} __attribute__((packed));

struct crtpLogGetItemRequest
{
  crtpLogGetItemRequest(uint8_t id)
    : header(5, 0)
    , command(0)
    , id(id)
    {
    }

    const crtp header;
    const uint8_t command;
    uint8_t id;
} __attribute__((packed));

struct crtpLogGetItemResponse
{
    static bool match(const Crazyradio::Ack& response) {
      return response.size > 5 &&
             crtp(response.data[0]) == crtp(5, 0) &&
             response.data[1] == 0;
    }

    crtpLogGetItemRequest request;
    uint8_t type;
    char text[28]; // group, name
} __attribute__((packed));

struct logBlockItem {
  uint8_t logType;
  uint8_t id;
} __attribute__((packed));

struct crtpLogCreateBlockRequest
{
  crtpLogCreateBlockRequest()
  : header(5, 1)
  , command(0)
  {
  }

  const crtp header;
  const uint8_t command;
  uint8_t id;
  logBlockItem items[16];
} __attribute__((packed));

// struct logAppendBlockRequest
// {
//   logAppendBlockRequest()
//     : header(5, 1)
//     , command(1)
//     {
//     }

//     const crtp header;
//     const uint8_t command;
//     uint8_t id;
//     logBlockItem items[16];
// } __attribute__((packed));

// struct logDeleteBlockRequest
// {
//   logDeleteBlockRequest()
//     : header(5, 1)
//     , command(2)
//     {
//     }

//     const crtp header;
//     const uint8_t command;
//     uint8_t id;
// } __attribute__((packed));

struct crtpLogStartRequest
{
  crtpLogStartRequest(
    uint8_t id,
    uint8_t period)
    : header(5, 1)
    , command(3)
    , id(id)
    , period(period)
    {
    }

    const crtp header;
    const uint8_t command;
    uint8_t id;
    uint8_t period; // in increments of 10ms
} __attribute__((packed));

struct crtpLogStopRequest
{
  crtpLogStopRequest(
    uint8_t id)
    : header(5, 1)
    , command(4)
    , id(id)
    {
    }

    const crtp header;
    const uint8_t command;
    uint8_t id;
} __attribute__((packed));

struct crtpLogResetRequest
{
  crtpLogResetRequest()
    : header(5, 1)
    , command(5)
    {
    }

    const crtp header;
    const uint8_t command;
} __attribute__((packed));

enum crtpLogControlResult {
  crtpLogControlResultOk            = 0,
  crtpLogControlResultOutOfMemory   = 12, // ENOMEM
  crtpLogControlResultCmdNotFound   = 8,  // ENOEXEC
  crtpLogControlResultWrongBlockId  = 2,  // ENOENT
  crtpLogControlResultBlockTooLarge = 7,  // E2BIG
  crtpLogControlResultBlockExists   = 17, // EEXIST

};

struct crtpLogControlResponse
{
    static bool match(const Crazyradio::Ack& response) {
      return response.size == 4 &&
             crtp(response.data[0]) == crtp(5, 1);
    }

    crtp header;
    uint8_t command;
    uint8_t requestByte1;
    uint8_t result; // one of crtpLogControlResult
} __attribute__((packed));

struct crtpLogDataResponse
{
    static bool match(const Crazyradio::Ack& response) {
      return response.size > 4 &&
             crtp(response.data[0]) == crtp(5, 2);
    }

    crtp header;
    uint8_t blockId;
    uint8_t timestampLo;
    uint16_t timestampHi;
    uint8_t data[26];
} __attribute__((packed));


// Port 0x06 (External Position Update)

struct crtpExternalPositionUpdate
{
  crtpExternalPositionUpdate(
    float x,
    float y,
    float z)
    : header(0x06, 0)
    , x(x)
    , y(y)
    , z(z)
  {
  }
  const crtp header;
  float x;
  float y;
  float z;
}  __attribute__((packed));

// Port 0x07 (Control)

extern uint16_t single2half(float number);

struct crtpLinearControlReference {
  crtpLinearControlReference(
    float _pos,
    float _vel,
    float _acc,
    float _jerk)
  {
    pos = single2half(_pos);
    vel = single2half(_vel);
    acc = single2half(_acc);
    jerk = single2half(_jerk);
  }
  uint16_t pos; // use uint16_t to hold float16_t
  uint16_t vel; // use uint16_t to hold float16_t
  uint16_t acc;
  uint16_t jerk;
} __attribute__((packed));

struct crtpAngularControlReference {
  crtpAngularControlReference(
    float _pos,
    float _vel)
  {
    pos = (_pos);
    vel = single2half(_vel);
  }
  uint16_t pos; // use uint16_t to hold float16_t
  uint16_t vel; // use uint16_t to hold float16_t
} __attribute__((packed)) ;

struct crtpControlPacket {
  crtpControlPacket(
    bool enable,
    float xpos, float xvel, float xacc, float xjerk,
    float ypos, float yvel, float yacc, float yjerk,
    float zpos, float zvel, float zacc, float zjerk,
    float yawpos, float yawvel)
  : header(0x07,0)
  , enable(enable)
  , x(xpos, xvel, xacc, xjerk)
  , y(ypos, yvel, yacc, yjerk)
  , z(zpos, zvel, zacc, zjerk)
  , yaw(yawpos, yawvel)
  {
  }

  const crtp header;
  uint8_t enable;
  crtpLinearControlReference x; // size 8
  crtpLinearControlReference y; // size 8
  crtpLinearControlReference z; // size 8
  crtpAngularControlReference yaw; // size 4
} __attribute__((packed)) ;

// Port 0x08 (sequence commander)
struct crtpPointPacket {
  crtpPointPacket(
    uint8_t packetType,
    uint8_t mode,
    float xpos, float xvel, float xacc, float xjerk,
    float ypos, float yvel, float yacc, float yjerk,
    float zpos, float zvel, float zacc, float zjerk,
    float yawpos, float yawvel)
  : header(0x08,0)
  , packetType(packetType)
  , mode(mode)
  , x(xpos, xvel, xacc, xjerk)
  , y(ypos, yvel, yacc, yjerk)
  , z(zpos, zvel, zacc, zjerk)
  , yaw(yawpos, yawvel)
  {
  }
  const crtp header;
  uint8_t packetType;
  uint8_t mode;
  crtpLinearControlReference x;
  crtpLinearControlReference y;
  crtpLinearControlReference z;
  crtpAngularControlReference yaw;
} __attribute__((packed)) ;

struct crtpTrajectoryPacket {
  crtpTrajectoryPacket(
    uint8_t packetType,
    float _data0, float _data1, float _data2, float _data3, float _data4, float _data5,
    float _time,
    uint8_t index,
    uint8_t dimension,
    uint8_t number,
    uint8_t type)
  : header(0x08,0),
  packetType(packetType),
  index(index),
  dimension(dimension),
  number(number),
  type(type)
  {
    data0 = single2half(_data0);
    data1 = single2half(_data1);
    data2 = single2half(_data2);
    data3 = single2half(_data3);
    data4 = single2half(_data4);
    data5 = single2half(_data5);
    time = single2half(_time);
  }
  const crtp header;
  uint8_t packetType;
  uint16_t data0;
  uint16_t data1;
  uint16_t data2;
  uint16_t data3;
  uint16_t data4;
  uint16_t data5;
  uint16_t time;
  uint8_t index;
  uint8_t dimension;
  uint8_t number;
  uint8_t type;
} __attribute__((packed));

struct crtpSynchronizationPacket {
  crtpSynchronizationPacket(
    uint8_t packetType,
    uint8_t synchronize,
    uint8_t circular0, uint8_t circular1, uint8_t circular2, uint8_t circular3,
    uint8_t number0, uint8_t number1, uint8_t number2, uint8_t number3, 
    float _time0, float _time1, float _time2, float _time3)
  : header(0x08,0),
  packetType(packetType),
  synchronize(synchronize),
  circular0(circular0),
  circular1(circular1),
  circular2(circular2),
  circular3(circular3),
  number0(number0),
  number1(number1),
  number2(number2),
  number3(number3)
  {
    time0 = single2half(_time0);
    time1 = single2half(_time1);
    time2 = single2half(_time2);
    time3 = single2half(_time3);
  }
  const crtp header;
  uint8_t packetType;
  uint8_t synchronize;
  uint8_t circular0;
  uint8_t circular1;
  uint8_t circular2;
  uint8_t circular3;
  uint8_t number0;
  uint8_t number1;
  uint8_t number2;
  uint8_t number3;
  uint16_t time0;
  uint16_t time1;
  uint16_t time2;
  uint16_t time3;
} __attribute__((packed));

// Port 13 (Platform)

// The crazyflie-nrf firmware sends empty packets with the signal strength, if nothing else is in the queue
struct crtpPlatformRSSIAck
{
    static bool match(const Crazyradio::Ack& response) {
      return crtp(response.data[0]) == crtp(15, 3);
    }

    crtp header;
    uint8_t reserved;
    uint8_t rssi;
};
