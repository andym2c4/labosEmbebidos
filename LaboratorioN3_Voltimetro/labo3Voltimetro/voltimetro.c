#include <voltimetro.h>
/*
------------------------------------VOLTIMETRO --------------------------------
Realizado por: Andy Montes Chu?occa
Correo:        andy.montes.c@uni.pe
Circuito que lee 8 entradas de voltaje, y mediante un keypad el usuario puede solicitar el voltaje y la conversion bits de cada entrada
Se utiliza un cristal de 20MHz, el muestreo se realiza utilizando el TMR0 a un tiempo de muestreo
de 10ms, con un convertidor A/D de 10 bits.

El funcionamiento es el siguiente:
   - El lcd muestra un mensaje de bienvenida y el menu principal
   - Si presionan la tecla A: se visualiza cada voltaje de entrada de forma automatica, cada entrada se mostrara por 2 segundos
   - Si presionan la tecla B: muestra los 8 resultados de voltaje
   - Si presionan la tecla C: el usuario podra solicitar los datos de voltaje (0-5) y valor de conversion (0-1023) 
      de un pin en especifico

Conexiones:
El LCD se esta conectando a los puertos D
El keypad se esta conectado al puerto B
Las entradas de voltaje al puerto A
*/
#BYTE PORTB=0x06
#BYTE TRISB=0x86
#BYTE PORTD=0x06
#BYTE PORTD=0X86
//incluimos el archivo kdb.c que incluye las funciones para el uso del keypad 4x4
#define use_portb_kbd TRUE
#include <kbd4x4.c>
//definimos los pines para el lcd
#define  lcd_ENABLE_PIN PIN_D3
#define  lcd_RS_PIN PIN_D1
#define  lcd_RW_PIN PIN_D2
#define  lcd_data4 PIN_D4
#define  lcd_data5 PIN_D5
#define  lcd_data6 PIN_D6
#define  lcd_data7 PIN_D7
#define  lcd_TYPE 2
//incluimos el archivo lcd
#include <lcd.c>

/*
---------------------------------Declaracion de Variables----------------------
*/
int empezar=0;             //variable para la visualizacion del menu principal
char botonPresionado;      //Variable que guardara el boton presionado
long lecturaA[8];          // vector para leer valor puertos A con 8 bits
float voltajeA[8];        // vector para almacenar valor canales como voltaje
void getPortA(int i);
//------------------- Interrupcion interna de timer 0----------------------------
#int_TIMER0
void TIMER0_isr(void){
    set_timer0(60);
    for(int i = 0; i < 8; i++){
        getPortA(i);
    }
}
//--------------------- Configuracion para la lectura analogica en puerto A
void AD_init(){
    setup_adc_ports(ALL_ANALOG);//define todo el puerto A como analogico
    setup_adc(adc_clock_internal);//declara la configuracion del reloj, por defaul guardara 20MHz
}
//-------------------- Activacion de Interrupciones -----------------------
void INT_Init(){
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_TIMER0);
}
//----------------- configuracion de puertos y otros modulos
void MCU_Init(){
   
   //----------------Inicializar el LCD------------------------------
   lcd_init();
   //----------------Inicializar el Keypad------------------------------
   kbd_init();
   port_b_pullups(true);
   ///configuracion de interrupcion y el puerto A
   INT_Init();
   AD_init();
   //----------------Configuracion del Timer0 ------------------------
   setup_timer_0 (RTCC_INTERNAL |RTCC_DIV_256);
   set_timer0(60);
}


