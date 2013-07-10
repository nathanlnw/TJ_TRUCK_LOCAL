/*
   vdr Vichle Driver Record 车辆行驶记录
 */

#include <rtthread.h>
#include <finsh.h>
#include "stm32f4xx.h"

#include <rtdevice.h>
#include <dfs_posix.h>

#include <time.h>

#include "sst25vf.h"
#include "Vdr.h"
#include "App_moduleConfig.h"


//  =================  行车记录仪 相关 ============================
/*
    StartPage :    6320          Start Address offset :   0x316000       

    Area Size :
                          213  Sector       = 1704 pages
                           ----------------------
                           
				扇区               
				1                                     00-07H
				90                                    08H               
				85                                    09H
				7                                      10H
				2                                      11H 
				2                                      12H
				1                                      13H
				1                                      14H  
				1                                      15H   

          ----------  只是在这里做了---  注释 ，具体操作在 Vdr.C  				
*/

#define       SECTORSIZE                  4096
#define       VDR_START_ADDRESS           0x316000
#define       MarkByte                    0x0F          // 表示写入过相关信息

//-------------------------- Sectors-----------  total   195 Sectors --------------
#define  VDR_00_07_SIZE      1
#define  VDR_08_SIZE         90    //   128 rec_size               1sector=32 recrods    2880/32=90
#define  VDR_09_SIZE         72    //  666=>>768 rec_size      1sector=5 recrods     360/5=72
#define  VDR_10_SIZE         7     //   234=>> 256 rec_size    1sector=16              100/16 =7
#define  VDR_11_SIZE         2     //   50 =>> 64                  1 sector=64             100/64 =2 
#define  VDR_12_SIZE         2     //    25 =>> 32                 1 sector=128            200/128=2
#define  VDR_13_SIZE         1     //     7==>>  8                 1 sector= 512             100/512=1
#define  VDR_14_SIZE         1     //     7==>>  8                 1 sector= 512             100/512=1
#define  VDR_15_SIZE         1     //  133==>256                  1 sector =16               10/16 =1

 //   存储区域最大条数
#define  VDR_08_MAXindex         2880
#define  VDR_09_MAXindex         360   
#define  VDR_10_MAXindex          100
#define  VDR_11_MAXindex         10
#define  VDR_12_MAXindex         200
#define  VDR_13_MAXindex         100
#define  VDR_14_MAXindex         100
#define  VDR_15_MAXindex         10   



//----------------------absolutely  address   --------------  
#define VDR_00_to_07H   VDR_START_ADDRESS 

#define VDR_08H_START	VDR_START_ADDRESS+VDR_00_07_SIZE*SECTORSIZE
#define VDR_08H_END		VDR_START_ADDRESS+VDR_00_07_SIZE*SECTORSIZE-1

#define VDR_09H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE)*SECTORSIZE
#define VDR_09H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE)*SECTORSIZE-1


#define VDR_10H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE)*SECTORSIZE
#define VDR_10H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE)*SECTORSIZE-1

#define VDR_11H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE)*SECTORSIZE
#define VDR_11H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE)*SECTORSIZE-1

#define VDR_12H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE)*SECTORSIZE
#define VDR_12H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE)*SECTORSIZE-1

#define VDR_13H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE+VDR_12_SIZE)*SECTORSIZE
#define VDR_13H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE+VDR_12_SIZE)*SECTORSIZE-1

#define VDR_14H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE+VDR_12_SIZE+VDR_13_SIZE)*SECTORSIZE
#define VDR_14H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE+VDR_12_SIZE+VDR_13_SIZE)*SECTORSIZE-1

#define VDR_15H_START	VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE+VDR_12_SIZE+VDR_13_SIZE+VDR_14_SIZE)*SECTORSIZE
#define VDR_15H_END		VDR_START_ADDRESS+(VDR_00_07_SIZE+VDR_08_SIZE+VDR_09_SIZE+VDR_10_SIZE+VDR_11_SIZE+VDR_12_SIZE+VDR_13_SIZE+VDR_14_SIZE)*SECTORSIZE-1 






//  -   no  use  actually----
#define VDR_08H_09H_START	VDR_START_ADDRESS+VDR_00_07_SIZE*SECTORSIZE
#define VDR_08H_09H_END		VDR_START_ADDRESS+VDR_00_07_SIZE*SECTORSIZE-1




/*
   实现每条记录128字节，便于定位。要做一定的处理。没有使用delta编码

   记录格式

   000--003  yy-mm-dd hh:mm   可以把时间转成UTC格式,便于比较查找
      又可以节省一个字节，无效的时间格式FFFFFFFFFF
   004--056  秒速度记录 速度要求+/- 1kmh
          原先60*8=480bit 现在用7bit保存需要 60*7=420bit=53byte
          7bit的 0b1111111 表示速度无效(gps未定位)
   057--116  状态信息
   117--126  单位分钟位置,参见行车记录仪 GBT19065 首个有效位置

 */


/*
   4MB serial flash 0x400000
 */

/*转换hex到bcd的编码*/
#define HEX_TO_BCD( A ) ( ( ( ( A ) / 10 ) << 4 ) | ( ( A ) % 10 ) )

static rt_thread_t tid_usb_vdr = RT_NULL;

struct _sect_info
{
	uint32_t	addr;               //开始的地址
	uint16_t	bytes_per_block;
	uint8_t		blocks;
} sect_info[7] =
{
	{ 0x00300000, 8192, 24	},      //block_08h_09h		8*1024Bytes	24Block		0x30000	192k	0x30000
	{ 0x00330000, 256,	128 },      //block_10h         234Bytes	100Block	0x8000	32k		0x 8000
	{ 0x00338000, 64,	128 },      //block 11          50Bytes		100Block	0x2000	8k		0x 2000
	{ 0x0033A000, 32,	256 },  //block 12          25Bytes		200block	0x2000	8k		0x 2000
	{ 0x0033C000, 8,	128 },  //block 13          7Bytes		100block	0x400	1k		0x 1000
	{ 0x0033D000, 8,	128 },  //block 14          7Bytes		100block	0x400	1k		0x 1000
	{ 0x0033E000, 256,	16	},  //block 15          133Bytes	10block		0x1000	4k		0x 1000
};



/*基于小时的时间戳,主要是为了比较大小使用
   byte0 year
   byte1 month
   byte2 day
   byte3 hour
 */
typedef unsigned int YMDH_TIME;

typedef struct
{
	uint8_t cmd;

	uint32_t	ymdh_start;
	uint8_t		minute_start;

	uint32_t	ymdh_end;
	uint8_t		minute_end;

	uint32_t	ymdh_curr;
	uint8_t		minute_curr;
	uint32_t	addr;

	uint16_t	blocks;         /*定义每次上传多少个数据块*/
	uint16_t	blocks_remain;  /*当前组织上传包是还需要的的blocks*/
}VDR_CMD;

VDR_CMD		vdr_cmd;

uint8_t		vdr_tx_info[1024];
uint16_t	vtr_tx_len = 0;

/*当前要写入数据的地址,8K边界对齐*/
static uint32_t		vdr_addr_curr_wr	= 0x0;
static YMDH_TIME	vdr_ymdh_curr_wr	= 0;

static uint8_t		fvdr_debug = 1;


/*传递写入文件的信息
   0...3  写入SerialFlash的地址
   4...31 文件名
 */
static uint8_t file_rec[32];

//=====================================================
VDR_INDEX         Vdr_Wr_Rd_Offset;      //  记录仪写索引 位置
VDR_DATA          VdrData;               //  行驶记录仪的databuf
VDR_TRIG_STATUS   VDR_TrigStatus;









