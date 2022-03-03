__kernel void calculate_pi (int operands_per_item, __local double* local_result, __global double* global_result) {

	// get local and global id and sizes
	int lid = get_local_id(0);
	int gid = get_global_id(0);
	int lsize = get_local_size();
	int gsize = get_global_size();
    
    // get number of groups
    int num_groups = get_num_groups(0);

	// initialize local values to zero
	for(int i = 0; i < lsize; i++)
		local_result[i] = 0;

	// wait for local results to be initialized to 0	
	barrier(CLK_LOCAL_MEM_FENCE);

	// set work item index
	int index = 2 * gid + 1;

	for(int operand = index; operand < (gsize + 1) * 2; operand += 2 * operands_per_item){
        // if global id is even then work item is adding its operands
		if(gid % 2 == 0)
            local_result[lid] += (1.0 / operand);

        // if global id is odd then work item is subtracting its operands
        if(gid % 2 == 1)
            local_result[lid] -= (1.0 / operand);
    }

    // wait for all work items in a group to finish
    barrier(CLK_LOCAL_MEM_FENCE);

}
