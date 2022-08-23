from onem2m import *


def convert_to_table(latest_data):
    data = latest_data.split(",")
    table = []
    for i in range(8):
        row = []
        for j in range(8):
            row.append(float(data[i*8 + j]))
        table.append(row)
    return table


def add_leading_zeros(num):
    temp = str(num)
    return temp.zfill(2)


def convert_time(input):
    month_arr = ["Jan", "Feb", "Mar", "Apr", "May",
                 "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    date = input[:10]
    divide = date.split("-")
    year = int(divide[0])
    month = int(divide[1])
    date_num = int(divide[2])

    time = input[11:19]
    time_divide = time.split(":")
    hour = int(time_divide[0])
    min = int(time_divide[1])
    sec = int(time_divide[2])
    if(int(min) >= 30):
        hour = (hour+6) % 24
    else:
        hour = (hour+5) % 24

    min = (min+30) % 60

    result = add_leading_zeros(date_num) + " " + month_arr[month-1] + " " + str(year) + " " + add_leading_zeros(
        hour) + ":" + add_leading_zeros(min) + ":" + add_leading_zeros(sec) + " IST"

    return result


def temp():
    status_code, response = get_data(
        "http://13.232.244.116:9000/records/oneentry")
    print("Response = ")
    print(response)
    print(response["grideye"])
    return response["grideye"], response["time"], response["pirvalue"]
