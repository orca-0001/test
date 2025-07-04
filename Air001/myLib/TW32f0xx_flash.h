/**
 * @file flash.h
 * @brief TW32F003内部flash读写库
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 *  flash的写和擦除操作必须打开HSI
 * 
 */
#ifndef _FLASH_H_
#define _FLASH_H_

#include "TW32f0xx.h"
#include "wiring.h"
#include "string.h"

/* Global define */
#define FLASH_PAGE_NUM         		(512U)          //总共页数
#define FLASH_PAGE_SIZE        		(128U)          //每页大小
#define FLASH_SIZE             		(FLASH_PAGE_SIZE*FLASH_PAGE_NUM)     //整个flash大小
#define FLASH_START_ADDR       		((uint32_t)0x08000000)   //flash起始地址
#define FLASH_END_ADDR         		((uint32_t)0x0800FFFF)   //flash结束地址

#define FLASH_ADDR_END_PROTECTED  	((uint32_t)0x08000800)		//这地址之前的数据被保护，不允许读写

const uint32_t _FlashTimmingParam[8] = {0x1FFF0F1C, 0x1FFF0F30, 0x1FFF0F44, 0x1FFF0F58, 0x1FFF0F6C, 0x1FFF0F1C, 0x1FFF0F1C, 0x1FFF0F1C};

class INTERNAL_FLASH {
public:
	INTERNAL_FLASH(void);
	~INTERNAL_FLASH(void);
		
	uint8_t erase_page(uint32_t Page_Address);
	uint8_t FLASH_erase_page_all(void);  //擦除flash全部数据

	/* flash读函数 */
	void read(uint32_t data_addr, uint8_t *buf, uint32_t length);
	void readChar(uint32_t data_addr,char *Data,uint32_t count);
	void readUChar(uint32_t data_addr,uint8_t *Data,uint32_t count);
	void readShort(uint32_t data_addr,int16_t *Data,uint32_t count);
	void readUShort(uint32_t data_addr,uint16_t *Data,uint32_t count);
	void readInt(uint32_t data_addr,int32_t *Data,uint32_t count);
	void readUInt(uint32_t data_addr,uint32_t *Data,uint32_t count);
	void readLong64(uint32_t data_addr,int64_t *Data,uint32_t count);
	void readULong64(uint32_t data_addr,uint64_t *Data,uint32_t count);
	void readFloat(uint32_t data_addr,float *Data,uint32_t count);
	void readDouble(uint32_t data_addr,double *Data,uint32_t count);

	int8_t readChar(uint32_t data_addr);
	uint8_t readUChar(uint32_t data_addr);
	int16_t readShort(uint32_t data_addr);
	uint16_t readUShort(uint32_t data_addr);
	int32_t readInt(uint32_t data_addr);
	uint32_t readUInt(uint32_t data_addr);
	int64_t readLong64(uint32_t data_addr);
	uint64_t readULong64(uint32_t data_addr);
	float readFloat(uint32_t data_addr);
	double readDouble(uint32_t data_addr);
	template <class T> T readALL (uint32_t address, T &);

	/* flash写函数 */
	void write(uint32_t data_addr, uint8_t *buf, uint16_t length);
	uint8_t writeChar(uint32_t data_addr,char *Data,uint32_t count);
	uint8_t writeUChar(uint32_t data_addr,uint8_t *Data,uint32_t count);
	uint8_t writeShort(uint32_t data_addr,int16_t *Data,uint32_t count);
	uint8_t writeUShort(uint32_t data_addr,uint16_t *Data,uint32_t count);
	uint8_t writeInt(uint32_t data_addr,int32_t *Data,uint32_t count);
	uint8_t writeUInt(uint32_t data_addr,uint32_t *Data,uint32_t count);
	uint8_t writeLong64(uint32_t data_addr,int64_t *Data,uint32_t count);
	uint8_t writeULong64(uint32_t data_addr,uint64_t *Data,uint32_t count);
	uint8_t writeFloat(uint32_t data_addr,float *Data,uint32_t count);
	uint8_t writeDouble(uint32_t data_addr,double *Data,uint32_t count);

	uint8_t writeChar(uint32_t data_addr,char Data);
	uint8_t writeUChar(uint32_t data_addr,uint8_t Data);
	uint8_t writeShort(uint32_t data_addr,int16_t Data);
	uint8_t writeUShort(uint32_t data_addr,uint16_t Data);
	uint8_t writeInt(uint32_t data_addr,int32_t Data);
	uint8_t writeUInt(uint32_t data_addr,uint32_t Data);
	uint8_t writeLong64(uint32_t data_addr,int64_t Data);
	uint8_t writeULong64(uint32_t data_addr,uint64_t Data);
	uint8_t writeFloat(uint32_t data_addr,float Data);
	uint8_t writeDouble(uint32_t data_addr,double Data);

