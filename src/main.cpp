#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#pragma warning(disable : 4819)
#endif

#include "Exceptions.h"
#include <ImageIO.h>
#include <ImagesCPU.h>
#include <ImagesNPP.h>

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include <cuda_runtime.h>
#include <npp.h>

#include <helper_cuda.h>

#include <sstream>
#include <vector>

std::vector<std::string> splitString(const std::string &str, char delimiter)
{
       std::vector<std::string> tokens;
       std::stringstream ss(str);
       std::string token;
       while (std::getline(ss, token, delimiter))
       {
              tokens.push_back(token);
       }
       return tokens;
}

bool printfNPPinfo(int argc, char *argv[])
{
       const NppLibraryVersion *libVer = nppGetLibVersion();

       printf("NPP Library Version %d.%d.%d\n", libVer->major, libVer->minor,
              libVer->build);

       int driverVersion, runtimeVersion;
       cudaDriverGetVersion(&driverVersion);
       cudaRuntimeGetVersion(&runtimeVersion);

       printf("  CUDA Driver  Version: %d.%d\n", driverVersion / 1000,
              (driverVersion % 100) / 10);
       printf("  CUDA Runtime Version: %d.%d\n", runtimeVersion / 1000,
              (runtimeVersion % 100) / 10);

       // Min spec is SM 1.0 devices
       bool bVal = checkCudaCapabilities(1, 0);
       return bVal;
}

void invertFilter(const std::string &filePath, const std::string &outputFile)
{
       try
       {
              std::cout << "Processing of " << filePath << " (Invert) started." << std::endl;
              npp::ImageCPU_8u_C1 hostSrc;
              npp::loadImage(filePath, hostSrc);
              npp::ImageNPP_8u_C1 deviceSrc(hostSrc);

              const NppiSize srcSize = {(int)deviceSrc.width(), (int)deviceSrc.height()};
              npp::ImageNPP_8u_C1 deviceDst(srcSize.width, srcSize.height);

              // Manual inversion: subtract pixel values from 255
              NPP_CHECK_NPP(nppiSubC_8u_C1RSfs(deviceSrc.data(), deviceSrc.pitch(), 255, deviceDst.data(), deviceDst.pitch(), srcSize, 0));

              npp::ImageCPU_8u_C1 hostDst(deviceDst.size());
              deviceDst.copyTo(hostDst.data(), hostDst.pitch());
              saveImage(outputFile, hostDst);
              std::cout << "Processing of " << filePath << " (Invert) ended. Result saved to: " << outputFile << std::endl;

              nppiFree(deviceSrc.data());
              nppiFree(deviceDst.data());
       }
       catch (npp::Exception &rException)
       {
              std::cerr << "Program error! The following exception occurred: \n";
              std::cerr << rException << std::endl;
              std::cerr << "Aborting." << std::endl;

              exit(EXIT_FAILURE);
       }
       catch (...)
       {
              std::cerr << "Program error! An unknown type of exception occurred. \n";
              std::cerr << "Aborting." << std::endl;

              exit(EXIT_FAILURE);
       }
}

void rotateBy90Filter(const std::string &filePath, const std::string &outputFile)
{
       try
       {
              std::cout << "Processing of " << filePath << " (Rotate by 90) started." << std::endl;
              npp::ImageCPU_8u_C1 hostSrc;
              npp::loadImage(filePath, hostSrc);
              npp::ImageNPP_8u_C1 deviceSrc(hostSrc);

              const NppiSize srcSize = {(int)deviceSrc.width(), (int)deviceSrc.height()};
              const NppiRect srcROI = {0, 0, srcSize.width, srcSize.height};

              // Rotated dimensions
              const NppiSize dstSize = {srcSize.height, srcSize.width};
              npp::ImageNPP_8u_C1 deviceDst(dstSize.width, dstSize.height);

              NppiRect srcRect = {0, 0, srcSize.width, srcSize.height};
              NppiRect dstRect = {0, 0, dstSize.width, dstSize.height};
              NPP_CHECK_NPP(nppiRotate_8u_C1R(deviceSrc.data(), srcSize, deviceSrc.pitch(), srcRect, deviceDst.data(), deviceDst.pitch(), dstRect, 90.0, srcSize.width / 2, srcSize.height / 2, NPPI_INTER_LINEAR));

              npp::ImageCPU_8u_C1 hostDst(deviceDst.size());
              deviceDst.copyTo(hostDst.data(), hostDst.pitch());
              saveImage(outputFile, hostDst);
              std::cout << "Processing of " << filePath << " (Rotate by 90) ended. Result saved to: " << outputFile << std::endl;

              nppiFree(deviceSrc.data());
              nppiFree(deviceDst.data());
       }
       catch (npp::Exception &rException)
       {
              std::cerr << "Program error! The following exception occurred: \n";
              std::cerr << rException << std::endl;
              std::cerr << "Aborting." << std::endl;

              exit(EXIT_FAILURE);
       }
       catch (...)
       {
              std::cerr << "Program error! An unknown type of exception occurred. \n";
              std::cerr << "Aborting." << std::endl;

              exit(EXIT_FAILURE);
       }
}

void applyFilter(const std::string &filterType, const std::string &filePath, const std::string &outputFile)
{
       if (filterType == "invert")
       {
              std::cout << "Selected Invert Filter." << std::endl;
              invertFilter(filePath, outputFile);
       }
       else if (filterType == "rotate90")
       {
              std::cout << "Selected Rotate by 90 Degrees Filter." << std::endl;
              rotateBy90Filter(filePath, outputFile);
       }
       
       else
       {
              std::cout << "Filter type isn't supported!" << std::endl;
       }
       cudaDeviceSynchronize();
       cudaDeviceReset();
}
int main(int argc, char *argv[])
{
       printf("%s Starting...\n\n", argv[0]);

       if (!printfNPPinfo(argc, argv))
       {
              exit(EXIT_SUCCESS);
       }

       findCudaDevice(argc, (const char **)argv);

       char *inputData;
       if (argc >= 2)
       {
              inputData = argv[1];
              if (!inputData)
              {
                     std::cerr << "Cannot read the input data!" << std::endl;
                     exit(EXIT_FAILURE);
              }
       }
       else
       {
              std::cerr << "Input folder or image missed!" << std::endl;
              exit(EXIT_FAILURE);
       }

       char *filterData;
       if (argc >= 3)
       {
              filterData = argv[2];
              if (!inputData)
              {
                     std::cerr << "Cannot read the filter type!" << std::endl;
                     exit(EXIT_FAILURE);
              }
       }
       else
       {
              std::cerr << "Filter type is missed!" << std::endl;
              exit(EXIT_FAILURE);
       }

       char *outputData;
       if (argc >= 4)
       {
              outputData = argv[3];
              if (!inputData)
              {
                     std::cerr << "Cannot read the filter type!" << std::endl;
                     exit(EXIT_FAILURE);
              }
       }
       else
       {
              std::cerr << "Filter type is missed!" << std::endl;
              exit(EXIT_FAILURE);
       }

       std::string filterType{filterData};
       std::string output{outputData};
       if (std::filesystem::is_directory(output))
       {
              std::filesystem::create_directory(output);
       }

       std::string inputValue{inputData};
       if (!std::filesystem::is_directory(inputValue))
       {
              std::string outputFile{output};
              if (std::filesystem::is_directory(output))
              {
                     const std::string fileName = std::filesystem::path(inputValue).filename().string();
                     const auto parts = splitString(fileName, '.');
                     outputFile += "/" + parts.front() + ".jpg";
              }
              applyFilter(filterType, inputValue, outputFile);
       }
       return 0;
}