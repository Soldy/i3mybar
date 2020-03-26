#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>


void dateTime(){
    printf("{");
    time_t rawtime = time(NULL);
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
    int now;
    FILE *fpf;
    printf("{");
    fpf = fopen("/sys/class/hwmon/hwmon4/temp2_input", "r");
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &now);
        fclose(fpf);
        float full = ((float) now / (float) 1000);
        if ( (float) 50 > full ){
            printf("\"color\":\"#00ff00\",");
        } else if ( (float) 55 > full ){
            printf("\"color\":\"#ffff00\",");
        } else if ( (float) 60 > full ){
            printf("\"color\":\"#ff9900\",");
        }else{
            printf("\"color\":\"#ff0000\",");
        }
        printf("\"full_text\":\"");
        printf("%.2f", full);
        printf("\"");
    }
    printf("}");
}
void battery(){
    int now;
    int full;
    FILE *fpf;
    FILE *fpn;
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
        float charge = ((float) now / (float) full)*100;
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
        printf("%.2f", charge);
        printf("\"");
    }
    printf("}");
}


int main(){
    int out = 0;
    printf("{\"version\": 1}\n[\n");
    while(out == 0){
        printf("[");
        battery();
        printf(",");
        temp();
        printf(",");
        dateTime();
        printf("],\n");
        fflush(stdout);
        sleep(1);
    }
}

