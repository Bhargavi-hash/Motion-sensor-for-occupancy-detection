## Libraries needed
> pandas<br>
> numpy<br>
> seaborn<br>
> matplotlib<br>
> pickle<br>
> joblib<br>

## steps to perform prediction on the given input
1) download the files into a folder.
2) run predict.py/prediction.ipynb by giving pixel_matrix_list and corresponding pir_status as the input to get the predicted number of people as output.

## Integration into Application
> 1) download ( prediction.ipynb and j_model.pkl ) or (predict.py and p_model.pkl) whichever u prefer into the same folder.
> 2) give the latest  pixel_matrix as list of lists and corresponding pir status as input in the prediction file.
> 3) get the no.of people as output from the prediction file.
> 4) we get heatmap and corresponding pixelated image also for the given pixel matrix.

## About Dataset
> - 1000+ data points are collected which consisted of attributes such as pir_state from motion sensor, pixel matrix from grid eye sensor, and the number of people(count) evaluated from the image taken at that time instant.<br>
> - This curated raw dataset is used to evaluate mean, standard deviation of the pixel matrix and are added as attributes to the training dataset.<br>
> - pixels where temperatures are above the threshold temperature are marked as active  remaining are considered passive to obtain the pixelated matrix.<br>
> - The no.of active cells is added as attribute in the training data.<br>
> - Sizes of top 3 biggest blobs of active cells are evaluated from the pixelated matrix using depth first search algorithm.<br>
> - The 3 values are added as attributes to training data with names being first_blob, second_blob, third_blob.<br>
> - Finally, the training data consists of 8 attributes out of which pir_state, mean, standard deviation, no.of active cells, first_blob, second_blob, third_blob are independent variables while count is the dependent variable.<br>

## About Model
> Random classifier neural network is used to train and create the model.<br>
> 
> 

## other details
> models are stored in j_model.pkl and p_model.pkl which are obtained by running training.ipynb and train.ipynb respectively.

