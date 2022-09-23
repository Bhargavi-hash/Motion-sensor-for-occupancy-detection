import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from model import RF,doit,find_blob_sizes

pir_status = 1 # or 0 which we get from thingspeak
pixel_matrix_list = [[26.75 ,26.75, 25.50,  25.50,  25.50,  25.25, 25.00,   24.25],
 [26.00,   25.50,  25.50,  25.00,24.75 ,24.50,  24.25, 24.50],
 [  27.00,   26.00,   25.25, 24.75, 24.75, 24.50,  24.50,  24.25],
 [27.00,   26.50,  25.25, 25.00,   24.50,  24.50,  24.25, 24.75],
 [ 27.25, 25.00,   25.00,   24.75,24.25 ,24.75, 24.25, 24.25],
 [ 27.50, 24.75, 24.75, 24.75, 24.75, 24.50,  24.25, 23.75],
 [26.75, 25.25, 25.00,   24.50,  24.75, 24.50,  25.00,   24.00],
 [25.25 ,25.00,   24.75, 24.50, 24.00,   24.00,   24.00,   23.50] ] # list of lists derived from thingspeak data

#print(pixel_matrix_list)
cells = np.array(pixel_matrix_list) # this is 8x8 numpy array
#print(cells)

## heat map  plot
heat_map = cells
ax = sns.heatmap(heat_map, linewidth=0.5, cmap = 'YlOrBr')
plt.show()

avg_temp=np.average(cells)
standard_deviation = np.std(cells) # calculate sd of the pixel matrix
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
plt.imshow( pixelated_image , cmap = 'hot' , interpolation = 'nearest' ) 
plt.show()

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

mydataset = {
  'pir_state' : [pir_status],
  'first_blob' : [first],
  'second_blob' : [second],
  'third_blob' : [third],
  'mean' : [avg_temp],
  'sd' : [standard_deviation], # standard deviation
  'no_of_active_cells' : [count_actv_cells]
}

input = pd.DataFrame(mydataset)

# print(input) # prints input data frame of single tuple
output = RF.predict(input) # return numpy array

print("predicted no.of people = ",output[0])




