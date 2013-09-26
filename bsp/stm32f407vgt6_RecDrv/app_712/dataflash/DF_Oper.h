
//================================================================
/*         ¶ÁÐ´AT45DB16º¯ÊýµÄÍ·ÎÄ¼þ
MSP430 IAR application builder : 2007-04-15 9:00:00
Target : MSP430F149
Crystal: 3.6864Mhz
*/
//================================================================
//#include "common.h" 
//#include "71x_type.h"

#ifndef _H_AT45
#define _H_AT45

#define    PageSIZE     512 
#define  DFBakSize   150//50 

//================================================================
/* 
  Flash Chip : SSST25VF032B-50-4I-S2AF 
  ChipSize       : 4MBytes       PageSize(vitual): 512Bytes  SectorSize:4K<=>8 Pages    Chip: 1024Sectors<=>8192Pages
  
  Regulation :
                  

                    
<Ò»>   ÏµÍ³²ÎÊý ÒÔ¼° Ó¦ÓÃ²ÎÊý  ÐÐ³µ¼ÇÂ¼Ïà¹ØµØÖ·´æ´¢²¿·Ö
*/


/*               Dataflash Page  ¹æ»®   ------->    Start          */

 /*  0. Page 0~9     Producet Info */
 #define DF_ProInfo_Page      0

  /*  1. page 10 -903	  ISP	*/
#define ISP_StartArea                                 0x1000        // ÆðÊ¼µØÖ· 
#define DF_APP1_PageNo		                          8             /*
                                                                                                                        start :   0x1000---- ºÍBoot ³ÌÐò¶ÔÓ¦  8 page
                                                                                                                        size        60 sector     480 page
                                                                                                                        
                                                                                                                         DF_APP_flah run PageNo:   50  ~ 903  page        */
																					/* 512K  -->1072 Page */ 


