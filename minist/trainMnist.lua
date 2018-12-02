require 'torch'
require 'nn'
require 'lab'
mnist = require 'mnist'

print('Read data set')
fullset = mnist.traindataset()
testset = mnist.testdataset()

print('Split validation set')
trainset = {
    size = 50000,
    data = fullset.data:sub(1,50000),
    label = fullset.label:sub(1,50000)
}
trainset.data = torch.Tensor(trainset.data:size()):copy(trainset.data)

validationset = {
    size = 10000,
    data = fullset.data:sub(50001,60000),
    label = fullset.label:sub(50001,60000)
}
validationset.data = torch.Tensor(validationset.data:size()):copy(validationset.data)

print('Normalize')
trainset.data = trainset.data - trainset.data:mean()
validationset.data = validationset.data - validationset.data:mean()


model = nn.Sequential()
model:add(nn.Reshape(1, 28, 28))
model:add(nn.CMul(1/256.0*3.2))
model:add(nn.SpatialConvolution(1, 20, 5, 5, 1, 1, 0, 0))
model:add(nn.SpatialSubSampling(2, 2 , 2, 2, 0, 0))
model:add(nn.SpatialConvolution(20, 50, 5, 5, 1, 1, 0, 0))
model:add(nn.SpatialSubSampling(2, 2 , 2, 2, 0, 0))
model:add(nn.Reshape(4*4*50))
model:add(nn.Linear(4*4*50, 500))
--model:add(nn.ReLU())
model:add(nn.Linear(500, 10))
model:add(nn.LogSoftMax())

model = require('mnist-weight-init')(model, 'xavier')

criterion = nn.ClassNLLCriterion()

--[[model = model:cuda()
criterion = criterion:cuda()
trainset.data = trainset.data:cuda()
trainset.label = trainset.label:cuda()
validationset.data = validationset.data:cuda()
validationset.label = validationset.label:cuda()]]

sgd_params = {
   learningRate = 1e-2,
   learningRateDecay = 1e-4,
   weightDecay = 1e-3,
   momentum = 1e-4
}

x, dl_dx = model:getParameters()

step = function(batch_size)
    local current_loss = 0
    local count = 0
    local shuffle = torch.randperm(trainset.size)
    batch_size = batch_size or 200
    for t = 1,trainset.size,batch_size do
        -- setup inputs and targets for this mini-batch
        local size = math.min(t + batch_size - 1, trainset.size) - t
        local inputs = torch.Tensor(size, 28, 28)--:cuda()
        local targets = torch.Tensor(size)--:cuda()
        for i = 1,size do
            local input = trainset.data[shuffle[i+t]]
            local target = trainset.label[shuffle[i+t]]
            -- if target == 0 then target = 10 end
            inputs[i] = input
            targets[i] = target
        end
        targets:add(1)
        local feval = function(x_new)
            -- reset data
            if x ~= x_new then x:copy(x_new) end
            dl_dx:zero()

            -- perform mini-batch gradient descent
            local loss = criterion:forward(model:forward(inputs), targets)
            model:backward(inputs, criterion:backward(model.output, targets))

            return loss, dl_dx
        end

        _, fs = optim.sgd(feval, x, sgd_params)

        -- fs is a table containing value of the loss function
        -- (just 1 value for the SGD optimization)
        count = count + 1
        current_loss = current_loss + fs[1]
    end

    -- normalize loss
    return current_loss / count
end

eval = function(dataset, batch_size)
    local count = 0
    batch_size = batch_size or 200
    
    for i = 1,dataset.size,batch_size do
        local size = math.min(i + batch_size - 1, dataset.size) - i
        local inputs = dataset.data[{{i,i+size-1}}]:cuda()
        local targets = dataset.label[{{i,i+size-1}}]:cuda()
        local outputs = model:forward(inputs)
        local _, indices = torch.max(outputs, 2)
        indices:add(-1)
        indices = indices:cuda()
        local guessed_right = indices:eq(targets):sum()
        count = count + guessed_right
    end

    return count / dataset.size
end

max_iters = 30

print('Start training')
do
    local last_accuracy = 0
    local decreasing = 0
    local threshold = 1 -- how many deacreasing epochs we allow
    for i = 1,max_iters do
        local loss = step()
        print(string.format('Epoch: %d Current loss: %4f', i, loss))
        local accuracy = eval(validationset)
        print(string.format('Accuracy on the validation set: %4f', accuracy))
        if accuracy < last_accuracy then
            if decreasing > threshold then break end
            decreasing = decreasing + 1
        else
            decreasing = 0
        end
        last_accuracy = accuracy
    end
end

testset.data = testset.data:double()
eval(testset)



