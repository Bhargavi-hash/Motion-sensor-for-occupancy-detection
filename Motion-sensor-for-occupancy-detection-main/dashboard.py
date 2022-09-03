from logging import log
#import tkinter as tk
from requests import status_codes
import streamlit as st
from onem2m import *
# from streamlit_autorefresh import st_autorefresh
import pymongo
from pymongo import MongoClient
import webbrowser
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
#from tkinter import *
import matplotlib.pyplot as plt
from sklearn import datasets
from sklearn import metrics
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
import statistics
import time
from functions import *
from PIL import Image
import random
#from skimage import io
import urllib.request as urllib2
import json
import time
from PIL import Image
import streamlit.components.v1 as components
# imagePIR = Image.open("https://thingspeak.com/channels/1837486/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15")


READ_API_KEY='G32R85RJUZL7CWY4'   
CHANNEL_ID= '1848200'

# def pir():
#     return '<iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1837486/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>
# '
def ThingspeakPIR():
    TS = urllib2.urlopen("http://api.thingspeak.com/channels/%s/feeds/last.json?api_key=%s" \
                       % (CHANNEL_ID,READ_API_KEY))

    response = TS.read()
    data = json.loads(response)
    a = data['created_at']
    b = data['field1']
    
    # d = data['field2']
    st.subheader("PIR value = {}".b)
    time.sleep(5)   

    TS.close()

def ThingspeakTEMP():
    
    # TS1 = urllib2.urlopen("http://api.thingspeak.com/channels/%s/feeds/last.json?api_key=%s" \
    #                    % (CHANNEL_ID,READ_API_KEY))

    # response = TS1.read()
    # print(response)
    # data=json.loads(response)
    st.json({

    'created_at':'2022-09-03 10:59:02.434',

    'Temperature': '00.00',

    'timestamp' : '2022-09-03 10:59:02.434'

    })


    # a = data['created_at']
    # b = data['field1']
    
    # d = data['field3']
    # print (data['field1'] + "   " + data['field3'])
    # time.sleep(5)   

    # TS.close()

connection_url='mongodb+srv://bhargavi:esw2022@eswpro.pkqjhmv.mongodb.net/?retryWrites=true&w=majority'
DB_NAME = 'userDetails'

ans = []

a = []

output = []
avg = []
sd = []
nfa = []


vis = []
cnt = 1



# def heatmap_display():
#     sns.set_theme()

#     # Load the example flights dataset and convert to long-form
#     flights_long = sns.load_dataset("data")
#     flights = flights_long.pivot("month", "year", "passengers")

#     # Draw a heatmap with the numeric values in each cell
#     f, ax = plt.subplots(figsize=(9, 6))
#     sns.heatmap(flights, annot=True, fmt="d", linewidths=.5, ax=ax)

def dfs(i, j, arr):
    global vis
    global cnt

    if i < 0 or j < 0 or i >= 8 or j >= 8 or vis[i][j] != 0 or arr[i][j] == 0:
        return

    vis[i][j] = cnt
    dfs(i+1, j, arr)

    dfs(i-1, j, arr)
    dfs(i, j+1, arr)
    dfs(i, j-1, arr)


def getblobs(arr):
    global vis
    global cnt
    cnt = 1
    actcnt = 0
    for i in range(8):
        ta = []
        for j in range(8):
            if arr[i][j] > 0:
                arr[i][j] = 255
                actcnt += 1
            else:
                arr[i][j] = 0
            ta.append(0)
        vis.append(ta)
    nfa.append(actcnt)

    for i in range(8):
        for j in range(8):
            if arr[i][j] == 255 and vis[i][j] == 0:
                dfs(i, j, arr)
                cnt += 1
    y = 0
    for i in range(8):
        for j in range(8):
            if vis[i][j] > y:
                y += 1
    c = []

    for i in range(y):
        c.append(0)

    for i in range(8):
        for j in range(8):
            if vis[i][j] > 0:
                c[vis[i][j]-1] += 1
    c.sort(reverse=True)
    d = [0, 0, 0]
    if 0 < len(c):
        d[0] = c[0]
    if 1 < len(c):
        d[1] = c[1]
    if 2 < len(c):
        d[2] = c[2]

    return np.array(d)

def getfeature(arr):
    print(arr)
    avge = 0
    brr = []
    act = 0
    ans = [[27.25, 27.43, 27.26, 27.46, 27.52, 27.55, 27.98, 28.06], [27.44, 27.39, 27.16, 27.37, 27.51, 27.6, 27.66, 27.57], [27.14, 27.22, 26.88, 27.5, 27.71, 27.76, 27.98, 28.03], [27.3, 27.0, 26.95, 27.57, 27.84, 27.41, 27.73, 27.96], [
        27.53, 27.55, 27.48, 27.56, 27.66, 27.38, 27.37, 27.86], [27.18, 27.7, 27.45, 27.27, 27.2, 27.29, 27.37, 27.8], [27.35, 27.17, 27.0, 27.2, 27.2, 27.24, 27.26, 27.53], [26.77, 27.27, 26.99, 27.26, 27.29, 27.59, 27.82, 27.72]]
    print(ans)
    for i in range(8):
        for j in range(8):

            brr.append(arr[i][j])
            avge += arr[i][j]

            if arr[i][j] > (ans[i][j]):
                act += 1
            arr[i][j] -= (ans[i][j])
    print(arr)
    blob = getblobs(arr)
    sde = statistics.pstdev(brr)
    temp = []
    temp = [avge/64, sde, act, blob[0], blob[1], blob[2]]
    return np.array(temp)

