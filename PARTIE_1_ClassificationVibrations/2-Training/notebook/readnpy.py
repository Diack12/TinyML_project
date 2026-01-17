

# Read the mean and std for normalization  in the inference phase
import numpy as np


mean = np.load("PARTIE_1_ClassificationVibrations/2-Training/notebook/mean.npy")
std = np.load("PARTIE_1_ClassificationVibrations/2-Training/notebook/std.npy")

print(mean)
print(std)