//======================================================
/*
      遍历获取当前行驶记录的操作位置

      type :    0  代表遍历所有      08  09  10  11 12 13  14 
      u8    :     0   :遍历所有  
      
*/
u8  Vdr_PowerOn_getWriteIndex(u8 type)   
{
   
       u16    i;
	   u8	  c;  
      
	  //	 1.  get  address	


     switch(type)
     	{
		   case 0x08:
		   	           // 1.1  先读取第一个字符做判断
		   	           for(i=0;i<2880;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_08H_START+i*128);
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_08H_Write=i; 
							  rt_kprintf("\r\n 08H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_08H_Write); 
                              break;
						    }
		   	           	}
					   // 1.2. 没有找到2880 那么 擦除第一个  从0 开始
					   if(i==2880)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_08H_START );  
						   Vdr_Wr_Rd_Offset.V_08H_Write=0;
					   	}							   	
		   	          
		   	           break;
		   case 0x09:
		   	           //  2.1 
		   	          for(i=0;i<360;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_09H_START+i*768);
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_09H_Write=i; 
							  rt_kprintf("\r\n 09H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_09H_Write); 
                              break;
						    }
		   	           	}
					   // 2. 没有找到360 那么 擦除第一个  从0 开始
					   if(i==360)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_09H_START );   
						   Vdr_Wr_Rd_Offset.V_09H_Write=0;  
					   	} 
					   break;
		   
		   case 0x10:
		   	           for(i=0;i<100;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_10H_START+i*256);
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_10H_Write=i; 
							  rt_kprintf("\r\n 10H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_10H_Write);  
                              break;
						    }
		   	           	}
					   // 2. 没有找到100 那么 擦除第一个  从0 开始
					   if(i==100)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_10H_START );     
						   Vdr_Wr_Rd_Offset.V_10H_Write=0;   
					   	}  
					   break;
		   
		   case 0x11:
		   	            for(i=0;i<100;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_11H_START+i*64);
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_11H_Write=i; 
							  rt_kprintf("\r\n 11H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_11H_Write);   
                              break;
						    }
		   	           	}
					   // 2. 没有找到100 那么 擦除第一个  从0 开始
					   if(i==100)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_11H_START );        
						   Vdr_Wr_Rd_Offset.V_11H_Write=0;       
					   	}  
					   break;

		   case 0x12: 
		   	            for(i=0;i<200;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_12H_START+i*32);
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_12H_Write=i; 
							  rt_kprintf("\r\n 12H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_12H_Write);   
                              break;
						    }
		   	           	}
					    // 2. 没有找到200 那么 擦除第一个  从0 开始 
					    if(i==200)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_12H_START );        
						   Vdr_Wr_Rd_Offset.V_12H_Write=0;           
					   	}  
						break;

		   case 0x13:   for(i=0;i<100;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_13H_START+i*8); 
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_11H_Write=i; 
							  rt_kprintf("\r\n 13H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_13H_Write);   
                              break;
						    }
		   	           	}
					   // 2. 没有找到100 那么 擦除第一个  从0 开始
					   if(i==100)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_13H_START );         
						   Vdr_Wr_Rd_Offset.V_13H_Write=0;       
					   	}  
						break;

		   case 0x14:   
		   	             for(i=0;i<100;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_14H_START+i*8); 
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_14H_Write=i; 
							  rt_kprintf("\r\n 14H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_14H_Write);    
                              break;
						    }
		   	           	}
					   // 2. 没有找到100 那么 擦除第一个  从0 开始
					   if(i==100)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_14H_START );        
						   Vdr_Wr_Rd_Offset.V_14H_Write=0;       
					   	}  
						break;

		   case 0x15:
		   	            for(i=0;i<10;i++)
		   	           	{
                            c=SST25V_ByteRead(VDR_15H_START+i*256);  
						    if(c==0xFF)
						    {
						      Vdr_Wr_Rd_Offset.V_15H_Write=i; 
							  rt_kprintf("\r\n 15H write=%d  \r\n",Vdr_Wr_Rd_Offset.V_15H_Write);    
                              break;
						    }
		   	           	}
					   // 2. 没有找到10 那么 擦除第一个  从0 开始
					   if(i==10)
					   	{
					   	   WatchDog_Feed();
				           SST25V_SectorErase_4KByte( VDR_15H_START );          
						   Vdr_Wr_Rd_Offset.V_15H_Write=0;          
					   	}  
						break;

     	}

}


//    遍历所有行车记录写入的起始地址   from  08H
void total_ergotic(void)    
{
    Vdr_PowerOn_getWriteIndex(0x08);
    Vdr_PowerOn_getWriteIndex(0x09);
	Vdr_PowerOn_getWriteIndex(0x10);
    Vdr_PowerOn_getWriteIndex(0x11);
	Vdr_PowerOn_getWriteIndex(0x12);
    Vdr_PowerOn_getWriteIndex(0x13);
	Vdr_PowerOn_getWriteIndex(0x14);
	Vdr_PowerOn_getWriteIndex(0x15);     

	Vdr_Wr_Rd_Offset.V_07H_Read=0;
	Vdr_Wr_Rd_Offset.V_08H_Read=0;
	Vdr_Wr_Rd_Offset.V_09H_Read=0;
	Vdr_Wr_Rd_Offset.V_10H_Read=0;
	Vdr_Wr_Rd_Offset.V_11H_Read=0;
	Vdr_Wr_Rd_Offset.V_12H_Read=0;
	Vdr_Wr_Rd_Offset.V_13H_Read=0;
	Vdr_Wr_Rd_Offset.V_14H_Read=0;
	Vdr_Wr_Rd_Offset.V_15H_Read=0; 
}
FINSH_FUNCTION_EXPORT( total_ergotic, total_ergotic );


/*
         根据输入的类型，和起始时间查找到符合条件的起始地址和条数
*/
void query_vdr(u8 type,u8 *intime, u16 current_start,u16 num)
{
  u16 i=0;
	u8  c;

   switch(type)
   {
		  case 0x08:
					  // 1.1  先读取当前写入过的时间
                      if(Vdr_Wr_Rd_Offset.V_08H_Write==0)
                      	{
                             current_start=0;
							 num=0;
							 break;
                      	}
                      
                      //  1.2    读取当前最近的记录的时间
					  
					  for(i=0;i<2880;i++)
					   {
						   c=SST25V_ByteRead(VDR_08H_START+i*128);
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_08H_Write=i; 
							 rt_kprintf("\r\n 08H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_08H_Write); 
							 break;
						   }
					   }
					  // 1.2. 没有找到2880 那么 擦除第一个	从0 开始
					  if(i==2880)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_08H_START );  
						  Vdr_Wr_Rd_Offset.V_08H_Write=0;
					   }							   
					 
					  break;
		  case 0x09:
					  //  2.1 
					 for(i=0;i<360;i++)
					   {
						   c=SST25V_ByteRead(VDR_09H_START+i*768);
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_09H_Write=i; 
							 rt_kprintf("\r\n 09H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_09H_Write); 
							 break;
						   }
					   }
					  // 2. 没有找到360 那么 擦除第一个  从0 开始
					  if(i==360)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_09H_START );	
						  Vdr_Wr_Rd_Offset.V_09H_Write=0;  
					   } 
					  break;
		  
		  case 0x10:
					  for(i=0;i<100;i++)
					   {
						   c=SST25V_ByteRead(VDR_10H_START+i*256);
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_10H_Write=i; 
							 rt_kprintf("\r\n 10H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_10H_Write);  
							 break;
						   }
					   }
					  // 2. 没有找到100 那么 擦除第一个  从0 开始
					  if(i==100)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_10H_START );	  
						  Vdr_Wr_Rd_Offset.V_10H_Write=0;	
					   }  
					  break;
		  
		  case 0x11:
					   for(i=0;i<100;i++)
					   {
						   c=SST25V_ByteRead(VDR_11H_START+i*64);
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_11H_Write=i; 
							 rt_kprintf("\r\n 11H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_11H_Write);   
							 break;
						   }
					   }
					  // 2. 没有找到100 那么 擦除第一个  从0 开始
					  if(i==100)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_11H_START );		 
						  Vdr_Wr_Rd_Offset.V_11H_Write=0;		
					   }  
					  break;
   
		  case 0x12: 
					   for(i=0;i<200;i++)
					   {
						   c=SST25V_ByteRead(VDR_12H_START+i*32);
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_12H_Write=i; 
							 rt_kprintf("\r\n 12H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_12H_Write);   
							 break;
						   }
					   }
					   // 2. 没有找到200 那么 擦除第一个  从0 开始 
					   if(i==200)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_12H_START );		 
						  Vdr_Wr_Rd_Offset.V_12H_Write=0;			
					   }  
					   break;
   
		  case 0x13:   for(i=0;i<100;i++)
					   {
						   c=SST25V_ByteRead(VDR_13H_START+i*8); 
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_11H_Write=i; 
							 rt_kprintf("\r\n 13H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_13H_Write);   
							 break;
						   }
					   }
					  // 2. 没有找到100 那么 擦除第一个  从0 开始
					  if(i==100)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_13H_START );		  
						  Vdr_Wr_Rd_Offset.V_13H_Write=0;		
					   }  
					   break;
   
		  case 0x14:   
						for(i=0;i<100;i++)
					   {
						   c=SST25V_ByteRead(VDR_14H_START+i*8); 
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_14H_Write=i; 
							 rt_kprintf("\r\n 14H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_14H_Write);	
							 break;
						   }
					   }
					  // 2. 没有找到100 那么 擦除第一个  从0 开始
					  if(i==100)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_14H_START );		 
						  Vdr_Wr_Rd_Offset.V_14H_Write=0;		
					   }  
					   break;
   
		  case 0x15:
					   for(i=0;i<10;i++)
					   {
						   c=SST25V_ByteRead(VDR_15H_START+i*256);	
						   if(c==0xFF)
						   {
							 Vdr_Wr_Rd_Offset.V_15H_Write=i; 
							 rt_kprintf("\r\n 15H write=%d	\r\n",Vdr_Wr_Rd_Offset.V_15H_Write);	
							 break;
						   }
					   }
					  // 2. 没有找到10 那么 擦除第一个	从0 开始
					  if(i==10)
					   {
						  WatchDog_Feed();
						  SST25V_SectorErase_4KByte( VDR_15H_START );		   
						  Vdr_Wr_Rd_Offset.V_15H_Write=0;		   
					   }  
					   break;
   
	   }

 
}


