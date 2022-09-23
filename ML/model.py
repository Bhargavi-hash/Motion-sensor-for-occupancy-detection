import numpy as np
import pandas as pd


data = pd.read_csv('sample.csv')
rows = data.shape[0]
# print(data.shape)
# print(rows)
# print(data.head())

# converting the string type data present in csv file to float and storing it in row x 64 array.
# pir_state, t_temp, field1, field2 , field3, field4, count

temps = np.zeros((rows, 64)) #this contains 


def convert_it(g_field, index):
    row_index = 0

    for i in g_field:    
        col_index = index

        x = 0
        count = 0
        for j in i :     

            if j == ',':
                x = 0
            elif j == '.':
                count = 1        
            elif count > 0:
                x = x*10 + int(j)
                count = count + 1
            else :
                x = x*10 + int(j)
        
            if count >= 3:
                count = 0
                x = x/100.0
                temps[row_index][col_index] = x
                col_index = col_index + 1

        row_index = row_index + 1

convert_it(data.field1, 0)
convert_it(data.field2, 16)
convert_it(data.field3, 32)
convert_it(data.field4, 48)

#preparing training data
data.drop(['t_temp','field1','field2','field3','field4'],axis=1,inplace=True)
# print(data.head())


def doit(arr, count, visited, i, j):
    if(i < 0 or i > 7 or j < 0 or j > 7):return count 
    elif(visited[i][j] != 0):return count 
    elif(arr[i][j] == 0):return count 

    if(arr[i][j] == 1):
        visited[i][j] = 1
        arr[i][j] = 0
        count = count + 1
        count = doit(arr, count, visited, i+1, j)
        count = doit(arr, count, visited, i-1, j)
        count = doit(arr, count, visited, i, j+1)
        count = doit(arr, count, visited, i, j-1)

        return count



def find_blob_sizes(arr):
    visited = np.zeros((8,8))
    count = 0
    ret = []
    for i in range(8):
        for j in range(8):
            if visited[i][j] == 1:
                continue
            count = doit(arr, count, visited, i, j)
            if count > 0 :
                #print(count)
                ret.append(count)
                count = 0
    
    return ret 




first_blob=[]
second_blob=[]
third_blob=[]
mean_list=[]
sd_list=[]
active_list=[]
for i in temps: # for each row in temps (64 values 1 D aray)

    avg_temp=np.average(i)
    cells=i.reshape(8,8) # this is 8x8 numpy array
    standard_deviation = np.std(cells) # calculate sd of the pixel matrix
    count_actv_cells = 0
    threshhold_temp = avg_temp + 0.75 # 1 #2 or 1.5 whatever
    for a in range(8):
        for r in range(8) :
            if( cells[a][r] >= threshhold_temp): 
                cells[a][r] = 1
                count_actv_cells+=1
            else: cells[a][r] = 0
    
    ans = find_blob_sizes(cells)   #This modifies cells and all values will be zero.
    ans.sort(reverse = True)
    
    # attributes to be used as independent variables
    if len(ans)>0:
        first = ans[0] # 1st biggest blob size
    else: first = 0
    if len(ans)>1:
        second = ans[1] # 2nd biggest blob size
    else :
        second =0
    if len(ans)>2:
        third = ans[2] # 3rd biggest blob size
    else: third = 0
    mean = avg_temp
    sd = standard_deviation # standard deviation
    active_cells = count_actv_cells
    #pir_status = its already there in dataframe.

    #adding data to column lists
    first_blob.append(first)
    second_blob.append(second)
    third_blob.append(third)
    mean_list.append(mean)
    sd_list.append(sd)
    active_list.append(active_cells)
    

data["first_blob"] = first_blob
data["second_blob"] = second_blob
data["third_blob"] = third_blob
data["mean"] = mean_list
data["sd"] = sd_list
data["no_of_active_cells"] = active_list

# print(data.head())

## dependent and independent variables

# Putting feature variable to X
X=data.drop(['count'],axis=1)
# Putting response variable to y
y = data['count']

## Train-Test-Split 

# now lets split the data into train and test
from sklearn.model_selection import train_test_split
# Splitting the data into train and test
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=1)
# print(X_train.shape)
# print(X_test.shape)

## Training the Model

# Using Random Forest Classifier algorithm to predict the weather
# Training model on test data

from sklearn.ensemble import RandomForestClassifier

RF = RandomForestClassifier(max_depth=32,n_estimators=40,random_state=1)
RF.fit(X_train,y_train)
y_pred = RF.predict(X_test)

## Measuring accuracy

from sklearn.metrics import accuracy_score
print("accuracy score : ",accuracy_score(y_test, y_pred))
# To see importance of each variable in prediction
# print(RF.feature_importances_)


## Prediction for a given tuple

# pir_status = 1 # or 0 which we get from thingspeak
# pixel_matrix_list = [[26.75 ,26.75, 25.50,  25.50,  25.50,  25.25, 25.00,   24.25],
#  [26.00,   25.50,  25.50,  25.00,24.75 ,24.50,  24.25, 24.50],
#  [  27.00,   26.00,   25.25, 24.75, 24.75, 24.50,  24.50,  24.25],
#  [27.00,   26.50,  25.25, 25.00,   24.50,  24.50,  24.25, 24.75],
#  [ 27.25, 25.00,   25.00,   24.75,24.25 ,24.75, 24.25, 24.25],
#  [ 27.50, 24.75, 24.75, 24.75, 24.75, 24.50,  24.25, 23.75],
#  [26.75, 25.25, 25.00,   24.50,  24.75, 24.50,  25.00,   24.00],
#  [25.25 ,25.00,   24.75, 24.50, 24.00,   24.00,   24.00,   23.50] ] # list of lists derived from thingspeak data

# #print(pixel_matrix_list)
# cells = np.array(pixel_matrix_list) # this is 8x8 numpy array
# #print(cells)
# avg_temp=np.average(cells)
# standard_deviation = np.std(cells) # calculate sd of the pixel matrix
# count_actv_cells = 0
# threshhold_temp = avg_temp + 0.75 # 1 #2 or 1.5 whatever
# for a in range(8):
#     for r in range(8) :
#         if( cells[a][r] >= threshhold_temp): 
#             cells[a][r] = 1
#             count_actv_cells+=1
#         else: cells[a][r] = 0

# ans = find_blob_sizes(cells)   #This modifies cells and all values will be zero.
# ans.sort(reverse = True)

# # attributes to be used as independent variables
# if len(ans)>0:
#     first = ans[0] # 1st biggest blob size
# else: first = 0
# if len(ans)>1:
#     second = ans[1] # 2nd biggest blob size
# else :
#     second =0
# if len(ans)>2:
#     third = ans[2] # 3rd biggest blob size
# else: third = 0

# mydataset = {
#   'pir_state' : [pir_status],
#   'first_blob' : [first],
#   'second_blob' : [second],
#   'third_blob' : [third],
#   'mean' : [avg_temp],
#   'sd' : [standard_deviation], # standard deviation
#   'no_of_active_cells' : [count_actv_cells]
# }

# input = pd.DataFrame(mydataset)

# # print(input) # prints input data frame of single tuple
# output = RF.predict(input) # return numpy array

# print("predicted count = ",output[0])
# # print(type(output[0]))