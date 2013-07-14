#include "Menu_Include.h"

unsigned char noselect_log[]={0x3C,0x7E,0xC3,0xC3,0xC3,0xC3,0x7E,0x3C};//����
unsigned char select_log[]={0x3C,0x7E,0xFF,0xFF,0xFF,0xFF,0x7E,0x3C};//ʵ��

unsigned char CarSet_0=1;
//unsigned char CarSet_0_Flag=1;

DECL_BMP(8,8,select_log); DECL_BMP(8,8,noselect_log); 

void Selec_123(u8 par)
{
u8 i=0;
if(par==1)
	{
	lcd_bitmap(35, 5, &BMP_select_log, LCD_MODE_SET);
	for(i=0;i<4;i++)
		lcd_bitmap(47+i*12, 5, &BMP_noselect_log ,LCD_MODE_SET);
	}
else if(par==2)
	{
	lcd_bitmap(35, 5, &BMP_noselect_log, LCD_MODE_SET);
	lcd_bitmap(47, 5, &BMP_select_log, LCD_MODE_SET);
	for(i=0;i<2;i++)
    	lcd_bitmap(59+i*12, 5, &BMP_noselect_log, LCD_MODE_SET);
	}
else if(par==3)
	{
	for(i=0;i<2;i++)
		lcd_bitmap(35+i*12, 5, &BMP_noselect_log, LCD_MODE_SET);
	lcd_bitmap(59, 5, &BMP_select_log, LCD_MODE_SET);
	for(i=0;i<2;i++)
		lcd_bitmap(71+i*12, 5, &BMP_noselect_log, LCD_MODE_SET);
	}
else if(par==4)
	{
	for(i=0;i<3;i++)
		lcd_bitmap(35+i*12, 5, &BMP_noselect_log, LCD_MODE_SET);
	lcd_bitmap(71, 5, &BMP_select_log, LCD_MODE_SET);
	lcd_bitmap(83, 5, &BMP_noselect_log, LCD_MODE_SET);
	}
else if(par==5)
	{
	for(i=0;i<4;i++)
		lcd_bitmap(35+i*12, 5, &BMP_noselect_log, LCD_MODE_SET);
	lcd_bitmap(83, 5, &BMP_select_log, LCD_MODE_SET);
	}
}
void CarSet_0_fun(u8 set_type)
{
    lcd_fill(0);
    lcd_text12( 0, 3,"ע��",4,LCD_MODE_SET);
    lcd_text12( 0,17,"����",4,LCD_MODE_SET);
    Selec_123(CarSet_0_counter);
    switch(set_type)
    	{
    	case 1:
    		lcd_text12(35,19,"���ƺ�����",10,LCD_MODE_INVERT);
    		break;
    	case 2:
    		lcd_text12(35,19,"��������ѡ��",12,LCD_MODE_INVERT);		
    		break;
    	case 3:
    		lcd_text12(35,19,"SIM��������",11,LCD_MODE_INVERT);
    		break;
    	case 4:
		lcd_text12(35,19,"VIN����",7,LCD_MODE_INVERT); 
		break;
	case 5:
		lcd_text12(35,19,"������ɫ����",12,LCD_MODE_INVERT); 	
    		break;
    	}
	lcd_update_all();
}
static void msg( void *p)
{

}
static void show(void)
{
 CounterBack=0;
 CarSet_0_fun(CarSet_0_counter);
}


static void keypress(unsigned int key)
{
switch(KeyValue)
	{
	case KeyValueMenu:
		if(menu_color_flag)
			{
			menu_color_flag=0;
			
			pMenuItem=&Menu_1_Idle;//������Ϣ�鿴����
			pMenuItem->show();
			}
		break;
	case KeyValueOk:
				
		if(CarSet_0_counter==1)
          	{
			pMenuItem=&Menu_0_1_license;//���ƺ�����
	        pMenuItem->show();
          	}
		else if(CarSet_0_counter==2)
			{
			pMenuItem=&Menu_0_2_CarType;//type
		    pMenuItem->show();
			}
		else if(CarSet_0_counter==3)
			{
			  pMenuItem=&Menu_0_3_Sim;//sim ����
		      pMenuItem->show();
			}
		else if(CarSet_0_counter==4)
			{			
			   pMenuItem=&Menu_0_4_vin;//vin 
		       pMenuItem->show(); 
			}
		else if(CarSet_0_counter==5)
			{
			pMenuItem=&Menu_0_5_Colour;//��ɫ 
		      pMenuItem->show();
			}
		break;
	case KeyValueUP:
		break;
	case KeyValueDown:  
		break;
	}
KeyValue=0;
}


static void timetick(unsigned int systick)
{
}

ALIGN(RT_ALIGN_SIZE)
MENUITEM	Menu_0_loggingin=
{
   "��������",
	8,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};
