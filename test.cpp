#include <unistd.h>
#include <iostream>
#include <time.h>
#include "MySerial.hpp"
using namespace std;
 

int main()
{
    MySerial Serial;
    Serial.begin("/dev/ttyUSB0",9600);
    printf("Serial test begin!\n");
    time_t timep;
    time (&timep);
    Serial.printf("Serial transmisson from %s%d ! \n","Raspberry Pi",3);
    for(int i=0;i<5;i++)
    {
        Serial.print("Hello world!");
        Serial.print(i);
        Serial.print('\n');
        printf("Sent...\n");
        sleep(1);
    }    
    printf("Wait response from uart...\n");
    while(Serial.available()==0){//Wait until uart get some data
        usleep(1000);
    }
    printf("Recv some byte from uart...\n");
    while(Serial.available()!=0)//Now read all the data buffered in mem
    {
        char data=Serial.readOne();
        cout<<data;
    }
    printf("\nFinished!\n");
    return 0;
}