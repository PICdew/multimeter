#line 1 "D:/Users/Forrest/Desktop/multimeter_MCV8/multimeter_MCV8/multimeter.c"
unsigned int adc_value_pos;
unsigned int adc_value_neg;
unsigned int adc_value;
unsigned long counter=0;
float cap_value;
unsigned char err_c=0;
unsigned char timer0;
unsigned char str[15];
unsigned char index=0;
unsigned const char menu[4][12]={"Volt Meter ","Amp Meter ","Cap Meter ","Res Meter "};
unsigned const char err[15]="Out of range";
unsigned const char msg1[6]="mode:";


char* codetxt_to_ramtxt(const char* ctxt){
static char txt[20];
char i;
 for(i =0; txt[i] = ctxt[i]; i++);
 return txt;
}

void ADC_Init(){
 ADCON1 = 0x80;
 TRISA = 0xFF;
}

void init(){
 TRISD=0;
 Lcd_Init(&PORTD);
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_CURSOR_OFF);
 ADC_Init();
 TRISB=0;
 PORTB=0;
 PORTB.F1=1;
 TRISC=0xff;
}

void get_volt_value(unsigned char x,unsigned char y){
 adc_value_pos=ADC_Read(x);
 adc_value_neg=ADC_Read(y);
 adc_value=adc_value_pos-adc_value_neg;
}

void volt_meter(){
 unsigned char range=1;
 unsigned char mult=1.0;
 while (PORTC.F1==0){
 if (PORTC.F0==1){
 range=1-range;
 }
 switch (range){
 case 0:PORTB.F1=1;mult=1.0;break;
 case 1:PORTB.F1=0;mult=4.0;break;
 }
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
 Lcd_out_cp(codetxt_to_ramtxt(msg1));
 Lcd_Cmd(LCD_SECOND_ROW);
 get_volt_value(0,1);
 if (adc_value<1017){

 FloatToStr(((mult*adc_value*5.0)/1024.0),str);
 Lcd_out_cp(str);
 Lcd_out_cp(" V");
 Lcd_out_cp(",R");
 Lcd_chr_cp(range+48);
 }
 else{
 Lcd_out_cp(codetxt_to_ramtxt(err));
 }
 delay_ms(200);
 }
}

void amp_meter(){
 while (PORTC.F1==0){
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
 Lcd_out_cp(codetxt_to_ramtxt(msg1));
 Lcd_Cmd(LCD_SECOND_ROW);
 get_volt_value(5,4);
 if (adc_value<1023){
 FloatToStr(((4.0*adc_value*5.0)/1024.0)/5.0,str);
 Lcd_out_cp(str);
 Lcd_out_cp(" A");
 }
 else{
 Lcd_out_cp(codetxt_to_ramtxt(err));
 }
 delay_ms(200);
 }
}

void res_meter(){
 while (PORTC.F1==0){
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
 Lcd_out_cp(codetxt_to_ramtxt(msg1));
 Lcd_Cmd(LCD_SECOND_ROW);
 adc_value=ADC_Read(2);
 FloatToStr(((adc_value/1024.0)*5)*10.0/(5-((adc_value/1024.0)*5)),str);
 Lcd_out_cp(str);
 Lcd_out_cp(" K");
 delay_ms(200);
 }
}

void cap_meter(){
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
 Lcd_out_cp(codetxt_to_ramtxt(msg1));
 Lcd_Cmd(LCD_SECOND_ROW);
 while (PORTC.F1==0){
 if (PORTC.F0==1){
 Lcd_out_cp("Plz wait");
 counter=0;
 OPTION_REG=0b10001000;
 INTCON.TMR0IF=0;
 INTCON.TMR0IE=1;
 INTCON.GIE=1;
 PORTB.F3=1;
 adc_value=ADC_Read(3);
 TMR0=0;
 while (adc_value<0.63*1024){adc_value=ADC_Read(3);}
 timer0=TMR0;
 INTCON.GIE=0;
 INTCON.TMR0IE=0;
 INTCON.TMR0IF=0;
 if (!err_c){
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
 Lcd_out_cp(codetxt_to_ramtxt(msg1));
 Lcd_Cmd(LCD_SECOND_ROW);
 cap_value = (counter*256.0+timer0)*1.0/1000000.0;
 if (cap_value < 1.0) {
 FloatToStr(cap_value*1000.0,str);
 Lcd_Out_cp(str);
 Lcd_Out_cp("nF");
 }
 else {
 FloatToStr(cap_value,str);
 Lcd_Out_cp(str);
 Lcd_Out_cp("uF");
 }
 }
 else {
 Lcd_out_cp(codetxt_to_ramtxt(err));
 err_c=0;}
 }
 delay_ms(200);
 }
}

void up(){
 if (index==0){return ;}
 index-=1;
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
}

void down(){
 if (index==3){return ;}
 index+=1;
 Lcd_Cmd(LCD_CLEAR);
 Lcd_Cmd(LCD_FIRST_ROW);
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
}

void ok(){
switch (index){
 case 0:delay_ms(150);volt_meter();
 break;
 case 1:delay_ms(150);amp_meter();
 break;
 case 2:delay_ms(150);cap_meter();
 break;
 case 3:delay_ms(150);res_meter();
 break;
 default:
 return;
 }
}

void display_menu(){
 Lcd_out_cp(codetxt_to_ramtxt(menu[index]));
 while (1){
 if (PORTC.F0==1){up();}
 else if (PORTC.F1==1){ok();Lcd_Cmd(LCD_CLEAR);Lcd_Cmd(LCD_FIRST_ROW);Lcd_out_cp(codetxt_to_ramtxt(menu[index]));}
 else if (PORTC.F2==1){down();}
 delay_ms(150);
 }
}

void main() {
 init();
 display_menu();
}


void interrupt(){
 counter++;
 if (counter==4294967295){err_c=1;}
 INTCON.TMR0IF=0;
}
