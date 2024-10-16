Image Rotation and Inversion using NPP
This project implements image processing using NVIDIA Performance Primitives (NPP) to perform two operations:

Image Inversion: Inverting the color of an image.
Image Rotation: Rotating an image by 90 degrees.
Project Structure
The project consists of the following key components:

src/main.cpp: The main source code for the application. It implements the image inversion and 90-degree rotation using the NPP library.
Common/: Common utility files and functions used across the project, including NPP exception handling and image loading.
Makefile: Script to build and compile the project using nvcc (NVIDIA CUDA Compiler).
Main Functions
invertFilter(const std::string& inputImagePath, const std::string& outputImagePath):
Applies a color inversion filter to the input image.
Uses NPP’s nppiSubC_8u_C1RSfs function to subtract each pixel value from 255.
rotateBy90Filter(const std::string& inputImagePath, const std::string& outputImagePath):
Rotates the image 90 degrees clockwise.
Uses NPP’s nppiRotate_8u_C1R function.
Requirements
CUDA Toolkit: The project requires the NVIDIA CUDA Toolkit with NPP libraries.
NVIDIA GPU: A CUDA-capable GPU is required for acceleration.
Supported NVIDIA Drivers: Ensure the correct version of the NVIDIA drivers is installed (e.g., driver version 535.129.03 is required for specific setups).
CUDA Toolkit Installation
Make sure that the CUDA toolkit is installed, and the necessary paths are set:

bash
Copy code
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
How to Build
Clone the repository to your local machine:

bash
Copy code
git clone https://github.com/your-repo/imageRotation.git
cd imageRotation
Build the project using the provided Makefile:

bash
Copy code
make
Clean the build (optional):

bash
Copy code
make clean
Usage
Running the Application
To run the application, use the following command:

bash
Copy code
./imageRotation <inputImagePath> <outputImagePath> <operation>
<inputImagePath>: Path to the input image (e.g., images/input.jpg).
<outputImagePath>: Path to save the processed image (e.g., output/inverted.jpg).
<operation>: Specify the operation as invert or rotate.
Examples
Inversion:

bash
Copy code
./imageRotation images/input.jpg output/inverted.jpg invert
Rotation by 90 degrees:

bash
Copy code
./imageRotation images/input.jpg output/rotated.jpg rotate90
Input/Output Directories
Place your input images in the images/ folder.
Output images will be stored in the output/ folder.
Makefile Overview
make: Builds the project.
make clean: Removes compiled files.
The project compiles with nvcc and links against the required NPP libraries:

-lnppial: Arithmetic and logical operations.
-lnppig: Geometric transforms.
-lnppif: Filter functions.
-lnppim: Image processing functions.
Dependencies
NVIDIA CUDA Toolkit
NPP Libraries (included in the CUDA Toolkit)
Known Issues
Ensure the correct GPU architecture is being targeted in the Makefile for older GPUs. The current version targets compute capability 3.5 (-gencode arch=compute_35,code=compute_35).
License
This project is licensed under the MIT License - see the LICENSE file for details.


