# ATL LLTA Controller Unit Test APP 

The objective of this project is to develop a low-level-test-app (LLTA) for testing the ATL-controller hardware interface and simulating the control algorithm. This will serve as an initial stage towards the development of a fully functional ATL-controller. The LLTA should enable the following results:

* Testing and monitoring of all ATL-controller hardware components (modules).
* Simulation of the control algorithm using Windows and parameter trainining (auto-tuning). 
* Benchmarking of the ATL-controller hardware modules
* Data filtering (sensor fusion)

A summary of the whole ATL-controller architecture as of today is shown below, with the exact naming reference from the ATL firmware components:

![General_block_small](https://user-images.githubusercontent.com/32825004/235073454-856715b3-56a0-4059-b672-6a9d7a2ba06c.png)

## Approach

I have kept in mine the following key aspects as i want to share my own part of the project clearly so that any developer can contribute and we can all be in the same loop. I am taking into consideration three essential aspects: portability, modularity, and readability.

Portability: the ability to create the firmware so that it can be deployed in all the ATL-controllers (KJ, Venom, Gravity-series, and other ESP32 based). For this reason, I have created a header file  ```atl_app_hardware.h``` where all the PIN-configuration for the hardware (schematics) can be modified to fit any of the controllers, ```atl_task_scheduler.h``` where RTOS task priority and stack size can be configure, ```atl_LLTA_task.h```is the task defined to run the Unit test LLTA and ```atl_RPID_task.h``` is defined to run the control algorithm RPID for the grill.

Modularity: Dividing the ATL-controllers hardware and peripherials components into separate, interchangeable and independent modules that can all be developed separately, tested and maintained. For this reason, I have blocked the systems as shown in the diagram above and serperated all the hardware interface into the ```components/atl_module_name/atl_module_name.c``` directory with refference to the diagram.

Readability: To easy comprehend, interpret and interact with the firmware. For this reason, I'm strictly using MISRA conding guide more specific the Texas Instruments standard [MISRA_guide](https://github.com/AtlasIoT-development/ATL-Playground/blob/main/software/0024-atl_LLTA_Controller_Test_APP/docs/Coding_Guide/MISRA_C_MCU_CodingGuidelines.pdf), as well as the guide that was previously created by ATL [atl_guide](https://github.com/AtlasIoT-development/ATL-Playground/blob/main/software/0024-atl_LLTA_Controller_Test_APP/docs/Coding_Guide/AtlasIoT%20-%20Firmware%20and%20Software%20version%20control%20rev%201.0.0.pdf), while the firmware is still in development some coding statements might not be correct at moment and will later be reviewed for changes.

# How to run the LLTA 

The LLTA app is runing two RTOS task for the LLTA unit testing and RPID algorithm simulator.
Bellow are stepts to runthe LLTA firmware.

* Requires VS Code and the v5 ESP-IDF.
* Install any serial monitor tool on Windows.
* Navigate to ```app_main.c``` and choose to run either of the two task ```app_create_LLTA_task()``` or ```app_create_RPID_task()```.
* Build and flash the LLTA firmware to the target ATL-controller.
* Open the serial monitor tool and select the COM port that the ATL-controller is connected to.
* If you choose the ```app_create_LLTA_task()```, the serial monitor will prompt you to enter the following input for the unit test (component test) as shown in this image below.
 
![intro](https://user-images.githubusercontent.com/32825004/235072382-efd52f91-bb91-4dc8-9078-c62c9dbaae1f.png)

* And if you choose ```app_create_RPID_task()```, the serial monitor will directly display the RPID response parameters as shown below. Input the Grill_set_temp and evaluate the performance and continue tunning the RPID parameters till satisfaction.

![image](https://github.com/AtlasIoT-development/ATL-Playground/assets/32825004/4bc09fdb-d98b-45f6-8fea-2b7412271ece)

# To do

* Implementing the FPID algorithm and upgrading from the previous RPID
* Sensor Data Filtering
* Intergrating the atl_ili9488 with the LLTA
* Intergrating atl_AWS_IoT with the LLTA

