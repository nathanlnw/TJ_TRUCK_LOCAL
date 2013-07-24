#include  <string.h>
#include "Menu_Include.h"
#include "Lcd.h"

#define width_hz   12
#define width_zf   6
#define top_line   14

static u8 License_SetFlag=1;
static u8 License_SetCounter_0=0,License_SetCounter_1=0,License_SetCounter_2=0,License_SetCounter_3=0;
static u8 License_SetComp=0;//主域名设置完成1
static u8 License_set_noeffect=0;
static u8 License_Type_flag=0;//区分组的选择和组内选择
static u8 License_Type_Counter=0;//  0: 数字    1:A-M         2:N-Z

unsigned char select_License[]={0x0C,0x06,0xFF,0x06,0x0C};

//-----  天地通要求 ----------------
unsigned char Car_HZ_code[31][2]={"冀","京","津","沪","渝","豫","云","辽","黑","湘",\
                                  "皖","鲁","新","苏","浙","赣","鄂","桂","甘","晋",\
                                  "蒙","陕","吉","闽","贵","粤","青","藏","川","宁",\
                                  "琼"};  
unsigned char ABC_License_0_9[10][1]={"0","1","2","3","4","5","6","7","8","9"};
unsigned char ABC_License_A_M[13][1]={"A","B","C","D","E","F","G","H","I","J","K","L","M"};
unsigned char ABC_License_N_Z[13][1]={"N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};


DECL_BMP(8,5,select_License);

void License_Type_Sel( u8 par)
{
	lcd_fill(0);
	lcd_text12(0,3,(char *)Menu_Car_license,License_SetFlag-1,LCD_MODE_SET);
	lcd_text12(5,18,"0-9  A-M  N-Z  汉字",19,LCD_MODE_SET);
	if(par==0)
		lcd_text12(5,18,"0-9",3,LCD_MODE_INVERT);
	else if(par==1)
		lcd_text12(5+5*6,18,"A-M",3,LCD_MODE_INVERT);
	else if(par==2)
		lcd_text12(5+10*6,18,"N-Z",3,LCD_MODE_INVERT);
	else if(par==3)
		lcd_text12(5+15*6,18,"汉字",4,LCD_MODE_INVERT);
	lcd_update_all();
}

void License_Set(u8 par,u8 type1_2)
{
	lcd_fill(0);
	lcd_text12(0,3,(char *)Menu_Car_license,License_SetFlag-1,LCD_MODE_SET);
	
	if(type1_2==1)
		{
		lcd_bitmap(par*width_zf, 14, &BMP_select_License, LCD_MODE_SET);
		lcd_text12(0,19,"0123456789",10,LCD_MODE_SET);
		}
	else if(type1_2==2)
		{
		lcd_bitmap(par*width_zf, 14, &BMP_select_License, LCD_MODE_SET);
		lcd_text12(0,19,"ABCDEFGHIJKLM",13,LCD_MODE_SET);
		}
	else if(type1_2==3)
		{
		lcd_bitmap(par*width_zf, 14, &BMP_select_License, LCD_MODE_SET);
		lcd_text12(0,19,"NOPQRSTUVWXYZ",13,LCD_MODE_SET);
		}
	else if(type1_2==4)
		{
		if((License_SetCounter_3>=0)&&(License_SetCounter_3<=9))
			{
			lcd_bitmap(3+par*width_hz, 14, &BMP_select_License, LCD_MODE_SET);
	        lcd_text12(0,20,"冀京津沪渝豫云辽黑湘",20,LCD_MODE_SET);
			}
		else if((License_SetCounter_3>=10)&&(License_SetCounter_3<=19))
			{
			lcd_bitmap(3+(par-10)*width_hz, 14, &BMP_select_License, LCD_MODE_SET);
	        lcd_text12(0,20,"皖鲁新苏浙赣鄂桂甘晋",20,LCD_MODE_SET);
			}
		else if((License_SetCounter_3>=20)&&(License_SetCounter_3<=29))
			{
			lcd_bitmap(3+(par-20)*width_hz, 14, &BMP_select_License, LCD_MODE_SET);
	        lcd_text12(0,20,"蒙陕吉闽贵粤青藏川宁",20,LCD_MODE_SET);
			}
		else if(License_SetCounter_3==30)
			{
			lcd_bitmap(3+(par-30)*width_hz, 14, &BMP_select_License, LCD_MODE_SET);
	        lcd_text12(0,20,"琼",2,LCD_MODE_SET);
			}
		}
	lcd_update_all();
}

static void msg( void *p)
{

}
static void show(void)
{
License_set_noeffect=1;

lcd_fill(0);
lcd_text12(0,3,"设置车牌号",10,LCD_MODE_INVERT);
lcd_text12(0,18,"无牌照车辆",10,LCD_MODE_SET);
lcd_update_all();
}


static void keypress(unsigned int key)
{
	switch(KeyValue)
		{
		case KeyValueMenu:

			License_SetFlag=1;
            License_SetCounter_0=0;
			License_SetCounter_1=0;
			License_SetCounter_2=0;
			License_SetCounter_3=0;
			License_SetComp=0;//主域名设置完成1

			License_Type_flag=0;//区分组的选择和组内选择
            License_Type_Counter=0;//  0: 数字    1:A-M         2:N-Z

			License_set_noeffect=0;
			memset(Menu_Car_license,0,sizeof(Menu_Car_license));
            pMenuItem=&Menu_0_loggingin;
			pMenuItem->show();
			break;
		case KeyValueOk:
			if((License_Type_flag==0)&&(License_SetComp==0))
				{
				if(License_set_noeffect==2)
					{
					License_SetFlag=1;
		            License_SetCounter_0=0;
					License_SetCounter_1=0;
					License_SetCounter_2=0;
					License_SetCounter_3=0;
					License_SetComp=0;//主域名设置完成1

					License_Type_flag=0;//区分组的选择和组内选择
	                License_Type_Counter=0;//  0: 数字    1:A-M         2:N-Z
	                License_set_noeffect=0;

			        
 
                    //==================================================== 
                    //   没设置车牌号时为   1
                    License_Not_SetEnable=1;


					//===================================================
					//写入车牌号是否设置标志
		            DF_WriteFlashSector(DF_License_effect,0,&License_Not_SetEnable,1); 
				    //设置下一项
					CarSet_0_counter=2;//设置第2项
					//rt_kprintf("\r\n设置下一项");
					pMenuItem=&Menu_0_loggingin;
					pMenuItem->show();
					}
				else if(License_set_noeffect==1)
					{
					License_set_noeffect=0;
					
					License_Not_SetEnable=0;
					//写入车牌号是否设置标志
		            DF_WriteFlashSector(DF_License_effect,0,&License_Not_SetEnable,1); 
					
					CounterBack=0;
					License_Type_Counter=0;
					License_Type_Sel(License_Type_Counter);
					License_Type_flag=1;
					//rt_kprintf("\r\n选择要输入的类型");
				
					}
				
				}
			else if((License_Type_flag==1)&&(License_SetComp==0))
				{
				License_Type_flag=2;
				if((License_SetFlag>=1)&&(License_SetFlag<=9))
					{
					if(License_Type_Counter==0)
						License_Set(License_SetCounter_0,1);
					else if(License_Type_Counter==1)
						License_Set(License_SetCounter_1,2);
					else if(License_Type_Counter==2)
						License_Set(License_SetCounter_2,3);
					else if(License_Type_Counter==3)
						License_Set(License_SetCounter_3,4);
					//rt_kprintf("\r\n第%d组",License_Type_Counter);
					}	
				}
			else if((License_Type_flag==2)&&(License_SetComp==0))
				{
				if((License_SetFlag>=1)&&(License_SetFlag<=8))
					{
					License_Type_flag=3;
					if(License_Type_Counter==0)
						{
						Menu_Car_license[License_SetFlag-1]=ABC_License_0_9[License_SetCounter_0][0];
						License_SetFlag++;	
						License_Set(License_SetCounter_0,1);
						//rt_kprintf("\r\n(0_9选择)=%d,%s",License_SetCounter_0,Menu_Car_license);
						}
					else if(License_Type_Counter==1)
						{
						Menu_Car_license[License_SetFlag-1]=ABC_License_A_M[License_SetCounter_1][0];
						License_SetFlag++;	
						License_Set(License_SetCounter_1,2);
					    //rt_kprintf("\r\n(A_M选择)=%d,%s",License_SetCounter_1,Menu_Car_license);
						}
					else if(License_Type_Counter==2)
						{
						Menu_Car_license[License_SetFlag-1]=ABC_License_N_Z[License_SetCounter_2][0];
						License_SetFlag++;
						License_Set(License_SetCounter_2,3);
						//rt_kprintf("\r\n(N_Z选择)=%d,%s",License_SetCounter_2,Menu_Car_license);
						}
					else if(License_Type_Counter==3)
						{
						memcpy(Menu_Car_license,(char *)Car_HZ_code[License_SetCounter_3],2);
						License_SetFlag++;
						License_SetFlag++;
						License_Set(License_SetCounter_3,4);
						//rt_kprintf("\r\n(汉字选择)=%d,%s",License_SetCounter_3,Menu_Car_license);
						}
					if((License_Type_flag==3)&&(License_SetFlag<=8))
						{
						License_Type_flag=1;
						License_SetCounter_0=0;
						License_SetCounter_1=0;
						License_SetCounter_2=0;
						License_SetCounter_3=0;

						License_Type_Sel(License_Type_Counter);
						//rt_kprintf("\r\n重新选组(1_2_3)=%d",License_Type_Counter);
						}
					}	
				}
			else if((License_Type_flag==3)&&(License_SetFlag==9))
				{
				License_SetComp=1;
				
			    License_Type_flag=0;
				License_SetFlag=1;
				lcd_fill(0);
				lcd_text12(18,3,"车牌号设置完成",14,LCD_MODE_SET);
				lcd_text12(6,18,"按确认键设置下一项",18,LCD_MODE_SET);
				lcd_update_all();
				//rt_kprintf("\r\n车牌号:%s",Menu_Car_license);
                //车牌号设置完成		
				}
			else if(License_SetComp==1)
				{
				License_SetFlag=1;
	            License_SetCounter_0=0;
				License_SetCounter_1=0;
				License_SetCounter_2=0;
				License_SetCounter_3=0;
				License_SetComp=0;//主域名设置完成1

				License_Type_flag=0;//区分组的选择和组内选择
                License_Type_Counter=0;//  0: 数字    1:A-M         2:N-Z
                License_set_noeffect=0;

				CarSet_0_counter=2;//设置第2项
				//rt_kprintf("\r\n设置下一项");
				pMenuItem=&Menu_0_loggingin;
				pMenuItem->show();
				}				
			break;
		case KeyValueUP:
			if(License_Type_flag==0)
				{
				License_set_noeffect=1;
				lcd_fill(0);
				lcd_text12(0,3,"设置车牌号",10,LCD_MODE_INVERT);
				lcd_text12(0,18,"无牌照车辆",10,LCD_MODE_SET);
				lcd_update_all();
				}
			else if(License_Type_flag==1)//选择是0-9  A-M  N-Z
				{//
				if(License_Type_Counter==0)
					License_Type_Counter=3;
				else if(License_Type_Counter>=1)
					License_Type_Counter--;
				License_Type_Sel(License_Type_Counter);
				//rt_kprintf("\r\n(  up)License_Type_Counter=%d",License_Type_Counter);

				}
			else if(License_Type_flag==2)//组内选择
				{
				if((License_SetFlag>=1)&&(License_SetFlag<=20)&&(License_SetComp==0))
					{
					if(License_Type_Counter==0)//数字
						{
						if(License_SetCounter_0==0)
							License_SetCounter_0=9;
						else 
							License_SetCounter_0--;

						License_Set(License_SetCounter_0,1);
						}
					else if(License_Type_Counter==1)//A-M
						{
						if(License_SetCounter_1==0)
							License_SetCounter_1=12;
						else
							License_SetCounter_1--;

						License_Set(License_SetCounter_1,2);
						}
					else if(License_Type_Counter==2)//N-Z
						{
						if(License_SetCounter_2==0)
							License_SetCounter_2=12;
						else 
							License_SetCounter_2--;

						License_Set(License_SetCounter_2,3);
						}	
					else if(License_Type_Counter==3)//汉字选择
						{
						if(License_SetCounter_3==0)
							License_SetCounter_3=30;
						else 
							License_SetCounter_3--;

						License_Set(License_SetCounter_3,4);
						}
					}
				}
			break;
		case KeyValueDown:
			if(License_Type_flag==0)
				{
				License_set_noeffect=2;
				lcd_fill(0);
				lcd_text12(0,3,"设置车牌号",10,LCD_MODE_SET);
				lcd_text12(0,18,"无牌照车辆",10,LCD_MODE_INVERT);
				lcd_update_all();
				}
			else if(License_Type_flag==1)//选择是0-9  A-M  N-Z
				{
				License_Type_Counter++;
				if(License_Type_Counter>3)
					License_Type_Counter=0;
				License_Type_Sel(License_Type_Counter);
				//rt_kprintf("\r\n(down)License_Type_Counter=%d",License_Type_Counter);
				}
			else if(License_Type_flag==2)//组内选择
				{
				if((License_SetFlag>=1)&&(License_SetFlag<=20)&&(License_SetComp==0))
					{
					if(License_Type_Counter==0)
						{
						License_SetCounter_0++;
						if(License_SetCounter_0>9)
							License_SetCounter_0=0;

						License_Set(License_SetCounter_0,1);
						}
					else if(License_Type_Counter==1)
						{
						License_SetCounter_1++;
						if(License_SetCounter_1>12)
							License_SetCounter_1=0;

						License_Set(License_SetCounter_1,2);
						}
					else if(License_Type_Counter==2)
						{
						License_SetCounter_2++;
						if(License_SetCounter_2>12)
							License_SetCounter_2=0;

						License_Set(License_SetCounter_2,3);
						}
					else if(License_Type_Counter==3)//汉字选择
						{
						License_SetCounter_3++;
						if(License_SetCounter_3>30)
							License_SetCounter_3=0;

						License_Set(License_SetCounter_3,4);
						}
					}	
				}
			break;
		}
	KeyValue=0;
}


static void timetick(unsigned int systick)
{

}

ALIGN(RT_ALIGN_SIZE)
MENUITEM	Menu_0_1_license=
{
	"车牌号",
	6,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};




