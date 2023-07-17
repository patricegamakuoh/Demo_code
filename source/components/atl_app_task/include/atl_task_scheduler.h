/********************************************************************************
 *    Copyright (c), 2023, Atlas IoT Co. Ltd. 
 *    ---------------------------------------------------------------------------                          
 *                      
 *    This software is the property of "Atlas IoT Co. Ltd and may 
 *    not be copied or reproduced otherwise than on to a single hard disk for 
 *    backup or archival purposes.  The source code is confidential  
 *    information and must not be disclosed to third parties or used without  
 *    the express written permission of Atlas IoT Co. Ltd.
 *    Copyright (c), 2023, Atlas IoT Co. Ltd. All rights reserved. 
 *    ----------------------------------------------------------------------------
 *    ----------------------------------------------------------------------------
 *    FILE DESCRIPTION
 *    ----------------------------------------------------------------------------
 *            @file:    atl_app_task.c     
 *          @author:    Patrice Gama 
 *            @date:    2023-03-26
 *       @Component:    atl_app_task
 *              
 *     @Description:    all the tasks priority, task stack size and the task core ID can
 *    be modified in this header file.
 *    ----------------------------------------------------------------------------
 *    REVISION HISTORY
 *    ----------------------------------------------------------------------------
 *        @revision:     
 *            @date:       
 *          @author:  
 *         @summary: 
 *    ----------------------------------------------------------------------------
 *********************************************************************************/

#ifndef ATL_SCHEDULER_TASK_H_
#define ATL_SCHEDULER_TASK_H_

/*atl_RPID controller task*/
#define RPID_TASK_STACK_SIZE	    		4096
#define RPID_TASK_PRIORITY			    	5
#define RPID_TASK_CORE_ID			     	0
 
/*atl_LLTA Unit test task*/
#define LLTA_TASK_STACK_SIZE	            4096
#define LLTA_TASK_PRIORITY		            3
#define LLTA_TASK_CORE_ID		            0

/*other task*/
#define OTHER_TASK_STACK_SIZE	    		8192
#define OTHER_TASK_PRIORITY	     		    4
#define OTHER_TASK_CORE_ID	      		    0

#endif /*ATL_SCHEDULER_TASK_H_*/