//   只能在初始化是用到
void erase_vdr(void)
{
   u16  i=0;
    // start address        0x316000        start sector :790th        total: 195 sector 

    //  1. erase  to  839          840-790=50 
    if(rt_mutex_take(DF_lock_mutex,150)==RT_EOK) 
{
		  DF_LOCK=1;


	//  32K
   for(i=0;i<10;i++)
   {
	 WatchDog_Feed();
	 SST25V_SectorErase_4KByte(VDR_START_ADDRESS+i*SECTORSIZE);  
     delay_ms(100);   
   }	 
   
   rt_kprintf("\r\n   <------------  vdr  area  erase  1 ------------------>\r\n");   
   // 195-50=145         145-64*2=17
   for(i=0;i<6;i++) 
   {
	 WatchDog_Feed();
	// SST25V_BlockErase_64KByte(VDR_START_ADDRESS+0xA000+0x10000*i);  // 64k  <=> 0x10000  
	 SST25V_BlockErase_32KByte(VDR_START_ADDRESS+0xA000+0x8000*i);  // 32k  <=> 0x8000  
     DF_delay_ms(500); 
	 WatchDog_Feed();
   }
   

 /*   for(i=0;i<64;i++)
   {
	 WatchDog_Feed();
	 SST25V_SectorErase_4KByte(VDR_START_ADDRESS+i*SECTORSIZE+8*SECTORSIZE);  
     delay_ms(1); 
   }	*/
   rt_kprintf("\r\n   <------------  vdr  area  erase   2  ------------------>\r\n"); 
 
 /*    for(i=0;i<64;i++)
   {
	 WatchDog_Feed();
	 SST25V_SectorErase_4KByte(VDR_START_ADDRESS+i*SECTORSIZE+114*SECTORSIZE);  
     delay_ms(1); 
   }	
*/
      rt_kprintf("\r\n   <------------  vdr  area  erase   3  ------------------>\r\n"); 	 
   // left   17  erase  sector   
 /*  for(i=0;i<17;i++)
   {
	 WatchDog_Feed();
	 SST25V_SectorErase_4KByte(VDR_START_ADDRESS+0x32000+0x80000+i*SECTORSIZE);     
     delay_ms(5);
   } 
   */


   DF_LOCK=0;
   rt_mutex_release(DF_lock_mutex);  
 }   

   rt_kprintf("\r\n   <------------  vdr  area  erase   over  ------------------>\r\n");  
}
FINSH_FUNCTION_EXPORT( erase_vdr, erase_vdr ); 



//    创建行驶记录仪数据  

void  VDR_product_08H_09H_10H(void)
{
  
  u16 j=0,cur=0;	 
  
  //  行驶记录相关数据产生 触发,  且定位的情况下   
  if((VDR_TrigStatus.Running_state_enable==1)&&(UDP_dataPacket_flag==0X02)) 	  
  { 	//	  必须在行驶状态下
			
			//-------------------  08H	 --------------------------------------------------
			if((Temp_Gps_Gprs.Time[2]==0)&&(VDR_TrigStatus.Running_state_enable==1)) 
			{  // 秒为0 
				// 0.  判断是否是第一次上电
				if((VdrData.H_08[0]==0x00)&&(VdrData.H_08[1]==0x00)) 
				{
					  rt_kprintf("\r\n first  get  08H	  \r\n");
				}
				else
				{ 
					  memcpy(VdrData.H_08_BAK,VdrData.H_08,126);
					  VdrData.H_08_saveFlag=1;	//	 保存  当前分钟的数据记录					 
				}	 
			   //  1.  年月日 时分秒	 秒是0
				 VdrData.H_08[0]=((Temp_Gps_Gprs.Date[0]/10)<<4)+(Temp_Gps_Gprs.Date[0]%10);	   
				 VdrData.H_08[1]=((Temp_Gps_Gprs.Date[1]/10)<<4)+(Temp_Gps_Gprs.Date[1]%10); 
				 VdrData.H_08[2]=((Temp_Gps_Gprs.Date[2]/10)<<4)+(Temp_Gps_Gprs.Date[2]%10);
				 VdrData.H_08[3]=((Temp_Gps_Gprs.Time[0]/10)<<4)+(Temp_Gps_Gprs.Time[0]%10);
				 VdrData.H_08[4]=((Temp_Gps_Gprs.Time[1]/10)<<4)+(Temp_Gps_Gprs.Time[1]%10);
				 VdrData.H_08[5]=0; 	
  
			   // 2.  initial 
			   memset(VdrData.H_08+6,0xFF,120);    // 默认是 0xFF
			}  
			   // 3.   save 	Minute		   1-59   s 		
			   VdrData.H_08[6+2*Temp_Gps_Gprs.Time[2]]=Speed_gps/10; //km/h
			   VdrData.H_08[7+2*Temp_Gps_Gprs.Time[2]]=Vehicle_sensor;	
  
		  //------------------------09H ---------  min=0;	sec=0	----------------------------
		   if((Temp_Gps_Gprs.Time[1]==0)&&(Temp_Gps_Gprs.Time[2]==0)) 
		   {
			   
				// 1.  判断是否是需要存储
					VdrData.H_09_saveFlag=1;  //   保存  当前分钟的数据记录 
			   //  2.  年月日 时分秒	 |	 分  秒是0
				 VdrData.H_09[0]=((Temp_Gps_Gprs.Date[0]/10)<<4)+(Temp_Gps_Gprs.Date[0]%10);	   
				 VdrData.H_09[1]=((Temp_Gps_Gprs.Date[1]/10)<<4)+(Temp_Gps_Gprs.Date[1]%10); 
				 VdrData.H_09[2]=((Temp_Gps_Gprs.Date[2]/10)<<4)+(Temp_Gps_Gprs.Date[2]%10);
				 VdrData.H_09[3]=((Temp_Gps_Gprs.Time[0]/10)<<4)+(Temp_Gps_Gprs.Time[0]%10);
				 VdrData.H_09[4]=0;
				 VdrData.H_09[5]=0;
  
				 //  initial  0xFF
				 memset(VdrData.H_09+6,0xFF,660);	 // 默认是 0xFF
  
		   }   
		   else
			// 3.  判断是否是第一次上电 ,  如果是那么给他赋值
		   if((VdrData.H_09[0]==0x00)&&(VdrData.H_09[1]==0x00))  // 年月为 0
		   {
				 VdrData.H_09[0]=((Temp_Gps_Gprs.Date[0]/10)<<4)+(Temp_Gps_Gprs.Date[0]%10);	   
				 VdrData.H_09[1]=((Temp_Gps_Gprs.Date[1]/10)<<4)+(Temp_Gps_Gprs.Date[1]%10); 
				 VdrData.H_09[2]=((Temp_Gps_Gprs.Date[2]/10)<<4)+(Temp_Gps_Gprs.Date[2]%10);
				 VdrData.H_09[3]=((Temp_Gps_Gprs.Time[0]/10)<<4)+(Temp_Gps_Gprs.Time[0]%10);
				 VdrData.H_09[4]=0; 
				 VdrData.H_09[5]=0;
		   }   
  
		   //  4 .	填写常规位置	1-59 min
		   if(Temp_Gps_Gprs.Time[2]==59)  // 59 秒时候做处理，如果是0 那么分钟就变了
		   {  
			  //   表 A.20		  (当前分钟的位置  +	当前分钟的平均速度) 
			  memcpy( VdrData.H_09+6+Temp_Gps_Gprs.Time[1]*11,VdrData.Longi,4);  // 经度
			  memcpy( VdrData.H_09+6+Temp_Gps_Gprs.Time[1]*11+4,VdrData.Lati,4); //纬度
			  VdrData.H_09[6+Temp_Gps_Gprs.Time[1]*11+8]=(GPS_Hight>>8);
			  VdrData.H_09[6+Temp_Gps_Gprs.Time[1]*11+9]=GPS_Hight; 		   
			  //  当前分钟的平均速度	 从AvrgSpd_MintProcess()  引用的变量
			  VdrData.H_09[6+Temp_Gps_Gprs.Time[1]*11+10]= PerMinSpdTotal/AspdCounter;			  
		   }
		 //-------------------- 10H    事故疑点数据  ---------------------------------
		 if((VdrData.H_10_saveFlag==0)&&((Vehicle_sensor&Vehicle_sensor_BAK)==0x80))
		 {	//	刹车状态发生变化
			  //  1.  行驶结束时间
			 time_now=Get_RTC();	 //  RTC  相关 
			 Time2BCD(VdrData.H_10); 
			 //   2.   机动车驾驶证号码
			 memcpy(VdrData.H_10+6,JT808Conf_struct.Driver_Info.DriverCard_ID,18);
			 //   3.  速度和状态信息   
					  //-----------------------  Status Register   --------------------------------
					cur=save_sensorCounter;  //20s的事故疑点
					for(j=0;j<100;j++)
					{
						VdrData.H_10[24+j*2]=Sensor_buf[cur].DOUBTspeed;	//速度
						VdrData.H_10[24+j*2+1]=Sensor_buf[cur].DOUBTstatus;   //状态	
						
						if(cur>0)	  // 从当前往前读取
						   cur--;
						else
						   cur=100; 						   
					}  
			 // 	4.	位置信息
			  memcpy( VdrData.H_10+224,VdrData.Longi,4);  // 经度
			  memcpy( VdrData.H_10+224+4,VdrData.Lati,4); //纬度
			  VdrData.H_10[224+8]=(GPS_Hight>>8);
					VdrData.H_10[224+9]=GPS_Hight;	
  
			 //--------------------------------------------------------------------------
			  
			 VdrData.H_10_saveFlag=1;
		 }			 
		 Vehicle_sensor_BAK=Vehicle_sensor;
  
   }
  
  
}


