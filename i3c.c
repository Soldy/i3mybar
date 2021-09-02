#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/sysinfo.h>

char * tempFile = "/sys/class/hwmon/hwmon4/temp2_input";

char * batteryFullFile ="/sys/class/power_supply/BAT0/charge_full";
char * batteryNowFile  = "/sys/class/power_supply/BAT0/charge_now";


char * cpuClockFile = "/sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq";

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


int cpuClock = 0;
float cpuGhz = 0.00;


struct sysinfo sys_info;


//char moveIcons[4][4]={"◜","◝","◞","◟"};
//char moveIcons[4][4]={"◴","◷","◶","◵"};
char moveIcons[23][4]={"⠁","⠃","⠇","⡇","⡏","⡟","⡿","⣿","⣿","⣿","⣾","⣼","⣸","⢸","⢰","⢠","⢀"};
int moveIconMax=17;
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
    printf("\"%s%s\",", moveIcons[moveIconI], moveIcons[moveIconI]);
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
       printf("\"█ %.2f\% █\"", percent);
    }else{
       printf("\"▁  %.2f\% \"", percent);

    }
    printf(",\"separator\": false");
    printf("}");
}

void cpuClockDetect(){
    fpf = fopen(cpuClockFile,"r");
    fscanf(fpf,"%d", &cpuClock);
    fclose(fpf);
    cpuGhz = (float)cpuClock / 1000000;
}

void cpuClockWrite(){
    printf("{");
    printf("\"color\":\"#ffffff\",\"name\":\"cpuClock\",");
    printf("\"full_text\":\"%.2f Ghz\"", cpuGhz);
    printf(",\"separator\": false");
    printf("}");
}

