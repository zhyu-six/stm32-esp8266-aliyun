#include "stm32f10x.h"                  // Device header
#include "usart1.h"

double Lotude,Latude;
char dest[23];



void errorLog(int num)
{
	
	while (1)
	{
	  	printf("ERROR%d\r\n",num);
	}
}

/*void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		printf("**************\r\n");
		printf(Save_Data.GPS_Buffer);

		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}


		}
	}
}*/
void ParseGps()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
//		printf("\r\n");
//		printf(GNRMC_Info.GPS_Buffer);

		//截取数据帧前七部分    							 对地航速 | 对地航向  日期
		//$GNRMC,112536.000,A,2322.75023,N,11326.28605,E,  0.00, |  0.00,  100722,,,A*78
		for (i = 0 ; i <= 7 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)//如果没有找到逗号
				{
					return;
					//ERROR
				}
				
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefulBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	
						case 2:
						{
							memcpy(usefulBuffer, subString, subStringNext - subString);//有效标志位
							if(usefulBuffer[0] == 'A')
								Save_Data.isUsefull = true;
							else if(usefulBuffer[0] == 'V')
								Save_Data.isUsefull = false;	
							break;
						}	
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	
						case 7:memcpy(Save_Data.speed, subString, subStringNext - subString);break;
						default:break;
					}
					subString = subStringNext;					
				}
			}
		}
		Save_Data.isParseData = true;	
	}
}


/*void printGpsBuffer()
{
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		
		printf("Save_Data.UTCTime = ");
		printf(Save_Data.UTCTime);
		printf("\r\n");

		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			printf("Save_Data.latitude = ");
			printf(Save_Data.latitude);
			printf("\r\n");


			printf("Save_Data.N_S = ");
			printf(Save_Data.N_S);
			printf("\r\n");

			printf("Save_Data.longitude = ");
			printf(Save_Data.longitude);
			printf("\r\n");

			printf("Save_Data.E_W = ");
			printf(Save_Data.E_W);
			printf("\r\n");
		}
		else
		{
			printf("GPS DATA is not usefull!\r\n");
		}
		
	}
}*/
float Lat;
float Lon;
float Spe=0;
//char dest[23];
void printGpsBuffer()
{
	int iLon = 0;
	int iLat = 0;
	int iSpe = 0;
	//$GNRMC,123211.000,A,2322.74250,N,11326.27041,E,3.21,217.19,100722,,,A*7A
	if (Save_Data.isParseData)
	{
		int i = 0;
		Save_Data.isParseData = false;		
		if(Save_Data.isUsefull)
		{
			float tmp = 0;		int j = 0;	
			Save_Data.isUsefull = false;
			for (i = 0; Save_Data.latitude[i] != '\0'; i++)
			{

				if (Save_Data.latitude[i] == '.')
				{
					continue;
				}
				if (i <= 1)
				{
					Lat = (Save_Data.latitude[0] - 48) * 10 + (Save_Data.latitude[1] - 48);
					//取出个位和十位
				}
				else
				{
					tmp += (Save_Data.latitude[i] - 48);
					tmp *= 10;
				}
			}
			for (j = 0; j <= 5; j++)
			{
				tmp /= 10;
			}
			Lat += tmp / 60;
			Latude=Lat;				//取得纬度
			//23 22.74250
			//23.xxxxx
			//int iLat = 0;			
			iLat = (int)Lat;
			Save_Data.latitude[0] = iLat / 10 + '0';
			Save_Data.latitude[1] = iLat % 10 + '0';
			Save_Data.latitude[2] = '.';
			Lat -= iLat;
			for (j = 3; j < 10; j++)
			{
				Lat *= 10;
				iLat = (int)Lat;
				Save_Data.latitude[j] = iLat + '0';
				Lat -= iLat;
			}							
			tmp = 0;
			//113.27041
			for (i = 0; Save_Data.longitude[i] != '\0'; i++)
			{

				if (Save_Data.longitude[i] == '.')
				{
					continue;
				}
				if (i <= 2)
				{
					Lon = (((Save_Data.longitude[0] - 48) * 10 + (Save_Data.longitude[1] - 48)) * 10) + (Save_Data.longitude[2] - 48);
					//取出个位和十位和百位
				}
				else
				{
					tmp += (Save_Data.longitude[i] - 48);
					tmp *= 10;
				}
			}
			for (j = 0; j <= 5; j++)
			{
				tmp /= 10;
			}
			//int iLon = 0;
			//113.43784
			Lon += tmp / 60;
			Lotude=Lon;					//取得经度
			iLon = (int)Lon;
			Save_Data.longitude[0] = iLon / 100 + '0';
			Save_Data.longitude[1] = (iLon % 100) / 10 + '0';
			Save_Data.longitude[2] = iLon % 10 + '0';
			Save_Data.longitude[3] = '.';
			Lon -= iLon;
			for (j = 4; j < 11; j++)
			{
				Lon *= 10;
				iLon = (int)Lon;
				Save_Data.longitude[j] = iLon + '0';
				Lon -= iLon;
			}
			dest[8] = dest[10] = dest[20] = ',';
			dest[9] = 'N'; dest[21] = 'E'; dest[22] = '\0';
			for(i = 0; i < 22; i++)
			{
				if(i <= 7)
					dest[i] = Save_Data.latitude[i];
				if(i >= 11 && i <= 19)
					dest[i] = Save_Data.longitude[i - 11];
			}
			
			//printf("\r\ndest = ");
			printf(dest);
			//printf("\r\n");
		}
		else
		{
			printf("GPS DATA Is Not Useful!");
		}	
	}
}