//   2.     config   information
#define    ConfigStart_offset                         808        //   Block   ÆðÊ¼Î»ÖÃ  Conifg  Struct Save      Sector 1 
#define    TiredCondifg_offset                        864        //   Block   ÆðÊ¼Î»ÖÃ  Conifg  Struct Save      Sector 3  
#define    JT808_BakSetting_offset                    960        //   Block   ÆðÊ¼Î»Ö
#define    JT808_Bak2Setting_offset                   6240        //   Block   ÆðÊ¼Î»Ö 
#define    JT808Start_offset                          1000        //   Block   ÆðÊ¼Î»ÖÃ  Conifg  Struct Save    Sector 2 
 


                                                                                          

 /*  2. Page  904 - 912           ×´Ì¬ÐÅÏ¢    */


 /*  3. Page  920 - 943        	GPSÓ¦ÓÃ ×î»ù±¾ÅäÖÃ    */   //¿ìËÙ¶ÁÐ´  
 #define  DF_socket_all                          1080     // BlockÆðÊ¼    socket 1 ,2, 3 
 #define  DF_APN_page			                 1088	 //
 #define  DF_ACC_ON_dur_Page                     1092     // ACC ¿ªÊ±·¢ËÍ¼ä¸ô     
 #define  DF_ACC_OFF_dur_Page					 1093 	 // ACC ¹ØÊ±·¢ËÍ¼ä¸ô 		   
 #define  DF_TCP_sd_Dur_Page                     1094     // TCP ·¢ËÍ¼ä¸ô
 #define  DF_TrigSDStatus_Page                   1095     // ´«¸ÐÆ÷´¥·¢ÉÏ±¨×´Ì¬
 #define  DF_CycleAdd_Page                       1096     // Block ÆðÊ¼-- ¼ÇÂ¼Ñ­»·´æ´¢¶ÁÐ´Æ«ÒÆµØÖ·µÄpage
 #define  DF_PhotoAdd_Page                       1104     // Block ÆðÊ¼--¼ÇÂ¼ÕÕÆ¬´æ´¢¶ÁÐ´Æ«ÒÆµØÖ·µÄpage 


 /*  4. ²»Í¬¿Í»§²úÆ·Ó¦ÓÃÌØÓÐ¹¦ÄÜ²ÎÊý           */
 #define  DF_DevConfirmCode_Page                 1112     // Block ÆðÊ¼-- ³µÁ¾Î±IP
 #define  DF_ListenNum_Page                      1113     // ÖÐÐÄ¼àÌýºÅÂë    
 #define  DF_Distance_Page                       1120     // Block ÆðÊ¼-- ³µÁ¾ÀÛ¼ÆÐÐÊ»Àï³Ì
 #define  DF_LoadState_Page                      1128     // Block ÆðÊ¼-- ³µÁ¾¸ºÔØ×´Ì¬ 
 #define  DF_Speed_GetType_Page               1136     // Block ÆðÊ¼--´æ´¢ËÙ¶È»ñÈ¡·½Ê½ 1Îª ËÙ¶È´«¸ÐÆ÷ 0ÎªGPS
 #define  DF_K_adjust_Page                          1144     // Block ÆðÊ¼--´æ´¢±êÊ¶ÊÇ·ñÌØÕ÷ÏµÊýÒÑ¾­±»×Ô¶¯Ð£×¼   1.Ð£×¼¹ý  0:ÉÐÎ´Ð£×¼
 #define  DF_ACCONFFcounter_Page              1152     // Block ÆðÊ¼--Òì³£¸´Î»Ê±´æ´¢ACCON_OffµÄ¼ÆÊýÊýÖµ
 #define  DF_TiredStartTime_Page                 1160     // Block ÆðÊ¼--Æ£ÀÍ¼ÝÊ»´¥·¢ºó£¬¼ÇÂ¼Æ£ÀÍ¼ÝÊ»µÄÆðÊ¼Ê±¼ä 
                                                         /*
                                                                        Byte1 Flag :   0 :Í£³µ  1:Í£³µµ«Ã»´¥·¢ 2:´¥·¢ÁË»¹Ã»½áÊø
                                                                        Byte2 TiredDrvStatus  Tired_drive.Tireddrv_status
                                                                        Byte3 On->off Flag
                                                                        Byte4~8: starttimeBCD 
                                                                  */ 
 #define  DF_OutGPS_Page                          1168    // Block ÆðÊ¼ -- ½ÓÍâ²¿GPSÐÅºÅÔ´×´Ì¬±êÖ¾   
 #define  DF_BD_Extend_Page                    1176   //  ±±¶·À©Õ¹

  //¸ÃÇøÓò½áÊø ÖÁ 1023  ( ¹²1024Page)                                                         


 /*  5.ÐÐ³µ¼ÇÂ¼ÒÇÏà¹Ø²ÎÊý  */ 
