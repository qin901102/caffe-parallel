# Caffe

Caffe is a deep learning framework developed with cleanliness, readability, and speed in mind.<br />
Consult the [project website](http://caffe.berkeleyvision.org) for all documentation.

This code tries to parallel the trainig synchronously. 

The basic idea:
1. Before calling the ForwardBackward, distribute the parameters to all gpus.
2. Simultaniously run one ForwardBackward on each gpu.
3. Collect the gradient from each gpu and integrate them to finish one ForwardBackward. 

How to use:
1. In the solver protocal file, we first set the gpu id by 'device_id:0'
2. If we would like to parallel it with another gpu 1, then add 'device_ids:1' 
   into the file.
3. It is highly recommended to set the random seed for each gpu. For the 0-th
   gpu, set it by 'random_seed: 1' as usual. For the gpu 1, set it by
   'random_seeds : 2'. 
4. Since it is data-paralleled, please set rand_skip in the data layer. 

Demo:
1. Let's use the mnist dataset. Firstly prepare the data by 
        sh data/mnist/get_mnist.sh
        sh examples/mnist/create_mnist.sh
2. Then, run the paralled code by 
        sh examples/mnist/train_lenet.sh
3. The above code is to run by gpu 0 and gpu 1. If gpu 2 is also available,
   please uncomment the corresponding sentence in the solver protocol file. 

Remarks:
This code also provides some other features, but can be ignored.
