import torch
import torchvision.models as models
import pnnx

def export_squeezenet1_1():
    model = models.squeezenet1_1(pretrained=True)
    x = torch.rand(1, 3, 227, 227)
    opt_model = pnnx.export(model, "squeezenet_v1.1.pt", x)

if __name__ == "__main__":
    export_squeezenet1_1()