	/* 写一页数据 */
	void write_one_page(uint32_t data_addr, uint8_t *dat);
};

INTERNAL_FLASH::INTERNAL_FLASH(void)
{
}

INTERNAL_FLASH::~INTERNAL_FLASH() 
{
}

/*********************************  擦除操作  ***********************************************/

/**
 * @brief flash整片擦除（不支持该功能）
 */
uint8_t INTERNAL_FLASH::FLASH_erase_page_all(void)
{
	//NULL
}

/**
 * @brief flash页擦除
 * @param Page_Address 地址(输入地址，他会擦除该地址所在的整页数据)
 * @return 1,成功; 0,失败
 */
uint8_t INTERNAL_FLASH::erase_page(uint32_t Page_Address)
{
	uint32_t data_addr =  Page_Address - FLASH_START_ADDR;		//相对flash首地址的偏移地址
	uint32_t secpos = data_addr / FLASH_PAGE_SIZE;  //页地址
	
	if(Page_Address < FLASH_ADDR_END_PROTECTED)return 0;

	if((FLASH->CR&FLASH_CR_LOCK) != 0x0U)	//解锁
	{
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;		
	}
	__HAL_FLASH_TIMMING_SEQUENCE_CONFIG();
	while((FLASH->SR&FLASH_FLAG_BSY) != 0x00U);  //等待最后一个操作完成
	FLASH->SR = FLASH_FLAG_SR_CLEAR;	//清状态
	FLASH->CR |= FLASH_CR_PER;
  	*(__IO uint32_t *)(FLASH_START_ADDR+secpos*FLASH_PAGE_SIZE) = 0xFF;	//必须为页首地址
	while((FLASH->SR&FLASH_FLAG_BSY) != 0x00U);  //等待最后一个操作完成
	FLASH->SR = FLASH_FLAG_SR_CLEAR;	//清状态
	FLASH->CR &= ~FLASH_CR_PER;

	FLASH->CR |= FLASH_CR_LOCK;
	return 1;
}

/*********************************  写入操作  ***********************************************/

/**
 * @brief flash在指定地址写入指定长度的数据
 * @param data_addr 地址
 * @param buf 		要写入的数据指针
 * @param length 	数量
 * @return none
 */
void INTERNAL_FLASH::write(uint32_t data_addr, uint8_t *buf, uint16_t length)
{
	uint32_t secpos;	//页地址
	uint16_t secoff;  //在页地址的偏移
	uint16_t secremain;	   
 	uint16_t i;
	uint8_t flash_buf[FLASH_PAGE_SIZE];   
	data_addr =  data_addr - FLASH_START_ADDR;		//相对flash首地址的偏移地址
	secpos = data_addr / FLASH_PAGE_SIZE;  //页地址
	secoff = data_addr % FLASH_PAGE_SIZE;  //在页地址内的偏移
	secremain = FLASH_PAGE_SIZE-secoff;	//该页的剩余空间大小   
	if(length <= secremain)	
	{
		secremain = length;
	}
	while(1)
	{
		read(FLASH_START_ADDR+secpos*FLASH_PAGE_SIZE, flash_buf, FLASH_PAGE_SIZE);	//读出整个扇区的内容
		for(i=0;i<secremain;i++)
		{
			if(flash_buf[secoff+i]!=0XFF){
				erase_page(FLASH_START_ADDR+secpos*FLASH_PAGE_SIZE);	//擦除整页
				break;
			}
		}
		for(i=0;i<secremain;i++)	    //复制
		{
			flash_buf[secoff+i] = buf[i];	  
		}
		write_one_page(FLASH_START_ADDR+secpos*FLASH_PAGE_SIZE, flash_buf);
		if(length == secremain)
		{
			break;	//全部写入完成
		}
		else{
			secpos++;	//页地址增1
			secoff = 0;	//偏移清零
		   	buf+=secremain;  //要写入的数据地址偏移
			data_addr+=secremain;//flash地址偏移	  
			length-=secremain;	//要写入的字节数递减
			if(length>FLASH_PAGE_SIZE){
				secremain = FLASH_PAGE_SIZE;	//下一页还是写不完
			}else{ 
				secremain = length;	  //下一个扇区可以写完了
			}
		}				
	}
}

