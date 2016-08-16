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
     //Ќеобходима€ пауза дл€ работы Flash-пам€ти программ
     MDR_EEPROM->CMD |= (0 << 3);

     MDR_RST_CLK->HS_CONTROL = 0x01;// вкл. HSE осцил€тора
     while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00);// ждем пока HSE выйдет в рабочий режим

     MDR_RST_CLK->PLL_CONTROL = ((1 << 2) | (0 << 8));//вкл. PLL  | коэф. умножени€ = 1
     while ((MDR_RST_CLK->CLOCK_STATUS & 0x02) != 0x02);// ждем когда PLL выйдет в раб. режим

     MDR_RST_CLK->CPU_CLOCK = ((2 << 0)//источник дл€ CPU_C1
                             | (1 << 2)//источник дл€ CPU_C2
                             | (0 << 4)//предделитель дл€ CPU_C3
                             | (1 << 8));//источник дл€ HCLK

     MDR_BKP->REG_0E |= (0 << 0); //режим встроенного регул€тора напр€жени€ DUcc
     MDR_BKP->REG_0E |= (0 << 3); //выбор доп.стабилизирующей нагрузки

     MDR_RST_CLK->PER_CLOCK = 0xFFFFFFFF;
}//void CPU_init

PORT_InitTypeDef PortInit;
/*
LCD MT-128641 v1 ћЁЋ“
1      3.3V
2      GND
3      V0 питание жк панели    ---> 3.3KOhm  ---> 18контакт UEE
4-11   D0...D7
12     LCD_E1   выбор первого кристалла
13     LCD_E2   выбор второго кристалла
14     LCD_RES  сигнал начальной установки
15     R/W      чтение/запись
16     LCD_A0   выбор команды/данные
17     E        clock строббирование данных
18     UEE      ¬ыход DC-DC преобразовател€
19     A        + ѕитание подсветки
20     K        - ѕитание подсветки


»спользуемые порты микроконтроллера
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
    // –ежим работы контроллера : Ќаправление передачи данных на выводах порта 1 Ц выход  0 - вход
    // PORT_OE_OUT, PORT_OE_IN
    PortInit.PORT_OE = PORT_OE_OUT;
    // –ежим работы вывода порта: 00Цпорт, 01-основна€ функци€,10Цальтернативна€ функци€,11Цпереопределенна€ функци€
    //  PORT_FUNC_PORT        = 0x0,
    //  PORT_FUNC_MAIN        = 0x1,
    //  PORT_FUNC_ALTER       = 0x2,
    //  PORT_FUNC_OVERRID     = 0x3
    PortInit.PORT_FUNC = PORT_FUNC_PORT;
    // 0 Ц аналоговый, 1 Ц цифровой
    PortInit.PORT_MODE = PORT_MODE_DIGITAL;
    // –азрешение подт€жки вверх: 0 Ц подт€жка в питание выключена, 1 Ц подт€жка в питание включена (есть подт€жка ~100 )
    PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
    // –азрешение подт€жки вниз: 0 Ц подт€жка в питание выключена, 1 Ц подт€жка в питание включена (есть подт€жка ~100 )
    PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    // –ежим работы входа: 0 Ц триггер Ўмита выключен гистерезис 200 м¬., 1 Ц триггер Ўмита включен гистерезис 400 м¬.
    PortInit.PORT_PD_SHM  =  PORT_PD_SHM_OFF;
    // –ежим работы выхода: 0 Ц управл€емый драйвер, 1 Ц открытый сток
    PortInit.PORT_PD = PORT_PD_DRIVER;
    // –ежим работы вывода порта:00 Ц зарезервировано (передатчик отключен), 01 Ц медленный фронт,
    // 10 Ц быстрый фронт, 11 Ц максимально быстрый фронт
    PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
    // –ежим работы входного фильтра: 0 Ц фильтр выключен, 1 Ц фильтр включен
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

