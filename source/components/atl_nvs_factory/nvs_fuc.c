/*
 * nvs_fuc.c
 *
 *  Created on: 2018��12��3��
 *      Author: Administrator
 */


/* Non-Volatile Storage (NVS) Read and Write a Value - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "defineh.h"
#include "nvs_flash.h"
#include "nvs.h"

int16_t nvs_old=0;
int16_t nvs_old1=0;
int16_t nvs_old2=0;

//nvs_handle_t my_handle;
nvs_handle_t my_handle2;

void nvs_init(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == 
ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void read_nvc(void)
{
	esp_err_t err;
    // Open
	nvs_handle_t my_handle;

	nvs_stats_t nvs_stats;

	nvs_get_stats(NULL, &nvs_stats);
	printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n",
	       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if(err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
       	// printf("Done\n");
        // Read
        // printf("Reading restart counter from NVS ... ");
        err = nvs_get_i16(my_handle, "nvsod", &nvs_old);
	    err = nvs_get_i16(my_handle, "nvsod1", &nvs_old1);
		
		if(nvs_old==version_id&&nvs_old1==Rversion_id)
			{
       		 	err = nvs_get_i16(my_handle, "psrKp", &psArgument_Kp);
        		err = nvs_get_i16(my_handle, "Seset", &Searing_Offset);
				err = nvs_get_i16(my_handle, "psATi", &psArgument_Ti);
				err = nvs_get_i16(my_handle, "psATd", &psArgument_Td);

				err = nvs_get_i16(my_handle, "ofet1", &meat_temp_offset1);
				err = nvs_get_i16(my_handle, "ofet2", &meat_temp_offset2);
				err = nvs_get_i16(my_handle, "ofet3", &meat_temp_offset3);
				err = nvs_get_i16(my_handle, "ofet4", &meat_temp_offset4);
				err = nvs_get_u8(my_handle,  "Fatep", &Fan_step_2);
				err = nvs_get_u8(my_handle,  "fcf", &f_c_f);
				err = nvs_get_u8(my_handle,  "connect", &connect_alrealy);
				err = nvs_get_u8(my_handle,  "upgrde", &upgrde_init);
				err = nvs_get_u8(my_handle,  "offblue", &close_blue);

				//err = nvs_get_str(my_handle, "xssid", xssid,sizeof(xssid));   
				// //  strlen����
				//err = nvs_get_str(my_handle, "xpwd", xpwd,sizeof(xpwd));
				size_t size1=sizeof(xssid);
				size_t size2=sizeof(xpwd);
				size_t size3=sizeof(blue_MAC1);
				size_t size4=sizeof(update_ver);

				err = nvs_get_blob(my_handle, "xssid", xssid,&size1);   
				err = nvs_get_blob(my_handle, "xpwd", xpwd,&size2);
				err = nvs_get_blob(my_handle, "bluem", blue_MAC1,&size3);
				err = nvs_get_blob(my_handle, "ver", update_ver,&size4);

				//printf("nvs_old in read1:%d\r\n",nvs_old);
				//printf("nvs_old1 in read1:%d\r\n",nvs_old1);
			}
		else if((version_id > nvs_old ||(version_id == nvs_old&&Rversion_id>nvs_old1)) && (nvs_old>0 || nvs_old1>0))
			{
				size_t size1=sizeof(xssid);
				size_t size2=sizeof(xpwd);
				err = nvs_get_u8(my_handle,  "connect", &connect_alrealy);

				err = nvs_get_blob(my_handle, "xssid", xssid,&size1);
				err = nvs_get_blob(my_handle, "xpwd", xpwd,&size2);

				err = nvs_get_u8(my_handle,  "upgrde", &upgrde_init);

				//printf("nvs_old in read2:%d\r\n",nvs_old);
				//printf("nvs_old1 in read2:%d\r\n",nvs_old1);
			}


			//printf("nvs_old in read3:%d\r\n",nvs_old);
			//printf("nvs_old1 in read3:%d\r\n",nvs_old1);
			//err = nvs_commit(my_handle);
	    	//printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

	    	// Close
	    	nvs_close(my_handle);
    }

	nvs_get_stats(NULL, &nvs_stats);
	printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n",
	       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);
	
/*
	err = nvs_open("storage", NVS_READWRITE, &my_handle2);
    if(err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle2!\n", esp_err_to_name(err));
    }
    else
    {
	    err = nvs_get_i16(my_handle2, "nvsod2", &nvs_old2);
		if(nvs_old2==0x5c)
			{
				size_t size1=sizeof(xssid);
				size_t size2=sizeof(xpwd);
				err = nvs_get_u8(my_handle2,  "connect", &connect_alrealy);
				
				err = nvs_get_blob(my_handle2, "xssid", xssid,&size1);   
				err = nvs_get_blob(my_handle2, "xpwd", xpwd,&size2);
				err = nvs_get_u8(my_handle2,  "upgrde", &upgrde_init);
			}
        // Close
	    	nvs_close(my_handle2);
      }
*/
}
////////////
void erase_handlenvs(void)
{
	esp_err_t err;
	nvs_handle_t my_handle;
	// Open
	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	}
	else
	{
		printf("Error (%s) \n", esp_err_to_name(err));
	 	nvs_erase_all(my_handle);
	 	err = nvs_commit(my_handle);
	 	//printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
	 	nvs_close(my_handle);
	}


}
void write_nvc(void)
{
	esp_err_t err;
	nvs_handle_t my_handle;

	nvs_stats_t nvs_stats;
/*
	nvs_entry_info_t info;

	nvs_iterator_t it = nvs_entry_find("nvs", "storage", NVS_TYPE_ANY);

	while (it != NULL) {

	        nvs_entry_info(it, &info);
	        it = nvs_entry_next(it);
	        printf("key '%s', type '%d' \n", info.key, info.type);
	};
*/
	
	nvs_get_stats(NULL, &nvs_stats);
	printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n",
	       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);

	// Open
	    err = nvs_open("storage", NVS_READWRITE, &my_handle);
	    if (err != ESP_OK)
	    {
	        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	    }
	    else
	    {
	    	//nvs_handle_t my_handle;
	    	// Write
	    	//printf("Updating restart counter in NVS ... ");
			printf("Error (%s) \n", esp_err_to_name(err));
	    	nvs_old=version_id;
			nvs_old1=Rversion_id;
	    	err = nvs_set_i16(my_handle, "psrKp", psArgument_Kp);
	    	//if (err != ESP_OK) printf("Error (%s) saving psArgument_Kp to NVS!\n", esp_err_to_name(err));
	    	err = nvs_set_i16(my_handle, "Seset", Searing_Offset);
	    	//if (err != ESP_OK) printf("Error (%s) saving Searing_Offset to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "nvsod", nvs_old);
			//if (err != ESP_OK) printf("Error (%s) saving nvs_old to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "nvsod1",nvs_old1);
			//if (err != ESP_OK) printf("Error (%s) saving nvs_old1 to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "psATi", psArgument_Ti);
			//if (err != ESP_OK) printf("Error (%s) saving psArgument_Ti to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "psATd", psArgument_Td);
			//if (err != ESP_OK) printf("Error (%s) saving psArgument_Td to NVS!\n", esp_err_to_name(err));

			err = nvs_set_i16(my_handle, "ofet1", meat_temp_offset1);
			//if (err != ESP_OK) printf("Error (%s) saving meat_temp_offset1 to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "ofet2", meat_temp_offset2);
			//if (err != ESP_OK) printf("Error (%s) saving meat_temp_offset2 to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "ofet3", meat_temp_offset3);
			//if (err != ESP_OK) printf("Error (%s) saving meat_temp_offset3 to NVS!\n", esp_err_to_name(err));
			err = nvs_set_i16(my_handle, "ofet4", meat_temp_offset4);
			//if (err != ESP_OK) printf("Error (%s) saving meat_temp_offset4 to NVS!\n", esp_err_to_name(err));
			err = nvs_set_u8(my_handle, "Fatep", Fan_step_2);
			//if (err != ESP_OK) printf("Error (%s) saving Fan_step to NVS!\n", esp_err_to_name(err));
			err = nvs_set_u8(my_handle, "fcf", f_c_f);
			//if (err != ESP_OK) printf("Error (%s) saving f_c_f to NVS!\n", esp_err_to_name(err));
			err = nvs_set_u8(my_handle, "connect", connect_alrealy);
			//if (err != ESP_OK) printf("Error (%s) saving connect_alrealy to NVS!\n", esp_err_to_name(err));
			err = nvs_set_u8(my_handle, "upgrde", upgrde_init);
			//if (err != ESP_OK) printf("Error (%s) saving upgrde_init to NVS!\n", esp_err_to_name(err));
			err = nvs_set_u8(my_handle, "offblue", close_blue);
			//if (err != ESP_OK) printf("Error (%s) saving close_blue to NVS!\n", esp_err_to_name(err));
			
			size_t size1=sizeof(xssid);
			size_t size2=sizeof(xpwd);
			size_t size3=sizeof(blue_MAC1);
			size_t size4=sizeof(update_ver);

			err = nvs_set_blob(my_handle, "xssid", xssid,size1);
			//if (err != ESP_OK) printf("Error (%s) saving xssid to NVS!\n", esp_err_to_name(err));
			err = nvs_set_blob(my_handle, "xpwd", xpwd,size2);
			//if (err != ESP_OK) printf("Error (%s) saving xpwd to NVS!\n", esp_err_to_name(err));
			err = nvs_set_blob(my_handle, "bluem", blue_MAC1,size3);
			//if (err != ESP_OK) printf("Error (%s) saving blue_MAC1 to NVS!\n", esp_err_to_name(err));
			err = nvs_set_blob(my_handle, "ver", update_ver,size4);
			//if (err != ESP_OK) printf("Error (%s) saving update_ver to NVS!\n", esp_err_to_name(err));

			//err = nvs_set_str(my_handle, "xpwd", xpwd);

	    	//printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

	    	// Commit written value.
	    	// After setting any values, nvs_commit() must be called to ensure changes are written
	    	// to flash storage. Implementations may write to storage at other times,
	    	// but this is not guaranteed.
	    	//printf("Committing updates in NVS ... ");
	    	err = nvs_commit(my_handle);
	    	//printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

	    	// Close
	    	nvs_close(my_handle);

	    	//printf("nvs_old in write1:%d\r\n",nvs_old);
	    	//printf("nvs_old1 in write1:%d\r\n",nvs_old1);
	    }


	    nvs_get_stats(NULL, &nvs_stats);
	    printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n",
	    	       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);

	    //printf("nvs_old in write2:%d\r\n",nvs_old);
	    //printf("nvs_old1 in write2:%d\r\n",nvs_old1);
		
