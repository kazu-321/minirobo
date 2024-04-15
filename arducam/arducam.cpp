#include "ThisThread.h"
#include "mbed.h"
#include "ov2640_regs.h"
#include "arducam.h"
#include <cstdint>

#define ENABLE_JPEG

ArduCAM::ArduCAM(PinName mosi, PinName miso, PinName sck, PinName cs, PinName sda, PinName scl) 
        : spi(mosi,miso,sck), _cs(cs), i2c(sda, scl)
{
    _cs = 1;
}

//Assert CS signal
void ArduCAM::CS_LOW(void)
{
    _cs = 0;
}

//Disable CS signal
void ArduCAM::CS_HIGH(void)
{
    _cs = 1;
}

//Reset the FIFO pointer to ZERO        
void ArduCAM::flush_fifo(void)
{
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

//Send capture command
void ArduCAM::start_capture(void)
{
    write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

//Clear FIFO Complete flag
void ArduCAM::clear_fifo_flag(void)
{
    write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

//Read FIFO single      
uint8_t ArduCAM::read_fifo(void)
{
    uint8_t data;
    data = bus_read(SINGLE_FIFO_READ);
    return data;
}

//Read Write FIFO length
//Support ArduCAM Mini only
uint32_t ArduCAM::read_fifo_length(void)
{
    uint32_t len1,len2,len3,length=0;
    len1 = read_reg(FIFO_SIZE1);
  len2 = read_reg(FIFO_SIZE2);
  len3 = read_reg(FIFO_SIZE3) & 0x07;
  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07ffff;
    return length;
}

//Send read fifo burst command
//Support ArduCAM Mini only
void ArduCAM::set_fifo_burst()
{
    spi.write(BURST_FIFO_READ);
}



void ArduCAM::InitCAM()
{
    uint8_t reg_val;
    wrSensorReg8_8(0xff, 0x01);
    wrSensorReg8_8(0x12, 0x80);

    ThisThread::sleep_for(100ms);
    if(m_fmt == JPEG)
    {
        wrSensorRegs8_8(OV2640_JPEG_INIT);
        wrSensorRegs8_8(OV2640_YUV422);
        wrSensorRegs8_8(OV2640_JPEG);
        wrSensorReg8_8(0xff, 0x01);
        wrSensorReg8_8(0x15, 0x00);
        wrSensorRegs8_8(OV2640_320x240_JPEG);
        // wrSensorReg8_8(0xff, 0x00);
        // wrSensorReg8_8(0x44, 0x32);
    }
    else
    {
        wrSensorRegs8_8(OV2640_QVGA);
    }
}

//Write ArduChip internal registers
void ArduCAM::write_reg(uint8_t addr, uint8_t data)
{
    bus_write(addr | 0x80, data);
}

//Read ArduChip internal registers
uint8_t ArduCAM::read_reg(uint8_t addr)
{
    uint8_t data;
    data = bus_read(addr & 0x7F);
    return data;
}

//Set corresponding bit  
void ArduCAM::set_bit(uint8_t addr, uint8_t bit)
{
    uint8_t temp;
    temp = read_reg(addr);
    write_reg(addr, temp | bit);

}

//Clear corresponding bit 
void ArduCAM::clear_bit(uint8_t addr, uint8_t bit)
{
    uint8_t temp;
    temp = read_reg(addr);
    write_reg(addr, temp & (~bit));
}

//Get corresponding bit status
uint8_t ArduCAM::get_bit(uint8_t addr, uint8_t bit)
{
    uint8_t temp;
    temp = read_reg(addr);
    temp = temp & bit;
    return temp;
}

//Set ArduCAM working mode
//MCU2LCD_MODE: MCU writes the LCD screen GRAM
//CAM2LCD_MODE: Camera takes control of the LCD screen
//LCD2MCU_MODE: MCU read the LCD screen GRAM
void ArduCAM::set_mode(uint8_t mode)
{
    switch(mode)
    {
        case MCU2LCD_MODE:
            write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
            break;
        case CAM2LCD_MODE:
            write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);
            break;
        case LCD2MCU_MODE:
            write_reg(ARDUCHIP_MODE, LCD2MCU_MODE);
            break;
        default:
            write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
            break;
    }
}

 
//I2C Write 8bit address, 8bit data
uint8_t ArduCAM::wrSensorReg8_8(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID;
    buff[1] = regDat;
    I2CBufferWrite(sensor_addr,buff,2, I2C_SEND_STOP);
 
//    ThisThread::sleep_for(1);
  return(1);
}
 
//I2C Read 8bit address, 8bit data
uint8_t ArduCAM::rdSensorReg8_8(uint8_t regID, uint8_t* regDat)
{
    uint8_t buff[20];
    buff[0] = regID;
    
    I2CBufferWrite(sensor_addr,buff,1, I2C_SEND_STOP);
    I2CBufferRead(sensor_addr+1,buff,1, I2C_SEND_STOP);
    *regDat = buff[0];
 
 //   ThisThread::sleep_for(1);
  return(1);
}
 
//I2C Write 8bit address, 16bit data
uint8_t ArduCAM::wrSensorReg8_16(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID;
    buff[1] = regDat >> 8;
    buff[2] = regDat & 0x00FF;
    I2CBufferWrite(sensor_addr,buff,3, I2C_SEND_STOP);
 
//    ThisThread::sleep_for(1);
    return(1);
}
 
//I2C Read 8bit address, 16bit data
uint8_t ArduCAM::rdSensorReg8_16(uint8_t regID, uint16_t* regDat)
{
//    uint8_t temp;
    uint8_t buff[20];
    buff[0] = regID;
    
    I2CBufferWrite(sensor_addr,buff,1, I2C_SEND_STOP);
    I2CBufferRead(sensor_addr+1,buff, 2, I2C_SEND_STOP);
    *regDat = (buff[0] << 8) | buff[1];
 
//    ThisThread::sleep_for(1);
    return(1);
}
 
//I2C Write 16bit address, 8bit data
uint8_t ArduCAM::wrSensorReg16_8(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    buff[2] = regDat;
    I2CBufferWrite(sensor_addr,buff,3, I2C_SEND_STOP);
 
//    ThisThread::sleep_for(1);
  return(1);
}
 
//I2C Read 16bit address, 8bit data
uint8_t ArduCAM::rdSensorReg16_8(uint16_t regID, uint8_t* regDat)
{
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    I2CBufferWrite(sensor_addr,buff,2, I2C_SEND_STOP);
    I2CBufferRead(sensor_addr+1,buff, 1, I2C_SEND_STOP);
    *regDat = buff[0];
 
//    ThisThread::sleep_for(1);
  return(1);
}
 
//I2C Write 16bit address, 16bit data
uint8_t ArduCAM::wrSensorReg16_16(int regID, int regDat)
{
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    buff[2] = regDat >> 8;
    buff[3] = regID & 0x00FF;
    I2CBufferWrite(sensor_addr,buff,4, I2C_SEND_STOP);
 
//    ThisThread::sleep_for(1);
  return(1);
}
 
//I2C Read 16bit address, 16bit data
uint8_t ArduCAM::rdSensorReg16_16(uint16_t regID, uint16_t* regDat)
{
//    uint8_t temp;
    uint8_t buff[20];
    buff[0] = regID >> 8;
    buff[1] = regID & 0x00FF;
    I2CBufferWrite(sensor_addr,buff,2, I2C_SEND_STOP);
    I2CBufferRead(sensor_addr+1,buff, 1, I2C_SEND_STOP);
    *regDat = (buff[0] << 8) | buff[1];
 
//    ThisThread::sleep_for(1);
  return(1);
}
 
//I2C Array Write 8bit address, 8bit data
int ArduCAM::wrSensorRegs8_8(const struct sensor_reg reglist[])
{
//    int err = 0;
    uint16_t reg_addr = 0;
    uint16_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xff) | (reg_val != 0xff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg8_8(reg_addr, reg_val);
    next++;
        
    } 
    
    return 1;
}
 
//I2C Array Write 8bit address, 16bit data
int ArduCAM::wrSensorRegs8_16(const struct sensor_reg reglist[])
{
//    int err = 0;
    
    uint32_t reg_addr = 0;
    uint32_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xff) | (reg_val != 0xffff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg8_16(reg_addr, reg_val);
        //  if (!err)
        //return err;
        next++;
    }  
    
    return 1;
}
 
