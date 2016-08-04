

data = csvread('rtrace.csv');

%data

std_data = std(data);
avg_data = mean(data);

std_data
avg_data