#define       DF_VehicleID_Page                        1192                           // Block ÆðÊ¼-³µÅÆºÅÂë
#define       DF_VehicleType_Page                      1200                           // Block ÆðÊ¼-³µÁ¾ÀàÐÍ
#define       DF_PropertiValue_Page                    1208                           // Block ÆðÊ¼-- ÌØÕ÷ÏµÊý
#define       DF_DriverID_Page                         1216                           // Block ÆðÊ¼--¼ÝÊ»Ô±ID ¼°±àÂë
#define       DF_TiredDrvAdd_Page                      1224                           // Block ÆðÊ¼--Æ£ÀÍ¼ÝÊ»µØÖ·
#define       DF_ExpSpdAdd_Page                        1232                           // Block ÆðÊ¼--³¬ËÙ±¨¾¯¼ÇÂ¼Æ«ÒÆµØÖ·
#define       DF_AccFireRecAdd_Page                    1240                           // Block ÆðÊ¼--Æû³µµã»ð¼ÇÂ¼µØÖ·Æ«ÒÆ
#define       DF_AvrgSpdPerMinAdd_Page                 1248                           // Block ÆðÊ¼--Ã¿·ÖÖÓÆ½¾ùËÙ¶È
#define       DF_AbnormalLogAdd_Page                   1256                           // Block ÆðÊ¼--Òì³£Log´æ´¢ 
#define       DF_RecordAdd_Page                        1264                           // Block ÆðÊ¼--ÐÐ³µ¼ÇÂ¼ÒÇÕý³£´æ´¢¼ÇÂ¼Æ«ÒÆµØÖ· 
#define       DF_MaxSpdPerDay_Page                     1272                           // Block ÆðÊ¼--µ±Ìì×î´óËÙ¶È
#define       DF_DayDistance_Page                      1280                           // Block ÆðÊ¼--µ±ÌìÀï³Ì  
#define       DF_DoubtAdd_Page                         1288                           // Block ÊÂ¹ÊÒÉµãÏà¹Ø
#define       DF_AvrgSpdSec_Page                       1296                           // Block ÆðÊ¼-Ã¿ÃëÖÓÆ½¾ùËÙ¶È
#define       DF_Login_Page				               1304                           // Block ÆðÊ¼-µÇÂ¼¼ÇÂ¼
#define       DF_Powercut_Page		                   1312                           // Block ÆðÊ¼-Íâ²¿µçÔ´¶Ï¿ª
#define       DF_Settingchg_Page	                   1320                           // Block ÆðÊ¼-²ÎÊýÐÞ¸Ä 
#define       DF_Minpos_Page                           1328                           // Block ÆðÊ¼-Ã¿·ÖÖÓÎ»ÖÃ´æ´¢
#define       DF_StdVer_Page                           1336                            // Block ÆðÊ¼-¹ú¼Ò±ê×¼°æ±¾
#define       DF_1stDate_page                          1344                           // Block ÆðÊ¼-³õ´Î°²×°Ê±¼ä 
#define       DF_DevOnlyID_Page                        1368                           // Block ÆðÊ¼-¼ÇÂ¼ÒÇÎ¨Ò»±àºÅ
#define       DF_SpeedLimt_Page                        1376                           // Block ÆðÊ¼-×î´óËÙ¶È×îÐ¡ËÙ¶È
#define       DF_TiredDoor_Page                        1384                           // Block ÆðÊ¼-Æ£ÀÍ¼ÝÊ»ÃÅÏÞ  
                                                                                       /*
                                                                                                       Á¬Ðø¼ÝÊ»Ê±¼ä¡¢µ±ÌìÀÛ¼Æ¼ÝÊ»Ê±¼ä¡¢×îÐ¡ÐÝÏ¢Ê±¼ä¡¢×î³¤Í£³µÊ±¼ä
                                                                                                      */
#define       DF_SDTime_Page                           1392                           // Block ÆðÊ¼-¶¨Ê±·½Ê½¼ä¸ô                
#define       DF_SDDistance_Page                       1400                           // Block ÆðÊ¼-¶¨¾à·¢ËÍ¾àÀë
#define       DF_SDMode_Page                           1408                           // Block ÆðÊ¼- ÖÕ¶ËÊý¾Ý·¢ËÍ·½Ê½
#define       DF_RTLock_Page                           1416                           // Block ÆðÊ¼- ÊµÊ±ÉÏ±¨ --
#define       DF_Event_Page                            1424                           // Block ÆðÊ¼- ÊÂ¼þÏà¹Ø  
#define       DF_Msg_Page                              1432                           // Block ÆðÊ¼- ÏûÏ¢Ïà¹Ø  
#define       DF_PhoneBook_Page                        1440                           // Block ÆðÊ¼- µç»°±¾Ïà¹Ø
#define       DF_CircleRail_Page                       1448                           // Block ÆðÊ¼- Ô²ÐÎÎ§À¸

#define       DF_RectangleRail_Page                    3000                           // Block ÆðÊ¼- ¾ØÐÎÎ§À¸  1288  --¹ý¼ìÑé¾ÍÓÃ7000 ÁË 24¸ö 

#define       DF_PolygenRail_Page                      1464                           // Block ÆðÊ¼- ¶à±ßÐÎÎ§À¸
#define       DF_PicIndex_Page                         1480                           // Block ÆðÊ¼- Í¼Ïñ¼ìË÷
#define       DF_SoundIndex_Page                       1488                           // Block ÆðÊ¼- ÒôÆµ¼ìË÷        
#define       DF_FlowNum_Page                          1496                           // Block ÆðÊ¼- Á÷Ë®ºÅ
   