//############################ FUNCIONES ##################################################
void pantallaBienvenida(){//menu principal, se mostrara de inmediato, al iniciar la alimentacion del circuito
   /*
   Mostraremos el mensaje de bienvenida , solicitaremos que el usuario ingrese una opcion, limpiaremos la pantalla
   y se?alaremos en que consiste las opciones a poder ingresar
   */
   
   lcd_putc("\f    Bienvenido  \nElija su opcion: ");
   delay_ms(700);
   lcd_putc("\fA. Leer Voltaje\nB. Ver valores");
   delay_ms(700);
   lcd_putc("\fC. Info de un\n pin especifico");
   delay_ms(700);
}
void getPortA(int n){//lectura del puerto A y convertirlo en voltaje
    set_adc_channel(n);          //selecciona el canal a trabajar
    delay_us(50);                //retardo = 1/10ms=1000/10=50 
    lecturaA[n] = read_adc();    //se almacena los valores en bits
    voltajeA[n] = lecturaA[n]*5.0/1023.0; //se realiza la conversion a bits
}
void opcionA(){
   for(int iA=0;iA<8;iA++){
      //float voltaje=voltajeA[i];
      for(int jA2=0;jA2<80;jA2++){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"\f    Voltaje %d",iA+1);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"    %.5f V ",voltajeA[iA]);
      delay_ms(30);
      }
   }
}
void opcionB(){
for(int jB=0;jB<80;jB++){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"\fV1=%fV|V2=%fV",voltajeA[0],voltajeA[1]);
   lcd_gotoxy(1,2);
   printf(lcd_putc,"V3=%fV|V4=%fV",voltajeA[2],voltajeA[3]);
   delay_ms(30);}
   for(int jB1=0;jB1<80;jB1++){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"\fV5=%fV|V6=%fV",voltajeA[4],voltajeA[5]);
   lcd_gotoxy(1,2);
   printf(lcd_putc,"V7=%fV|V8=%fV",voltajeA[6],voltajeA[7]);
   delay_ms(30);}
}
void opcionC(){
   int solicitar=1;
   while(solicitar){
      lcd_putc("\fIngrese el No.\ncanal A ");
      char numero;
      numero=kbd_getc();
      delay_ms(20);
      switch(numero){
         case '1':
            for(int iC1=0;iC1<80;iC1++){
               lcd_gotoxy(1,1);
               printf(lcd_putc,"\fV1=%fV",voltajeA[0]);
               lcd_gotoxy(1,2);
               printf(lcd_putc,"bits=%LdV",lecturaA[0]);
               delay_ms(30);
            }
            solicitar=0;
            break;
         
         case '2':
            for(int iC2=0;iC2<80;iC2++){
               lcd_gotoxy(1,1);
               printf(lcd_putc,"\fV2=%fV",voltajeA[1]);
               lcd_gotoxy(1,2);
               printf(lcd_putc,"bits=%LdV",lecturaA[1]);delay_ms(30);
            }
            solicitar=0;
            break;
         case '3':for(int iC3=0;iC3<80;iC3++){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fV3=%fV",voltajeA[2]);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"bits=%LdV",lecturaA[2]);delay_ms(30);}
            solicitar=0;
            break;
         case '4':for(int iC4=0;iC4<80;iC4++){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fV4=%fV",voltajeA[3]);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"bits=%LdV",lecturaA[3]);delay_ms(30);}
            solicitar=0;
            break;
         case '5':for(int iC5=0;iC5<80;iC5++){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fV5=%fV",voltajeA[4]);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"bits=%LdV",lecturaA[4]);delay_ms(30);}
            solicitar=0;
            break;
         case '6':for(int iC6=0;iC6<80;iC6++){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fV6=%fV",voltajeA[5]);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"bits=%LdV",lecturaA[5]);delay_ms(30);}
            solicitar=0;
            break;
         case '7':for(int iC7=0;iC7<80;iC7++){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fV7=%fV",voltajeA[6]);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"bits=%LdV",lecturaA[6]);delay_ms(30);}
            solicitar=0;
            break;
         case '8':for(int iC8=0;iC8<80;iC8++){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fV8=%fV",voltajeA[7]);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"bits=%LdV",lecturaA[7]);delay_ms(30);}
            solicitar=0;
            break;
          default:
            break;
      }
      
   }
}
//###############################################################################################
void main(){
   MCU_Init();
   while(TRUE){
      pantallaBienvenida();
      int empezar=0;
      while(~empezar){
         for(int iV=0;iV<100;iV++){
            lcd_putc("\fIngrese Opcion :");
            delay_ms(10);
            botonPresionado = kbd_getc();
            if(botonPresionado=='A'){
               opcionA();
               break;
            }else if(botonPresionado=='B'){
               opcionB();
               break;
            }else if(botonPresionado=='C'){
               opcionC();
               break;
            }
         }
         break;
      }
   }
}
