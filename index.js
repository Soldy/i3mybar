#!/usr/bin/nodejs

"use strict"

const fs = require('fs');
console.log('{"version": 1, "stop_signal": 10, "cont_signal": 12, "click_events": true }');
console.log('[');
    

function battery(){
    let color = "#ffffff";
    let full = fs.readFileSync(
            '/sys/class/power_supply/BAT0/charge_full'
        );
    let now = fs.readFileSync(
            '/sys/class/power_supply/BAT0/charge_now'
        );
    let stat = fs.readFileSync(
            '/sys/class/power_supply/BAT0/status'
        );
    let  alarm = fs.readFileSync(
            '/sys/class/power_supply/BAT0/status'
        );
    let uevent = fs.readFileSync(
            '/sys/class/power_supply/BAT0/uevent'
        );
    let container={};
    for (let ev of uevent.toString().split("\n"))
        if (typeof ev.split("=")[1] !== "undefined")
            container[ev.split("=")[0]] = ev.split("=")[1];
    if (container['POWER_SUPPLY_STATUS'] === "Discharging")
        color="#ffff00";
    let charg = parseInt(
            (parseInt(now)*100)/parseInt(full));
    if ( 20 > charg )
        color="#ff0000";
    return {
        "full_text":charg.toString()+"%",
        "color":  color
    };
}


function ukTime (){
    return (new Intl.DateTimeFormat([], {
        timeZone: 'Europe/London',
        hour: 'numeric', minute: 'numeric', second: 'numeric',
     })).format(new Date());
}

function losTime (){
     return (new Intl.DateTimeFormat([], {
         timeZone: 'America/Los_Angeles',
         hour: 'numeric', minute: 'numeric', second: 'numeric',
     })).format(new Date());
}

function fTime (){
    return (new Intl.DateTimeFormat([], {
        hour: 'numeric', minute: 'numeric', second: 'numeric',
    })).format(new Date((new Date(2019, 6, 13, 16, 0, 0)).getTime() -
    (+new Date)).getTime());
}

function starDate(){
    return (Math.floor(((100000/86400)*(new
         Date()))/1000)/100000).toFixed(4).toString();
    }

function lTime (){
    return (new Intl.DateTimeFormat([], {
        hour: 'numeric', minute: 'numeric', second: 'numeric',
    })).format(new Date());
}

setInterval(()=>{
    let out = []
    out.push(battery());
    out.push({
        "full_text": ukTime(),
        "color": "#eeeeee"
    });
    out.push({
         "full_text": starDate(),
         "color": "#eeeeee"
    });
    console.log(JSON.stringify(out)+',');
},1000);