void VDR_product_11H_Start(void)
{
           //    11 H   连续驾驶开始时间   和起始位置     从60s 开始算驾驶起始
                   if(VDR_TrigStatus.Run_baseon_spd_10s_couter==60)
                   	{ 
                   	    //  1.   机动车驾驶证号
                   	    memcpy(VdrData.H_11,JT808Conf_struct.Driver_Info.DriverCard_ID,18);
					    //   2.   起始时间
						time_now=Get_RTC();     //  RTC  相关 
			            Time2BCD(VdrData.H_11+18); 
                        //   3.  起始位置                        
						memcpy( VdrData.H_11+30,VdrData.Longi,4);  // 经度
						memcpy( VdrData.H_11+30+4,VdrData.Lati,4); //纬度
						VdrData.H_11[30+8]=(GPS_Hight>>8);
						VdrData.H_11[30+9]=GPS_Hight;	
                   	}
}

void VDR_product_11H_End(void)
{
            //         11 H     相关
        if(VDR_TrigStatus.Run_baseon_spd_10s_couter>TiredConf_struct.TiredDoor.Door_DrvKeepingSec)
        	{
                    //   2.   结束时间
						time_now=Get_RTC();     //  RTC  相关 
			            Time2BCD(VdrData.H_11+25); 
                        //   3.  起始位置                        
						memcpy( VdrData.H_11+40,VdrData.Longi,4);  // 经度
						memcpy( VdrData.H_11+40+4,VdrData.Lati,4); //纬度
						VdrData.H_11[40+8]=(GPS_Hight>>8);
						VdrData.H_11[40+9]=GPS_Hight;	
                    //    4.   save 
                    VdrData.H_11_saveFlag=1; 
        	}
}




void VDR_product_12H(u8  value)    // 插拔卡记录
{
     //   1. 时间发生的时间
		time_now=Get_RTC();     //  RTC  相关 
        Time2BCD(VdrData.H_12); 

     //  2.   机动车驾驶证号
         memcpy(VdrData.H_12+6,JT808Conf_struct.Driver_Info.DriverCard_ID,18);
     //  3.  结果 
         VdrData.H_12[24]=value;

	 // 4.  save 
	     VdrData.H_12_saveFlag=1; 
}


//  Note :  定义了但是没有用，不带电池情况下，断电操作SPI DF  危害会很大
void VDR_product_13H(u8  value)    //  外部供电记录
{
       //   1. 时间发生的时间
		time_now=Get_RTC();     //  RTC  相关 
        Time2BCD(VdrData.H_13); 
       //  2.  value
         VdrData.H_13[6]=value;
       //  3. save
       VdrData.H_13_saveFlag=1; 
}


void VDR_product_14H(u8 cmdID)    // 记录仪参数修改记录
{

    //   1. 时间发生的时间
		time_now=Get_RTC();     //  RTC  相关 
        Time2BCD(VdrData.H_14); 
       //  2.  value
         VdrData.H_14[6]=cmdID;
       //  3. save
       VdrData.H_14_saveFlag=1;
}


 
// note  :    需要索引检索   08 H 的数据记录 ，这里不需要填充。
void VDR_product_15H(u8  value)    //  采集指定速度记录 
{
    




}

void  VDR_get_15H_StartEnd_Time(u8  *Start, u8 *End)  
{
    u8  i=0;

	for(i=0;i<6;i++)
	{
      VdrData.H_15_Set_StartDateTime[i]=Start[i];
	  VdrData.H_15_Set_EndDateTime[i]=End[i];
	}
	
}



/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
static unsigned long mymktime( uint32_t year, uint32_t mon, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec )
{
	if( 0 >= (int)( mon -= 2 ) )
	{
		mon		+= 12;
		year	-= 1;
	}
	return ( ( ( (unsigned long)( year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day ) + year * 365 - 719499 ) * 24 + hour * 60 + min ) * 60 + sec );
}

/*
   压缩数据，将8bit转为7bit,并移位
   每到整分钟时，准备好数据  5Byte(yymmddhhmm)+60Byte(8bit Speed) +60Byte( Status)+10Byte(首个速度)=135 bytes
 */
