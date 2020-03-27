#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/sysinfo.h>

FILE *fpf;
FILE *fpn;
char color;
int now;
int full;
int out = 0;
float current;
float charge;
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

void cpu(){
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
    printf("{");
    if(10 > cpuUsage){
       printf("\"color\":\"#0000aa\",");
    }else if(20 > cpuUsage){
       printf("\"color\":\"#0000ff\",");
    }else if(40 > cpuUsage){
       printf("\"color\":\"#00ff00\",");
    }else if(50 > cpuUsage){
       printf("\"color\":\"#ffff00\",");
    }else if(70 > cpuUsage){
       printf("\"color\":\"#ff9900\",");
    }else{
       printf("\"color\":\"#ff0000\",");
    }
    printf("\"full_text\":");
    if(10 > cpuUsage){
       printf("\" %.2f\%\"", cpuUsage);
    }else{
       printf("\"%.2f\%\"", cpuUsage);
    }
    printf("}");
}

void mem(){
    sysinfo(&sys_info);
    memUsage = 100.00-(100.00*(float)((float)sys_info.freeram/(float)sys_info.totalram));
    printf("{");
    if(10 > memUsage){
       printf("\"color\":\"#0000aa\",");
    }else if(20 > memUsage){
       printf("\"color\":\"#0000ff\",");
    }else if(40 > memUsage){
       printf("\"color\":\"#00ff00\",");
    }else if(50 > memUsage){
       printf("\"color\":\"#ffff00\",");
    }else if(70 > memUsage){
       printf("\"color\":\"#ff9900\",");
    }else{
       printf("\"color\":\"#ff0000\",");
    }
    printf("\"full_text\":");
    if(10 > memUsage){
       printf("\" %.2f\%\"", memUsage);
    }else{
       printf("\"%.2f\%\"", memUsage);
    }
    printf("}");

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
    printf("\"%02d:%02d:%02d\",", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    printf("\"color\":\"#ffffff\"");
    printf("}");
}


void temp(){
    printf("{");
    fpf = fopen("/sys/class/hwmon/hwmon4/temp2_input", "r");
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &now);
        fclose(fpf);
        current = ((float) now / (float) 1000);
        if ( (float) 50 > current ){
            printf("\"color\":\"#00ff00\",");
        } else if ( (float) 55 > current ){
            printf("\"color\":\"#ffff00\",");
        } else if ( (float) 60 > current ){
            printf("\"color\":\"#ff9900\",");
        }else{
            printf("\"color\":\"#ff0000\",");
        }
        printf("\"full_text\":\"");
        printf("%.2f℃", current);
        printf("\"");
    }
    printf("}");
}
void battery(){
    printf("{");
    fpf = fopen("/sys/class/power_supply/BAT0/charge_full", "r");
    fpn = fopen("/sys/class/power_supply/BAT0/charge_now", "r");
    if((fpf == NULL) && (fpn == NULL)){
        printf("\"color\":\"#ff0000\",\"full_text\":\"Err\"");
    }else{
        fscanf(fpf,"%d", &full);
        fscanf(fpn,"%d", &now);
        fclose(fpf);
        fclose(fpn);
        charge = ((float) now / (float) full)*100;
        if ( charge > (float) 95 ){
            printf("\"color\":\"#00ff00\",");
        } else if ( (float)10 > charge ){
            printf("\"color\":\"#ff0000\",");
        } else if ( (float)25 > charge ){
            printf("\"color\":\"#ffff00\",");
        }else{
            printf("\"color\":\"#00ffff\",");
        }
        printf("\"full_text\":\"");
        printf("%.2f\%\"", charge);
    }
    printf("}");
}


int main(){
    printf("{\"version\": 1}\n[\n");
    while(out == 0){
        printf("[");
        battery();
        printf(",");
        cpu();
        printf(",");
        mem();
        printf(",");
        temp();
        printf(",");
        dateTime();
        printf("],\n");
        fflush(stdout);
        sleep(1);
    }
}

