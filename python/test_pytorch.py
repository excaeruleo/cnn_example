#! /usr/bin/python3

import torch
x = torch.rand(5, 3)
print(x)

print("pytorch cuda activated: " + str(torch.cuda.is_available()) )