static void compress_data( uint8_t *src, uint8_t *dst )
{
	uint8_t *psrc;
	uint8_t *pdst = dst;
	uint8_t c1, c2, c3, c4, c5, c6, c7, c8;
	uint8_t i;

/*头5个字节不需要了*/
	psrc = src + 5;

	for( i = 0; i < 7; i++ )
	{
		c1		= *psrc++;
		c2		= *psrc++;
		c3		= *psrc++;
		c4		= *psrc++;
		c5		= *psrc++;
		c6		= *psrc++;
		c7		= *psrc++;
		c8		= *psrc++;
		*pdst++ = c1 | ( ( c2 >> 7 ) & 0x01 );
		*pdst++ = ( c2 << 1 ) | ( ( c3 >> 6 ) & 0x03 );
		*pdst++ = ( c3 << 2 ) | ( ( c4 >> 5 ) & 0x07 );
		*pdst++ = ( c4 << 3 ) | ( ( c5 >> 4 ) & 0x0f );
		*pdst++ = ( c5 << 4 ) | ( ( c6 >> 3 ) & 0x1f );
		*pdst++ = ( c6 << 5 ) | ( ( c7 >> 2 ) & 0x3f );
		*pdst++ = ( c7 << 6 ) | ( ( c8 >> 1 ) & 0x7f );
	}

	for( i = 0; i < 74; i++ )
	{
		*pdst++ = *psrc++;
	}
}

/*
   解压缩数据，将7bit转为8bit
 */

static void decompress_data( uint8_t *src, uint8_t *dst )
{
	uint8_t *psrc	= src;
	uint8_t *pdst	= dst;
	uint8_t c1, c2, c3, c4, c5, c6, c7;
	uint8_t i;
/*字节就是7bit 速度压缩后的编码53byte*/
	for( i = 0; i < 7; i++ )
	{
		c1	= *psrc++;
		c2	= *psrc++;
		c3	= *psrc++;
		c4	= *psrc++;
		c5	= *psrc++;
		c6	= *psrc++;
		c7	= *psrc++;

		*pdst++ = c1 & 0xfe;
		*pdst++ = ( ( c1 & 0x01 ) << 7 ) | ( ( c2 >> 1 ) & 0x7e );
		*pdst++ = ( ( c2 & 0x03 ) << 6 ) | ( ( c3 >> 2 ) & 0x3e );
		*pdst++ = ( ( c3 & 0x07 ) << 5 ) | ( ( c4 >> 3 ) & 0x1e );
		*pdst++ = ( ( c4 & 0x0f ) << 4 ) | ( ( c5 >> 4 ) & 0x0e );
		*pdst++ = ( ( c5 & 0x1f ) << 3 ) | ( ( c6 >> 5 ) & 0x06 );
		*pdst++ = ( ( c6 & 0x3f ) << 2 ) | ( ( c7 >> 6 ) & 0x02 );
		*pdst++ = ( ( c7 & 0x7f ) << 1 );
	}
	for( i = 0; i < 74; i++ )
	{
		*pdst++ = *psrc++;
	}
}

/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
static void dump( uint8_t *info, uint16_t len )
{
	uint16_t	i, j = 0;
	uint8_t		*p = info;
	for( i = 0; i < len; i++ )
	{
		if( j == 0 )
		{
			rt_kprintf( "\r\n>" );
		}
		rt_kprintf( "%02x ", *p++ );
		j++;
		if( j == 16 )
		{
			j = 0;
		}
	}
}

/*
   初始化VDR存储区域，并定位到最大可写入的小时记录地址
   0:只是初始化
   1:格式化
 */
uint32_t vdr_init( uint8_t cmd )
{
	uint8_t		buf[128];
	uint32_t	addr;

	YMDH_TIME	i;
/*遍历所有的小时记录头*/
	for( addr = VDR_08H_09H_START; addr < VDR_08H_09H_END; addr += 8192 )
	{
		rt_kprintf( "\r\nVDR>addr=0x%08x", addr );
		SST25V_BufferRead( buf, addr, 16 );
		i = ( buf[0] << 24 ) | ( buf[1] << 16 ) | ( buf[2] << 8 ) | ( buf[3] );
		if( i == 0xFFFFFFFF )
		{
			continue; /*不是有效的数据头*/
		}
#ifdef STRICT_MATCH
		if( i > vdr_ymdh_curr_wr )
#else
		if( buf[3] > ( vdr_ymdh_curr_wr & 0xff ) )
#endif
		{
			vdr_addr_curr_wr	= addr;
			vdr_ymdh_curr_wr	= i;
		}
	}

	if( vdr_ymdh_curr_wr == 0 ) /*没找到*/
	{
		vdr_addr_curr_wr = VDR_08H_09H_START;
	}
	rt_kprintf( "\r\nvdr_init vdr_ymdh_curr_wr=%08x  vdr_addr_curr_wr=%08x", vdr_ymdh_curr_wr, vdr_addr_curr_wr );
	return 0;
}

FINSH_FUNCTION_EXPORT( vdr_init, init vdr );


/*
   写入1分钟的vdr数据 135字节
   有可能没有写入，但要空出空间,但是长时间没有数据如何处理或空出

   改成固定长度的访问(每小时8KB)

   传递进来的是
    yymmddhhmm	  (5byte)
    60组速度和状态  (120Byte)
    单位分钟位置信息 (10Byte) 取该分钟范围内首个有效的位置信息，否则为7FFFFFFFH；
 */
void vdr_write_minute( uint8_t *vdrinfo )
{
	uint8_t		buf[128];
	uint8_t		*p = vdrinfo;

	YMDH_TIME	h_tm;
	uint32_t	addr;
	uint8_t		year	= *( p + 0 );
	uint8_t		month	= *( p + 1 );
	uint8_t		day		= *( p + 2 );
	uint8_t		hour	= *( p + 3 );
	uint8_t		min		= *( p + 4 );

	h_tm = ( year << 24 ) | ( month << 16 ) | ( day << 8 ) | ( hour );

	if( h_tm != vdr_ymdh_curr_wr ) /*又过了一个小时*/
	{
		/*todo:要不要把剩下的内容写成缺省值*/

		vdr_ymdh_curr_wr	= h_tm;
		vdr_addr_curr_wr	+= 0x1FFF;
		vdr_addr_curr_wr	&= 0xFFFF2000;
		if( vdr_addr_curr_wr >= VDR_08H_09H_END )
		{
			vdr_addr_curr_wr = VDR_08H_09H_START;
		}
		/*清除随后的8k,用作存储,这里没有判首次上电，多搽除了一次，没有关系*/
		SST25V_SectorErase_4KByte( vdr_addr_curr_wr );
		SST25V_SectorErase_4KByte( vdr_addr_curr_wr + 4096 );
		/*写入当前的小时记录字段*/
		SST25V_strWrite( p, vdr_addr_curr_wr, 5 );
	}
	/*转换到分钟的存储位置,第一个128byte是小时时间戳头，后面才开始分钟内秒速度状态*/
	addr = vdr_addr_curr_wr + min * 128 + 128;
	compress_data( vdrinfo, buf );
	SST25V_strWrite( buf, addr, 128 );
}

/*
   减少一小时
 */
static YMDH_TIME vdr_ymdh_decrease( YMDH_TIME tm )
{
	uint8_t y, m, d, h;

	y	= tm >> 24; /*2000--2255年*/
	m	= tm >> 16;
	d	= tm >> 8;
	h	= tm & 0xff;

	if( h == 0 )
	{
		h = 23;
		if( d == 1 )
		{
			switch( m )
			{
				case 1: m = 12; y--; d = 31;
					break;
				case 3: m = 2; d = ( y % 4 == 0 ) ? 29 : 28;
					break;
				case 5:
				case 7:
				case 8:
				case 10:
				case 12: m--; d = 30;
					break;
				case 2:
				case 4:
				case 6:
				case 9:
				case 11:
					m--; d = 31;
					break;
			}
		}else
		{
			d--;
		}
	}else
	{
		h--;
	}
	return ( y << 24 ) | ( m << 16 ) | ( d << 8 ) | h;
}

//------- stuff     user data       ---------------
u16  stuff_drvData(u8 type,u16 Start_recNum,u16 REC_nums,u8 *dest)
{
    u16  res_len=0,get_len=0;
    u16  i=0;

	for(i=0;i<REC_nums;i++)
	{
	  switch(type)
	  	{
	  	   case 0x08:
                     get_len=get_08h(Start_recNum+i,dest+res_len);
					 break;
		   case 0x09:
                     get_len=get_09h(Start_recNum+i,dest+res_len);    
					 break;
		   case 0x10:
                     get_len=get_10h(Start_recNum+i,dest+res_len);
					 break;
		   case 0x11:
                     get_len=get_11h(Start_recNum+i,dest+res_len);
					 break;
		   case 0x12:
                     get_len=get_12h(Start_recNum+i,dest+res_len);
					 break;	
		   case 0x13:
                     get_len=get_13h(Start_recNum+i,dest+res_len);
					 break;		
		   case 0x14:
                     get_len=get_14h(Start_recNum+i,dest+res_len);
					 break;		
		   case 0x15:
                     get_len=get_15h(Start_recNum+i,dest+res_len); 
					 break;	
		   default:
		   	         return 0;

	  	}
	   
	   res_len+=get_len;
	}    
    return   res_len;
}









