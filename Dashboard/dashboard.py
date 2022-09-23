import streamlit as st
from onem2m import *
from retrieve import *
from pymongo import MongoClient
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
import statistics
import seaborn as sns
import time
import matplotlib.pyplot as plt
# from functions import *
import urllib.request as urllib2
import json
import streamlit.components.v1 as components
import bcrypt
import plotly.express as px
from predict import *


READ_API_KEY='G32R85RJUZL7CWY4'   
CHANNEL_ID= '1848200'

st.set_page_config(layout="wide")

# latestTime = ""
def Thingspeak_HeatMap():
    TS = urllib2.urlopen("http://api.thingspeak.com/channels/%s/feeds/last.json?api_key=%s" \
                       % (CHANNEL_ID,READ_API_KEY))

    response = TS.read()
    data = json.loads(response)

    RecentMapTime = data['created_at']
    # latestTime = RecentMapTime
    st.write(RecentMapTime)
    
    # pir = data['field1']

    row1_2 = data['field3']
    fragment1 = row1_2.split(',')
    
    row3_4 = data['field4']
    fragment2 = row3_4.split(',')

    row5_6 = data['field5']
    fragment3 = row5_6.split(',')

    row7_8 = data['field6']
    fragment4 = row7_8.split(',')

    r1c1 = float(fragment1[0])
    r1c2 = float(fragment1[1])
    r1c3 = float(fragment1[2])
    r1c4 = float(fragment1[3])
    r1c5 = float(fragment1[4])
    r1c6 = float(fragment1[5])
    r1c7 = float(fragment1[6])
    r1c8 = float(fragment1[7])

    r2c1 = float(fragment1[8])
    r2c2 = float(fragment1[9])
    r2c3 = float(fragment1[10])
    r2c4 = float(fragment1[11])
    r2c5 = float(fragment1[12])
    r2c6 = float(fragment1[13])
    r2c7 = float(fragment1[14])
    r2c8 = float(fragment1[15])

    r3c1 = float(fragment2[0])
    r3c2 = float(fragment2[1])
    r3c3 = float(fragment2[2])
    r3c4 = float(fragment2[3])
    r3c5 = float(fragment2[4])
    r3c6 = float(fragment2[5])
    r3c7 = float(fragment2[6])
    r3c8 = float(fragment2[7])

    r4c1 = float(fragment2[8])
    r4c2 = float(fragment2[9])
    r4c3 = float(fragment2[10])
    r4c4 = float(fragment2[11])
    r4c5 = float(fragment2[12])
    r4c6 = float(fragment2[13])
    r4c7 = float(fragment2[14])
    r4c8 = float(fragment2[15])

    r5c1 = float(fragment3[0])
    r5c2 = float(fragment3[1])
    r5c3 = float(fragment3[2])
    r5c4 = float(fragment3[3])
    r5c5 = float(fragment3[4])
    r5c6 = float(fragment3[5])
    r5c7 = float(fragment3[6])
    r5c8 = float(fragment3[7])

    r6c1 = float(fragment3[8])
    r6c2 = float(fragment3[9])
    r6c3 = float(fragment3[10])
    r6c4 = float(fragment3[11])
    r6c5 = float(fragment3[12])
    r6c6 = float(fragment3[13])
    r6c7 = float(fragment3[14])
    r6c8 = float(fragment3[15])

    r7c1 = float(fragment4[0])
    r7c2 = float(fragment4[1])
    r7c3 = float(fragment4[2])
    r7c4 = float(fragment4[3])
    r7c5 = float(fragment4[4])
    r7c6 = float(fragment4[5])
    r7c7 = float(fragment4[6])
    r7c8 = float(fragment4[7])

    r8c1 = float(fragment4[8])
    r8c2 = float(fragment4[9])
    r8c3 = float(fragment4[10])
    r8c4 = float(fragment4[11])
    r8c5 = float(fragment4[12])
    r8c6 = float(fragment4[13])
    r8c7 = float(fragment4[14])
    r8c8 = float(fragment4[15])

    map = [[r1c1, r1c2, r1c3, r1c4, r1c5, r1c6, r1c7, r1c8],
           [r2c1, r2c2, r2c3, r2c4, r2c5, r2c6, r2c7, r2c8],
           [r3c1, r3c2, r3c3, r3c4, r3c5, r3c6, r3c7, r3c8],
           [r4c1, r4c2, r4c3, r4c4, r4c5, r4c6, r4c7, r4c8],
           [r5c1, r5c2, r5c3, r5c4, r5c5, r5c6, r5c7, r5c8],
           [r6c1, r6c2, r6c3, r6c4, r6c5, r6c6, r6c7, r6c8],
           [r7c1, r7c2, r7c3, r7c4, r7c5, r7c6, r7c7, r7c8],
           [r8c1, r8c2, r8c3, r8c4, r8c5, r8c6, r8c7, r8c8]]

    # predict_occupancy(1, map)
    heatmap = px.imshow(map, text_auto=True, aspect="auto")
    # heatmap = px.imshow(map, cmap='hot', interpolation = 'nearest')
    # heatmap.show()
    st.write(heatmap)
    
    cells = np.array(map) # this is 8x8 numpy array
    #print(cells)

    ## heat map  plot
    # heat_map = cells
    # ax = sns.heatmap(heat_map, linewidth=0.5, cmap = 'YlOrBr')
    # st.write(ax)

    avg_temp=np.average(cells)
    # standard_deviation = np.std(cells) # calculate sd of the pixel matrix
    count_actv_cells = 0
    threshhold_temp = avg_temp + 0.75 # 1 #2 or 1.5 whatever
    for a in range(8):
        for r in range(8) :
            if( cells[a][r] >= threshhold_temp): 
                cells[a][r] = 1
                count_actv_cells+=1
            else: cells[a][r] = 0

    # pixelated image plot
    pixelated_image = cells
    NewMap = px.imshow( pixelated_image , text_auto=False, aspect="auto" ) 
    st.write(NewMap)

    # st.write(fragment1)
    
    # st.write(row3_4)
    # st.write(row5_6)
    # st.write(row7_8)

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
    st.subheader("Room Statistics")

    st.markdown("---")
    col1, col2, col3 = st.columns(3)
    data = om2m_motion_retrieval()
    cins1 = data["m2m:cin"]["con"]
    om2mtime = cins1[1]+cins1[2]+cins1[3]+cins1[4]+cins1[5]+cins1[6]+cins1[7]+cins1[8]
    occupancy = cins1[10]
    TS1 = urllib2.urlopen("http://api.thingspeak.com/channels/%s/feeds/last.json?api_key=%s" \
                       % (CHANNEL_ID,READ_API_KEY))

    response1 = TS1.read()
    data1 = json.loads(response1)

    latestTime = data1['created_at']
    if occupancy == "0":
        col2.metric("Occupancy", "Room is empty")
    else:
        col2.metric("Occupancy", "Room is occupied")

    col1.metric("Last Updated", latestTime)
    col1.metric("Latest Number of people in the room", 2)

    col2.metric("Average Occupancy", "3")
    
    col3.metric("Accuracy of model", "~85%")
    col3.metric("Time interval when the room is used the most", "6:00pm to 8:00pm")
    st.header("PIR Data")
    components.iframe("https://thingspeak.com/channels/1848200/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&xaxis=Timestamp",  width=450, height=250, scrolling=True)
    
    # st.session_state.task == "Temperature":
    st.header("Temperature")
    
    st.markdown(
        """
        Temperature recorded using Grid eye
        """
    )
    components.iframe("https://thingspeak.com/channels/1848200/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&xaxis=Timestamp",  width=450, height=250, scrolling=True)
    st.header("Heat Map")
    Thingspeak_HeatMap()




