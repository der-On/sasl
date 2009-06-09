
-- power avialability
defineProperty("gauge_power", globalPropertyi("sim/custom/xap/gauge_power_avail"))
defineProperty("battery_on", globalPropertyi("sim/cockpit/electrical/battery_on"))  -- ON/OFF main battery
defineProperty("battery_volts", globalPropertyf("sim/cockpit2/electrical/battery_voltage_actual_volts[0]"))  -- APU N1
defineProperty("left_gen_amp", globalPropertyf("sim/cockpit2/electrical/generator_amps[0]"))  -- APU generator ampers
defineProperty("right_gen_amp", globalPropertyf("sim/cockpit2/electrical/generator_amps[1]"))  -- APU generator ampers
defineProperty("avionics", globalPropertyi("sim/cockpit2/switches/avionics_power_on"))  -- avionics switcher (TP156)


-- minimun battery voltage, wich can power the aircraft
local MIN_BAT_VOLT = 18


-- calculate misc systems parameters
function update()

   -- is gauge power avialble?
   if get(avionics) == 1 then -- all gauges are OFF when avionics is OFF
      if get(battery_on) == 1 and get(battery_volts) > MIN_BAT_VOLT then 
          set(gauge_power, 1)
      elseif get(left_gen_amp) + get(right_gen_amp) > 10 then 
          set(gauge_power, 1)
      else 
          set(gauge_power, 0)
      end
   else 
       set(gauge_power, 0)
   end
end