// 00H  采集行车记录仪执行标准版本
u16   get_00h(u8  *p_in ,u16 inLen, u8 *p_out)   
{


}

// 01H  采集当前驾驶员人信息
u16   get_01h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

//  02H   采集记录仪的实时时间
u16   get_02h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

// 03H  采集累计行驶里程
u16   get_03h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

u16   get_04h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

u16   get_05h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

u16   get_06h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

u16   get_07h(u8  *p_in ,u16 inLen, u8 *p_out)  
{


}

/*
   行驶速度记录
   不管他，自己填数据，自己报
   单条126字节每分钟
   48*60=2880包

    每一条   128 字节

 */

u16 get_08h( u16 indexnum,u8 *p)    
{
	int			i;
	uint8_t		buf[128];
    u32  addr=0;
	u8  FCS=0;


    
	//	  1.  get  address	 
	 addr=VDR_08H_START+indexnum*128;

   //    2. read   
      SST25V_BufferRead(buf, addr, 128 );
   
       OutPrint_HEX("08H content",buf,128); // debug   
   //     3.  FCS  check
           FCS=0;
       for(i=0;i<127;i++)
	 	   FCS^=buf[i];
	   if(buf[127]!=FCS)
	   	{
	   	     rt_kprintf("\r\n  08H read fcs error  save:%X  cacu: %X \r\n",buf[127],FCS);
             return  0;
	   	}
     if(buf[0]==MarkByte)
      {  
         memcpy(p,buf+1,126);
	     return  126;
      }	
	 else 
	 	 return 0; 
}



FINSH_FUNCTION_EXPORT( get_08h, get_08 );


/*
   位置信息
   360小时   ，每小时666字节
 */

u16  get_09h( u16 indexnum,u8 *p)  
{
	 int		 i;
	 uint8_t	 buf[669];
	 u32  addr=0;
	 u8  FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_09H_START+indexnum*768;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 669 ); //2+content 
	   
	   OutPrint_HEX("09H content",buf,669); // debug 
	//	   3.  FCS	check
	          FCS=0;
		for(i=0;i<667;i++)
			FCS^=buf[i];
		if(buf[667]!=FCS)
		 {
			  rt_kprintf("\r\n	09H read fcs error	save:%X  cacu: %X \r\n",buf[667],FCS);
			  return  0;
		 }
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,666); 
		  return  666;
	   } 
	  else 
		  return 0; 

}

FINSH_FUNCTION_EXPORT( get_09h, get_09 );


/*
   事故疑点
   234Byte
   共100个
 */
u16 get_10h( u16 indexnum,u8 *p)  
{
	 int		 i;
	 uint8_t	 buf[237];
	 u32  addr=0;
	 u8   FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_10H_START+indexnum*256;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 236 );	
	   OutPrint_HEX("10H content",buf,236); // debug 
	//	   3.  FCS	check
	         FCS=0;
		for(i=0;i<235;i++)
			FCS^=buf[i];
		if(buf[235]!=FCS)
		 {
			  rt_kprintf("\r\n	10H read fcs error	save:%X  cacu: %X \r\n",buf[235],FCS);
			  return  0;
		 }
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,234); 
		  return  234;
	   } 
	  else 
		  return 0; 

}

FINSH_FUNCTION_EXPORT( get_10h, get_10 );


/*超时驾驶记录
   50Bytes   100条

      每个记录   64   个字节
 */

u16 get_11h( u16 indexnum,u8 *p)  
{
	
	 int		 i;
	 uint8_t	 buf[60];
	 u32  addr=0;
	 u8  FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_11H_START+indexnum*64;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 52 );	
	   OutPrint_HEX("11H content",buf,52); // debug 
	//	   3.  FCS	check
	        FCS=0;
		for(i=0;i<51;i++)
			FCS^=buf[i];
		if(buf[51]!=FCS)
		 {
			  rt_kprintf("\r\n	11H read fcs error	save:%X  cacu: %X \r\n",buf[51],FCS);
			  return  0;
		 }
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,50); 
		  return  50;
	   } 
	  else 
		  return 0; 

	
}

FINSH_FUNCTION_EXPORT( get_11h, get_11 );


/*驾驶员身份登录
   25Bytes   200条
 */

u16 get_12h( u16 indexnum,u8 *p)  
{
     int		 i;
	 uint8_t	 buf[30];
	 u32  addr=0;
	 u8  FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_12H_START+indexnum*32;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 27 );
	   
	   OutPrint_HEX("12H content",buf,27); // debug 
	//	   3.  FCS	check
	       FCS=0;
		for(i=0;i<26;i++)
			FCS^=buf[i];
		if(buf[26]!=FCS)
		 {
			  rt_kprintf("\r\n	11H read fcs error	save:%X  cacu: %X \r\n",buf[26],FCS); 
			  return  0;
		 }
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,25); 
		  return  25;
	   } 
	  else 
		  return 0; 
}

FINSH_FUNCTION_EXPORT( get_12h, get_12 );


/*外部供电记录
   7字节 100条**/
u16 get_13h( u16 indexnum,u8 *p)  
{
     int		 i;
	 uint8_t	 buf[10];
	 u32  addr=0;
	 u8  FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_13H_START+indexnum*8;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 8 );	
	   OutPrint_HEX("13H content",buf,8); // debug 
	//	   3.  FCS	check
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,7); 
		  return  7;
	   } 
	  else 
		  return 0; 

}

FINSH_FUNCTION_EXPORT( get_13h, get_13 );


/*记录仪参数
   7字节 100条    8 */
u16 get_14h( u16 indexnum,u8 *p)  
{
     int		 i;
	 uint8_t	 buf[10];
	 u32  addr=0;
	 u8  FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_14H_START+indexnum*8;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 8 );	
	   OutPrint_HEX("14H content",buf,8); // debug 
	//	   3.  FCS	check
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,7); 
		  return  7;
	   } 
	  else 
		  return 0; 

}

FINSH_FUNCTION_EXPORT( get_14h, get_14 );


/*
   速度状态日志
   133Byte    256
   共10个
 */
u16 get_15h( u16 indexnum,u8 *p)  
{
	
     int		 i;
	 uint8_t	 buf[30];
	 u32  addr=0;
	 u8  FCS=0;
	
	
	 
	 //    1.  get	address   
	  addr=VDR_15H_START+indexnum*256;
	
	//	  2. read	
	   SST25V_BufferRead(buf, addr, 135 );
	   OutPrint_HEX("15H content",buf,135); // debug 
	//	   3.  FCS	check
		for(i=0;i<134;i++)
			FCS^=buf[i];
		if(buf[134]!=FCS)
		 {
			  rt_kprintf("\r\n	15H read fcs error	save:%X  cacu: %X \r\n",buf[134],FCS); 
			  return  0;
		 }
	  if(buf[0]==MarkByte)
	   {  
		  memcpy(p,buf+1,133); 
		  return  133;
	   } 
	  else 
		  return 0;

}

FINSH_FUNCTION_EXPORT( get_15h, get_15 );








//==========  write   ------------------------------------------------------------------------------
u16  vdr_creat_08h( u16 indexnum,u8 *p, u16 inLen) 
{
    u8  inbuf[128];
	u8  FCS=0,i=0;
	u32 inaddress=0;//  写入地址

    //  1.  Stuff  Head 
	inbuf[0]=MarkByte;           //
    //  2.   Index  get  address  
    if(indexnum>VDR_08_MAXindex)
    	{
             Vdr_Wr_Rd_Offset.V_08H_Write=0; 
			 indexnum=0;
    	}
	memcpy(inbuf+1,p,inLen);   //  126 bytes
   //   3.   caculate  fcs
     for(i=0;i<127;i++)
	 	FCS^=inbuf[i];
	 inbuf[127]=FCS;   
   //    4.  get  address 	
	inaddress=VDR_08H_START+indexnum*128;

	//  判断擦除相关区域
	if( ( inaddress & 0x0FFF ) == 0 )
			{
			    WatchDog_Feed();
				SST25V_SectorErase_4KByte( inaddress );
				delay_ms(20);
			}
   //     5 .  write
     WatchDog_Feed(); 
    SST25V_strWrite(inbuf, inaddress, 128 );  
    delay_ms(5);

	return  indexnum;
}