/*
	    err = nvs_open("storage", NVS_READWRITE, &my_handle2);
		  if (err != ESP_OK)
			   {
				   printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
			   }
		   else
			   {
				   //nvs_handle_t my_handle;
				   // Write
				   //printf("Updating restart counter in NVS ... ");
				   nvs_old2=0x5c;
				   err = nvs_set_i16(my_handle2, "nvsod2", nvs_old2);

				   size_t size1=sizeof(xssid);
				   size_t size2=sizeof(xpwd);
				   err = nvs_set_blob(my_handle2, "xssid", xssid,size1);
				   err = nvs_set_blob(my_handle2, "xpwd", xpwd,size2);

				  err = nvs_set_u8(my_handle2, "connect", connect_alrealy);
				  err = nvs_set_u8(my_handle2, "upgrde", upgrde_init);
				  // printf("meat_temp_offset1=%d\n", meat_temp_offset1);
				  // printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
		
				   // Commit written value.
				   // After setting any values, nvs_commit() must be called to ensure changes are written
				   // to flash storage. Implementations may write to storage at other times,
				   // but this is not guaranteed.
				   //printf("Committing updates in NVS ... ");
				   err = nvs_commit(my_handle2);
				   //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
		
				   // Close
				   nvs_close(my_handle2);
			   }
*/
}
//+++++++++++++
void check_nvs_stats(void)
{
	nvs_stats_t nvs_stats;
	nvs_get_stats(NULL, &nvs_stats);
	printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n",
	       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);
}