// 16  ÎÄ±¾ÐÅÏ¢
#define       TextStart_offdet                         1504

//17.  ÓòÃû
#define       DF_DomainName_Page                        1512            

#define       DF_question_Page                         4000        //16m 4000  32m 4600                     // ÌáÎÊÐÅÏ¢´æ´¢ÆðÊ¼Ò³               

    //¸ÃÇøÓò½áÊø     

/*                    
<¶þ>   Ñ­»·´æ´¢ÉÏ±¨  ÐÐ³µ¼ÇÂ¼ÒÇÏà¹Ø¹¦ÄÜ Êý¾Ý´æ´¢Çø
*/

/*  I.  Function App Area                  ×¢: ÒÔÏÂPage ¹æ»®»ùÓÚ   SST25VF16     ÓÃÓÚÁÙÊ±Test                   */

//                     Name                                     PageNum                	 	                     Description                             
// 1.  Cycle Save Send Area
#define       CycleStart_offset                       1768                          // Ñ­»·´æ´¢ÉÏ±¨´æ´¢ÇøÓò(Basic »ù±¾±Ø±¸)        1 record=32 Bytes




// 2. Vehicle  Status Record Area 
#define       VehicleRecStart_offset                  2792                         // ÐÐ³µ¼ÇÂ¼ÒÇÊý¾Ý´æ´¢ÇøÓò(ÊÂ¹ÊÒÉµãÊý¾Ý)  1 record=256Bytes

// 3. Average Speed Per Minute
#define       AverageSpdStart_offset                  2992                         // ³µÁ¾Ã¿·ÖÖÓÆ½¾ùËÙ¶È(ÒªÇó¼ÇÂ¼ÖÁÉÙ360h)          1 record =70 Bytes

// 4. Tired Driving Record
#define       TiredDrvStart_offset                    3392                          //  Æ£ÀÍ¼ÝÊ»¼ÇÂ¼ÆðÊ¼Æ«ÒÆ

// 5. Exp  Speed  Record
#define       ExpSpdStart_offset                      3400                          //  ³¬ËÙ±¨¾¯Æ«ÒÆ

// 6. Average Minte position
#define      AvrgMintPosit_offset                     3408                         // ³µÁ¾Ã¿Ð¡Ê±ÄÚÃ¿·ÖÖÓÎ»ÖÃ¼ÇÂ¼   1 record =512 Bytes


// 7. Average Speed Per Second
#define      AvrgSpdSec_offset                        3432                         // ³µÁ¾Ã¿ÃëÖÓÆ½¾ùËÙ¶È¼ÇÂ¼       1 record =70 Bytes

// 8. Acc  Work  On  Record
#define       AccWorkOnStart_offset                 3512                          // ´ò»ð¼ÇÂ¼

// 9. Abnormal  Record 
#define       AbNormalStart_offset                    3420                         // Éè±¸Òì³£Log´æ´¢

// 10.  LogIn  Record
#define      LogIn_offset                             3528                         // ÓÃ»§µÇÂ¼¼ÇÂ¼

// 11. PowerCut Record
#define      PowerCut_offset                          3536                         // Íâ²¿µçÔ´¶Ï¿ª¼ÇÂ¼


// 12. Setting Change Record
#define     SettingChg_offset                         3544                    // ²ÎÊýÐÞ¸Ä¼ÇÂ¼


// 13. GPS MODULE
#define     DF_GPS_MODULE_offset                         3600                   // GPS ¶¨Î»Ä£¿éÅÐ¶Ï 

// 14. Picture   Area
                                                     /* 
                                                                filename            cameraNum    size
                                                                   19                         1             4
                                                     */
