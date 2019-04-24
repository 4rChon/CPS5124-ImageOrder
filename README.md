# CPS5124-ImageOrder
An image order implementation for CPS5124

![image](https://i.imgur.com/goO1aBQ.png "4096 samples per pixel, 512x512")

### Command Line Arguments
```-batch```: Render all files in the specified ```load_path``` directory if true. Render single input file specified by ```input_file``` otherwise. Default: true

```-load_path```: Relative folder containing scene definitions. Default: "./definitions/"

```-save_path```: Relative save folder location. Default: "./renders/"

```-input_file```: Name of definition file. Unused if ```batch``` is true. Default: "in.json"

```-output_file```: Name of output file. Unused if ```batch``` is true. Default: "out.ppm"

```-tonemapper```: "sigmoid" or "linear" tonemapper. Default: "sigmoid"

```-progress```: Output X/Y progress updates every 1000 pixels where X is current pixel and Y is total number of pixels. Default: false

```-stratified_sampling```: "uniform" or "stratified" sampling strategy. Default: "stratified"
