# DistBlock: Realtime Distortion Stacking Application

DistBlock is a real-time distortion stacking application/plugin that allows users to stack various clipping functions to create custom distortion sounds. Developed using the JUCE framework, it features multiple serial processing slots, real-time adjustable parameters, and more.

### Features

- Input/Output RMS Meter and Global Adjustable Gain
- Ten serial processing slots for dynamically inserting or deleting processing blocks
- Multiple real-time adjustable parameters
- Multiple predefined transfer functions
- IR Loader (Fast Convolution)

## How to Use

### Insert a New Block

1. Click on any empty block to open the type selection panel.
2. Choose to create either a distortion block or an IR block.
3. Click the 'Create Effect Block' button to insert the new processing block.

### Adjust Parameters

1. Click on any non-empty processing block to open the parameter adjustment panel.
2. For distortion blocks, adjust the mix, pre-gain, post-gain, low-cut frequency, and high-cut frequency parameters.
3. For IR Loader blocks, adjust the mix parameter.
4. Bypass or engage any processing block to compare the results.

### Delete a Block

1. Click on any non-empty processing block to open the parameter adjustment panel.
2. Click the 'Delete' button to remove the processing block in real-time.

### Warning

- Adding multiple distortion blocks with high pre-gain parameters requires a cab simulation IR block at the end to avoid harsh sounds due to excessive high frequencies.

### Provided Resource IR Files for Testing

- **Cabinet IR.wav:** IR of a guitar cabinet.
- **Hall Reverb IR.wav:** IR of hall reverb.
- Located in the `./Resource` folder.
