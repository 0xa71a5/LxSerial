#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <fstream> 
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <queue>
#include <stdarg.h>
#include <ctype.h>
using namespace std;


class LxSerial{
public://public functions
    void begin(char devs[],int speed_num=115200)
    {
        read_buffer_size = 512;
        FALSE = 0;
        TRUE = 1;
        speed_arr = new int(8);
        name_arr  = new int(8);
        int local_speed_arr[] = { B115200 ,B38400, B19200, B9600, B4800, B2400, B1200, B300 };
        int local_name_arr[]  = { 115200  ,38400 ,  19200,  9600,  4800,  2400,  1200,  300 }; 
        memcpy(speed_arr,local_speed_arr,sizeof(local_speed_arr));
        memcpy(name_arr,local_name_arr,sizeof(local_name_arr)); 

        fd = OpenDev(devs);
        set_speed(fd,speed_num);
        if (set_Parity(fd,8,1,'N') == FALSE)  {
            printf("Set Parity Error\n");
            exit (0);
        }
    }

    void print(string data)
    {
        
        const char *buffer=data.c_str();
        int    Length=data.length();
        int    nByte;
        nByte = write(fd, buffer ,Length);
    }

    void print(int data)
    {
        char str_buffer[15];
        sprintf(str_buffer,"%d",data);
        print(string(str_buffer));
    }

    void print(float data)
    {
        char str_buffer[20];
        sprintf(str_buffer,"%f",data);
        print(string(str_buffer));
    }

    void print(char data)
    {
        char str_buffer[2]={0};
        str_buffer[0] = data;
        print(string(str_buffer));
    }

    void printf(char *fmt,...)
    {
        va_list ap;
        char str_buffer[256];
        va_start(ap,fmt);
        vsprintf(str_buffer,fmt,ap);
        print(string(str_buffer));
        va_end(ap);
    }

    int available() // Return how many byte received and bufferd
    {
        int read_len=0;
        read_len = read(fd, read_buffer , read_buffer_size );
        if(read_len>0)
        {
            for(int i = 0;i<read_len;i++)
                rx_fifo.push(read_buffer[i]);
        }
        return rx_fifo.size();
    }

    char readOne() // Read one byte
    {
        if(available())
        {
            if(rx_fifo.size()!=0)
            {
                char oneByte = rx_fifo.front();
                rx_fifo.pop();
                return oneByte;
            }
        }
        return 0x00;
    }

private://private functions
    int set_Parity(int fd,int databits,int stopbits,int parity)
    { 
        struct termios options; 
        if  ( tcgetattr( fd,&options)  !=  0) { 
            perror("SetupSerial 1");     
            return(FALSE);  
        }
        options.c_cflag &= ~CSIZE; 
        switch (databits)  
        {   
        case 7:     
            options.c_cflag |= CS7; 
            break;
        case 8:     
            options.c_cflag |= CS8;
            break;   
        default:    
            fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
        }
        switch (parity) 
        {   
            case 'n':
            case 'N':    
                options.c_cflag &= ~PARENB;   /* Clear parity enable */
                options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
                break;  
            case 'o':   
            case 'O':     
                options.c_cflag |= (PARODD | PARENB);  
                options.c_iflag |= INPCK;             /* Disnable parity checking */ 
                break;  
            case 'e':  
            case 'E':   
                options.c_cflag |= PARENB;     /* Enable parity */    
                options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/     
                options.c_iflag |= INPCK;       /* Disnable parity checking */
                break;
            case 'S': 
            case 's':  /*as no parity*/   
                options.c_cflag &= ~PARENB;
                options.c_cflag &= ~CSTOPB;break;  
            default:   
                fprintf(stderr,"Unsupported parity\n");    
                return (FALSE);  
            }  
        switch (stopbits)
        {   
            case 1:    
                options.c_cflag &= ~CSTOPB;  
                break;  
            case 2:    
                options.c_cflag |= CSTOPB;  
               break;
            default:    
                 fprintf(stderr,"Unsupported stop bits\n");  
                 return (FALSE); 
        } 
        /* Set input parity option */ 
        if (parity != 'n')   
            options.c_iflag |= INPCK; 
        tcflush(fd,TCIFLUSH);
        options.c_cc[VTIME] = 150;  
        options.c_cc[VMIN] = 0; /* Update the options and do it NOW */

        options.c_lflag &= ~ (ICANON | ECHO | ECHOE | ISIG);//debug here
        if (tcsetattr(fd,TCSANOW,&options) != 0)   
        { 
            perror("SetupSerial 3");   
            return (FALSE);  
        } 
        return (TRUE);  
    }


    void set_speed(int fd, int speed){
      int   i; 
      int   status; 
      struct termios   Opt;
      tcgetattr(fd, &Opt); 
      for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
        if  (speed == name_arr[i]) {     
          tcflush(fd, TCIOFLUSH);     
          cfsetispeed(&Opt, speed_arr[i]);  
          cfsetospeed(&Opt, speed_arr[i]);   
          status = tcsetattr(fd, TCSANOW, &Opt);  
          if  (status != 0) {        
            perror("tcsetattr fd1");  
            return;     
          }    
          tcflush(fd,TCIOFLUSH);   
        }  
      }
    }
    int OpenDev(char *Dev)
    {
        int fd = open( Dev, O_RDWR | O_NOCTTY | O_NDELAY   );  //O_RDWR       //| O_NOCTTY | O_NDELAY    
        if (-1 == fd)   
        {           
            perror("Can't Open Serial Port");
            return -1;      
        }   
        else    
            return fd;
    }
private://private variables
    queue<char> rx_fifo;
    char read_buffer[512];
    int read_buffer_size ;
    int *speed_arr ;
    int *name_arr ; 
    int fd;
    int nread;
    char buff[512];
    //char *dev  = "/dev/ttyUSB0";  
    int FALSE;
    int TRUE;
};