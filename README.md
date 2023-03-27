# manometer_reading
This code is been developed for reading circular manometer dials.
It uses Hough circles to locate the circular dial. It then uses hough line transforms to find the needle.
Based on the angle of the needle the model is able to give the value reading.
Model can be runned by using the binary file DisplayImage and takes one argument which is the path to the image.
The range and angle has been hard coded which can be changed from the source code to handle new models of manometers.

Rushabh Nalin Mehta
MSc RWTH Robotic Systems Engineering

Output can be seen here:

![Gauge-1_results](https://user-images.githubusercontent.com/84985854/227984537-b9baf399-aebd-4341-90d5-907e93454ddd.jpg)
![Gauge-2_results](https://user-images.githubusercontent.com/84985854/227984555-927fe866-82db-40b2-8bc5-5d0a5c87efbb.jpg)
![Gauge-3_results](https://user-images.githubusercontent.com/84985854/227984564-9cd45c81-9071-49c8-8a5e-2c1c917f9bfb.jpg)
![Gauge-4_results](https://user-images.githubusercontent.com/84985854/227984570-7be727ed-f0db-45e1-bdef-cadbffa50000.jpg)
![Gauge-5_results](https://user-images.githubusercontent.com/84985854/227984579-980e2400-1545-4002-862d-e0d61df87d62.jpg)
