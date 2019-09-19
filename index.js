#!/usr/bin/nodejs

"use strict"

const fs = require('fs');
console.log('{"version": 1, "stop_signal": 10, "cont_signal": 12, "click_events": true }');
console.log('[');
    


const batteryClass = function(){
    this.db={
         "charge":0,
         "batteryStatus":"Full"
    }
    this.container={}
    this.charge = function(){
         let color = "#ffffff";
         if (this.container['POWER_SUPPLY_STATUS'] === "Discharging")
             color="#ffff00";
         if ( 20 > this.db.charge )
             color="#ff0000";
        return {
             "full_text":this.db.charge.toString()+"%",
             "color":  color
        };
    }
    let uevent = [];
    let that = this;
    let read=function(){
        uevent = fs.readFileSync(
            '/sys/class/power_supply/BAT0/uevent'
        ).toString().split("\n");
        for (let ev of uevent)
            if (typeof ev.split("=")[1] !== "undefined")
                that.container[ev.split("=")[0]] = ev.split("=")[1];
            that.db['charge'] = parseInt(
                (parseInt(
                    that.container['POWER_SUPPLY_CHARGE_NOW']
                 )*100)/parseInt(
            that.container['POWER_SUPPLY_CHARGE_FULL']));
    }
    setInterval(function(){
        try{
            read();
        }catch(e){

        }
    }, 3000);
}

let battery = new batteryClass();

const timeClass = function(){
     this.los = function(){
         return {
            "full_text": ukFormat.format(Date.now()),
             "color": "#eeeeee"
         }
     }
     this.uk = function(){
         return {
            "full_text": ukFormat.format(Date.now()),
             "color": "#eeeeee"
         }
     }
     function starDate(){
         return
             (Math.floor(((100000/86400)*(Date.now()))/1000)/100000)
                 .toFixed(4)
                 .toString();
     }
     let losFormat = new Intl.DateTimeFormat([], {
         timeZone: 'America/Los_Angeles',
         hour: 'numeric', minute: 'numeric', second: 'numeric',
     });
     let ukFormat = new Intl.DateTimeFormat([], {
        timeZone: 'Europe/London',
        hour: 'numeric', minute: 'numeric', second: 'numeric',
     });
}

var timer =  new timeClass();


function fTime (){
    return (new Intl.DateTimeFormat([], {
        hour: 'numeric', minute: 'numeric', second: 'numeric',
    })).format(new Date((new Date(2019, 6, 13, 16, 0, 0)).getTime() -
    (+new Date)).getTime());
}


function lTime (){
    return (new Intl.DateTimeFormat([], {
        hour: 'numeric', minute: 'numeric', second: 'numeric',
    })).format(new Date());
}

setInterval(()=>{
    let out = []
    try{
        out.push(battery.charge());
        out.push(timer.uk());
         out.push(timer.starDate());
    }catch(e){
         out.push({
            "full_text": e.toString(),
            "color": "#eeeeee"
        });
    }
    console.log(JSON.stringify(out)+',');
},1000);

