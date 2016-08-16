#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"


void CPU_init ( void );
void Ports_init(void);

void main(void)
{
    CPU_init();
    Ports_init();

    while(1)
    {
        if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_10))
        {
            PORT_SetBits(MDR_PORTD,PORT_Pin_10);
        }
        else
        {
            PORT_ResetBits(MDR_PORTD,PORT_Pin_10);
        }
    }
}

void CPU_init ( void ){
     //����������� ����� ��� ������ Flash-������ ��������
     MDR_EEPROM->CMD |= (0 << 3);

     MDR_RST_CLK->HS_CONTROL = 0x01;// ���. HSE ����������
     while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00);// ���� ���� HSE ������ � ������� �����

     MDR_RST_CLK->PLL_CONTROL = ((1 << 2) | (0 << 8));//���. PLL  | ����. ��������� = 1
     while ((MDR_RST_CLK->CLOCK_STATUS & 0x02) != 0x02);// ���� ����� PLL ������ � ���. �����

     MDR_RST_CLK->CPU_CLOCK = ((2 << 0)//�������� ��� CPU_C1
                             | (1 << 2)//�������� ��� CPU_C2
                             | (0 << 4)//������������ ��� CPU_C3
                             | (1 << 8));//�������� ��� HCLK

     MDR_BKP->REG_0E |= (0 << 0); //����� ����������� ���������� ���������� DUcc
     MDR_BKP->REG_0E |= (0 << 3); //����� ���.��������������� ��������

     MDR_RST_CLK->PER_CLOCK = 0xFFFFFFFF;
}//void CPU_init

PORT_InitTypeDef PortInit;
/*
LCD MT-128641 v1 ����
1      3.3V
2      GND
3      V0 ������� �� ������    ---> 3.3KOhm  ---> 18������� UEE
4-11   D0...D7
12     LCD_E1   ����� ������� ���������
13     LCD_E2   ����� ������� ���������
14     LCD_RES  ������ ��������� ���������
15     R/W      ������/������
16     LCD_A0   ����� �������/������
17     E        clock �������������� ������
18     UEE      ����� DC-DC ���������������
19     A        + ������� ���������
20     K        - ������� ���������


������������ ����� ����������������
PA0 - PA7      D0 - D7        out
PE4            LCD_E1         out
PE5            LCD_E2         out

PC7            E (CLOCK)      out

PE11           LCD_A0         out


PC10 - PC14    BUTTON1 - BUTTON5
PD10 - PD14    LED1 - LED5


*/
void Ports_init(void)
{
    // ����� ������ ����������� : ����������� �������� ������ �� ������� ����� 1 � �����  0 - ����
    // PORT_OE_OUT, PORT_OE_IN
    PortInit.PORT_OE = PORT_OE_OUT;
    // ����� ������ ������ �����: 00�����, 01-�������� �������,10��������������� �������,11����������������� �������
    //  PORT_FUNC_PORT        = 0x0,
    //  PORT_FUNC_MAIN        = 0x1,
    //  PORT_FUNC_ALTER       = 0x2,
    //  PORT_FUNC_OVERRID     = 0x3
    PortInit.PORT_FUNC = PORT_FUNC_PORT;
    // 0 � ����������, 1 � ��������
    PortInit.PORT_MODE = PORT_MODE_DIGITAL;
    // ���������� �������� �����: 0 � �������� � ������� ���������, 1 � �������� � ������� �������� (���� �������� ~100�)
    PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
    // ���������� �������� ����: 0 � �������� � ������� ���������, 1 � �������� � ������� �������� (���� �������� ~100�)
    PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    // ����� ������ �����: 0 � ������� ����� �������� ���������� 200 ��., 1 � ������� ����� ������� ���������� 400 ��.
    PortInit.PORT_PD_SHM  =  PORT_PD_SHM_OFF;
    // ����� ������ ������: 0 � ����������� �������, 1 � �������� ����
    PortInit.PORT_PD = PORT_PD_DRIVER;
    // ����� ������ ������ �����:00 � ��������������� (���������� ��������), 01 � ��������� �����,
    // 10 � ������� �����, 11 � ����������� ������� �����
    PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
    // ����� ������ �������� �������: 0 � ������ ��������, 1 � ������ �������
    PortInit.PORT_GFEN = PORT_GFEN_OFF;

    PortInit.PORT_Pin = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7;
    PORT_Init(MDR_PORTA, &PortInit);    //D0 ... D7

    PortInit.PORT_Pin = PORT_Pin_7;  //CLOCK
    PORT_Init(MDR_PORTC, &PortInit);

    PortInit.PORT_Pin = PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_11;
    PORT_Init(MDR_PORTE, &PortInit);  //E1 , E2 , A0

    PortInit.PORT_Pin = PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14;
    PORT_Init(MDR_PORTD, &PortInit);  //LED1 - LED5

    PortInit.PORT_OE = PORT_OE_IN;
    PortInit.PORT_Pin = PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14;
    PORT_Init(MDR_PORTC, &PortInit);  //BUTTON1 - BUTTON5

}

