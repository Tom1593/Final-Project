import torch
import torch.nn as nn
import numpy as np
import model as M
import os
import tensorflow as tf

def save_model(model,acc):
    print('==>>>Saving model ...')
    state = {
        'acc':acc,
        'state_dict':model.state_dict() 
    }
    curr_dir = os.path.dirname(os.path.abspath(__file__))
    new_dir = "Params"
    Path = os.path.join(curr_dir,new_dir)
    try:
        os.mkdir(Path)
    except OSError as error:
        #all is good probably folder exists
        pass
    for param_tensor in model.state_dict():
        f = open(Path +"\\"+ param_tensor + ".txt",'w')
        TENSOR = Ternarize_to_Save(model.state_dict()[param_tensor])
        f.writelines([tf.compat.as_str_any(tensor.numpy())+"\n" for tensor in TENSOR])
        f.close()
        break
    torch.save(state,'model_state.pkl')
    print('*** DONE! ***')

def Ternarize_to_Save(tensor):
    output = torch.zeros(tensor.size())
    delta = M.Delta(tensor)
    alpha = M.Alpha(tensor,delta)
    for i in range(tensor.size()[0]):
        for w in tensor[i].view(1,-1):
            pos_one = (w > delta[i]).type(torch.FloatTensor)
            neg_one = -1 * (w < -delta[i]).type(torch.FloatTensor)
        out = torch.add(pos_one,neg_one).view(tensor.size()[1:])
        output[i] = torch.add(output[i],out)
    return output

class TernarizeOp:
    def __init__(self,model):
        count_targets = 0
        for m in model.modules():
            if isinstance(m,nn.Conv2d) or isinstance(m,nn.Linear):
                count_targets += 1
        self.ternarize_range = np.linspace(0,count_targets-1,count_targets).astype('int').tolist()
        self.num_of_params = len(self.ternarize_range)
        self.saved_params = []
        self.target_modules = []
        for m in model.modules():
            if isinstance(m,nn.Conv2d) or isinstance(m,nn.Linear):
                tmp = m.weight.data.clone()
                self.saved_params.append(tmp) #tensor
                self.target_modules.append(m.weight) #Parameter
    
    def SaveWeights(self):
        for index in range(self.num_of_params):
            self.saved_params[index].copy_(self.target_modules[index].data)

    def TernarizeWeights(self):
        for index in range(self.num_of_params):
            self.target_modules[index].data = self.Ternarize(self.target_modules[index].data)
    
    def Ternarize(self,tensor):
        tensor = tensor.cpu()
        output = torch.zeros(tensor.size())
        delta = self.Delta(tensor)
        alpha = self.Alpha(tensor,delta)
        for i in range(tensor.size()[0]):
            for w in tensor[i].view(1,-1):
                pos_one = (w > delta[i]).type(torch.FloatTensor)
                neg_one = torch.mul((w < -delta[i]).type(torch.FloatTensor),-1)
            out = torch.add(pos_one,neg_one).view(tensor.size()[1:])
            output[i] = torch.add(output[i],torch.mul(out,alpha[i]))
        return output#.cuda()
            

    def Alpha(self,tensor,delta):
        Alpha = []
        for i in range(tensor.size()[0]):
            count = 0
            abssum = 0
            absvalue = tensor[i].view(1,-1).abs()
            for w in absvalue:
                truth_value = w > delta[i] #print to see
            count = truth_value.sum()
            abssum = torch.matmul(absvalue,truth_value.type(torch.FloatTensor).view(-1,1))
            Alpha.append(abssum/count)
        alpha = Alpha[0]
        for i in range(len(Alpha) - 1):
            alpha = torch.cat((alpha,Alpha[i+1]))
        return alpha

    def Delta(self,tensor):
        n = tensor[0].nelement()
        if(len(tensor.size()) == 4):     #convolution layer
            delta = 0.7 * tensor.norm(1,3).sum(2).sum(1).div(n)
        elif(len(tensor.size()) == 2):   #fc layer
            delta = 0.7 * tensor.norm(1,1).div(n)
        return delta
            
    
    def Ternarization(self):
        self.SaveWeights()
        self.TernarizeWeights()
    
    def Restore(self):
        for index in range(self.num_of_params):
            self.target_modules[index].data.copy_(self.saved_params[index])
    
