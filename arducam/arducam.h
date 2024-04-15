#ifndef MBED_ARDUCAM_H
#define MBED_ARDUCAM_H 

#include "mbed.h"
#include <cstdint>

//******************************************************************************
// Macros
//******************************************************************************
#define I2C_SEND_STOP      0x01
#define I2C_SEND_START     0x02
#define sensor_addr  0x60
#define SPAWN_TASK_PRIORITY            9//9
#define HTTP_SERVER_APP_TASK_PRIORITY  3//1
#define CAMERA_SERVICE_PRIORITY        3
#define OSI_STACK_SIZE                 8 * 1024

#define cport(port, data) port &= data
#define sport(port, data) port |= data

#define swap(type, i, j) {type t = i; i = j; j = t;}

#define fontbyte(x) cfont.font[x]  

#define regtype volatile uint32_t
#define regsize uint32_t
#define PROGMEM


    #define pgm_read_byte(x)        (*((char *)x))
//  #define pgm_read_word(x)        (*((short *)(x & 0xfffffffe)))
    #define pgm_read_word(x)        ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
    #define pgm_read_byte_near(x)   (*((char *)x))
    #define pgm_read_byte_far(x)    (*((char *)x))
//  #define pgm_read_word_near(x)   (*((short *)(x & 0xfffffffe))
//  #define pgm_read_word_far(x)    (*((short *)(x & 0xfffffffe)))
    #define pgm_read_word_near(x)   ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
    #define pgm_read_word_far(x)    ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x))))
    #define PSTR(x)  x
    #define F(X) (X)

#define BMP     0
#define JPEG    1

#define OV2640      3

#define OV2640_320x240      1   //320x240
#define OV2640_640x480      2   //640x480
#define OV2640_1280x720     3   //1280x720
#define OV2640_1920x1080    4   //1920x1080
#define OV2640_2048x1563    5   //2048x1563
#define OV2640_2592x1944    6   //2592x1944

/****************************************************/
/* I2C Control Definition                                                   */
/****************************************************/
#define I2C_ADDR_8BIT 0
#define I2C_ADDR_16BIT 1
#define I2C_REG_8BIT 0
#define I2C_REG_16BIT 1
#define I2C_DAT_8BIT 0
#define I2C_DAT_16BIT 1

/* Register initialization tables for SENSORs */
/* Terminating list entry for reg */
#define SENSOR_REG_TERM_8BIT                0xFF
#define SENSOR_REG_TERM_16BIT               0xFFFF
/* Terminating list entry for val */
#define SENSOR_VAL_TERM_8BIT                0xFF
#define SENSOR_VAL_TERM_16BIT               0xFFFF

/****************************************************/
/* ArduChip related definition                                          */
/****************************************************/
#define RWBIT                   0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1          0x00  //TEST register
#define ARDUCHIP_TEST2          0x01  //TEST register

#define ARDUCHIP_FRAMES         0x01  //Bit[2:0]Number of frames to be captured

#define ARDUCHIP_MODE           0x02  //Mode register
#define MCU2LCD_MODE            0x00
#define CAM2LCD_MODE            0x01
#define LCD2MCU_MODE            0x02

#define ARDUCHIP_TIM            0x03  //Timming control
#define HREF_LEVEL_MASK         0x01  //0 = High active ,       1 = Low active
#define VSYNC_LEVEL_MASK        0x02  //0 = High active ,       1 = Low active
#define LCD_BKEN_MASK           0x04  //0 = Enable,             1 = Disable
#define DELAY_MASK              0x08  //0 = no delay,           1 = delay one clock
#define MODE_MASK               0x10  //0 = LCD mode,           1 = FIFO mode
#define FIFO_PWRDN_MASK         0x20  //0 = Normal operation,   1 = FIFO power down
#define LOW_POWER_MODE          0x40  //0 = Normal mode,        1 = Low power mode

#define ARDUCHIP_FIFO           0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK         0x01
#define FIFO_START_MASK         0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO           0x06  //GPIO Write Register
#define GPIO_RESET_MASK         0x01  //0 = default state,      1 =  Sensor reset IO value
#define GPIO_PWDN_MASK          0x02  //0 = Sensor power down IO value, 1 = Sensor power enable IO value

#define BURST_FIFO_READ         0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ        0x3D  //Single FIFO read operation

#define ARDUCHIP_REV            0x40  //ArduCHIP revision
#define VER_LOW_MASK            0x3F
#define VER_HIGH_MASK           0xC0

#define ARDUCHIP_TRIG           0x41  //Trigger source
#define VSYNC_MASK              0x01
#define SHUTTER_MASK            0x02
#define CAP_DONE_MASK           0x08

#define FIFO_SIZE1              0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2              0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3              0x44  //Camera write FIFO size[18:16

class ArduCAM
{
    public:
        ArduCAM(PinName mosi, PinName miso, PinName sck, PinName cs, PinName sda, PinName scl);
        void InitCAM(void);
        
        void CS_HIGH(void);
        void CS_LOW(void);
        
        void flush_fifo(void);
        void start_capture(void);
        void clear_fifo_flag(void);
        uint8_t read_fifo(void);
        
        uint8_t read_reg(uint8_t addr);
        void write_reg(uint8_t addr, uint8_t data);
        
        uint32_t read_fifo_length(void);
        void set_fifo_burst(void);
        void set_bit(uint8_t addr, uint8_t bit);
        void clear_bit(uint8_t addr, uint8_t bit);
        uint8_t get_bit(uint8_t addr, uint8_t bit);
        void set_mode(uint8_t mode);
        
        int wrSensorRegs(const struct sensor_reg*);
        int wrSensorRegs8_8(const struct sensor_reg*);
        int wrSensorRegs8_16(const struct sensor_reg*);
        int wrSensorRegs16_8(const struct sensor_reg*);
        int wrSensorRegs16_16(const struct sensor_reg*);
            
        uint8_t wrSensorReg(int regID, int regDat);
        uint8_t wrSensorReg8_8(int regID, int regDat);
        uint8_t wrSensorReg8_16(int regID, int regDat);
        uint8_t wrSensorReg16_8(int regID, int regDat);
        uint8_t wrSensorReg16_16(int regID, int regDat);
            
        uint8_t rdSensorReg8_8(uint8_t regID, uint8_t* regDat);
        uint8_t rdSensorReg16_8(uint16_t regID, uint8_t* regDat);
        uint8_t rdSensorReg8_16(uint8_t regID, uint16_t* regDat);
        uint8_t rdSensorReg16_16(uint16_t regID, uint16_t* regDat);
        
        void OV2640_set_JPEG_size(uint8_t size);
        void set_format(uint8_t fmt);
        
        int bus_write(int address, int value);
        uint8_t bus_read(int address);  
        
    protected:
        SPI spi;                // does SPI MOSI, MISO and SCK        
        DigitalOut _cs;         // does SPI CE
        I2C i2c;                // does I2C SDA, SCL
        regtype *P_CS;
        regsize B_CS;
        uint8_t m_fmt;
        uint8_t sensor_model;
        
        uint32_t I2CInit(void);
        int32_t I2CBufferRead(int32_t ucDevAddr, uint8_t *ucBuffer, int32_t ulSize,unsigned char ucFlags);
        int32_t I2CBufferWrite(int32_t ucDevAddr, uint8_t *ucBuffer, int32_t ulSize,unsigned char ucFlags);
};        

#endif