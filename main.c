/***************************************************************************************/
/*******************************PROJECT : GSM-WIFI GATEWAY*****************************/
/************************************BY ZEINEB AMOURI*********************************/
/************************************************************************************/
#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
int d1,d2,d3;
int N;
int n,a,g;
int i=-1;
int j,k,l;
int count;
char fbuffer[1000];
char REP_ESP[1000];
char REP_GSM[2000];
char BUFFER_request[1000];
char DATA1[4];
char DATA2[4];
char DATA3[4];
int v,X,f,x21,com=0,z=0;
int comp;

////////////////////////////////////////////////
///////////////////////////////////////////////

char* substring(char *result, const char *input, int start, int m)
{
    while (m> 0)
    {
        *result = *(input + start);
        result++;
        input++;
        m--;
    }
    *result = '\0';
    return result;
}
//////////SYSTEM_CLOCK////////////////////////////
///////////////////////////////////////////////
void SystemInit_1() /// CLK_TIMER=168MHz
{

  RCC->CFGR |= 0x00009400;        // AHB_presc=1  APB1_presc=4
  RCC->CR |= 0x00010000;          // HSE_ON
  while (!(RCC->CR & 0x00020000));// wait until HSE READY

  RCC->PLLCFGR = 0x07402A04;      // PLL  M=4, N=168, P=2  Q=7    168 Mhz
  RCC->CR |= 0x01000000;          // PLL_ON
  while(!(RCC->CR & 0x02000000)); // wait Pll_ready

  FLASH->ACR = 0x00000605;
  RCC->CFGR |= 0x00000002;        // System Clk is PLL
  while ((RCC->CFGR & 0x0000000F) != 0x0000000A);

}


/************USART2***************/ // for ESP configuration
void Usart2_Init()
{
  // Enable clock for GPIOA
  RCC->AHB1ENR |= 0x00000001;
  // Enable clock for USART2
  RCC->APB1ENR|=0x00020000;
  //enable USART2_TX to PA2 and USART2_RX to PA3
  GPIOA->AFR[0]|=0x00007700;
  // configuring the USART2 ALTERNATE function  to PA2 and PA3
  GPIOA->MODER|=0x000000A0;
  //  BaudRate:115200
  USART2->BRR= 0x016d;
  // USART2 enable + RXNEIE & IDLEIE & RE & TE
  USART2->CR1|=0x0000203C;
  NVIC_EnableIRQ(USART2_IRQn);
  NVIC_SetPriority(USART2_IRQn,2);
}

/*********************************************************************************
/************USART4***************/ // for GSM configuration
void Uart4_Init()
{
  // Enable clock for GPIOA
 RCC->AHB1ENR |= 0x00000001;
  // Enable clock for UART4
  RCC->APB1ENR|=0x00080000;
  //enable UART4_TX to PA0 and USART2_RX to PA1
  GPIOA->AFR[0]|=0x00000088;
  // configuring the UART4 ALTERNATE function  to PA1 and PA0
  GPIOA->MODER|=0x0000000A;
  //  BaudRate:9600
  UART4->BRR= 0x1120;
  // USART2 enable + RXNEIE & IDLEIE & RE & TE
  UART4->CR1|=0x0000203C;
  NVIC_EnableIRQ(UART4_IRQn);
  NVIC_SetPriority(UART4_IRQn,1);
}

/***********************************************************************
/*USART3*/ //TO PRINT ESP & GSM RESPONSES ON THE HYPERTERMINAL
void Usart3_Init()
{
  // Enable clock for GPIOB
  RCC->AHB1ENR |= 0x00000002;
  // Enable clock for USART3
  RCC->APB1ENR|=0x00040000;
  //enable USART3_TX to PB10 and USART3_RX to PB11
  GPIOB->AFR[1]=0x00007700;
  // configuring the USART3 ALTERNATE function  to PB10 and PB11
  GPIOB->MODER|=0x2AA00000;
  //  BaudRate:115200
  USART3->BRR = 0x016d;
  // USART3 enable
  USART3->CR1|=0x0000202C;

}
/**********************************************************************************
/**********************************************************************************
 //ONE CHAR TRANSMISSION
 **********************************************************************************/
