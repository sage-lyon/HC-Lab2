__kernel void calculate_pi (int operands_per_item, __local double* local_results, __global double* global_results, __global double* final_result) {

	// get local and global id and sizes
	int lid = get_local_id(0);
	int gid = get_global_id(0);
	int lsize = get_local_size(0);
	int gsize = get_global_size(0);
    
    // get number of groups and group id
    int num_groups = get_num_groups(0);
    int group_id = get_group_id(0);

	// initialize local values to zero
	for(int i = 0; i < lsize; i++)
		local_results[i] = 0;

	// wait for local results to be initialized to 0	
	barrier(CLK_LOCAL_MEM_FENCE);

	// set work item index
	int index = 2 * gid + 1;

	for(int operand = index; operand < (gsize + 1) * 2; operand += 2 * operands_per_item){
		// if global id is even then work item is adding its operands
		if(gid % 2 == 0)
		    local_results[lid] += (1.0 / operand);

		// if global id is odd then work item is subtracting its operands
		if(gid % 2 == 1)
		    local_results[lid] -= (1.0 / operand);
    }

    // wait for all work items in a group to finish
    barrier(CLK_LOCAL_MEM_FENCE);

    // One member of each group reduces local_results to a single element of global_results
    if(lid == 0){
        for(int i = 0; i < lsize; i++){
            global_results[group_id] += local_results[i];
	    }
    }
    

    // wait for the global_results to be finish
    barrier(CLK_GLOBAL_MEM_FENCE);

    // One work item reduces global_results into a final result
    if(gid == 0){
    	*final_result = 0;
        for(int i = 0; i < num_groups; i++){
        	*final_result += global_results[i];
	}
    }
}
