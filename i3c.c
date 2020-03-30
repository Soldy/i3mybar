#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/sysinfo.h>

char * tempFile = "/sys/class/hwmon/hwmon4/temp2_input";
//char tempFile = fopen("/sys/class/hwmon/hwmon0/temp1_input";

char * batteryFullFile ="/sys/class/power_supply/BAT0/charge_full";
char *  batteryNowFile  = "/sys/class/power_supply/BAT0/charge_now";
//char batteryFullFile ="/sys/class/power_supply/BAT0/energy_full";
//char batteryNowFile  = "/sys/class/power_supply/BAT0/energy_now";


FILE *fpf;
FILE *fpn;
char color;
int batteryNow;
int batteryFull;
float batteryCharge;
int out = 0;
int tempNow;
float tempCurrent;
float cpuUsage, lastCpuUsage;
float memUsage, lastMemUsage;
time_t rawtime;
char str[100];
const char d[2] = " ";
char* token;
int i = 0,times,lag;
long int sum = 0, idle, lastSum = 0,lastIdle = 0;
long double idleFraction;

struct sysinfo sys_info;


//char moveIcons[4][4]={"◜","◝","◞","◟"};
//char moveIcons[4][4]={"◴","◷","◶","◵"};
char moveIcons[17][4]={"⠁","⠃","⠇","⡇","⡏","⡟","⡿","⣿","⣾","⣼","⣸","⢸","⢰","⢠","⢀"," "};
int moveIconMax=15;
//char moveIcons[4][4]={"▘","▝","▗","▖"};
//int moveIconMax=4;
int moveIconI=0;

void matrixTimer(){
    moveIconI++;
    if(moveIconI==moveIconMax)
        moveIconI=0;
}

void matrix(){
    printf("{");
    printf("\"full_text\":");
    printf("\"%s%s  \",", moveIcons[moveIconI], moveIcons[moveIconI]);
    printf("\"color\":\"#00ff00\",\"separator\": false");
    printf("}");
}

void printPercentage(char * name, float percent){
    printf("{");
    if(10 > percent){
       printf("\"color\":\"#0000aa\",");
    }else if(20 > percent){
       printf("\"color\":\"#0000ff\",");
    }else if(40 > percent){
       printf("\"color\":\"#00ff00\",");
    }else if(60 > percent){
       printf("\"color\":\"#ffff00\",");
    }else if(85 > percent){
       printf("\"color\":\"#ff9900\",");
    }else{
       printf("\"color\":\"#ff0000\",");
    }
    printf("\"name\":\"%s\",", name);
    printf("\"full_text\":");
    if(10 > percent){
       printf("\"▁  %.2f\% ▁\"", percent);
    }else if(20 > percent){
       printf("\"▂ %.2f\% ▂\"", percent);
    }else if(40 > percent){
       printf("\"▃ %.2f\% ▃\"", percent);
    }else if(60 > percent){
       printf("\"▄ %.2f\% ▄\"", percent);
    }else if(80 > percent){
       printf("\"▅ %.2f\% ▅\"", percent);
    }else if(90 > percent){
       printf("\"▆ %.2f\% ▆\"", percent);
    }else if(100 > percent){
       printf("\"▇ %.2f\% ▇\"", percent);
    }else if(percent >= 100.00){
       printf("\"█%.2f\% █\"", percent);
    }else{
       printf("\"▁  %.2f\%▁\"", percent);
    }
    printf("}");
}

void cpuDetect(){
    fpf = fopen("/proc/stat","r");
    fgets(str,100,fpf);
    fclose(fpf);
    token = strtok(str,d);
    sum=0;
    i=0;
    while(token!=NULL){
        token = strtok(NULL,d);
        if((token!=NULL)){
            sum += atoi(token);
            if(i==3)
                idle = atoi(token);
        }
        i++;
    }
    cpuUsage = ((1.0 - (float)((float)(idle-lastIdle)/(float)(sum-lastSum)))*100);
    lastIdle = idle;
    lastSum = sum;
    lastCpuUsage = cpuUsage;
}

void cpuWrite(){
    printPercentage("cpu", cpuUsage);
}

void memDetect(){
    sysinfo(&sys_info);
    memUsage = 100.00-(100.00*((float)sys_info.freeram/(float)sys_info.totalram));
}
void memWrite(){
    printPercentage("memory" ,memUsage);
}

void dateTime(){
    printf("{");
    rawtime = time(NULL);
    if (rawtime == -1) {
        printf("Err");
    }
    struct tm *ptm = localtime(&rawtime);
    if (ptm == NULL) {
        printf("Err");
    }
    printf("\"full_text\":");
    printf("\"%04d-%02d-%02d %02d:%02d:%02d\",", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    printf("\"color\":\"#ffffff\",\"separator\": false");
    printf("}");
}


void tempDetect(){
    fpf = fopen(tempFile, "r");
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &tempNow);
        fclose(fpf);
        tempCurrent = ((float) tempNow / (float) 1000);
    }
}

void tempWrite(){
    printf("{");
        if ( 50 > tempCurrent ){
            printf("\"color\":\"#00ff00\",");
        } else if ( 55 > tempCurrent ){
            printf("\"color\":\"#ffff00\",");
        } else if ( 60 > tempCurrent ){
            printf("\"color\":\"#ff9900\",");
        }else{
            printf("\"color\":\"#ff0000\",");
        }
        printf("\"full_text\":\"");
        printf("  %.2f ℃  ", tempCurrent);
        printf("\"");

    printf("}");
}

void batteryDetect(){
    fpf = fopen(batteryFullFile, "r");
    fpn = fopen(batteryNowFile, "r");
    if((fpf == NULL) && (fpn == NULL)){
        batteryCharge = -1.00;
    }else{
        fscanf(fpf,"%d", &batteryFull);
        fscanf(fpn,"%d", &batteryNow);
        fclose(fpf);
        fclose(fpn);
        batteryCharge = ((float) batteryNow / (float) batteryFull)*100;
    }
}

void batteryWrite(){
    printf("{");
    if ( batteryCharge > (float) 95 ){
        printf("\"color\":\"#00ff00\",");
    } else if ( (float)10 > batteryCharge ){
        printf("\"color\":\"#ff0000\",");
    } else if ( (float)25 > batteryCharge ){
        printf("\"color\":\"#ffff00\",");
    }else{
        printf("\"color\":\"#00ffff\",");
    }
    printf("\"full_text\":\"");
    printf("%.2f\%\"", batteryCharge);
    printf("}");
}
int main(){
    printf("{\"version\": 1}\n[\n");
    while(out == 0){
        matrixTimer();
        batteryDetect();
        cpuDetect();
        tempDetect();
        memDetect();
        printf("[");
        batteryWrite();
        printf(",");
        cpuWrite();
        printf(",");
        memWrite();
        printf(",");
        tempWrite();
        printf(",");
        matrix();
        printf(","); 
        dateTime();
        printf(",");
        matrix();
        printf("],\n");
        fflush(stdout);
        usleep(50000);
    }
}