void SendChar4(char Tx)
{
	while((UART4->SR&0x80)==0);  // WAIT UNTIL DR WILL BE AVAILABLE
	UART4->DR=Tx;
}

/**********************************************************************************
//STRING TRANSMISSION
 **********************************************************************************/
void SendTxt4(char *Adr)
{
  while(*Adr)
  {
    SendChar4(*Adr);
    Adr++;
  }
}

/**********************************************************************************
 //ONE CHAR TRANSMISSION
 **********************************************************************************/
void SendChar3(char Tx)
{                                  //TXE !=0
	while((USART3->SR&0x80)==0);
	USART3->DR=Tx;
}

/**********************************************************************************
//STRING TRANSMISSION
 **********************************************************************************/
void SendTxt3(char *Adr)
{
  while(*Adr)
  {
    SendChar3(*Adr);
    Adr++;
  }
}
/**********************************************************************************
 //ONE CHAR TRANSMISSION
 **********************************************************************************/
void SendChar2(char Tx)
{
	while((USART2->SR&0x80)==0);
	USART2->DR=Tx;
}
/**********************************************************************************
//STRING TRANSMISSION
 **********************************************************************************/
void SendTxt2(char *Adr)
{
  while(*Adr)
  {
    SendChar2(*Adr);
    Adr++;
  }
}
/////////////////////////////////////////////////////

void Delay(int count)
{
	while(count--);
}
//////////////////////////////////////////////////////////////
/***********-CONFIG_ESP AS AP/SERVER-*************************************/
void config_AP()
{
	SendTxt2("AT\r\n");
	Delay(42000000);
	Delay(42000000);

	SendTxt2("AT+RST\r\n");
	Delay(42000000);
	Delay(42000000);

	SendTxt2("AT+CIFSR\r\n");
	Delay(42000000);
	Delay(42000000);

	SendTxt2("AT+CWMODE=2\r\n");
	Delay(84000000);

	SendTxt2("AT+CIPMUX=1\r\n");
	Delay(84000000);

	SendTxt2("AT+CIPSERVER=1,80\r\n");
	Delay(84000000);
}

//////////////////////////////////////////////////////////////
/***********-CONFIG_GSM-*************************************/
void config_GSM()
{
	SendTxt4("AT\r\n");
	Delay(42000000);
	Delay(42000000);

	SendTxt4("AT+CPIN?\r\n");
	Delay(42000000);
	Delay(42000000);

	SendTxt4("AT+CREG?\r\n");
	Delay(84000000);

	SendTxt4("AT+CGATT?\r\n");
	Delay(84000000);

	SendTxt4("AT+CIPSHUT\r\n");
	Delay(84000000);

	SendTxt4("AT+CIPSTATUS\r\n");
	Delay(84000000);
	Delay(42000000);

	SendTxt4("AT+CIPMUX=0\r\n");
	Delay(84000000);
}


int main(void)
{
 //System clock : HSExPLL=8x21=168MHz
 SystemInit_1();
 Uart4_Init();
 Usart3_Init();
 Usart2_Init();
 SendTxt3("BEGIN CONFIG ESP\r\n");
 config_AP();
 SendTxt3("END CONFIG ESP\r\n");
 Delay(42000000);
 RCC->AHB1ENR|=0x8;
 GPIOD->MODER=0x55<<24;
// SendTxt3("BEGIN CONFIG GSM\r\n");
 //config_GSM();

  while(1);
}

///////////////INTERRUPTIONS///////////////////////
//////////////////////////////////////////////////

