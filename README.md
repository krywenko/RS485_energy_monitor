# RS485_energy_monitor  based on the  emontx shield
this is a diverter  and energy monitor based on the emontx shield using wifi to transmit MQTT data to my open wrt router  energy monitor that is setup with collectd-MQTT module . plus it controlls the  a diverting module and screen via a rs485 module


GTI-LIMITER_DIVERTER_SERIAL_A is a basic version that only set up to speak to the  display module/diverter 
GTI-LIMITER_DIVERTER_SERIAL_A_MULTI is a  control that set up  for use with multiple SSR control  or display module

energy7Mon_rs485_slave_a  is the dislay module and single diverter
EnergyMon_RS485_Multi - is the multi ssr controller module it can control up 8 ssr 

SERial_READ_Publish _openwrt  -- is MQTT tranfere agent for the R3 that is loaded on the esp

RS485 and  RS485esp are the required rs485 libiries 

hardware required is found here  and if you need help  you can comment there as well

https://community.openenergymonitor.org/t/energy-diverter-based-on-rs485-wemos-uno-wifi-r3-r2-and-emontx-shield/8577