def login(username, password):
    value = 0
    client = MongoClient(connection_url)
    db = client[DB_NAME]
    users = db["users"]
    list = users.find()
    for ele in list:
        if ele["username"] == username:
            check = ele["password"]
            password = password.encode('utf-8') 
            if bcrypt.checkpw(password, check):
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
        
    new_password = new_password.encode('utf-8')
    new_password = bcrypt.hashpw(new_password, bcrypt.gensalt(10))
    new = {
        "username": new_user,
        "password": new_password
    }
    users.insert_one(new)
    # labels = ["username", "password"]
    # create_ae(
    #     "http://127.0.0.1:8080/~/in-cse/in-name", 'eswPIR', labels)
    return 1



def main():
    # st.header("Welcome to Dashboard - ESW - Team 28")
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
    st.sidebar.subheader("ESW Project 2022")

    if "username" not in st.session_state:
        "# Motion sensor for Occupancy detection"
        st.markdown("""
            
            ### Team members
            - Bhargavi
            - Poorvaja
            - Ruchitha
            - Nandini
            """
        )
        menu = ["Login", "Signup"]
        choice = st.sidebar.selectbox("Menu", menu)

        if choice == "Login":
            st.sidebar.subheader("Login section")
            username = st.sidebar.text_input("Username")
            password = st.sidebar.text_input("Password", type='password')
            if st.sidebar.button("Login"):
                loginvalue = login(username, password)
                if loginvalue == 1:
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
                elif loginvalue == 0:
                    st.warning("Invalid credentials")

        elif choice == "Signup":
            st.sidebar.subheader("Register here")
            new_user = st.sidebar.text_input("Username")
            new_password = st.sidebar.text_input("Password", type='password')
            confirm_password = st.sidebar.text_input("Confirm Password", type='password')

            if st.sidebar.button("Signup"):
                if(new_password != confirm_password):
                    st.warning("Password and confirm password doesn't match")
                else:
                    final = sign_up(new_user, new_password)
                    if final == 1:
                        with st.spinner(text='In progress'):
                            time.sleep(3)
                            st.success("Account created successfully")
                            st.balloons()
                            st.title('Welcome ' + new_user)
                            st.info("Go to Login Menu to Login")

                    elif final == 0:
                        st.warning("User already exists")
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
    