void   UART4_IRQHandler()
{

	if((UART4->SR&0x0020)!=0) //TEST FLAG RXNE
    {
	  g++;
	  REP_GSM[g]=UART4->DR;
	}

	if((UART4->SR&0x0010)!=0)//TEST FLAG IDLE
	{

		a=strlen(REP_GSM);
		 for(k=0;k<a;k++)
		 	   {
		 		 SendChar3(REP_GSM[k]);
		 	   }

		 for(k=0;k<a;k++)
		       {
			 REP_GSM[k]=0;
		       }
		  g=-1;
	      UART4->SR;//CLEAR IDLE
          UART4->DR;
 }
}


void   USART2_IRQHandler()
{

	if((USART2->SR&0x0020)!=0) //TEST FLAG RXNE
    {
	  i++;
	  REP_ESP[i]=USART2->DR;
	}

	if((USART2->SR&0x0010)!=0)//TEST FLAG IDLE
	{
		 n=strlen(REP_ESP);
		 if(strstr(REP_ESP,"CONNECT"))
						 {
			               GPIOD->ODR|=0x2000;  // turn on ORANGE led
						 }
		 if(strstr(REP_ESP,"ERROR"))
								 {
								         {

				                           GPIOD->ODR=0x4000;  // turn on RED led
								         }
								 }
		 if(strstr(REP_ESP,"IPD"))
				 {

                           if (z==0)
				         {
                           SendTxt3("begin sending DATA\r\n");
                           GPIOD->ODR|=0x1000;  // turn on YELLOW led
				           SendTxt3("BEGIN CONFIG GSM\r\n");
					       config_GSM();
					       SendTxt3("END CONFIG GSM\r\n");
					       z=1;
				         }
					       ////////
					       ////////
					       for (v=0;v<n;v++)
					         {
					       		if (REP_ESP[v]==*":")
					       			{

					       				X=v+1;
					       				com=1;
					       				v++;

					       	        }

					       			else  if ((REP_ESP[v]==*"&") &(com==1))
					       			{
					       				f=v-X;


					       				substring(DATA1,REP_ESP,X,f);
					       			//	SendTxt4("data1 : \n");
					       				//SendTxt4(DATA1);
					       				com=2;
					       				X=v+1;
					       				v++;

					       			}
					       		else if ((REP_ESP[v]==*"&")&(com==2))
					       			{
					       				f=v-X;

					       				x21=X;
					       				substring(DATA2,REP_ESP,x21,f);
					       				X=v+1 ;
					       			//	SendTxt4("\n data2 :\n");
					       			//	SendTxt4(DATA2);
					       				com=3;
					       				v++;

					       			}

					       		else if(com==3)
					       				{


					       				  f=n-x21;
					       				  substring(DATA3,REP_ESP,X,n-X);
					       				 // SendTxt4("\ndata3 :\n");
					       				//  SendTxt4(DATA3);
					       				  v=n;
					       			      com=0;
					       			      f=0;
					       				 }

					       }

					      d1=atoi(DATA1);
					      d2=atoi(DATA2);
					      d3=atoi(DATA3);
                                   ////////////////
					             /////////////////

					sprintf(BUFFER_request,"GET /update?key=I5DA4OM2PS7CJNWK&field1=%d&field2=%d&field3=%d\r\n",d1,d2,d3);
					SendTxt4("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n");
					Delay(84000000);
					SendTxt4("AT+CIPSEND\r\n");

					Delay(84000000);
				 	SendTxt4(BUFFER_request);
				 	SendTxt2("\r\n");
					Delay(84000000);
					Delay(42000000);
				 	SendTxt4("\x1A\r\n");
					Delay(84000000);
					SendTxt2("\r\n");
					SendTxt4("AT+CIPSHUT\r\n");
					Delay(84000000);
		 }
		 if (!(strstr(REP_ESP,"IPD")))
		 {
		 for(j=0;j<n;j++)
		 	   {
		 		 SendChar3(REP_ESP[j]);
		 	   }
		 }
		 for(j=0;j<n;j++)
		       {
			           REP_ESP[j]=0;
		       }
		  i=-1;
	      USART2->SR;//CLEAR IDLE
          USART2->DR;
    }
}

