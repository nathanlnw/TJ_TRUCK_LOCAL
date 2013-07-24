#include  <string.h>
#include "Menu_Include.h"

#define  VIN_width1  6


static u8 VIN_SetFlag=1;
static u8 VIN_SetCounter_0=0,VIN_SetCounter_1=0,VIN_SetCounter_2=0;


unsigned char select_vin[]={0x0C,0x06,0xFF,0x06,0x0C};
unsigned char ABC_0_9[10][1]={"0","1","2","3","4","5","6","7","8","9"};
unsigned char ABC_A_M[13][1]={"A","B","C","D","E","F","G","H","I","J","K","L","M"};
unsigned char ABC_N_Z[13][1]={"N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};

unsigned char VIN_Type_flag=0;//区分组的选择和组内选择
unsigned char VIN_Type_Counter=0;//  0: 数字    1:A-M         2:N-Z

DECL_BMP(8,5,select_vin);

void VIN_Type_Sel( u8 par)
{
lcd_fill(0);
lcd_text12(0,3,(char *)Menu_Vin_Code,VIN_SetFlag-1,LCD_MODE_SET);
lcd_text12(20,18,"0-9  A-M  N-Z",13,LCD_MODE_SET);
if(par==0)
	lcd_text12(20,18,"0-9",3,LCD_MODE_INVERT);
else if(par==1)
	lcd_text12(20+5*6,18,"A-M",3,LCD_MODE_INVERT);
else if(par==2)
	lcd_text12(20+10*6,18,"N-Z",3,LCD_MODE_INVERT);
lcd_update_all();
}
void VIN_Set(u8 par,u8 type1_2)
{
	lcd_fill(0);
	lcd_text12(0,3,(char *)Menu_Vin_Code,VIN_SetFlag-1,LCD_MODE_SET);
	
	if(type1_2==1)
		{
		lcd_bitmap(par*VIN_width1, 14, &BMP_select_vin, LCD_MODE_SET);
		lcd_text12(0,19,"0123456789",10,LCD_MODE_SET);
		}
	else if(type1_2==2)
		{
		lcd_bitmap(par*VIN_width1, 14, &BMP_select_vin, LCD_MODE_SET);
		lcd_text12(0,19,"ABCDEFGHIJKLM",13,LCD_MODE_SET);
		}
	else if(type1_2==3)
		{
		lcd_bitmap(par*VIN_width1, 14, &BMP_select_vin, LCD_MODE_SET);
		lcd_text12(0,19,"NOPQRSTUVWXYZ",13,LCD_MODE_SET);
		}
	lcd_update_all();
}

static void msg( void *p)
{

}
static void show(void)
{
CounterBack=0;

VIN_Type_Counter=0;
VIN_Type_Sel(VIN_Type_Counter);
VIN_Type_flag=1;
//rt_kprintf("\r\n选择要输入的类型");
}


static void keypress(unsigned int key)
{
	switch(KeyValue)
		{
		case KeyValueMenu:
			pMenuItem=&Menu_0_loggingin;
			pMenuItem->show();
			memset(Menu_Vin_Code,0,sizeof(Menu_Vin_Code));
			VIN_SetFlag=1;

			VIN_SetCounter_0=0;
			VIN_SetCounter_1=0;
			VIN_SetCounter_2=0;
			break;
		case KeyValueOk:	
			if(VIN_Type_flag==1)
				{
				VIN_Type_flag=2;
				if((VIN_SetFlag>=1)&&(VIN_SetFlag<=17))
					{
					if(VIN_Type_Counter==0)
						VIN_Set(VIN_SetCounter_0,1);
					else if(VIN_Type_Counter==1)
						VIN_Set(VIN_SetCounter_1,2);
					else if(VIN_Type_Counter==2)
						VIN_Set(VIN_SetCounter_2,3);
					//rt_kprintf("\r\n第%d组",VIN_Type_Counter);
					}	
				}
			else if(VIN_Type_flag==2)
				{
				VIN_Type_flag=3;
				   //---------------------------------------
				   if((VIN_SetFlag>=1)&&(VIN_SetFlag<=17))
					{
					if(VIN_Type_Counter==0)
						{
						//VIN_Set(VIN_SetCounter_0,1);
						Menu_Vin_Code[VIN_SetFlag-1]=ABC_0_9[VIN_SetCounter_0][0];
						VIN_SetFlag++;	
						VIN_Set(VIN_SetCounter_0,1);
						//rt_kprintf("\r\n(0_9选择)=%d",VIN_SetCounter_0);
						}
					else if(VIN_Type_Counter==1)
						{
						//VIN_Set(VIN_SetCounter_1,2);
						Menu_Vin_Code[VIN_SetFlag-1]=ABC_A_M[VIN_SetCounter_1][0];
						VIN_SetFlag++;	
						VIN_Set(VIN_SetCounter_1,2);
						//rt_kprintf("\r\n(A_M选择)=%d",VIN_SetCounter_1);
						}
					else if(VIN_Type_Counter==2)
						{
						//VIN_Set(VIN_SetCounter_2,3);
						Menu_Vin_Code[VIN_SetFlag-1]=ABC_N_Z[VIN_SetCounter_2][0];
						VIN_SetFlag++;	
						VIN_Set(VIN_SetCounter_2,3);
						//rt_kprintf("\r\n(N_Z选择)=%d",VIN_SetCounter_2);   
						}
					}
				   //------------------------------------
					if(VIN_Type_flag==3)
					{
					VIN_Type_flag=1;
					VIN_SetCounter_0=0;
					VIN_SetCounter_1=0;
					VIN_SetCounter_2=0;

					VIN_Type_Sel(VIN_Type_Counter);
					//rt_kprintf("\r\n重新选组(1_2_3)=%d",VIN_Type_Counter);  
					}
				}
		
			if(VIN_SetFlag==18)//设置完成
				{
				VIN_SetFlag=19;
				lcd_fill(0);
				lcd_text12(0,5,(char *)Menu_Vin_Code,17,LCD_MODE_SET);
				lcd_text12(25,19,"VIN设置完成",11,LCD_MODE_SET);
				lcd_update_all();	
				}
			else if(VIN_SetFlag==19)
				{
				VIN_SetFlag=1;
				CarSet_0_counter=5;

				pMenuItem=&Menu_0_loggingin;
				pMenuItem->show();
				}
			break;
		case KeyValueUP:
			if(VIN_Type_flag==1)//选择是0-9  A-M  N-Z
				{//
				if(VIN_Type_Counter==0)
					VIN_Type_Counter=2;
				else if(VIN_Type_Counter>=1)
					VIN_Type_Counter--;
				VIN_Type_Sel(VIN_Type_Counter);
				//rt_kprintf("\r\n(  up)VIN_Type_Counter=%d",VIN_Type_Counter);

				}
			else if(VIN_Type_flag==2)//组内选择
				{
				if((VIN_SetFlag>=1)&&(VIN_SetFlag<=17))
					{
					if(VIN_Type_Counter==0)
						{
						if(VIN_SetCounter_0==0)
							VIN_SetCounter_0=9;
						else 
							VIN_SetCounter_0--;

						VIN_Set(VIN_SetCounter_0,1);
						}
					else if(VIN_Type_Counter==1)
						{
						if(VIN_SetCounter_1==0)
							VIN_SetCounter_1=12;
						else
							VIN_SetCounter_1--;

						VIN_Set(VIN_SetCounter_1,2);
						}
					else if(VIN_Type_Counter==2)
						{
						if(VIN_SetCounter_2==0)
							VIN_SetCounter_2=12;
						else 
							VIN_SetCounter_2--;

						VIN_Set(VIN_SetCounter_2,3);
						}	
					}
				}
			break;
		case KeyValueDown:
			if(VIN_Type_flag==1)//选择是0-9  A-M  N-Z
				{
				VIN_Type_Counter++;
				if(VIN_Type_Counter>2)
					VIN_Type_Counter=0;
				VIN_Type_Sel(VIN_Type_Counter);
				//rt_kprintf("\r\n(down)VIN_Type_Counter=%d",VIN_Type_Counter);
				}
			else if(VIN_Type_flag==2)//组内选择
				{
				if((VIN_SetFlag>=1)&&(VIN_SetFlag<=17))
					{
					if(VIN_Type_Counter==0)
						{
						VIN_SetCounter_0++;
						if(VIN_SetCounter_0>9)
							VIN_SetCounter_0=0;

						VIN_Set(VIN_SetCounter_0,1);
						}
					else if(VIN_Type_Counter==1)
						{
						VIN_SetCounter_1++;
						if(VIN_SetCounter_1>12)
							VIN_SetCounter_1=0;

						VIN_Set(VIN_SetCounter_1,2);
						}
					else if(VIN_Type_Counter==2)
						{
						VIN_SetCounter_2++;
						if(VIN_SetCounter_2>12)
							VIN_SetCounter_2=0;

						VIN_Set(VIN_SetCounter_2,3);
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
MENUITEM	Menu_0_4_vin=
{
"VIN设置",
	7,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};


