import serial
import csv
from datetime import datetime

# Change COM3 to your actual Arduino port
ser = serial.Serial('COM3', 9600, timeout=1)

filename = r"C:\Users\Admin\Desktop\WOD_telemetry_"+str(datetime.now().strftime('%Y~%m~%d_%H;%M;%S'))+".csv"
with open(filename, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=';')
    # Write header
    writer.writerow(['sat_time', 'vbat', 'volt_3V3', 'volt_5V',
                     'current_3V3', 'current_5V', 'charging_power',
                     'consumed_power', 'free_memory', 'resets'])

    print(f"Saving to {filename} ... Press Ctrl+C to stop")

    while True:
        line = ser.readline().decode('utf-8').strip()
        if line:
            print(line)  # still show on screen
            # parse only WOD lines
            if 'Vbat:' in line:
                vbat = line.split(':')[1].strip().replace(' mV','')
            if 'Volt_3V3:' in line:
                v3v3 = line.split(':')[1].strip().replace(' mV','')
            if 'Volt_5V:' in line:
                v5v = line.split(':')[1].strip().replace(' mV','')
            if 'Current_3V3:' in line:
                i3v3 = line.split(':')[1].strip().replace(' mA','')
            if 'Current_5V:' in line:
                i5v = line.split(':')[1].strip().replace(' mA','')
            if 'Charging Power:' in line:
                cp = line.split(':')[1].strip().replace(' mW','')
            if 'Consumed Power:' in line:
                pp = line.split(':')[1].strip().replace(' mW','')
            if 'Free Memory:' in line:
                fm = line.split(':')[1].strip().replace(' bytes','')
            if 'Resets:' in line:
                rs = line.split(':')[1].strip()
            if 'Sat Time:' in line:
                st = line.split(':')[1].strip().replace(' sec','')
                # Write one complete row when last field arrives
                writer.writerow([st, vbat, v3v3, v5v,
                                  i3v3, i5v, cp, pp, fm, rs])
                csvfile.flush()  # save immediately



'''
 How to use it

1. Install Python on Windows if not already installed
2. Install the `pyserial` library:

pip install pyserial

3. Check your Arduino COM port in Device Manager (e.g. `COM3`)
4. Run the script while Arduino is connected
5. The file `WOD_telemetry_YYYYMMDD_HHMMSS.csv` will be created on your laptop




## What the CSV file will look like

sat_time,vbat,volt_3V3,volt_5V,current_3V3,current_5V,charging_power,consumed_power,free_memory,resets
12,3842,3301,4987,143,211,512,287,347821,3
15,3756,3298,4991,151,198,487,301,347650,3
18,3901,3305,4978,138,224,521,279,347432,3

'''