//I2C Array Write 16bit address, 8bit data
int ArduCAM::wrSensorRegs16_8(const struct sensor_reg reglist[])
{
//    int err = 0;
    
    uint32_t reg_addr = 0;
    uint8_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xffff) | (reg_val != 0xff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg16_8(reg_addr, reg_val);
        //if (!err)
        //return err;
       next++;
    } 
    
    return 1;
}
 
//I2C Array Write 16bit address, 16bit data
int ArduCAM::wrSensorRegs16_16(const struct sensor_reg reglist[])
{
//    int err = 0;
    
    uint32_t reg_addr = 0;
    uint32_t reg_val = 0;
    const struct sensor_reg *next = reglist;
    
    while ((reg_addr != 0xffff) | (reg_val != 0xffff))
    {       
        reg_addr = next->reg;
        reg_val = next->val;
        wrSensorReg16_16(reg_addr, reg_val);
        //if (!err)
        //   return err;
       next++;
    } 
    
    return 1;
}

void ArduCAM::OV2640_set_JPEG_size(uint8_t size)
{
    wrSensorRegs16_8(OV2640_JPEG_INIT); 
    ThisThread::sleep_for(100ms);
    wrSensorRegs16_8(OV2640_QVGA);
}

void ArduCAM::set_format(uint8_t fmt)
{
    if(fmt == BMP)
        m_fmt = BMP;
    else
        m_fmt = JPEG;
}