#define       PicStart_offset                          4096                          // Block ÆðÊ¼Î»ÖÃ Í¼Æ¬´æ´¢ÇøÓò(Current Save) ½«À´Òª·Åµ½TF¿¨ÖÐ
#define       PicStart_offset2                        4424                          // Block ÆðÊ¼Î»ÖÃ Í¼Æ¬2ÇøÓò 
#define       PicStart_offset3                        4752                          // Block ÆðÊ¼Î»ÖÃ Í¼Æ¬3ÇøÓò 
#define       PicStart_offset4                        5080                          // Block ÆðÊ¼Î»ÖÃ Í¼Æ¬4ÇøÓò  



// 15  Sound  Area
#define       SoundStart_offdet                      5248      //4200                 32K ¿Õ¼ä        // Block ÆðÊ¼Î»ÖÃ 15sÉùÒô´æ´¢ÇøÓò(Current Save) ½«À´Òª·Åµ½TF¿¨ÖÐ
			                                                                /*  
			                                                                             filesize              filename 
			                                                                                4  Bytes          5thstart   
			                                                                */
#define       SoundFileMax_Sectors                   5                              //  5 sect=5*8 pages =20s data



#define       DF_DeviceID_offset                      5400                 // Block ÆðÊ¼Î»ÖÃ   ³µÁ¾ID  12  Î» BCD   
#define       DF_License_effect                       5416
#define       DF_Vehicle_Struct_offset                5424                 // block ÆðÊ¼Î»ÖÃ   
#define       DF_VehicleBAK_Struct_offset             6200                 // block ÆðÊ¼Î»ÖÃ   
#define       DF_VehicleBAK2_Struct_offset            6216                 // block ÆðÊ¼Î»ÖÃ      

#define       DF_SIMID_12D                            6000                 // Block  	ÆðÊ¼Î»ÖÃ     
#define       DF_LOGIIN_Flag_offset                          6040                 // Block      ÆðÊ¼Î»ÖÃ 




//----  ²¹³ä
#define    DF_Broadcast_offset                      5300       //  Block   ÆðÊ¼Î»ÖÃ  ²¥±¨ÆðÊ¼µØÖ·
#define    DF_Route_Page                               5400      // 1304                           // Block ÆðÊ¼- Â·Ïß
#define    DF_turnPoint_Page                         5500       //  ¹Õµã
 #define   DF_AskQuestion_Page                    5600       //  ÖÐÐÄÌáÎÊ    
/*                Dataflash     <------------   End              */ 



//  =================  ÐÐ³µ¼ÇÂ¼ÒÇ Ïà¹Ø ============================
/*
    StartPage :    6320          Start Address offset :   0x316000       

    Area Size :
                          213   Sector       = 1704  pages
                           ----------------------
                           
				ÉÈÇø               
				1                                      00-07H
				135                                   08H               
				64                                     09H  
				7                                      10H
				2                                      11H 
				2                                      12H
				1                                      13H
				1                                      14H  
				1                                      15H   

          ----------  Ö»ÊÇÔÚÕâÀï×öÁË---  ×¢ÊÍ £¬¾ßÌå²Ù×÷ÔÚ Vdr.C 
				
*/



//-------------------------------------------------------


extern  u8   DF_LOCK;    //     Dataflash  Lock  


extern void	mDelaymS( u8 ms );
extern void DF_delay_us(u16 j);
extern void DF_delay_ms(u16 j);
extern void DF_ReadFlash(u16 page_counter,u16 page_offset,u8 *p,u16 length);
extern void DF_WriteFlash(u16 page_counter,u16 page_offset,u8 *p,u16 length);
extern void DF_ReadFlash(u16 page_counter,u16 page_offset,u8 *p,u16 length);
extern void DF_WriteFlashSector(u16 page_counter,u16 page_offset,u8 *p,u16 length);//512bytes Ö±½Ó´æ´¢
extern void DF_WriteFlashRemote(u16 page_counter,u16 page_offset,u8 *p,u16 length);//512bytes Ö±½Ó´æ´¢
extern void DF_WriteFlashDirect(u16 page_counter,u16 page_offset,u8 *p,u16 length);
extern void DF_Erase(void);
extern void DF_init(void); 

#endif