//	666=>>768 rec_size   
u16  vdr_creat_09h( u16 indexnum,u8 *p, u16 inLen) 
{
    u8  inbuf[669];     
	u8  FCS=0;
	u16 i=0;
	u32 inaddress=0;//  写入地址
   
    //  1.  Stuff  Head 
	inbuf[0]=MarkByte;           //
    //  2.   Index  get  address  
    if(indexnum>VDR_09_MAXindex)
    	{
             Vdr_Wr_Rd_Offset.V_09H_Write=0; 
			 indexnum=0;
    	}
	memcpy(inbuf+1,p,inLen);   // 666 bytes   
   //   3.   caculate  fcs
       for(i=0;i<667;i++)
	    	FCS^=inbuf[i];
	   
	   inbuf[667]=FCS;   
   //    4.  get  address 	
	inaddress=VDR_09H_START+indexnum*768;

	//  判断擦除相关区域
	if( ( inaddress&0x0FFF)==0 )
			{
			    WatchDog_Feed();
				SST25V_SectorErase_4KByte( inaddress );   
				delay_ms(20); 
			}
   //     5 .  write
    WatchDog_Feed(); 
    SST25V_strWrite(inbuf, inaddress, 668 ); // 2+content
    delay_ms(10); 
   
     return  indexnum;

} 

/*
   事故疑点 Accident Point    234
 */
 
u16  vdr_creat_10h( u16 indexnum,u8 *p, u16 inLen) 
{
   
      u8  inbuf[256];
	  u8  FCS=0,i=0;
	  u32 inaddress=0;//  写入地址
	 
	  //  1.  Stuff  Head 
	  inbuf[0]=MarkByte	;	   //
	  //  2.   Index  get  address	
	  if(indexnum>VDR_10_MAXindex)
		  {
			   Vdr_Wr_Rd_Offset.V_10H_Write=0; 
			   indexnum=0;
		  }
	  memcpy(inbuf+1,p,inLen);	 // 666 bytes
	 //   3.   caculate  fcs
	   for(i=0;i<235;i++)
		  FCS^=inbuf[i];
	   inbuf[235]=FCS;	 
	 //    4.  get	address   
	  inaddress=VDR_10H_START+indexnum*256;
   
	  //  判断擦除相关区域
	  if( ( inaddress & 0x0FFF ) == 0 )
			  {
				  WatchDog_Feed();
				  SST25V_SectorErase_4KByte( inaddress );
				  delay_ms(20);
			  }
	 // 	5 .  write
	  WatchDog_Feed(); 
	  SST25V_strWrite(inbuf, inaddress, 236 ); // 2+content
	  delay_ms(10);
   
	   return  indexnum;

}

/***********************************************************
* Function:
* Description:       recordsize 5==>> 64 size 
* Input:             100   条
* Input:
* Output:
* Return:
* Others:
***********************************************************/

u16  vdr_creat_11h( u16 indexnum,u8 *p, u16 inLen) 
{

   
	u8	inbuf[60];
	u8	FCS=0,i=0;
	u32 inaddress=0;//	写入地址
   
	//	1.	Stuff  Head 
	inbuf[0]=MarkByte;		 //
	//	2.	 Index	get  address  
	if(indexnum>VDR_11_MAXindex)
		{
			 Vdr_Wr_Rd_Offset.V_11H_Write=0; 
			 indexnum=0;
		}
	memcpy(inbuf+1,p,inLen);   // 666 bytes
   //	3.	 caculate  fcs
	 for(i=0;i<51;i++)
		FCS^=inbuf[i];
	 inbuf[51]=FCS;   
   //	 4.  get  address	
	inaddress=VDR_11H_START+indexnum*64;
   
   //	  5 .  write   
	SST25V_OneSector_Write(inbuf, inaddress, 52 ); // 2+content     读擦写  
     WatchDog_Feed(); 
	 return  indexnum;

}
/*
   外部供电记录
   都存在4k的记录里，1次读出

      25 =>>   32
 */

u16  vdr_creat_12h( u16 indexnum,u8 *p, u16 inLen) 
{
  
   u8  inbuf[30];
   u8  FCS=0,i=0;
   u32 inaddress=0;//  写入地址
  
   //  1.  Stuff  Head 
   inbuf[0]=MarkByte;		//
   //  2.	Index  get	address  
   if(indexnum>VDR_12_MAXindex)
	   {
			Vdr_Wr_Rd_Offset.V_12H_Write=0; 
			indexnum=0;
	   }
   memcpy(inbuf+1,p,inLen);   // 666 bytes
  //   3.	caculate  fcs
	for(i=0;i<26;i++)
	   FCS^=inbuf[i];
	inbuf[26]=FCS;	 
  //	4.	get  address   
   inaddress=VDR_12H_START+indexnum*32; 
  
  //	 5 .  write   
   SST25V_OneSector_Write(inbuf, inaddress, 27 ); // 2+content	   读擦写   
    WatchDog_Feed(); 
   return	indexnum;

}


/*都存在4k的记录里，1次读出
     7 bytes    >> 8       record size     

*/
u16  vdr_creat_13h( u16 indexnum,u8 *p, u16 inLen) 
{
   u8  inbuf[10];
   u8  FCS=0,i=0;
   u32 inaddress=0;//  写入地址
  
   //  1.  Stuff  Head 
   inbuf[0]=MarkByte;		//
   //  2.	Index  get	address  
   if(indexnum>VDR_13_MAXindex)
	   {
			Vdr_Wr_Rd_Offset.V_13H_Write=0; 
			indexnum=0;
	   }
   memcpy(inbuf+1,p,inLen);   // 666 bytes
/*  //   3.	caculate  fcs
	for(i=0;i<8;i++)
	   FCS^=inbuf[i];
	inbuf[8]=FCS;	 */
  //	4.	get  address   
   inaddress=VDR_13H_START+indexnum*8; 
  
  //	 5 .  write   
   SST25V_OneSector_Write(inbuf, inaddress, 8 ); // 2+content	   读擦写   
   WatchDog_Feed();   
   return	indexnum;
    

}

/*都存在4k的记录里，1次读出
     7 bytes    >> 8 	  record size 

*/
u16  vdr_creat_14h( u16 indexnum,u8 *p, u16 inLen) 
{
   u8  inbuf[10];
   u8  FCS=0,i=0;
   u32 inaddress=0;//  写入地址
  
   //  1.  Stuff  Head 
   inbuf[0]=MarkByte;		//
   //  2.	Index  get	address  
   if(indexnum>VDR_14_MAXindex)
	   {
			Vdr_Wr_Rd_Offset.V_14H_Write=0; 
			indexnum=0;
	   }
   memcpy(inbuf+1,p,inLen);   // 666 bytes
/*  //   3.	caculate  fcs
	for(i=0;i<8;i++)
	   FCS^=inbuf[i];
	inbuf[8]=FCS;	 */
  //	4.	get  address   
   inaddress=VDR_14H_START+indexnum*8; 
  
  //	 5 .  write   
   SST25V_OneSector_Write(inbuf, inaddress, 8 ); // 2+content	   读擦写   
   WatchDog_Feed(); 
   return	indexnum;
  
}