void cpuUsageDetect(){
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

void cpuUsageWrite(){
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
    printf("\"color\":\"#ffffff\",\"separator\": false,");
    printf("\"border\": 0,");
    printf("\"border_top\": 0,");
    printf("\"border_right\": 0,");
    printf("\"border_bottom\": 9,");
    printf("\"border_left\": 0");
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
        printf("%.2f℃", tempCurrent);
        printf("\",\"separator\": false");

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

char easterSpecialText[12][2]={"H","A","P","P","Y"," ","E","A","S","T","E","R"};
char easterSpecialColor[6][7]={"#0000ff","#00ffff","#00ff00","#ffff00","#ff0000","#ffffff"};
int easterSpecialColorI[12]={0,4,2,5,1,3,4,5,0,4,1,3};
int easterSpecialWaitI[12]={0,0,0,0,0,0,0,0,0,0,0,0};
int easterSpecialI=0;

void easterSpecialChar(){
    printf("{");
    printf("\"color\":\"%s\",", easterSpecialColor[easterSpecialColorI[easterSpecialI]]);
    printf("\"name\":\"%s\",", easterSpecialText[easterSpecialI]);
    printf("\"full_text\":\"%s\"",  easterSpecialText[easterSpecialI]);
    printf(",\"separator\": false");
    printf("},");
}

void easterSpecialSpace(){
    printf("{");
    printf("\"color\":\"#ffffff\",");
    printf("\"name\":\"space\",");
    printf("\"full_text\":\"                                                                \",");
    printf("\"separator\": false");
    printf("},");
}


void easterSpecial(){
    for(easterSpecialI = 0; easterSpecialI < 12; easterSpecialI++){
        if (easterSpecialColorI[easterSpecialI] == 6){
            easterSpecialColorI[easterSpecialI] = 0;
        }
        easterSpecialChar();
        if (easterSpecialWaitI[easterSpecialI] == 6){
            easterSpecialWaitI[easterSpecialI] = 0;
            easterSpecialColorI[easterSpecialI]++;
        }
        easterSpecialWaitI[easterSpecialI]++;
    }
    easterSpecialSpace();
}


int quoteI = 0;
char quoteText[200][200]={
    "“No woman gets an orgasm from shining the kitchen floor.” -- Betty Friedan",
    "“The main reason Santa is so jolly is because he knows where all the bad girls live.” -- George Carlin",
    "“It's not true that I had nothing on. I had the radio on.” -- Marilyn Monroe",
    "“Dancing is a perpendicular expression of a horizontal desire.” -- George Bernard Shaw",
    "“Rejection is an opportunity for your selection.“ -- Bernard Branson",
    "Only in our dreams are we free. The rest of the time we need wages. -- Terry Pratchett",
    "Imagination, not intelligence, made us human. -- Terry Pratchett",
    "Stories of imagination tend to upset those without one. -- Terry Pratchett",
    "My programming language was solder. -- Terry Pratchett",
    "In ancient times cats were worshiped as gods; they have not forgotten this. -- Terry Pratchett",
    "Life doesn't happen in chapters at least, not regular ones. -- Terry Pratchett",
    "Nerds are the only people who know how to operate the video recorder. -- Terry Pratchett",
    "I keep vaguely wondering what Macs are like, but the ones I've seen spend too much time being friendly. -- Terry Pratchett",
    "Dickens, as you know, never got round to starting his home page. -- Terry Pratchett",
    "You can’t make people happy by law. -- Terry Pratchett",
    "How can you not be an antisemite, being a socialist! -- Adolf Hitler",
    "We must on principle free ourselves from any class standpoint. -- Adolf Hitler",
    "There are no such things as classes: they cannot be. Class means caste and caste means race. -- Adolf Hitler",
    "The hammer will once more become the symbol of the German worker and the sickle the sign of the German peasan -- Adolf Hitler",
    "For them, the mere pledge of i'I believe' is not enough, but rather the oath: I fight! -- Adolf Hitler",
    "The dullness of fact is the mother of fiction. -- Isaac Asimov",
    "Religion is more conservative than any other aspect of human life. -- Isaac Asimov",
    "I recognize the necessity of animal experiments with my mind but not with my heart. -- Isaac Asimov",
    "Hypocrisy is a universal phenomenon. It ends with death, but not before. -- Isaac Asimov",
    "It is surely better to be wronged than to do wrong. -- Isaac Asimov",
    "The military mind remains unparalleled as a vehicle of creative stupidity. -- Isaac Asimov",
    "When you write a short story ... you had better know the ending first. -- Isaac Asimov",
    "Writing is hard work. The fact that I love doing it doesn't make it less hard work. -- Isaac Asimov",
    "I am an atheist, out and out. It took me a long time to say it. -- Isaac Asimov",
    "There are limits beyond which your folly will not carry you. -- Isaac Asimov",
    "The saddest aspect of life right now is that science gathers knowledge faster than society gathers wisdom. -- Isaac Asimov",
    "All mankind, right down to those you most despise, are your neighbors. -- Isaac Asimov",
    "We are meant to know, or we are amoebae. -- Isaac Asimov",
    "If anyone can be considered the greatest writer who ever lived, it is Shakespeare. -- Isaac Asimov",
    "Ideas are cheap. It's only what you do with them that counts. -- Isaac Asimov",
    "Properly read, it is the most potent force for atheism ever conceived. -- Isaac Asimov",
    "A robot may not injure a human being, or, through inaction, allow a human being to come to harm. -- Isaac Asimov",
    "A robot must obey the orders given it by human beings except where such orders would conflict with the First Law. -- Isaac Asimov",
    "A robot must protect its own existence as long as such protection does not conflict with the First or Second Laws. -- Isaac Asimov",
    "A robot may not injure humanity, or, through inaction, allow humanity to come to harm. -- Isaac Asimov",
    "Fifty years, is a long time. -- Isaac Asimov",
    "There is nothing so eternally adhesive as the memory of power. -- Isaac Asimov",
    "Economics is on the side of humanity now. -- Isaac Asimov",
    "It's a poor blaster that doesn't point both ways. -- Isaac Asimov",
    "It pays to be obvious, especially if you have a reputation for subtlety. -- Isaac Asimov",
    "Never let your sense of morals prevent you from doing what is right. -- Isaac Asimov",
    "To succeed, planning alone is insufficient. One must improvise as well. -- Isaac Asimov",
    "Secrecy as deep as this is past possibility without nonexistence as well. -- Isaac Asimov",
    "ThE spell of power never quite releases its hold. -- Isaac Asimov",
    "It is better to go to defeat with free will than to live in a meaningless security as a cog in a machine. -- Isaac Asimov",
    "It is easier to find men who will volunteer to die than to find those who are willing to endure pain with patience. -- Julius Caesar",
    "Veni, vidi, vici. / Came, Saw, Conquered -- Julius Ceasar",
    "Alea iacta est./ The die is cast. -- Julius Ceasar",
    "Gallia est pacata./ Gaul is subdued. -- Julius Ceasar",
    "I assure you I had rather be the first man here than the second man in Rome. -- Julius Ceasar",
    "It is not the well-fed long-haired man I fear, but the pale and the hungry looking. -- Julius Ceasar",
    "Καὶ σύ, τέκνον / And you, son? -- Julius Ceasar",
    "Horum omnium fortissimi sunt Belgae. / Of all these, the Belgae are the bravest-strongest. -- Julius Ceasar",
    "Fere libenter homines id quod volunt credunt. / In most cases men willingly believe what they wish. -- Julius Ceasar",
    "It is easier to find men who will volunteer to die than to find those who are willing to endure pain with patience. -- Julius Ceasar",
    "From the heights of these pyramids, forty centuries look down on us. -- Napoléon Bonaparte",
    "J'aurais dû mourir à Waterloo / I ought to have died at Waterloo -- Napoléon Bonaparte",
    "The word impossible is not French.-- Napoléon Bonaparte",
    "More glorious to merit a sceptre than to possess one. -- Napoléon Bonaparte",
    "Those who are free from common prejudices acquire others. -- Napoléon Bonaparte",
    "Kiss the feet of Popes provided their hands are tied. -- Napoléon Bonaparte",
    "He who fears being conquered is certain of defeat. -- Napoléon Bonaparte",
    "What is a throne? a bit of wood gilded and covered in velvet. I am the state -- Napoléon Bonaparte",
    "The superior man is never in anyone's way. -- Napoléon Bonaparte",
    "There are only two forces that unite men: fear and interest. -- Napoléon Bonaparte",
    "Success is the most convincing talker in the world. -- Napoléon Bonaparte",
    "Imagination governs the world. -- Napoléon Bonaparte",
    "We frustrate many designs against us by pretending not to see them. -- Napoléon Bonaparte",
    "Parties weaken themselves by their fear of capable men. -- Napoléon Bonaparte",
    "The laws of circumstance are abolished by new circumstances. -- Napoléon Bonaparte",
    "Jesus Christ was the greatest republican. -- Napoléon Bonaparte",
    "A general must be a charlatan. -- Napoléon Bonaparte",
    "There is a joy in danger. -- Napoléon Bonaparte",
    "War is becoming an anachronism; -- Napoléon Bonaparte",
    "In war, groping tactics, half-way measures, lose everything. -- Napoléon Bonaparte",
    "Turks can be killed, but they can never be conquered. -- Napoléon Bonaparte",
    "Morality has nothing to do with such a man as I am. -- Napoléon Bonaparte",
    "You must not fear death, my lads; defy him, and you drive him into the enemy's ranks. -- Napoléon Bonaparte",
    "A good sketch is better than a long speech. -- Napoléon Bonaparte",
    "Ability is nothing without opportunity. -- Napoléon Bonaparte",
    "Never interrupt your enemy when he is making a mistake. -- Napoléon Bonaparte",
    "Yes, yes, our friend Dirac has a religion, and its creed runs: ”There is no God, and Dirac is his prophet.“ -- Wolfgang Pauli",
    "It is always the older that emanates the new one. -- Wolfgang Pauli",
    "I cannot believe God is a weak left-hander. -- Wolfgang Pauli",
    "I don't mind your thinking slowly; I mind your publishing faster than you think.. -- Wolfgang Pauli",
    "The best that most of us can hope to achieve in physics is simply to misunderstand at a deeper level. -- Wolfgang Pauli",
    "Computers are good at following instructions, but not at reading your mind. -- Donald Knuth",
    "Never underestimate the bandwidth of a station wagon full of tapes hurtling down the highway. -- Andrew S. Tanenbaum",
    "An adversary capable of implanting the right virus or accessing the right terminal can cause massive damage. -- George Tenet",
    "A computer would deserve to be called intelligent if it could deceive a human into believing that it was human. -- Alan Turing",
    "The danger of computers becoming like humans is not as great as the danger of humans becoming like computers. -- Konrad Zuse",
    "The only legitimate use of a computer is to play games. -- Eugene Jarvis",
    "My son was one of a kind. You are the first of a kind. David? --  Hobby",
    "A year spent in artificial intelligence is enough to make one believe in God. -- Alan Perlis",
    "It may be that our role on this planet is not to worship God but to create him. -- Arthur C. Clarke",
    "The AI does not hate you, nor does it love you, but you are made out of atoms which it can use for something else. -- Eliezer Yudkowsky",
    "The techniques of artificial intelligence are to the mind what bureaucracy is to human social interaction. -- Terry Winograd",
    "The cleaner and nicer the program, the faster it's going to run. And if it doesn't, it'll be easy to make it fast. -- Joshua Bloch",
    "To me programming is more than an important practical art. It is also a gigantic undertaking in the foundations of knowledge. -- Grace Hopper",
    "Computer Science is embarrassed by the computer. -- Alan Perlis",
    "Prolonged contact with the computer turns mathematicians into clerks and vice versa. --  Alan Perlis, ",
    "Structured Programming supports the law of the excluded muddle. -- Alan Perlis",
    "There are two ways to write error-free programs; only the third one works. -- Alan Perlis",
    "When someone says: 'I want a programming language in which I need only say what I wish done', give him a lollipop. -- Alan J. Perlis",
    "Software and cathedrals are much the same – first we build them, then we pray. -- Sam Redwine",
    "Why bother with subroutines when you can type fast? -- Vaughn Rokosz",
    "A Netscape engineer who shan't be named once passed a pointer to JavaScript, stored it as a string and later passed it back to C, killing 30. -- B R",
    "Real Programmers always confuse Christmas and Halloween because Oct31 == Dec25. -- Andrew Rutherford ",
    "Don't get suckered in by the comments … they can be terribly misleading. -- Dave Storer",
    "The three chief virtues of a programmer are: Laziness, Impatience and Hubris. -- Larry Wall",
    "Beware of bugs in the above code; I have only proved it correct, not tried it. -- Donald Knuth",
    "A documented bug is not a bug; it is a feature. -- James P. MacLennan",
    "Given enough eyeballs, all bugs are shallow. -- Eric S. Raymond",
    "The last bug isn't fixed until the last user is dead. -- Sidney Markowitz,",
    "If someone claims to have the perfect programming language, he is either a fool or a salesman or both. -- Bjarne Stroustrup",
    "Some people, when confronted with a problem, think .”I know, I'll use sed.” Now they have two problems. -- Jamie Zawinski",
    "Algorithms are the computational content of proofs. -- Robert Harper",
    "Numerical analysis is very much an experimental science. -- Peter Wynn",
    "When fundraising, it’s AI/When hiring, it’s ML/When implementing, it’s linear regression/When debugging, it’s printf()",
};

char quoteCommand[200]="";
char quoteQuote[200]="";
FILE *quoteFile;


int quoteTurn = 20000000;

void quoteRead(){
   sprintf(quoteCommand, "fortune -s -n 180");
   quoteFile = popen(quoteCommand,"r");
   fgets(quoteQuote, 200 , quoteFile);
   strtok(quoteQuote, "\n");
   pclose(quoteFile);
}

void quoteFunc(){
    if(quoteTurn > 20000){
       quoteI = rand() % 123 ;
//       quoteRead();
       quoteTurn=0;
    }
    quoteTurn++;
    printf("{");
    printf("\"color\":\"#ffffff\",");
    printf("\"name\":\"Quote\",");
    printf("\"full_text\":\" 😊😊  %s  😊😊 \",", quoteText[quoteI]);
//    printf("\"full_text\":\" 😊😊  %s  😊😊 \",", quoteQuote);
    printf("\"separator\": false");
    printf("},");
}

int main(){
    srand(time(NULL));   
    printf("{\"version\": 1}\n[\n");
    while(out == 0){
        matrixTimer();
        batteryDetect();
        cpuUsageDetect();
        cpuClockDetect();
        tempDetect();
        memDetect();
        printf("[");
//        quoteFunc();
//        easterSpecial();
        batteryWrite();
        printf(",");
        cpuClockWrite();
        printf(",");
        cpuUsageWrite();
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
        usleep(500000);
    }
}