def algo(k):
    X = np.load("./fv.npy")
    Y = np.load("./output.npy")

    target = [0, 1, 2]
    feature_names = ["average", "standard_deviation",
                     "active_pixels", "blob1", "blob2", "blob3"]

    X_train, X_test, y_train, y_test = train_test_split(X, Y, test_size=0.40)
    clf = RandomForestClassifier(n_estimators=50)
    clf.fit(X_train, y_train)
    print(X_test.shape, X_train.shape, y_train.shape, y_test.shape)
    y_pred = clf.predict([k])
    return y_pred[0]

def dashboard():
    st.success("Logged in as {}". format(st.session_state.username))
    st.session_state.task = st.sidebar.selectbox(
        "Sensors", ['Heat Map', 'Temperature', 'Data'])
    if st.session_state.task == "Heat Map":
        st.header("Heat Map")
        # grideye, time_stamp, pir = temp()
        # people = algo(getfeature(convert_to_table(grideye)))
        # st.subheader("Time:" + str(convert_time(time_stamp)))
        # new_title = '<p style="color:#ff0000; font-size: 42px;">{${people}}</p>'
        # st.header("Number of people in the room : " + str(people))
        # hm = sns.heatmap(data=convert_to_table(grideye))
        # st.pyplot()
        # components.iframe("https://thingspeak.com/channels/1848200/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&xaxis=Timestamp")
        st.markdown(
            """
            Here goes the heat map - Grid eye sensor
            - Uses 8*8 grid
            """
        )
        # ThingspeakTEMP()
        #heatmap_display()
    elif st.session_state.task == "Temperature":
        st.header("Temperature")
        # grideye, time_stamp, pir = temp()
        # st.subheader(convert_time(time_stamp))
        # data = convert_to_table(grideye)
        # st.dataframe(data)
        st.markdown(
            """
            Temperature recorded using Grid eye
            """
        )
        components.iframe("https://thingspeak.com/channels/1848200/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&xaxis=Timestamp")
        
    elif st.session_state.task == "Data":
        # on = Image.open("./on.jpg")
        # off = Image.open("./off.jpg")
        # grideye, time_stamp, pir = temp()
        # st.subheader(convert_time(time_stamp))
        st.subheader("Time stamp here")
        #people = algo(getfeature(convert_to_table(grideye)))
        st.subheader("Some statistics")
        st.header("PIR Data")
        components.iframe("https://thingspeak.com/channels/1848200/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&xaxis=Timestamp")
        # ThingspeakPIR()
        # people = 0
        # if people == 0:
        #     st.image(off)
        # else:
        #     ran = random.randint(1, 10)
        #     if ran <= 8:
        #         st.image(on)
        #     else:
        #         st.image(off)



def login(username, password):
    value = 0
    client = MongoClient(connection_url)
    db = client[DB_NAME]
    users = db["users"]
    list = users.find()
    for ele in list:
        if ele["username"] == username and ele["password"] == password:
            value = 1
    return value

def sign_up(new_user, new_password):
    client = MongoClient(connection_url)
    db = client[DB_NAME]
    users = db["users"]
    list = users.find()
    for ele in list:
        if ele["username"] == new_user:
            return 0
    new = {
        "username": new_user,
        "password": new_password
    }
    users.insert_one(new)
    labels = ["username", "password"]
    # create_ae(
    #     "http://127.0.0.1:8080/~/in-cse/in-name", 'eswPIR', labels)
    return 1



def main():
    st.header("Welcome to Dashboard - ESW - Team 28")
#     st.markdown(
# """
# Motion sensor for occupancy detection
#  - Bhargavi         
#  - Poorvaja
#  - Ruchitha         
#  - Nandini
# """
#     )
    st.sidebar.title("Welcome to the Dashboard app of Team-28.")
    st.sidebar.subheader("Please select an option from Below")

    if "username" not in st.session_state:
        menu = ["Login", "Signup"]
        choice = st.sidebar.selectbox("Menu", menu)

        if choice == "Login":
            st.sidebar.subheader("Login section")
            username = st.sidebar.text_input("Username")
            password = st.sidebar.text_input("Password", type='password')
            if st.sidebar.button("Login"):
                # loginvalue = login(username, password)
                # if loginvalue == 1:
                    st.session_state.username = username
                    st.header("Fields to be displayed here")
                    st.markdown(
                """

                 - Temperature
                 - Heat map
                 - Occupancy statistics
                 - etc..
                """
                    )
                    st.experimental_rerun()
                # elif loginvalue == 0:
                #     st.warning("Invalid credentials")

        elif choice == "Signup":
            st.sidebar.subheader("Register here")
            new_user = st.sidebar.text_input("Username")
            new_password = st.sidebar.text_input("Password", type='password')
            confirm_password = st.sidebar.text_input("Confirm Password", type='password')

            if st.sidebar.button("Signup"):
                if(new_password != confirm_password):
                    st.warning("Password and confirm password doesn't match")
                else:
                    # final = sign_up(new_user, new_password)
                    # if final == 1:
                        with st.spinner(text='In progress'):
                            time.sleep(3)
                            st.success("Account created successfully")
                            st.balloons()
                            st.title('Welcome ' + new_user)
                            st.info("Go to Login Menu to Login")

                    # elif final == 0:
                    #     st.warning("User already exists")
    else:
        logout = st.button("Logout")
        if logout:
            del st.session_state.username
            st.experimental_rerun()
        else:
            dashboard()


if __name__ == "__main__":
    import os

    DEBUG = os.environ.get("DEBUG", "false").lower() not in ["false", "no", "0"]
    main()
    