/**
 * @brief flash写入多个有符号8位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeChar(uint32_t data_addr,char *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(uint8_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count);
	return 1;
}

/**
 * @brief flash写入多个无符号8位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeUChar(uint32_t data_addr,uint8_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(uint8_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count);
	return 1;
}

/**
 * @brief flash写入多个有符号16位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeShort(uint32_t data_addr,int16_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(int16_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(int16_t));
	return 1;
}

/**
 * @brief flash写入多个无符号16位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeUShort(uint32_t data_addr,uint16_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(uint16_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(uint16_t));
	return 1;
}

/**
 * @brief flash写入多个有符号32位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeInt(uint32_t data_addr,int32_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(int32_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(int32_t));
	return 1;
}

/**
 * @brief flash写入多个无符号32位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeUInt(uint32_t data_addr,uint32_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(uint32_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(uint32_t));
	return 1;
}

/**
 * @brief flash写入多个有符号64位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeLong64(uint32_t data_addr,int64_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(int64_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(int64_t));
	return 1;
}

/**
 * @brief flash写入多个无符号64位数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeULong64(uint32_t data_addr,uint64_t *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(uint64_t) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(uint64_t));
	return 1;
}

/**
 * @brief flash写入多个float类型数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeFloat(uint32_t data_addr,float *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(float) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(float));
	return 1;
}

/**
 * @brief flash写入多个double类型数据.
 * @param data_addr 起始写地址
 * @param buf 		要写入的数据的指针
 * @param length 	数据长度
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeDouble(uint32_t data_addr,double *Data,uint32_t count)
{
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
    if (data_addr + count*sizeof(double) > FLASH_END_ADDR) 	//超出flash范围
	{	
		return 0;
    }
	write(data_addr, (uint8_t *)Data,  count*sizeof(double));
	return 1;
}

/**
 * @brief flash写入一个有符号8位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeChar(uint32_t data_addr,char Data)
{
	char temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(char));
	return 1;
}

/**
 * @brief flash写入一个无符号8位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeUChar(uint32_t data_addr,uint8_t Data)
{
	uint8_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(uint8_t));
	return 1;
}

/**
 * @brief flash写入一个有符号16位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeShort(uint32_t data_addr,int16_t Data)
{
	int16_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(int16_t));
	return 1;
}

/**
 * @brief flash写入一个无符号16位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeUShort(uint32_t data_addr,uint16_t Data)
{
	uint16_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(uint16_t));
	return 1;
}

/**
 * @brief flash写入一个有符号32位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeInt(uint32_t data_addr,int32_t Data)
{
	int32_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(int32_t));
	return 1;
}

/**
 * @brief flash写入一个无符号32位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeUInt(uint32_t data_addr,uint32_t Data)
{
	uint32_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(uint32_t));
	return 1;
}

/**
 * @brief flash写入一个有符号64位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeLong64(uint32_t data_addr,int64_t Data)
{
	int64_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(int64_t));
	return 1;
}

/**
 * @brief flash写入一个无符号64位数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeULong64(uint32_t data_addr,uint64_t Data)
{
	uint64_t temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(uint64_t));
	return 1;
}

/**
 * @brief flash写入一个float类型数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeFloat(uint32_t data_addr,float Data)
{
	float temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(float));
	return 1;
}

/**
 * @brief flash写入一个double类型数据.
 * @param data_addr 起始写地址
 * @param Data 		要写入的数据
 * @return 1,写入成功;0,失败.
 */
uint8_t INTERNAL_FLASH::writeDouble(uint32_t data_addr,double Data)
{
	double temp[1];
	temp[0] = Data;
	if(data_addr < FLASH_ADDR_END_PROTECTED)return 0;
	write(data_addr, (uint8_t *)temp,sizeof(double));
	return 1;
}


/*********************************  读取操作  ***********************************************/
/**
 * @brief flash从指定地址读出指定长度的数据
 * @param data_addr 起始读地址
 * @param buf 		读出的缓冲指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::read(uint32_t data_addr, uint8_t *buf, uint32_t length)
{
	for(uint32_t i = 0; i < length;i++)
	{
		buf[i] = (*(__IO uint8_t*)(data_addr+i));
	}
}

/**
 * @brief flash读取多个有符号8位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readChar(uint32_t data_addr,char *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count);
}

/**
 * @brief flash读取多个有符号8位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readUChar(uint32_t data_addr,uint8_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count);
}

/**
 * @brief flash读取多个有符号16位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readShort(uint32_t data_addr,int16_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(int16_t));
}

/**
 * @brief flash读取多个无符号16位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readUShort(uint32_t data_addr,uint16_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(uint16_t));
}

/**
 * @brief flash读取多个有符号32位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readInt(uint32_t data_addr,int32_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(int32_t));
}

/**
 * @brief flash读取多个无符号32位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readUInt(uint32_t data_addr,uint32_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(uint32_t));
}

/**
 * @brief flash读取多个有符号64位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readLong64(uint32_t data_addr,int64_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(int64_t));
}

/**
 * @brief flash读取多个无符号64位数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readULong64(uint32_t data_addr,uint64_t *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(uint64_t));
}

/**
 * @brief flash读取多个float类型数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readFloat(uint32_t data_addr,float *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(float));
}

/**
 * @brief flash读取多个double类型数据.
 * @param data_addr 起始读地址
 * @param buf 		读出的数据缓存指针
 * @param length 	数据长度
 * @return none
 */