int ArduCAM::bus_write(int address, int value)
{
    // take the SS pin low to select the chip:
    _cs = 0;
    ThisThread::sleep_for(100ms);
    //  send in the address and value via SPI:
    spi.write(address);
    spi.write(value);
    // take the SS pin high to de-select the chip:
    _cs = 1;
    
    return value;
}

uint8_t ArduCAM::bus_read(int address)
{ 
    uint8_t value = 0;
    // take the SS pin low to select the chip:
    _cs = 0;
    //  send in the address and value via SPI:
    spi.write(address);
    value = spi.write(0x00);
    // take the SS pin high to de-select the chip:
    _cs = 1;
    return value;
}

uint32_t ArduCAM::I2CInit()
{
 
    i2c.frequency (100000);
 
    return 0;
}

int32_t ArduCAM::I2CBufferRead(int32_t ucDevAddr, uint8_t *ucBuffer,
                            int32_t ulSize, unsigned char ucFlags)
{
//  Uart_Write((uint8_t*)"I2CBufferRead \n\r");
   
    i2c.start();       
    
    // Set I2C slave read address
    i2c.write(ucDevAddr);
    if(ulSize == 1){
        ucBuffer[0] = i2c.read(0);
    }else{  
        for(int i=0;i<ulSize;i++){
           ucBuffer[i] = i2c.read(0);
        }
    }    
    
    i2c.stop();

/*    int32_t err = 0;
    
//    RTOS_MUTEX_ACQUIRE(&g_i2cLock);
        
    err = i2c.read(ucDevAddr,(char*)ucBuffer,ulSize);
    if(err == 1){
       Uart_Write((uint8_t*)"Return error I2C read\n\r");
//       RTOS_MUTEX_RELEASE(&g_i2cLock);
       return -1;
    }
    ThisThread::sleep_for(1);
//    RTOS_MUTEX_RELEASE(&g_i2cLock);   */
    return 0;
}

int32_t ArduCAM::I2CBufferWrite(int32_t ucDevAddr, uint8_t *ucBuffer,
                             int32_t ulSize,unsigned char ucFlags)
{
//  Uart_Write((uint8_t*)"I2CBufferWrite \n\r");

    i2c.start();       
   
   // Set I2C slave write address
    i2c.write(ucDevAddr);
 
    if(ulSize == 1){
        i2c.write(ucBuffer[0]);
    }else{  
        for(int i=0;i<ulSize;i++){
           i2c.write(ucBuffer[i]);
        }
    }    
 
        i2c.stop();
/*        
    int32_t err = 0;
    
//    RTOS_MUTEX_ACQUIRE(&g_i2cLock);      
    err = i2c.write(ucDevAddr,(char*)ucBuffer,ulSize);
    if(err == 1){
        Uart_Write((uint8_t*)"Return error I2C write\n\r");
//      RTOS_MUTEX_RELEASE(&g_i2cLock);
        return -1;      
    }
    ThisThread::sleep_for(1);
//    RTOS_MUTEX_RELEASE(&g_i2cLock);       */
    return 0;
} 