/*都存在4k的记录里，1次读出
       133   >>  256 
*/
u16  vdr_creat_15h( u16 indexnum,u8 *p, u16 inLen) 
{
	
	 u8  inbuf[260];
	 u8  FCS=0,i=0;
	 u32 inaddress=0;//  写入地址
	
	 //  1.  Stuff	Head 
	 inbuf[0]=MarkByte; 	  //
	 //  2.   Index  get  address  
	 if(indexnum>VDR_15_MAXindex)
		 {
			  Vdr_Wr_Rd_Offset.V_15H_Write=0; 
			  indexnum=0;
		 }
	 memcpy(inbuf+1,p,inLen);	// 666 bytes
	//	 3.   caculate	fcs
	  for(i=0;i<134;i++)
		 FCS^=inbuf[i];
	  inbuf[134]=FCS;   
	//	  4.  get  address	 
	 inaddress=VDR_15H_START+indexnum*256; 
	
	//	   5 .	write	
	 SST25V_OneSector_Write(inbuf, inaddress, 135); // 2+content	 读擦写   
	 WatchDog_Feed(); 
	 return   indexnum;

}

//---------------------------------------------------------------------------------

/*
     input:  type       <->    08H   09H 
               startindex  <->  起始索引  
               getnum  <->  获取记录条数
     output:
                 stuff length
                 outStr        填充到该指针下

*/
u16  multiRec_get_08h( u8  type,u16  startindex, u16   getnum , u8  *outStr )
{
    u16  i=0;

     for(i=0;i<getnum;i++)
     	{  //--------------------------------------------------------------------------------------------
			 switch(type)
			 	{
			 	   case 0x08:  
                              get_08h(startindex,outStr);
							  outStr=outStr+126;  // 偏移地址
							  startindex++;
							  if(startindex>VDR_08_MAXindex)
							  	     startindex=0; 
					          break;
			 	   case 0x09:  

					          break;
				   case 0x10:  

					          break;
				   case 0x11:  

					          break;
				   case 0x12:  

					          break;
				   case 0x13:  

					          break;
				   case 0x14:  

					          break;			  
		           case 0x15:  

					          break;
				   default:
				   	          return 0;

			 	}
     	}//--------------------------------------------------------------------------  		 
}











/*
   接收vdr命令,天津平台看到有粘包的情况
   通过 Recoder_obj 返回
 */
uint8_t vdr_rx( uint8_t * info, uint16_t count )
{
	uint8_t		*p = info;
	uint8_t		cmd;
	uint16_t	len;
	uint8_t		dummy;
	uint16_t	blocks;
	uint8_t		i, j, k;

	rt_kprintf( "\r\n vdr_rx %d bytes\r\n", count );
	for( i = 0; i < count; i++ )
	{
		rt_kprintf( "%02x ", *p++ );
	}

	if( *p++ != 0xAA )
	{
		return 1;
	}
	if( *p++ != 0x75 )
	{
		return 2;
	}
	cmd		= *p++;
	len		= ( *p++ << 8 );
	len		|= *p++;
	dummy	= *p++;

	vdr_cmd.ymdh_start = 0;
	for( k = 0; k < 4; k++ )
	{
		i					= *p++;
		j					= ( i >> 4 ) * 10 + ( i & 0x0f );
		vdr_cmd.ymdh_start	<<= 8;
	}
	p					+= 2;
	vdr_cmd.ymdh_end	= 0;
	for( k = 0; k < 4; k++ )
	{
		i					= *p++;
		j					= ( i >> 4 ) * 10 + ( i & 0x0f );
		vdr_cmd.ymdh_end	<<= 8;
	}
	p				+= 2;
	blocks			= ( *p++ << 8 );
	blocks			|= *p;
	vdr_cmd.blocks	= blocks;

	rt_kprintf( "\r\nVDR> rx start:%08x end:%08x block:%d", vdr_cmd.ymdh_start, vdr_cmd.ymdh_end, vdr_cmd.blocks );

	return 0;
}

/*更新行车记录仪数据*/
void thread_usb_vdr( void* parameter )
{
	int			i, res;
	int			count = 0;
	uint8_t		buf[520];
	uint32_t	addr;
	int			fd = -1;
	uint8_t		*p;
/*查找U盘*/
	if( rt_device_find( "udisk" ) == RT_NULL ) /*没有找到*/
	{
		rt_kprintf( "\r\nVDR>没有UDISK" );
		return;
	}

/*查找指定文件*/

	p		= (uint8_t*)parameter;
	addr	= *p++ << 24;
	addr	|= *p++ << 16;
	addr	|= *p++ << 8;
	addr	|= *p++;
	rt_kprintf( "\r\nusb write to %08x", addr );
	strcpy( (char*)buf, "/udisk/" );
	strcat( (char*)buf, p );


/*
   addr=0x00300000;
   strcpy(buf,"/udisk/vdr.bin");
 */
	fd = open( buf, O_RDONLY, 0 );
	if( fd >= 0 )
	{
		rt_kprintf( "\r\nVDR>读取文件" );
	} else
	{
		rt_kprintf( "\r\nVDR>文件不存在" );
		return;
	}
/*写入*/

	while( 1 )
	{
		WatchDog_Feed( );
		rt_thread_delay( RT_TICK_PER_SECOND / 5 );
		res = read( fd, buf, 512 );
		if( res < 0 )
		{
			rt_kprintf( "\r\nVDR>读取文件出错，res=%x", res );
			goto end_upgrade_usb_1;
		}else
		{
			if( res == 0 )
			{
				rt_kprintf( "\r\nVDR>写入完成 %d byte", count );
				goto end_upgrade_usb_1;
			}
			/*写入数据*/
			count += res;
			if( ( addr & 0x0FFF ) == 0 )
			{
				SST25V_SectorErase_4KByte( addr );
				//sst25_erase_4k
			}

			for( i = 0; i < 512; i++ )
			{
				SST25V_ByteWrite( buf[i], addr );
				addr++;
			}

			//sst25_write_through( add, uint8_t *p, uint16_t len)
			rt_kprintf( "\r\nVDR>写入 %d byte", count );
			if( res < 512 )
			{
				rt_kprintf( "\r\nVDR>写入完成 %d byte", count );
				goto end_upgrade_usb_1;
			}
		}
	}
end_upgrade_usb_1:
	if( fd >= 0 )
	{
		close( fd );
	}
}

/*
   调试vdr

   vdr_cmd(0x09,"12:34:56","13:11:03",1)

 */
uint8_t vdr_start( uint8_t cmd, char *start, char *end, uint16_t blocks )
{
	int h, m, s;
	fvdr_debug = 1;

	sscanf( start, "%d:%d:%d", &h, &m, &s );
	vdr_cmd.ymdh_start		= h;
	vdr_cmd.minute_start	= m;

	sscanf( end, "%d:%d:%d", &h, &m, &s );
	vdr_cmd.ymdh_end	= h;
	vdr_cmd.minute_end	= m;

	vdr_cmd.blocks			= blocks;
	vdr_cmd.blocks_remain	= blocks;

	vdr_cmd.ymdh_curr = vdr_cmd.ymdh_end;

	vdr_cmd.cmd = cmd;
	
/*
	switch( cmd )
	{
		case 0x08: vdr_cmd.addr = 0x00300000; get_08h( ); break;
		case 0x09: get_09h( ); break;
		case 0x10: get_10h( ); break;
		case 0x11: get_11h( ); break;
		case 0x12: get_12h( ); break;
		case 0x13: get_13h( ); break;
		case 0x14: get_14h( ); break;
		case 0x15: get_15h( ); break;
	}
*/

}

FINSH_FUNCTION_EXPORT( vdr_start, debug vdr );


/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
void vdr_import( uint32_t addr, char* filename )
{
	file_rec[0] = addr >> 24;
	file_rec[1] = addr >> 16;
	file_rec[2] = addr >> 8;
	file_rec[3] = addr & 0xFF;
	strcpy( file_rec + 4, filename );

	tid_usb_vdr = rt_thread_create( "usb_vdr", thread_usb_vdr, (void*)&file_rec, 1024, 9, 5 );
	if( tid_usb_vdr != RT_NULL )
	{
		rt_kprintf( "线程创建成功" );
		rt_thread_startup( tid_usb_vdr );
	}else
	{
		rt_kprintf( "线程创建失败" );
	}
}

FINSH_FUNCTION_EXPORT( vdr_import, import vdr );

#define DBG_VDR

#ifdef DBG_VDR
static uint8_t testbuf[1000];
#endif


/************************************** The End Of File **************************************/