void INTERNAL_FLASH::readDouble(uint32_t data_addr,double *Data,uint32_t count)
{
	read(data_addr, (uint8_t *)Data,  count*sizeof(double));
}

/**
 * @brief flash读取一个有符号8位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
int8_t INTERNAL_FLASH::readChar(uint32_t data_addr)
{
	int8_t value = 0;
	return readALL(data_addr,value);
}

/**
 * @brief flash读取一个无符号8位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
uint8_t INTERNAL_FLASH::readUChar(uint32_t data_addr)
{
	uint8_t value = 0;
	return readALL(data_addr,value);
}

/**
 * @brief flash读取一个有符号16位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
int16_t INTERNAL_FLASH::readShort(uint32_t data_addr)
{
	int16_t value = 0;
	return readALL(data_addr,value);
}

/**
 * @brief flash读取一个无符号16位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
uint16_t INTERNAL_FLASH::readUShort(uint32_t data_addr)
{
    uint16_t value=0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取一个有符号32位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
int32_t INTERNAL_FLASH::readInt(uint32_t data_addr)
{
    int32_t value=0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取一个无符号32位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
uint32_t INTERNAL_FLASH::readUInt(uint32_t data_addr)
{
    uint32_t value=0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取一个有符号64位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
int64_t INTERNAL_FLASH::readLong64(uint32_t data_addr)
{
    int64_t value=0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取一个无符号64位数据.
 * @param data_addr 读地址
 * @return 读到的数据
 */
uint64_t INTERNAL_FLASH::readULong64(uint32_t data_addr)
{
    uint64_t value=0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取一个float类型小数.
 * @param data_addr 读地址
 * @return 读到的数据
 */
float INTERNAL_FLASH::readFloat(uint32_t data_addr)
{
    float value=0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取一个double类型小数.
 * @param data_addr 读地址
 * @return 读到的数据
 */
double INTERNAL_FLASH::readDouble(uint32_t data_addr)
{
    double value = 0;
    return readALL(data_addr,value);
}

/**
 * @brief flash读取数据模板.
 * @param data_addr 读地址
 * @return 读到的数据
 */
template <class T> T INTERNAL_FLASH::readALL(uint32_t data_addr,T &value)
{
    if( (data_addr-FLASH_START_ADDR + sizeof(T)) > FLASH_SIZE )
        return value;
    memcpy((uint8_t*) &value,(uint8_t*)(data_addr),sizeof(T));
    return value;
}

/**
 * @brief flash页写入
 * @param data_addr 页首地址(必须为首地址)
 * @param buf 		要写入的数据指针
 * @return none
 */
void INTERNAL_FLASH::write_one_page(uint32_t data_addr, uint8_t *buf)
{
	uint8_t index=0;
	uint32_t primask_bit;
	uint32_t dest = data_addr;
	uint32_t *src = (uint32_t *)buf;

	if((FLASH->CR&FLASH_CR_LOCK) != 0x0U)	//解锁
	{
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;		
	}
	__HAL_FLASH_TIMMING_SEQUENCE_CONFIG();
	while((FLASH->SR&FLASH_FLAG_BSY) != 0x00U);  //等待最后一个操作完成
	FLASH->SR = FLASH_FLAG_SR_CLEAR;	//清状态
	FLASH->CR |= FLASH_CR_PG;	//写操作
	__ASM volatile ("MRS %0, primask" : "=r" (primask_bit) );
	__disable_irq();
	while(index<32U)
	{
		*(uint32_t *)dest = *src;
		src += 1U;
		dest += 4U;
		index++;
		if(index == 31)
		{
			FLASH->CR |= FLASH_CR_PGSTRT;	//program启动
		}
	}
	__ASM volatile ("MSR primask, %0" : : "r" (primask_bit) : "memory");
	__enable_irq();
	while((FLASH->SR&FLASH_FLAG_BSY) != 0x00U);  //等待最后一个操作完成
	FLASH->SR = FLASH_FLAG_SR_CLEAR;	//清状态
	FLASH->CR &= ~FLASH_CR_PG;

	FLASH->CR |= FLASH_CR_LOCK;
}


INTERNAL_FLASH flash;


#endif




