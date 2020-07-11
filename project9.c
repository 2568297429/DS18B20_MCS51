#include<reg52.h>
#include<intrins.h>
#include<stdio.h>
//fosc@11.0592MHz
#define uchar unsigned char
#define uint unsigned int
sbit ds=P2^2;           //����ds18b20���ݶ˿�
float tmp_float=0;//�¶ȱ���
uint flag=0;//��־�����������������������
void delay(uint z)//��ʱ����
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
void ds_init()//ds18b20��ʼ��
{
	uint i;
	ds=1;
	_nop_();//������ʱ
	ds=0;
	i=103;
  while(i>0)
		i--;//����ʱ
	ds=1;
	i=4;
  while(i>0)
		i--;//��ʱ
}
bit readbit()       //��ȡ1λ����
{
  uint i=0;
  bit temp_bit;
	ds=1;
	_nop_();
	ds=0;
	i++;//��ʱ
	ds=1;
	i++;
	i++;//��ʱ
	temp_bit=ds;//��ȡλ����
	i=8;
	while(i>0)
		i--;
	return temp_bit;
}
uchar readbyte()//��ȡ1�ֽ�����
{
	uchar temp_byte,i,j;
	temp_byte=0;
	for(i=1;i<=8;i++)
  {
    j=readbit();
    temp_byte=(j<<7)|(temp_byte>>1);   //��λ����Ϊ�ֽ�
  }
  return(temp_byte);
}
void writebyte(uchar temp_data)   //��ds18b20д1�ֽ�
{
  uint i;
  uchar j;
  bit bit_out;
  for(j=1;j<=8;j++)
  {
    bit_out=temp_data&0x01;
    temp_data=temp_data>>1;
    if(bit_out)     //д1
    {
      ds=0;
      i++;
			i++;//��ʱ
      ds=1;
      i=8;
			while(i>0)
				i--;
			ds=1;
    }
    else
    {
      ds=0;       //д0
      i=8;
			while(i>0)
				i--;
      ds=1;
			i++;
			i++;//��ʱ
    }

  }
	ds=1;
}
void tmp_init(void)  //DS18B20��ʼ��ȡ����
{
  ds_init();
  delay(2);
  writebyte(0xcc);  // ����ROM
  writebyte(0x44);  // �����¶�ת��
}

float get_tmp()               //����¶�
{
  uchar a,b;
	float temp_tmp_float;
	uint temp;
  ds_init();
  delay(2);
  writebyte(0xcc);
  writebyte(0xbe);
  a=readbyte();//����8λ
  b=readbyte();//����8λ
  temp=b;
  temp<<=8;             //���������ֽ�
  temp=temp|a;
  temp_tmp_float=temp*0.0625;
  return temp_tmp_float;
}
void com_init()//���ڳ�ʼ��
{
	TMOD=0x20;
	PCON=0x00;
	SCON=0x50;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
}
void send_tmp(char *string_in)//�����ַ���
{
	do
	{
		SBUF=*string_in++;
		while(!TI);
		TI=0;
	}while(*string_in);
}
void main()
{
	uchar tmp_out[4];
	com_init();

	while(1)
	{
		tmp_init();
		tmp_float=get_tmp();
		if(flag<=2)//���������Ƭ��������ʱ�����ķǷ��¶�ֵ
		{
			flag++;
			delay(500);
			continue;
		}
		sprintf(tmp_out,"%f",tmp_float);//�Ѹ����¶�ֵת��Ϊ�ַ�
		send_tmp(tmp_out);//�����ַ��¶�ֵ
		delay(500);
	}
}